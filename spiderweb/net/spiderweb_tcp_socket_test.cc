#include "spiderweb/net/spiderweb_tcp_socket.h"

#include "core/internal/asio_cast.h"
#include "fmt/ranges.h"
#include "gtest/gtest.h"
#include "spiderweb/core/spiderweb_event_spy.h"
#include "spiderweb/core/spiderweb_eventloop.h"
#include "spiderweb/io/private/spiderweb_stream_private.h"
#include "spiderweb/net/private/spiderweb_tcp_socket_private.h"
#include "spiderweb/test/spiderweb_test_stream.hpp"

using TestIoPrivate = spiderweb::io::IoPrivate<spiderweb::net::TcpSocket::Private>;

class MockSocket : public spiderweb::Object {
 public:
  explicit MockSocket(spiderweb::EventLoop &loop, spiderweb::Object *parent = nullptr)
      : spiderweb::Object(parent),
        event_loop(loop),
        stream(spiderweb::AsioService(&loop)),
        d(std::make_shared<TestIoPrivate>(&socket)) {
  }

  ~MockSocket() override {
    d->impl.q = nullptr;
  }

  void SimulateConnectSuccess() {
    using ::testing::_;

    ON_CALL(stream, is_open()).WillByDefault(testing::Return(false));
    ON_CALL(stream, async_connect(_, _))
        .WillByDefault(
            [this](const asio::ip::tcp::endpoint &, const test_stream::ConnectHandler &handler) {
              event_loop.QueueTask([handler]() { handler(asio::error_code()); });
            });

    EXPECT_CALL(stream, is_open());
    EXPECT_CALL(stream, async_connect(_, _));
  }

  void ShouldCallReadWhenConnectSuccess() {
    using ::testing::_;
    EXPECT_CALL(stream, async_read_some(_, _));
  }

  void SimulateConnectFailed(const asio::error_code &ec) {
    using ::testing::_;

    EXPECT_CALL(stream, is_open()).WillOnce(testing::Return(false));
    EXPECT_CALL(stream, async_connect(_, _))
        .WillOnce([&](const asio::ip::tcp::endpoint &, const test_stream::ConnectHandler &handler) {
          event_loop.QueueTask([&, handler]() { handler(ec); });
        });
  }

  void SimulateWriteSuccess() {
    using ::testing::_;

    EXPECT_CALL(stream, async_write_some(_, _))
        .WillRepeatedly(
            [this](const asio::const_buffers_1 &buffers, const test_stream::WriteHandler &handler) {
              event_loop.QueueTask(
                  [handler, buffers]() { handler(asio::error_code(), buffers.size()); });
            });
  }

  void SimulatePendingWrite() {
    using ::testing::_;

    EXPECT_CALL(stream, async_write_some(_, _))
        .WillRepeatedly(
            [](const asio::const_buffers_1 &buffers, const test_stream::WriteHandler &handler) {});
  }

  spiderweb::EventLoop          &event_loop;
  test_stream                    stream;
  spiderweb::net::TcpSocket      socket;
  asio::ip::tcp::endpoint        endpoint;
  std::shared_ptr<TestIoPrivate> d;
};

TEST(spiderweb_tcp_socket, ConnectToHostSuccess) {
  spiderweb::EventLoop loop;
  MockSocket           mocker(loop);

  spiderweb::EventSpy spy(&mocker.socket, &spiderweb::net::TcpSocket::ConnectionEstablished);

  mocker.SimulateConnectSuccess();
  mocker.ShouldCallReadWhenConnectSuccess();

  mocker.d->StartOpen(mocker.stream, mocker.endpoint);
  spy.Wait();
  EXPECT_EQ(spy.Count(), 1);
}

TEST(spiderweb_tcp_socket, ConnectToHostFailed_ConnectionRefused) {
  spiderweb::EventLoop loop;
  MockSocket           mocker(loop);

  spiderweb::EventSpy spy(&mocker.socket, &spiderweb::net::TcpSocket::Error);

  mocker.SimulateConnectFailed(asio::error::connection_refused);

  mocker.d->StartOpen(mocker.stream, mocker.endpoint);
  spy.Wait();
  EXPECT_EQ(spy.Count(), 1);
  auto error_code = std::get<0>(spy.LastResult<std::error_code>());
  EXPECT_EQ(error_code, asio::error::connection_refused);
  std::cout << error_code.message() << '\n';
}

