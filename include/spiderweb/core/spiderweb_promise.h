#pragma once

#include <absl/types/any.h>

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace spiderweb {

class AlreadyContinued : public std::runtime_error {
 public:
  explicit AlreadyContinued(const std::string& msg) : std::runtime_error(msg) {
  }
};

template <typename T>
class Promise;

template <typename T>
struct IsPromise : std::false_type {};

template <typename T>
struct IsPromise<Promise<T>> : std::true_type {};

template <typename T>
struct Tag {
  static char tag;
};
template <typename T>
inline char Tag<T>::tag;

struct Error {
  Error() = default;

  Error(const Error&) = delete;

  Error& operator=(const Error&) = delete;

  Error(Error&&) noexcept = default;

  Error& operator=(Error&&) noexcept = default;

  explicit operator bool() const {
    return tag != nullptr;
  }

  template <typename T>
  static Error Make(T value) {
    Error err;

    err.tag = &Tag<T>::tag;
    err.value = std::move(value);

    return err;
  }

  template <typename T>
  bool Is() const {
    return tag == &Tag<T>::tag;
  }

  template <typename T>
  T& Get() {
    return absl::any_cast<T&>(value);
  }

  template <typename T>
  const T& Get() const {
    return absl::any_cast<const T&>(value);
  }

  void*     tag = nullptr;
  absl::any value;
};

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

template <typename Input, typename F>
struct ThenResult {
  using Type = detail::InvokeResultT<F, Input>;
  using Unwraped = detail::UnwrapPromiseT<Type>;

  static Promise<Unwraped> CreatePromise() {
    return Promise<Unwraped>();
  }
};

struct Void {};

template <typename T>
struct Wrapper {
  using Wrape = T;
  using UnWrape = T;
};

template <>
struct Wrapper<void> {
  using Wrape = Void;
  using UnWrape = void;
};

template <typename T>
using Wrap = typename Wrapper<T>::Wrape;

template <typename T>
using UnWrap = typename Wrapper<T>::UnWrape;

template <typename T>
struct PromiseValue {
  Wrap<T> v{};
  Error   error{};
};

template <typename T>
inline constexpr bool IsParam = !std::is_void<T>::value;

template <typename T>
inline constexpr bool IsResult = !std::is_void<T>::value;

template <typename R, typename F, typename... Args, typename = std::enable_if_t<IsResult<R>>>
Wrap<R> invoke(F&& f, Args&&... args) {
  return std::forward<F>(f)(std::forward<Args>(args)...);
}

template <typename R, typename F, typename... Args, typename = std::enable_if_t<!IsResult<R>>>
Void invoke(F&& f, Args&&... args) {
  std::forward<F>(f)(std::forward<Args>(args)...);
  return Void{};
}

template <typename T, typename F>
auto invoke_func(F&& f, Wrap<T>&& v) -> Wrap<InvokeResultT<F, T>> {
  using R = InvokeResultT<F, T>;
  if constexpr (IsParam<T>) {
    return invoke<R>(std::forward<F>(f), v);
  } else {
    return invoke<R>(std::forward<F>(f));
  }
}

template <typename Input, typename Output>
struct Resolver {
  template <typename F, typename Next>
  static void Resolve(F&& f, PromiseValue<Input> resolved, Next& next) {
    next.Resolve(invoke_func<Input>(std::forward<F>(f), std::move(resolved.v)));
  }
};

struct BasicTag {};

struct PromiseTag {};

template <typename E, typename F>
struct ErrorInvoke {
  using RawRet = std::invoke_result_t<std::decay_t<F>, E&>;
  using Output = UnwrapPromiseT<RawRet>;

  static constexpr auto is_promise = IsPromise<typename detail::ThenResult<E&, F>::Type>::value;

  template <typename Input>
  static auto CreateErrorThen(Promise<Output>& next, F&& f) {
    using Tag = std::conditional_t<is_promise, PromiseTag, BasicTag>;
    return CreateErrorThenImpl<Input>(next, std::forward<F>(f), Tag{});
  }

  template <typename Input>
  static auto CreateErrorThenImpl(Promise<Output>& next, F&& f, BasicTag) {
    return [next, f = std::forward<F>(f)](detail::PromiseValue<Input> v) mutable {
      if (!v.error) {
        next.Resolve(std::move(v.v));
        return;
      }

      if (!v.error.template Is<E>()) {
        next.ResolveError(std::move(v.error));
        return;
      }

      auto ret = detail::invoke<Output>(std::move(f), v.error.template Get<E>());
      next.Resolve(std::move(ret));
    };
  }

  template <typename Input>
  static auto CreateErrorThenImpl(Promise<Output>& next, F&& f, PromiseTag) {
    return [next, f = std::forward<F>(f)](detail::PromiseValue<Input> v) mutable {
      if (!v.error) {
        next.Resolve(std::move(v.v));
        return;
      }

      if (!v.error.template Is<E>()) {
        next.ResolveError(std::move(v.error));
        return;
      }

      auto lazy = std::move(f)(v.error.template Get<E>());

      lazy.Then([next](Output val) mutable { next.Resolve(std::move(val)); })
          .OnError(Tag<Error>{}, [next](Error& err) mutable { next.ResolveError(std::move(err)); });
    };
  }
};

template <typename Input, typename F>
struct ThenInvoke {
  using Output = typename detail::ThenResult<Input, F>::Unwraped;

  static constexpr auto is_promise = IsPromise<typename detail::ThenResult<Input, F>::Type>::value;

  static auto CreateThen(Promise<Output>& next, F&& f) {
    using Tag = std::conditional_t<is_promise, PromiseTag, BasicTag>;
    return CreateThenImpl(next, std::forward<F>(f), Tag{});
  }

 private:
  static auto CreateThenImpl(Promise<Output>& next, F&& f, BasicTag) {
    return [next, f = std::forward<F>(f)](detail::PromiseValue<Input> v) mutable {
      if (!v.error) {
        detail::Resolver<Input, Output>::Resolve(std::forward<F>(f), std::move(v), next);
      } else {
        next.ResolveError(std::move(v.error));
      }
    };
  }

  static auto CreateThenImpl(Promise<Output>& next, F&& f, PromiseTag) {
    return [next, f = std::forward<F>(f)](detail::PromiseValue<Input> v) mutable {
      if (v.error) {
        next.ResolveError(std::move(v.error));
        return;
      }

      auto lazy = detail::invoke_func<Input>(std::move(f), std::move(v.v));

      lazy.Then([next](Output val) mutable { next.Resolve(std::move(val)); })
          .OnError(Tag<Error>{}, [next](Error& err) mutable { next.ResolveError(std::move(err)); });
    };
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

  void ResolveError(Error err);

  template <typename E>
  void Reject(E e);

  template <typename F>
  auto Then(F&& f) -> Promise<typename detail::ThenResult<T, F>::Unwraped>;

  template <typename E, typename F>
  auto OnError(Tag<E>, F&& f) -> Promise<T>;

 private:
  enum class State : uint8_t {
    kPending = 0,
    kFinished,
  };

  struct Shared {
    std::mutex                                   mutex;
    detail::PromiseValue<T>                      resolved;
    std::function<void(detail::PromiseValue<T>)> then;
    State                                        state = State::kPending;
  };

  std::shared_ptr<Shared> d;
};

template <typename T>
Promise<std::vector<T>> PromiseAll(std::vector<Promise<T>> promises);

template <typename T>
Promise<T>::Promise() : d(std::make_shared<Shared>()) {
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
  detail::PromiseValue<T>                      resolved;
  {
    std::lock_guard<std::mutex> _(d->mutex);

    if (d->state == State::kFinished) {
      return;
    }

    d->resolved.v = std::forward<U>(v);
    d->state = State::kFinished;

    if (d->then) {
      then = std::move(d->then);
      resolved = std::move(d->resolved);
    }
  }

  if (then) {
    then(std::move(resolved));
  }
}

template <typename T>
void Promise<T>::Resolve() {
  Resolve(detail::Void());
}

template <typename T>
void Promise<T>::ResolveError(Error err) {
  std::function<void(detail::PromiseValue<T>)> then;
  detail::PromiseValue<T>                      resolved;
  {
    std::lock_guard<std::mutex> _(d->mutex);

    if (d->state == State::kFinished) {
      return;
    }

    d->resolved.error = std::move(err);
    d->state = State::kFinished;

    if (d->then) {
      then = std::move(d->then);
      resolved = std::move(d->resolved);
    }
  }

  if (then) {
    then(std::move(resolved));
  }
}

template <typename T>
template <typename E>
void Promise<T>::Reject(E e) {
  this->ResolveError(Error::Make(std::move(e)));
}

template <typename T>
template <typename F>
auto Promise<T>::Then(F&& f) -> Promise<typename detail::ThenResult<T, F>::Unwraped> {
  auto next = detail::ThenResult<T, F>::CreatePromise();
  auto then = detail::ThenInvoke<T, F>::CreateThen(next, std::forward<F>(f));

  detail::PromiseValue<T> resolved;
  bool                    should_then = false;
  {
    std::lock_guard<std::mutex> _(d->mutex);
    if (d->then) {
      throw AlreadyContinued("Then");
    }

    if (d->state == State::kFinished) {
      should_then = true;
      resolved = std::move(d->resolved);
    } else {
      d->then = std::move(then);
    }
  }

  if (should_then) {
    then(std::move(resolved));
  }

  return next;
}

template <typename T>
template <typename E, typename F>
auto Promise<T>::OnError(Tag<E>, F&& f) -> Promise<T> {
  using Input = T;
  using RetType = detail::UnwrapPromiseT<std::invoke_result_t<std::decay_t<F>, E&>>;

  Promise<T> next;
  auto then = detail::ErrorInvoke<E, F>::template CreateErrorThen<Input>(next, std::forward<F>(f));

  static_assert(std::is_same<RetType, T>::value,
                "OnError callback must return T (the Promise value type)");

  detail::PromiseValue<T> resolved;
  bool                    should_then = false;
  {
    std::lock_guard<std::mutex> _(d->mutex);

    if (d->then) {
      throw AlreadyContinued("OnError");
    }

    if (d->state == State::kFinished) {
      should_then = true;
      resolved = std::move(d->resolved);
    } else {
      d->then = std::move(then);
    }
  }

  if (should_then) {
    then(std::move(resolved));
  }

  return next;
}

template <typename T>
Promise<std::vector<T>> PromiseAll(std::vector<Promise<T>> promises) {
  Promise<std::vector<T>> next;

  if (promises.empty()) {
    next.Resolve(std::vector<T>{});
    return next;
  }

  struct Result {
    std::mutex     mutex;
    std::vector<T> vals;
    std::size_t    tasks = 0;
  };

  auto result = std::make_shared<Result>();

  result->vals.resize(promises.size());
  result->tasks = promises.size();

  for (std::size_t i = 0; i < promises.size(); ++i) {
    auto& pro = promises[i];

    pro.Then([next, result, i](T val) mutable {
         std::lock_guard<std::mutex> _(result->mutex);

         result->vals[i] = std::move(val);
         result->tasks--;
         if (result->tasks == 0) {
           next.Resolve(std::move(result->vals));
         }
       })
        .OnError(Tag<Error>{},
                 [next, result, i](Error& err) mutable { next.ResolveError(std::move(err)); });
  }

  return next;
}

}  // namespace spiderweb
