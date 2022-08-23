#ifndef SPIDERWEB_TIMER_H
#define SPIDERWEB_TIMER_H

#include "asio/steady_timer.hpp"
#include "spiderweb_eventloop.h"

namespace spiderweb {
/**
 * @brief Timer:
 *
 * a time for a timed task.
 *
 * time can be set repeatable.
 *
 * it alse can be set trigger once only
 *
 * @note default, it is a repeatable timer
 *
 * timer instace can not be used call across threads.
 *
 * created in A thread, must be used in A thread.
 *
 * defualt interval is 3000ms
 */
class Timer : public Object {
 public:
  Timer(Object *parent = nullptr);

  ~Timer();

  /**
   * @brief set timer tick based on milliseconds
   *
   * @param timeout_ms
   */
  void SetInterval(uint32_t timeout_ms);

  /**
   * @brief return interval of the timer
   *
   * @return
   */
  uint32_t Interval() const;

  /**
   * @brief if set true, the timer will only triggered once
   *
   * @param flag
   */
  void SetSingalShot(bool flag);

  void Start();

  void Stop();

  void Reset(const uint64_t timeout_ms);

  bool IsRunning() const;

  /**
   * @brief signal, if the timer timeout, it emit this signal.
   *
   * user can connect this timer
   */
  EventSignal<void()> timeout;

 private:
  class Private;
  std::unique_ptr<Private> d;
};
}  // namespace spiderweb

#endif
