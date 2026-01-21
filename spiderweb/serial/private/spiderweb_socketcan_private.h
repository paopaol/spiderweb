#ifndef SPIDERWEB_SOCKETCAN_PRIVATE_H
#define SPIDERWEB_SOCKETCAN_PRIVATE_H

#include "absl/memory/memory.h"
#include "asio.hpp"
#include "canary/filter.hpp"
#include "canary/frame_header.hpp"
#include "canary/interface_index.hpp"
#include "canary/isotp.hpp"
#include "canary/raw.hpp"
#include "canary/socket_options.hpp"
#include "spdlog/spdlog.h"
#include "spiderweb/core/spiderweb_eventloop.h"
#include "spiderweb/io/spiderweb_buffer.h"
#include "spiderweb/serial/spiderweb_socketcan.h"
#include "spiderweb/type/spiderweb_binary_view.h"

namespace spiderweb {
namespace serial {
class SocketCan::Private {
 public:
  explicit Private(SocketCan* qq) : q(qq) {
  }

  template <typename AsyncStream, typename Handler>
  void Open(AsyncStream&, const std::error_code& ec, Handler&& handler) {
    q->QueueTask([ec, h = std::forward<Handler>(handler)]() { h(ec); });
  }

  inline const char* Description() const {
    return "SocketCan";
  }

  template <typename AsyncStream, typename Handler>
  void Read(AsyncStream& stream, const asio::mutable_buffers_1& buffer, Handler&& handler) {
    stream.async_receive(buffer, std::forward<Handler>(handler));
  }

  template <typename AsyncStream, typename Handler>
  void Write(AsyncStream& stream, const asio::mutable_buffers_1& buffers, Handler&& handler) {
    stream.async_send(buffers, std::forward<Handler>(handler));
  }

  void Readden(const io::BufferReader& reader) {
    if (reader.Len() < sizeof(CanFrame)) {
      return;
    }

    CanFrame f;
    auto*    p = reinterpret_cast<char*>(&f);
    reader.Read(p, sizeof(f));

    spider_emit q->FrameRead(f);
  }

  void Written(std::size_t size) {
    spider_emit q->BytesWritten(size);
  }

  void Error(const asio::error_code& ec) {
    spider_emit Object::Emit(q, &SocketCan::Error, ec);
  }

  template <typename AsyncStream>
  void Close(AsyncStream& stream) {
    std::error_code ec;
    stream.close(ec);
  }

  void OpenSuccess() {
    spider_emit Object::Emit(q, &SocketCan::OpenSuccess);
  }

  void OpenFailed(const asio::error_code& ec) {
    spider_emit Object::Emit(q, &SocketCan::OpenError, ec);
  }

  SocketCan*                             q = nullptr;
  SocketCanType                          type;
  std::unique_ptr<canary::raw::socket>   raw_sock;
  std::unique_ptr<canary::isotp::socket> isotp_sock;
};

}  // namespace serial
}  // namespace spiderweb

#endif
