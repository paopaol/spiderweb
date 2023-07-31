#pragma once

#include <future>

#include "spiderweb/core/spiderweb_thread.h"
#include "spiderweb/ppk_assert.h"

namespace spiderweb {

template <typename T>
using Future = std::future<T>;

template <typename T>
using Callback = std::function<void(T)>;

template <class T>
struct IsVoid : std::is_same<void, typename std::remove_cv<T>::type> {};

namespace detail {

template <typename T, typename Enable = void>
struct Callee {
  template <typename F, typename U>
  static void Run(F &&f, U &&u) {
    f(u());
  }
};

template <typename T>
struct Callee<T, typename std::enable_if<IsVoid<T>::value>::type> {
  template <typename F, typename U>
  static void Run(F &&f, U &&u) {
    u();
    f();
  }
};
}  // namespace detail

template <typename T>
class ThreadData : public T {
 public:
  using Deleter = std::function<void(ThreadData *)>;
  using Notify = std::function<void()>;

  template <typename U>
  using Then = std::function<void(U &&result)>;

  template <typename... Args>
  static ThreadData *New(Thread *thread, Args &&...args) {
    PPK_ASSERT_FATAL(thread->IsRunning());

    std::promise<ThreadData *> pro;

    thread->QueueTask([&]() {
      auto *p = new ThreadData(thread, std::forward<Args>(args)...);
      pro.template set_value(p);
    });

    return pro.get_future().get();
  }

  static void Destory(ThreadData *data, const Deleter &deleter = DefaultDelete) {
    PPK_ASSERT_FATAL(data->thread_->IsRunning());

    std::promise<bool> pro;

    data->thread_->QueueTask([&]() {
      deleter(data);
      pro.set_value(true);
    });

    pro.get_future().get();
  }

  static void AsyncDelete(ThreadData *data, const Notify &notify,
                          const Deleter &deleter = DefaultDelete) {
    data->thread_->QueueTask([=]() {
      deleter(data);
      notify();
    });
  }

  template <typename Signature, typename... Args>
  auto Call(Signature signature, Args &&...args)
      -> decltype((std::declval<T>().*signature)(std::forward<Args>(args)...)) {
    PPK_ASSERT_FATAL(thread_->IsRunning());

    using Return = decltype((std::declval<T>().*signature)(std::forward<Args>(args)...));
    std::packaged_task<Return()> f(
        std::bind(std::forward<Signature>(signature), this, std::forward<Args>(args)...));
    thread_->QueueTask(std::move([&]() { f(); }));
    return f.get_future().get();
  }

  template <typename Signature, typename... Args>
  void AsyncCall(Signature &&signature, Args &&...args) {
    PPK_ASSERT_FATAL(thread_->IsRunning());

    thread_->QueueTask(
        std::bind(std::forward<Signature>(signature), this, std::forward<Args>(args)...));
  }

  template <typename Signature, typename... Args, typename F>
  void AsyncCallThen(Signature &&signature, Args &&...args, F &&notify) {
    PPK_ASSERT_FATAL(thread_->IsRunning());

    const auto task =
        std::bind(std::forward<Signature>(signature), this, std::forward<Args>(args)...);

    const auto wrapper = [=]() { detail::Callee<decltype(task())>::Run(notify, task); };

    thread_->QueueTask(std::move(wrapper));
  }

 private:
  template <typename... Args>
  explicit ThreadData(Thread *thread, Args &&...args)
      : T(std::forward<Args>(args)...), thread_(thread) {
  }

  static void DefaultDelete(ThreadData *data) {
    delete data;
  }

  Thread *thread_ = nullptr;
};

template <typename T>
void LazyDestory(T *data) {
  data->DeleteLater();
}

}  // namespace spiderweb
