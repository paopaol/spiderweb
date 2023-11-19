#ifndef SPIDERWEB_STREAM_PRIVATE_H
#define SPIDERWEB_STREAM_PRIVATE_H

#include <type_traits>

#include "asio.hpp"
#include "spdlog/spdlog.h"
#include "spiderweb/core/spiderweb_eventloop.h"
#include "spiderweb/core/spiderweb_traits.h"
#include "spiderweb/io/spiderweb_buffer.h"

namespace spiderweb {
namespace io {

/**
 * @brief an asio async stream delegate.
 *
 * It mainly adds a high-level encapsulation to asio's async stream, so that users don't have to
 *
 * care about the size adjustment of the send buffer and the receive buffer. At the same time,
 *
 * events such as sending, receiving, and errors will be notified to the user. Since the template is
 *
 * used here, it also means that the specific operation of the stream needs to be implemented by the
 *
 * template user. For example, tcpsocket needs to call async_read_some, async_write_some and so on.
 *
 * Since the processing flow of asio's tcp udp, pipe serial, etc. is basically the same, this
 *
 * unified use will be extracted.
 *
 */
template <typename IoImpl>
class IoPrivate : public std::enable_shared_from_this<IoPrivate<IoImpl>> {
 public:
  template <typename Delegated>
  explicit IoPrivate(Delegated *qq) : impl(qq) {
  }

  ~IoPrivate() = default;

  inline bool IsStopped() const {
    return stopped;
  }

  template <typename AsyncStream, typename... Args>
  void StartOpen(AsyncStream &stream, Args &&...args) {
    if (stream.is_open()) {
      spdlog::warn("{}({}) alreay connected or connecting", impl.Description(), fmt::ptr(impl.q));
      return;
    }

    stopped = false;
    auto self = this->shared_from_this();

    impl.Open(stream, std::forward<Args>(args)...,
              [this, self, &stream](const asio::error_code &ec) { HandleOpen(stream, ec); });
  }

  template <typename AsyncStream, typename... Args>
  void StartOpenEx(AsyncStream &stream, bool check, Args &&...args) {
    if (check && stream.is_open()) {
      spdlog::warn("{}({}) alreay connected or connecting", impl.Description(), fmt::ptr(impl.q));
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

    const auto buffer = asio::buffer(recv_buffer.beginWrite(), recv_buffer.leftSpace());

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

  template <typename AsyncStream, typename Byte,
            typename = typename std::enable_if<IsByte<Byte>::value>::type>
  void StartWrite(AsyncStream &stream, const std::vector<Byte> &data) {
    StartWrite(stream, static_cast<const uint8_t *>(data.data()), data.size());
  }

  template <typename AsyncStream>
  void StartWrite(AsyncStream &stream, const std::string &data) {
    StartWrite(stream, reinterpret_cast<const uint8_t *>(data.data()), data.size());
  }

  template <typename AsyncStream>
  void StartWrite(AsyncStream &stream, const uint8_t *data, std::size_t size) {
    if (stopped) {
      spdlog::warn("{}({}) stopped", impl.Description(), fmt::ptr(impl.q));
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
      spdlog::warn("{}({}) stopped", impl.Description(), fmt::ptr(impl.q));
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

}  // namespace io
}  // namespace spiderweb

#endif
