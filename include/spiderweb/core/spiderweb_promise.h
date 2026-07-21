#pragma once

#include <absl/types/any.h>
#include <absl/types/optional.h>

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <functional>
#include <memory>
#include <mutex>
#include <type_traits>
#include <utility>

namespace spiderweb {

template <typename T>
class Promise;

template <typename T>
struct IsPromise : std::false_type {};

template <typename T>
struct IsPromise<Promise<T>> : std::true_type {};

namespace detail {

template <typename T>
struct UnwrapPromise {
  using Type = T;
};

template <typename T>
struct UnwrapPromise<Promise<T>> {
  using Type = T;
};

template <typename T>
using UnwrapPromiseT = typename UnwrapPromise<T>::Type;

template <typename F, typename T>
struct InvokeResult {
  using Type = std::invoke_result_t<std::decay_t<F>, T>;
};

template <typename F>
struct InvokeResult<F, void> {
  using Type = std::invoke_result_t<std::decay_t<F>>;
};

template <typename F, typename T>
using InvokeResultT = typename InvokeResult<F, T>::Type;

template <typename... Args>
struct FirstArg {
  using Type = void;
};

template <typename First, typename... Rest>
struct FirstArg<First, Rest...> {
  using Type = std::decay_t<First>;
};

template <typename T>
struct FuncArgTraits;

template <typename Ret, typename... Args>
struct FuncArgTraits<Ret (*)(Args...)> {
  using Type = typename FirstArg<Args...>::Type;
  using RetType = Ret;
};

template <typename Ret, typename... Args>
struct FuncArgTraits<Ret (&)(Args...)> {
  using Type = typename FirstArg<Args...>::Type;
  using RetType = Ret;
};

template <typename R, typename... Args>
struct FuncArgTraits<R(Args...)> {
  using Type = typename FirstArg<Args...>::Type;
  using RetType = R;
};

template <typename R, typename C, typename... Args>
struct FuncArgTraits<R (C::*)(Args...) const> {
  using Type = typename FirstArg<Args...>::Type;
  using RetType = R;
};

template <typename R, typename C, typename... Args>
struct FuncArgTraits<R (C::*)(Args...)> {
  using Type = typename FirstArg<Args...>::Type;
  using RetType = R;
};

template <typename R, typename C, typename... Args>
struct FuncArgTraits<R (C::*)(Args...) const volatile> {
  using Type = typename FirstArg<Args...>::Type;
  using RetType = R;
};

template <typename F>
struct FuncArgTraits {
 private:
  using OperatorType = decltype(&F::operator());

 public:
  using Type = typename FuncArgTraits<OperatorType>::Type;
  using RetType = typename FuncArgTraits<OperatorType>::RetType;
};

template <typename F>
using FuncArgTypeT = typename FuncArgTraits<std::decay_t<F>>::Type;

template <typename F>
using FuncRetTypeT = typename FuncArgTraits<std::decay_t<F>>::RetType;

template <typename Input, typename F>
struct ThenResult {
  using Type = detail::InvokeResultT<F, Input>;
  using Unwraped = detail::UnwrapPromiseT<Type>;

  static Promise<Unwraped> CreatePromise() {
    return Promise<Unwraped>();
  }
};

struct Empty {};

template <typename T, typename = void>
struct PromiseValue {
  using Type = T;

  template <typename F>
  auto Call(F&& f) -> InvokeResultT<F, T> {
    return std::forward<F>(f)(v);
  }

  T                         v;
  absl::optional<absl::any> error;
};

template <>
struct PromiseValue<void, void> {
  using Type = Empty;

  template <typename F>
  auto Call(F&& f) -> InvokeResultT<F, void> {
    return std::forward<F>(f)();
  }

  Empty                     v;
  absl::optional<absl::any> error;
};

template <typename Input, typename Output>
struct Resolver {
  template <typename F, typename Next>
  static void Resolve(F&& f, PromiseValue<Input> resolved, Next& next) {
    next.Resolve(resolved.Call(std::forward<F>(f)));
  }
};

template <typename Input>
struct Resolver<Input, void> {
  template <typename F, typename Next>
  static void Resolve(F&& f, PromiseValue<Input> resolved, Next& next) {
    resolved.Call(std::forward<F>(f));
    next.Resolve();
  }
};

}  // namespace detail

template <typename T>
class Promise {
 public:
  Promise();

  Promise(const Promise&) = default;

  Promise& operator=(const Promise&) = default;

  Promise(Promise&& rh) noexcept;

  Promise& operator=(Promise&& rh) noexcept;

  template <typename U>
  void Resolve(U&& v);

  void Resolve();

  template <typename U>
  void ResolveError(U&& v);

  void Wait();

  template <typename F>
  auto Then(F&& f) -> Promise<typename detail::ThenResult<T, F>::Unwraped>;

