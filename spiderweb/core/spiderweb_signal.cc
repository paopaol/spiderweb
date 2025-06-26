#include "spiderweb/core/spiderweb_signal.h"

#include "asio/signal_set.hpp"
#include "core/internal/asio_cast.h"

namespace spiderweb {
class Signals::Private {
 public:
  Private(Signals* qq, Object* parent, int sig_number)
      : q(qq), set_(AsioService(GetLoop(parent)), sig_number) {
  }

  void SetUp() {
    auto self = q->shared_from_this();
    set_.async_wait([self, this](const asio::error_code& ec, int signal_number) {
      if (ec) {
        return;
      }

      spider_emit q->Triggered(signal_number);
    });
  }

  Signals*         q = nullptr;
  asio::signal_set set_;
};

Signals::Signals(int sig_number, Object* parent)
    : Object(parent), d(new Private(this, parent, sig_number)) {
}

Signals::~Signals() = default;

void Signals::Start() {
  d->SetUp();
}

ErrorCode Signals::AddSignal(int sig_number) {
  ErrorCode ec;

  d->set_.add(sig_number, ec);

  return ec;
}

ErrorCode Signals::RemoveSignal(int sig_number) {
  ErrorCode ec;
  d->set_.remove(sig_number, ec);
  return ec;
}

ErrorCode Signals::Cancel() {
  ErrorCode ec;
  d->set_.cancel(ec);

  return ec;
}
}  // namespace spiderweb
