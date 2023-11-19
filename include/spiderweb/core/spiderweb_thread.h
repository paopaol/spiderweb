#pragma once

#include <functional>
#include <memory>
#include <thread>

namespace spiderweb {

/**
 * @brief Thread Is EventLoop Thread.
 * with this class, we can simple start a thread, that run an event loop.
 * and, we can call `QueueTask()`  post a task, the task will called by the
 * event loop in this thread
 *
 * @example
 *  using TimerPtr = std::unique_ptr<spiderweb::Timer>;
 *
 *  spiderweb::Thread t;
 *  TimerPtr          timer;
 *
 *  r.Start();
 *  r.QueueTask([](){
 *    timer = absl::make_unique<spiderweb::Timer>();
 *
 *    timer->SetInterval(100);
 *    timer->SetSingalShot(true);
 *
 *    timer->Start();
 *  });
 *  :sleep(100000);
 */
class Thread {
  class Private;

 public:
  using Id = std::thread::id;

  Thread();

  ~Thread();

  Thread(const Thread &) = delete;

  Thread &operator=(const Thread &rh) = delete;

  /**
   * @brief Thread The Loop Thread
   */
  void Start();

  void Quit();

  void QueueTask(std::function<void()> &&f);

  bool IsRunning() const;

  Id GetThreadId() const;

 private:
  std::shared_ptr<Private> d;
};
}  // namespace spiderweb