  template <typename F>
  auto OnError(F&& f) -> Promise<typename detail::FuncRetTypeT<F>>;

 private:
  enum class State : uint8_t {
    kFinished,
    kPending,
  };

  struct PromiseContext {
    std::mutex                                   mutex;
    detail::PromiseValue<T>                      resolved;
    std::function<void(detail::PromiseValue<T>)> then;
    State                                        state = State::kPending;
  };

  std::shared_ptr<PromiseContext> d;
};

template <typename T>
Promise<T>::Promise() : d(std::make_shared<PromiseContext>()) {
}

template <typename T>
Promise<T>::Promise(Promise&& rh) noexcept {
  this->d = std::move(rh.d);
}

template <typename T>
Promise<T>& Promise<T>::operator=(Promise&& rh) noexcept {
  if (this == &rh) {
    return *this;
  }
  std::swap(this->d, rh.d);

  return *this;
}

template <typename T>
template <typename U>
void Promise<T>::Resolve(U&& v) {
  std::function<void(detail::PromiseValue<T>)> then;

  {
    std::lock_guard<std::mutex> _(d->mutex);

    if (d->state == State::kFinished) {
      return;
    }

    d->resolved.v = std::forward<U>(v);
    d->state = State::kFinished;

    if (d->then) {
      then = std::move(d->then);
    }
  }

  if (then) {
    then(std::move(d->resolved));
  }
}

template <typename T>
void Promise<T>::Resolve() {
  Resolve(detail::Empty());
}

template <typename T>
template <typename U>
void Promise<T>::ResolveError(U&& v) {
  std::function<void(detail::PromiseValue<T>)> then;

  {
    std::lock_guard<std::mutex> _(d->mutex);

    if (d->state == State::kFinished) {
      return;
    }

    d->resolved.error = absl::any(std::forward<U>(v));
    d->state = State::kFinished;

    if (d->then) {
      then = std::move(d->then);
    }
  }

  if (then) {
    then(std::move(d->resolved));
  }
}

template <typename Input, typename F>
struct Invoke {
  struct BasicTag {};

  struct PromiseTag {};

  static const auto is_promise = IsPromise<typename detail::ThenResult<Input, F>::Type>::value;

  using Output = typename detail::ThenResult<Input, F>::Unwraped;

  static auto CreateThen(Promise<Output>& next, F&& f) {
    using Tag = std::conditional_t<is_promise, PromiseTag, BasicTag>;
    return CreateThenImpl(next, std::forward<F>(f), Tag{});
  }

  static auto CreateThenImpl(Promise<Output>& next, F&& f, BasicTag) {
    return [next, f = std::forward<F>(f)](detail::PromiseValue<Input> v) mutable {
      if (!v.error) {
        detail::Resolver<Input, Output>::Resolve(std::forward<F>(f), std::move(v), next);
      } else {
        next.ResolveError(*v.error);
      }
    };
  }

  static auto CreateThenImpl(Promise<Output>& next, F&& f, PromiseTag) {
    return [next, f = std::forward<F>(f)](detail::PromiseValue<Input> v) mutable {
      if (v.error) {
        next.ResolveError(*v.error);
        return;
      }

      auto lazy = v.Call(std::forward<decltype(f)>(f));

      lazy.Then([next](Output val) mutable { next.Resolve(std::move(val)); })
          .OnError([next](absl::any err) mutable {
            next.ResolveError(std::move(err));
            return false;
          });
    };
  }
};

template <typename T>
template <typename F>
auto Promise<T>::Then(F&& f) -> Promise<typename detail::ThenResult<T, F>::Unwraped> {
  auto next = detail::ThenResult<T, F>::CreatePromise();
  auto then = Invoke<T, F>::CreateThen(next, std::forward<F>(f));

  bool should_then = false;
  {
    std::lock_guard<std::mutex> _(d->mutex);

    if (d->state == State::kFinished) {
      should_then = true;
    } else {
      d->then = std::move(then);
    }
  }

  if (should_then) {
    then(d->resolved);
  }

  return next;
}

template <typename T>
template <typename F>
auto Promise<T>::OnError(F&& f) -> Promise<typename detail::FuncRetTypeT<F>> {
  using Input = T;
  using ErrorT = detail::FuncArgTypeT<F>;
  using Output = detail::FuncRetTypeT<F>;

  Promise<Output> next;

  auto then = [next, f = std::forward<F>(f)](detail::PromiseValue<Input> v) mutable {
    if (v.error) {
      next.ResolveError(std::forward<F>(f)(absl::any_cast<ErrorT>(v.error.value())));
    }
  };

  bool should_then = false;
  {
    std::lock_guard<std::mutex> _(d->mutex);

    if (d->state == State::kFinished) {
      should_then = true;
    } else {
      d->then = std::move(then);
    }
  }

  if (should_then) {
    then(std::move(d->resolved));
  }

  return next;
}

}  // namespace spiderweb
