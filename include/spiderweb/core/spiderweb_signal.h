#pragma once

#include "spiderweb/core/spiderweb_error_code.h"
#include "spiderweb/core/spiderweb_object.h"

namespace spiderweb {

/**
 * @brief Signals:
 *
 * A signal set represents one or more signals.
 *
 * Users can register the signal they are interested in,
 *
 * and when the signal arrives, the corresponding event
 *
 * will be triggered to the user
 */
class Signals : public Object {
 public:
  explicit Signals(int sig_number, Object* parent = nullptr);

  ~Signals() override;

  ErrorCode AddSignal(int sig_number);

  ErrorCode RemoveSignal(int sig_number);

  ErrorCode Cancel();

  Notify<int> Triggered;

 private:
  class Private;

  std::unique_ptr<Private> d;
};
};  // namespace spiderweb
