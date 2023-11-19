#include "spiderweb/serial/spiderweb_socketcan.h"

#include "absl/memory/memory.h"
#include "core/internal/asio_cast.h"
#include "io/private/spiderweb_stream_private.h"
#include "private/spiderweb_socketcan_private.h"
#include "spdlog/fmt/bin_to_hex.h"
#include "spiderweb/core/internal/thread_check.h"

namespace spiderweb {
namespace serial {
SocketCan::SocketCan(Object *parent)
    : Object(parent), d(std::make_shared<io::IoPrivate<Private>>(this)) {
}

SocketCan::~SocketCan() {
  SPIDERWEB_CALL_THREAD_CHECK(SocketCan::~SocketCan);
  d->impl.q = nullptr;
}

void SocketCan::Open(const std::string &can, SocketCanType type) {
  SPIDERWEB_CALL_THREAD_CHECK(SocketCan::Open);

  d->impl.type = type;

  canary::filter filter;
  filter.negation(true);
  filter.remote_transmission(true);
  filter.extended_format(true);

  std::error_code ec;
  const auto      idx = canary::get_interface_index(can, ec);
  switch (d->impl.type) {
    case SocketCanType::kRaw: {
      auto const ep = canary::raw::endpoint{idx};
      d->impl.raw_sock = absl::make_unique<canary::raw::socket>(AsioService(ownerEventLoop()), ep);
      d->impl.raw_sock->set_option(canary::filter_if_any(&filter, 1));
      d->StartOpenEx(*d->impl.raw_sock, false, ec);
    } break;
    case SocketCanType::kIsotp: {
      auto const ep = canary::isotp::endpoint{idx};
      d->impl.isotp_sock =
          absl::make_unique<canary::isotp::socket>(AsioService(ownerEventLoop()), ep);
      d->impl.isotp_sock->set_option(canary::filter_if_any(&filter, 1));
      d->StartOpenEx(*d->impl.isotp_sock, false, ec);
    } break;
  }
}

void SocketCan::Close() {
  SPIDERWEB_CALL_THREAD_CHECK(SocketCan::Close);

  switch (d->impl.type) {
    case SocketCanType::kRaw: {
      d->Close(*d->impl.raw_sock);
    } break;
    case SocketCanType::kIsotp: {
      d->Close(*d->impl.isotp_sock);
    } break;
  }
}

void SocketCan::Write(const CanFrame &frame) {
  SPIDERWEB_CALL_THREAD_CHECK(SocketCan::Write);

  const auto b = canary::net::buffer(&frame, sizeof(frame));
  spdlog::debug("{:n}",
                spdlog::to_hex(static_cast<const char *>(b.begin()->data()),
                               static_cast<const char *>(b.begin()->data()) + b.begin()->size()));

  switch (d->impl.type) {
    case SocketCanType::kRaw: {
      d->StartWrite(*d->impl.raw_sock, static_cast<const uint8_t *>(b.data()), b.size());
    } break;
    case SocketCanType::kIsotp: {
      d->StartWrite(*d->impl.isotp_sock, static_cast<const uint8_t *>(b.data()), b.size());
    } break;
  }
}

}  // namespace serial
}  // namespace spiderweb
