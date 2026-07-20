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
class Future;

template <typename T>
struct IsFuture : std::false_type {};

template <typename T>
struct IsFuture<Future<T>> : std::true_type {};

namespace detail {

template <typename T>
struct UnwrapFuture {
  using Type = T;
};

template <typename T>
struct UnwrapFuture<Future<T>> {
  using Type = T;
};

template <typename T>
using UnwrapFutureT = typename UnwrapFuture<T>::Type;

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
  using Unwraped = detail::UnwrapFutureT<Type>;

  static Future<Unwraped> CreateFuture() {
    return Future<Unwraped>();
  }
};

struct Empty {};

template <typename T, typename = void>
struct FutureValue {
  using Type = T;

  template <typename F>
  auto Call(F&& f) -> InvokeResultT<F, T> {
    return std::forward<F>(f)(v);
  }

  T                         v;
  absl::optional<absl::any> error;
};

template <>
struct FutureValue<void, void> {
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
  static void Resolve(F&& f, FutureValue<Input> resolved, Next& next) {
    next.Resolve(resolved.Call(std::forward<F>(f)));
  }
};

template <typename Input>
struct Resolver<Input, void> {
  template <typename F, typename Next>
  static void Resolve(F&& f, FutureValue<Input> resolved, Next& next) {
    resolved.Call(std::forward<F>(f));
    next.Resolve();
  }
};

}  // namespace detail

template <typename T>
class Future {
 public:
  Future();

  Future(const Future&) = default;

  Future& operator=(const Future&) = default;

  Future(Future&& rh) noexcept;

  Future& operator=(Future&& rh) noexcept;

  template <typename U>
  void Resolve(U&& v);

  void Resolve();

  template <typename U>
  void ResolveError(U&& v);

  void Wait();

  template <typename F>
  auto Then(F&& f) -> Future<typename detail::ThenResult<T, F>::Unwraped>;

  template <typename F>
  auto OnError(F&& f) -> Future<typename detail::FuncRetTypeT<F>>;

 private:
  enum class State : uint8_t {
    kFinished,
    kPending,
  };

  struct FutureContext {
    std::mutex                                  mutex;
    detail::FutureValue<T>                      resolved;
    std::function<void(detail::FutureValue<T>)> then;
    State                                       state = State::kPending;
  };

  std::shared_ptr<FutureContext> d;
};

template <typename T>
Future<T>::Future() : d(std::make_shared<FutureContext>()) {
}

template <typename T>
Future<T>::Future(Future&& rh) noexcept {
  this->d = std::move(rh.d);
}

template <typename T>
Future<T>& Future<T>::operator=(Future&& rh) noexcept {
  if (this == &rh) {
    return *this;
  }
  std::swap(this->d, rh.d);

  return *this;
}

template <typename T>
template <typename U>
void Future<T>::Resolve(U&& v) {
  std::function<void(detail::FutureValue<T>)> then;

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
void Future<T>::Resolve() {
  Resolve(detail::Empty());
}

template <typename T>
template <typename U>
void Future<T>::ResolveError(U&& v) {
  std::function<void(detail::FutureValue<T>)> then;

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
  struct BasicTypeTag {};

  struct FutureTypeTag {};

  static const auto is_future = IsFuture<typename detail::ThenResult<Input, F>::Type>::value;

  using Output = typename detail::ThenResult<Input, F>::Unwraped;

  static auto CreateThen(Future<Output>& next, F&& f) {
    using Tag = std::conditional_t<is_future, FutureTypeTag, BasicTypeTag>;
    return CreateThenImpl(next, std::forward<F>(f), Tag{});
  }

  static auto CreateThenImpl(Future<Output>& next, F&& f, BasicTypeTag) {
    return [next, f = std::forward<F>(f)](detail::FutureValue<Input> v) mutable {
      if (!v.error) {
        detail::Resolver<Input, Output>::Resolve(std::forward<F>(f), std::move(v), next);
      } else {
        next.ResolveError(*v.error);
      }
    };
  }

  static auto CreateThenImpl(Future<Output>& next, F&& f, FutureTypeTag) {
    return [next, f = std::forward<F>(f)](detail::FutureValue<Input> v) mutable {
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
auto Future<T>::Then(F&& f) -> Future<typename detail::ThenResult<T, F>::Unwraped> {
  auto next = detail::ThenResult<T, F>::CreateFuture();
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
auto Future<T>::OnError(F&& f) -> Future<typename detail::FuncRetTypeT<F>> {
  using Input = T;
  using ErrorT = detail::FuncArgTypeT<F>;
  using Output = detail::FuncRetTypeT<F>;

  Future<Output> next;

  auto then = [next, f = std::forward<F>(f)](detail::FutureValue<Input> v) mutable {
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
