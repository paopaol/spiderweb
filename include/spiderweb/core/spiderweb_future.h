#pragma once

#include <absl/types/any.h>
#include <absl/types/optional.h>

#include <algorithm>
#include <cassert>
#include <functional>
#include <memory>
#include <mutex>
#include <type_traits>
#include <utility>

namespace spiderweb {

namespace detail {

template <typename F, typename T>
struct InvokeResult {
  using Type = std::invoke_result_t<F, T>;
};

template <typename F>
struct InvokeResult<F, void> {
  using Type = std::invoke_result_t<F>;
};

template <typename F, typename T>
using InvokeResultT = typename InvokeResult<F, T>::Type;

template <typename T>
struct FuncArgTraits;

template <typename R, typename Arg>
struct FuncArgTraits<R(Arg)> {
  using Type = std::decay_t<Arg>;
  using ReturnType = R;
};

template <typename R, typename C, typename Arg>
struct FuncArgTraits<R (C::*)(Arg) const> {
  using Type = std::decay_t<Arg>;
  using ReturnType = R;
};

template <typename F>
using FuncArgTypeT = typename FuncArgTraits<decltype(&std::decay_t<F>::operator())>::Type;

template <typename F>
using FuncReturnTypeT = typename FuncArgTraits<decltype(&std::decay_t<F>::operator())>::ReturnType;

struct Empty {};

template <typename T, typename = void>
struct FutureValue {
  using Type = T;

  template <typename F>
  auto Call(F&& f) -> InvokeResultT<F, T> {
    return f(v);
  }

  template <typename E, typename F>
  auto CallError(F&& f) -> InvokeResultT<F, E> {
    auto err = absl::any_cast<E>(error.value());
    return f(err);
  }

  T                         v;
  absl::optional<absl::any> error;
};

template <>
struct FutureValue<void, void> {
  using Type = Empty;

  template <typename F>
  auto Call(F&& f) -> InvokeResultT<F, void> {
    return f();
  }

  template <typename E, typename F>
  auto CallError(F&& f) -> InvokeResultT<F, void> {
    auto err = absl::any_cast<E>(error.value());
    return f(err);
  }

  Empty                     v;
  absl::optional<absl::any> error;
};

template <typename Input, typename Output>
struct Resolver {
  template <typename F, typename Next>
  static void Resolve(F&& f, FutureValue<Input> resolved, Next& next) {
    next.Resolve(resolved.Call(f));
  }

  template <typename E, typename F, typename Next>
  static void ResolveError(F&& f, FutureValue<Input> resolved, Next& next) {
    next.ResolveError(resolved.template CallError<E>(f));
  }
};

template <typename Input>
struct Resolver<Input, void> {
  template <typename F, typename Next>
  static void Resolve(F&& f, FutureValue<Input> resolved, Next& next) {
    resolved.Call(f);
    next.Resolve();
  }

  template <typename E, typename F, typename Next>
  static void ResolveError(F&& f, FutureValue<Input> resolved, Next& next) {
    next.ResolveError(resolved.template CallError<E>(f));
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
  auto Then(F&& f) -> Future<detail::InvokeResultT<F, T>>;

  template <typename F>
  auto OnError(F&& f) -> Future<detail::FuncReturnTypeT<F>>;

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
    then(d->resolved);
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
    then(d->resolved);
  }
}

template <typename T>
template <typename F>
auto Future<T>::Then(F&& f) -> Future<detail::InvokeResultT<F, T>> {
  using Input = T;
  using Output = detail::InvokeResultT<F, Input>;

  Future<Output> next;

  auto then = [next, f = std::forward<F>(f), this](detail::FutureValue<Input> v) mutable {
    if (!v.error) {
      detail::Resolver<Input, Output>::Resolve(f, v, next);
    } else {
      next.ResolveError(*v.error);
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
    then(d->resolved);
  }

  return next;
}

template <typename T>
template <typename F>
auto Future<T>::OnError(F&& f) -> Future<detail::FuncReturnTypeT<F>> {
  using Input = T;
  using ErrorT = detail::FuncArgTypeT<F>;
  using Output = detail::FuncReturnTypeT<F>;

  Future<Output> next;

  auto then = [next, f = std::forward<F>(f)](detail::FutureValue<Input> v) mutable {
    if (v.error) {
      detail::Resolver<Input, Output>::template ResolveError<ErrorT>(f, v, next);
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
    then(d->resolved);
  }

  return next;
}

}  // namespace spiderweb