TEST(spiderweb_tcp_socket, CloseConnectingSocket) {
  spiderweb::EventLoop loop;
  MockSocket           mocker(loop);

  spiderweb::EventSpy spy(&mocker.socket, &spiderweb::net::TcpSocket::Error);

  mocker.SimulateConnectFailed(asio::error::connection_aborted);

  mocker.d->StartOpen(mocker.stream, mocker.endpoint);
  mocker.d->Close(mocker.stream);
  spy.Wait();
  EXPECT_EQ(spy.Count(), 1);
  auto error_code = std::get<0>(spy.LastResult<std::error_code>());
  EXPECT_EQ(error_code, asio::error::connection_aborted);
  std::cout << error_code.message() << '\n';
}

TEST(spiderweb_tcp_socket, WriteSuccess) {
  using ::testing::_;

  spiderweb::EventLoop loop;
  MockSocket           mocker(loop);

  spiderweb::EventSpy spy(&mocker.socket, &spiderweb::net::TcpSocket::BytesWritten);
  spiderweb::EventSpy on_conn(&mocker.socket, &spiderweb::net::TcpSocket::ConnectionEstablished);

  mocker.SimulateConnectSuccess();
  mocker.ShouldCallReadWhenConnectSuccess();
  mocker.SimulateWriteSuccess();

  mocker.d->StartOpen(mocker.stream, mocker.endpoint);
  on_conn.Wait();
  EXPECT_EQ(on_conn.Count(), 1);
  mocker.d->StartWrite(mocker.stream, "1234");

  spy.Wait();
  EXPECT_TRUE(spy.Count() == 1);
  std::cout << std::get<0>(spy.LastResult<std::size_t>()) << '\n';
}

TEST(spiderweb_tcp_socket, MultiWrite) {
  using ::testing::_;

  spiderweb::EventLoop loop;
  MockSocket           mocker(loop);

  spiderweb::EventSpy spy(&mocker.socket, &spiderweb::net::TcpSocket::BytesWritten);
  spiderweb::EventSpy on_conn(&mocker.socket, &spiderweb::net::TcpSocket::ConnectionEstablished);

  mocker.SimulateConnectSuccess();
  mocker.ShouldCallReadWhenConnectSuccess();
  EXPECT_CALL(mocker.stream, async_write_some(_, _))
      .WillOnce(
          [&](const asio::const_buffers_1 &buffers, const test_stream::WriteHandler &handler) {
            // first call only write "1234"
            EXPECT_TRUE(std::memcmp(buffers.data(), "1234", 4) == 0);
            EXPECT_EQ(buffers.size(), 4);
            loop.QueueTask([handler, buffers]() { handler(asio::error_code(), buffers.size()); });
          })
      .WillOnce(
          [&](const asio::const_buffers_1 &buffers, const test_stream::WriteHandler &handler) {
            // second call will write remaing data  "5678abcd"
            EXPECT_TRUE(std::memcmp(buffers.data(), "5678abcd", 4) == 0);
            EXPECT_EQ(buffers.size(), 8);
            loop.QueueTask([handler, buffers]() { handler(asio::error_code(), buffers.size()); });
          });

  mocker.d->StartOpen(mocker.stream, mocker.endpoint);
  on_conn.Wait();
  EXPECT_EQ(on_conn.Count(), 1);

  /**
   * @brief call StartWrite 3 times, but async_write_some ony call twice
   */
  mocker.d->StartWrite(mocker.stream, "1234");
  mocker.d->StartWrite(mocker.stream, "5678");
  mocker.d->StartWrite(mocker.stream, "abcd");

  spy.Wait();
  EXPECT_TRUE(spy.Count() == 2);
}

TEST(spiderweb_tcp_socket, ReadSuccess) {
  using ::testing::_;

  spiderweb::EventLoop loop;
  MockSocket           mocker(loop);

  spiderweb::EventSpy spy(&mocker.socket, &spiderweb::net::TcpSocket::BytesRead);

  mocker.SimulateConnectSuccess();
  EXPECT_CALL(mocker.stream, async_read_some(_, _))
      .WillOnce(
          [&](const asio::mutable_buffers_1 &buffers, const test_stream::ReadHandler &handler) {
            asio::buffer_copy(buffers, asio::buffer("hello", 5));
            const auto size = 5;
            loop.QueueTask([handler]() { handler(asio::error_code(), size); });
          })
      .WillOnce(
          [&](const asio::mutable_buffers_1 &buffers, const test_stream::ReadHandler &handler) {
            asio::buffer_copy(buffers, asio::buffer("world", 5));
            const auto size = 5;
            loop.QueueTask([handler]() { handler(asio::error_code(), size); });
          })
      .WillOnce(
          [&](const asio::mutable_buffers_1 &buffers, const test_stream::ReadHandler &handler) {});

  mocker.d->StartOpen(mocker.stream, mocker.endpoint);
  spy.Wait(500, 10);
  EXPECT_EQ(spy.Count(), 2);

  auto last = std::get<0>(spy.LastResult<spiderweb::io::BufferReader>());

  std::vector<char> data;
  last.Read(data);

  fmt::print("{}\n", data);
}

