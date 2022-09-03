#ifndef SPIDERWEB_STREAM_PRIVATE_H
#define SPIDERWEB_STREAM_PRIVATE_H

#include "asio.hpp"
#include "core/spiderweb_eventloop.h"
#include "io/spiderweb_buffer.h"
#include "net/spiderweb_tcp_socket.h"
#include "spdlog/spdlog.h"

namespace spiderweb {
namespace net {

template <typename IoImpl>
class IoPrivate : public std::enable_shared_from_this<IoPrivate<IoImpl>> {
 public:
  explicit IoPrivate(TcpSocket *qq) : impl(qq) {
  }

  ~IoPrivate() = default;

  inline bool IsStopped() const {
    return stopped;
  }

  template <typename AsyncStream, typename... Args>
  void StartOpen(AsyncStream &stream, Args &&...args) {
    if (stream.is_open()) {
      spdlog::warn("TcpSocket({}) alreay connected or connecting", fmt::ptr(impl.q));
      return;
    }

    stopped = false;
    auto self = this->shared_from_this();

    impl.Open(stream, std::forward<Args>(args)...,
              [this, self, &stream](const asio::error_code &ec) { HandleOpen(stream, ec); });
  }

  template <typename AsyncStream>
  void HandleOpen(AsyncStream &stream, const asio::error_code &ec) {
    if (stopped) {
      return;
    }

    if (ec) {
      Stop(stream);
      /**
       * @brief we use Object::Emit here, beause maybe `q` has gone.
       */
      impl.Error(ec);
      return;
    }
    StartRead(stream);
    impl.OpenSuccess();
  }

  template <typename AsyncStream>
  void StartRead(AsyncStream &stream) {
    if (stopped) {
      return;
    }

    /**
     * @brief
     *
     * Since we use our own custom buffer as the receive buffer. There are many
     *
     * reasons to use your own buffer. For example, it can be easily read and written,
     *
     * and the user does not have to care about the management of the buffer size. But
     *
     * asio has its own asynchronous read and write method, although we can use a buffer
     *
     * of constant size to temporarily receive, and then copy the data to our own buffer
     *
     * every time we receive data. Looks fine, just one more copy. So we can avoid it
     *
     * as much as possible. The Write process of @ref io::Buffer is:
     *
     * 1. PrepareWrite
     *
     * 2. Copy
     *
     * 3. Commit
     *
     * Therefore, in StartRead, we separate these three steps and execute them.
     */
    if (recv_buffer.leftSpace() < kSpaceGrowSize) {
      recv_buffer.PrepareWrite(kSpaceGrowSize);
    }

    const auto &buffer = asio::buffer(recv_buffer.beginWrite(), recv_buffer.leftSpace());

    auto self = this->shared_from_this();
    impl.Read(stream, buffer, [this, self, &stream](const asio::error_code &ec, std::size_t n) {
      if (stopped) {
        return;
      }

      if (ec) {
        Stop(stream);
        /**
         * @brief we use Object::Emit here, beause maybe `q` has gone.
         */
        impl.Error(ec);
        return;
      }

      recv_buffer.CommitWrite(n);
      StartRead(stream);
      impl.Readden(io::BufferReader(this->recv_buffer));
    });
  }

  template <typename AsyncStream>
  void StartWrite(AsyncStream &stream, const std::vector<uint8_t> &data) {
    StartWrite(stream, data.data(), data.size());
  }

  template <typename AsyncStream>
  void StartWrite(AsyncStream &stream, const std::string &data) {
    StartWrite(stream, reinterpret_cast<const uint8_t *>(data.data()), data.size());
  }

  template <typename AsyncStream>
  void StartWrite(AsyncStream &stream, const uint8_t *data, std::size_t size) {
    if (stopped) {
      spdlog::warn("TcpSocket({}) stopped", fmt::ptr(impl.q));
      return;
    }

    const bool should_write = send_buffer.Len() == 0;

    send_buffer.Write(reinterpret_cast<const char *>(data), size);

    if (should_write) {
      StartWrite(stream);
    }
  }

  template <typename AsyncStream>
  void StartWrite(AsyncStream &stream) {
    if (stopped) {
      spdlog::warn("TcpSocket({}) stopped", fmt::ptr(impl.q));
      return;
    }

    if (send_buffer.Len() == 0) {
      return;
    }

    auto self = this->shared_from_this();
    impl.Write(stream, asio::buffer(send_buffer.lastRead(), send_buffer.Len()),
               [this, self, &stream](const asio::error_code &ec, std::size_t size) {
                 HandleWrite(stream, ec, size);
               });
  }

  template <typename AsyncStream>
  void HandleWrite(AsyncStream &stream, const asio::error_code &ec, std::size_t size) {
    if (stopped) {
      return;
    }

    if (ec) {
      Stop(stream);
      /**
       * @brief we use Object::Emit here, beause maybe `q` has gone.
       */
      impl.Error(ec);
      return;
    }

    char *ptr = nullptr;
    send_buffer.ZeroCopyRead(ptr, size);
    StartWrite(stream);

    impl.Written(size);
  }

  template <typename AsyncStream>
  inline void Stop(AsyncStream &stream) {
    stopped = true;
    Close(stream);
  }

  template <typename AsyncStream>
  inline void Close(AsyncStream &stream) {
    if (close_called) {
      return;
    }

    impl.Close(stream);
    close_called = true;
  }

  IoImpl                   impl;
  bool                     stopped = true;
  io::Buffer               recv_buffer;
  io::Buffer               send_buffer;
  static const std::size_t kSpaceGrowSize = 8192;
  bool                     close_called = false;
};

}  // namespace net
}  // namespace spiderweb

#endif
