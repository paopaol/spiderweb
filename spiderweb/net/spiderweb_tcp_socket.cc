#include "spiderweb_tcp_socket.h"

#include <thread>

#include "asio.hpp"
#include "core/spiderweb_eventloop.h"
#include "spiderweb/io/spiderweb_buffer.h"

namespace spiderweb {
namespace net {

#define THREAD_CHECK(name) \
  assert(std::this_thread::get_id() == ThreadId() && "can not call " #name " In other thread")

class TcpSocket::Private {
 public:
  explicit Private(TcpSocket *qq) : q(qq), socket(qq->ownerEventLoop()->IoService()) {
  }

  void StartConnect(const asio::ip::tcp::endpoint &endpoint) {
    socket.async_connect(endpoint, std::bind(&Private::HandleConnect, this, std::placeholders::_1));
  }

  void HandleConnect(const asio::error_code &ec) {
    if (stopped) {
      return;
    }

    if (ec) {
      socket.close();
      spider_emit q->Error(ec);
      return;
    }
    StartRead();
    spider_emit q->ConnectionEstablished();
  }

  void StartRead() {
    if (stopped) {
      return;
    }

    /**
     * @brief
     *
     * Since we use our own custom buffer as the receive buffer. There are many reasons to use your
     *
     * own buffer. For example, it can be easily read and written, and the user does not have to
     *
     * care about the management of the buffer size. But asio has its own asynchronous read and
     *
     * write method, although we can use a buffer of constant size to temporarily receive, and then
     *
     * copy the data to our own buffer every time we receive data. Looks fine, just one more copy.
     *
     * So we can avoid it as much as possible.
     *
     * The Write process of io::Buffer is:
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

    socket.async_read_some(buffer, [this](const asio::error_code ec, std::size_t n) {
      if (stopped) {
        return;
      }

      if (ec) {
        Stop();
        spider_emit q->Error(ec);
        return;
      }
      recv_buffer.CommitWrite(n);

      StartRead();
      spider_emit q->BytesRead(io::BufferReader(recv_buffer));
    });
  }

  void StartWrite(const uint8_t *data, std::size_t size) {
    if (stopped) {
      return;
    }

    send_buffer.Write(reinterpret_cast<const char *>(data), size);

    asio::async_write(socket, asio::buffer(send_buffer.lastRead(), send_buffer.Len()),
                      asio::transfer_all(),
                      std::bind(&Private::HanleWrite, this, std::placeholders::_1));
  }

  void HanleWrite(const asio::error_code &ec) {
    if (stopped) {
      return;
    }

    if (ec) {
      Stop();
      spider_emit q->Error(ec);
      return;
    }

    char       *ptr = nullptr;
    std::size_t written = send_buffer.Len();
    send_buffer.ZeroCopyRead(ptr, send_buffer.Len());

    spider_emit q->BytesWritten(written);
  }

  void Stop() {
    stopped = true;
    CloseSocket();
  }

  void CloseSocket() {
    if (close_called) {
      return;
    }

    socket.close();
    close_called = true;
  }

  TcpSocket               *q = nullptr;
  bool                     stopped = false;
  asio::ip::tcp::socket    socket;
  io::Buffer               recv_buffer;
  io::Buffer               send_buffer;
  static const std::size_t kSpaceGrowSize = 8192;
  bool                     close_called = false;
};

TcpSocket::TcpSocket(Object *parent) : Object(parent), d(new Private(this)) {
}

TcpSocket::~TcpSocket() = default;

void TcpSocket::ConnectToHost(const std::string &ip, uint16_t port) {
  THREAD_CHECK(TcpSocket::ConnectToHost);

  asio::ip::tcp::endpoint endpoint(asio::ip::make_address(ip), port);

  d->StartConnect(endpoint);
}

void TcpSocket::DisConnectFromHost() {
  THREAD_CHECK(TcpSocket::DisConnectFromHost);

  d->CloseSocket();
}

bool TcpSocket::IsClosed() const {
  /**
   * @brief note that default construct IsClosed is false;
   */
  return d->stopped;
}

void TcpSocket::Write(const uint8_t *data, std::size_t size) {
  THREAD_CHECK(TcpSocket::Write);

  d->StartWrite(data, size);
}

}  // namespace net
}  // namespace spiderweb