TEST(spiderweb_tcp_socket, ReadFailed) {
  using ::testing::_;

  spiderweb::EventLoop loop;
  MockSocket           mocker(loop);

  spiderweb::EventSpy spy(&mocker.socket, &spiderweb::net::TcpSocket::Error);

  mocker.SimulateConnectSuccess();
  EXPECT_CALL(mocker.stream, async_read_some(_, _))
      .WillOnce(
          [&](const asio::mutable_buffers_1 & /*buffers*/, const test_stream::ReadHandler &hander) {
            loop.QueueTask([hander]() { hander(asio::error::connection_aborted, 0); });
          });

  mocker.d->StartOpen(mocker.stream, mocker.endpoint);
  spy.Wait();

  EXPECT_EQ(spy.Count(), 1);
  EXPECT_TRUE(mocker.d->IsStopped());
}

TEST(spiderweb_tcp_socket, WriteClosedSocket) {
  spiderweb::EventLoop loop;

  spiderweb::net::TcpSocket socket;

  spiderweb::EventSpy spy(&socket, &spiderweb::net::TcpSocket::Error);

  std::vector<uint8_t> data{0x30, 0x31, 0x32, 0x33, 0x34};
  socket.Write(data.data(), data.size());

  spy.Wait(3000, 100000);
  ASSERT_EQ(spy.Count(), 0);

  loop.QueueTask([&]() { loop.Quit(); });

  loop.Exec();
}

TEST(spiderweb_tcp_socket, SafeDelete1) {
  spiderweb::EventLoop loop;

  auto               *socket = new spiderweb::net::TcpSocket;
  spiderweb::EventSpy spy(socket, &spiderweb::net::TcpSocket::Error);

  std::vector<uint8_t> data{0x30, 0x31, 0x32, 0x33, 0x34};

  /**
   * @brief the actual delete operation(it's Private) should called after all
   *
   * write operations are done
   */
  socket->ConnectToHost("127.0.0.1", 12345);
  socket->Write(data.data(), data.size());
  socket->DeleteLater();
  socket->Write(data.data(), data.size());
  socket->Write(data.data(), data.size());
  socket = nullptr;

  spy.Wait(3000, 100000);
  /**
   * @brief because DeleteLater will delete the socket,before all write
   * operations done,
   *
   * so, no any Error emited
   */
  EXPECT_EQ(spy.Count(), 0);

  loop.QueueTask([&]() { loop.Quit(); });

  loop.Exec();
}

TEST(spiderweb_tcp_socket, SafeDelete) {
  spiderweb::EventLoop loop;
  auto                *mocker = new MockSocket(loop);

  spiderweb::EventSpy spy(&mocker->socket, &spiderweb::net::TcpSocket::Error);

  std::vector<uint8_t> data{0x30, 0x31, 0x32, 0x33, 0x34};

  mocker->SimulateConnectSuccess();
  mocker->ShouldCallReadWhenConnectSuccess();
  mocker->SimulatePendingWrite();

  mocker->d->StartOpen(mocker->stream, mocker->endpoint);
  mocker->d->StartWrite(mocker->stream, "1234");
  mocker->DeleteLater();
  mocker->d->StartWrite(mocker->stream, "1234");
  mocker->d->StartWrite(mocker->stream, "1234");

  /**
   * @brief the actual delete operation(it's Private) should called after all
   *
   * write operations are done
   */

  spy.Wait(3000, 100000);
  /**
   * @brief because DeleteLater will delete the socket,before all write
   * operations done,
   *
   * so, no any Error emited
   */
  EXPECT_EQ(spy.Count(), 0);

  loop.QueueTask([&]() { loop.Quit(); });

  loop.Exec();
}
