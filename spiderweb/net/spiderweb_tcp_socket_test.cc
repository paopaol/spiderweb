#include "spiderweb/net/spiderweb_tcp_socket.h"

#include "gtest/gtest.h"
#include "spiderweb/core/spiderweb_event_spy.h"
#include "spiderweb/core/spiderweb_eventloop.h"
#include "spiderweb/net/private/spiderweb_tcp_socket_private.h"
#include "spiderweb/test/spiderweb_test_stream.hpp"

class MockSocket : public spiderweb::Object {
 public:
  explicit MockSocket(spiderweb::EventLoop &loop, spiderweb::Object *parent = nullptr)
      : spiderweb::Object(parent),
        event_loop(loop),
        stream(loop.IoService()),
        d(std::make_shared<spiderweb::net::TcpSocket::Private>(&socket)) {
  }

  ~MockSocket() override {
    d->q = nullptr;
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
        .WillRepeatedly([this](const asio::const_buffers_1     &buffers,
                               const test_stream::WriteHandler &handler) {});
  }

  spiderweb::EventLoop                               &event_loop;
  test_stream                                         stream;
  spiderweb::net::TcpSocket                           socket;
  asio::ip::tcp::endpoint                             endpoint;
  std::shared_ptr<spiderweb::net::TcpSocket::Private> d;
};

TEST(spiderweb_tcp_socket, ConnectToHostSuccess) {
  spiderweb::EventLoop loop;
  MockSocket           mocker(loop);

  spiderweb::EventSpy spy(&mocker.socket, &spiderweb::net::TcpSocket::ConnectionEstablished);

  mocker.SimulateConnectSuccess();

  mocker.d->StartConnect(mocker.stream, mocker.endpoint);
  spy.Wait();
  EXPECT_EQ(spy.Count(), 1);
}

TEST(spiderweb_tcp_socket, ConnectToHostFailed_ConnectionRefused) {
  spiderweb::EventLoop loop;
  MockSocket           mocker(loop);

  spiderweb::EventSpy spy(&mocker.socket, &spiderweb::net::TcpSocket::Error);

  mocker.SimulateConnectFailed(asio::error::connection_refused);

  mocker.d->StartConnect(mocker.stream, mocker.endpoint);
  spy.Wait();
  EXPECT_EQ(spy.Count(), 1);
  auto error_code = std::get<0>(spy.LastResult<std::error_code>());
  EXPECT_EQ(error_code, asio::error::connection_refused);
  std::cout << error_code.message() << std::endl;
}

TEST(spiderweb_tcp_socket, CloseConnectingSocket) {
  spiderweb::EventLoop loop;
  MockSocket           mocker(loop);

  spiderweb::EventSpy spy(&mocker.socket, &spiderweb::net::TcpSocket::Error);

  mocker.SimulateConnectFailed(asio::error::connection_aborted);

  mocker.d->StartConnect(mocker.stream, mocker.endpoint);
  mocker.d->CloseSocket();
  spy.Wait();
  EXPECT_EQ(spy.Count(), 1);
  auto error_code = std::get<0>(spy.LastResult<std::error_code>());
  EXPECT_EQ(error_code, asio::error::connection_aborted);
  std::cout << error_code.message() << std::endl;
}

TEST(spiderweb_tcp_socket, WriteSuccess) {
  using ::testing::_;

  spiderweb::EventLoop loop;
  MockSocket           mocker(loop);

  spiderweb::EventSpy spy(&mocker.socket, &spiderweb::net::TcpSocket::BytesWritten);
  spiderweb::EventSpy on_conn(&mocker.socket, &spiderweb::net::TcpSocket::ConnectionEstablished);

  mocker.SimulateConnectSuccess();
  mocker.SimulateWriteSuccess();

  mocker.d->StartConnect(mocker.stream, mocker.endpoint);
  on_conn.Wait();
  EXPECT_EQ(on_conn.Count(), 1);
  mocker.d->StartWrite(mocker.stream, "1234");

  spy.Wait();
  EXPECT_TRUE(spy.Count() == 1);
  std::cout << std::get<0>(spy.LastResult<std::size_t>()) << std::endl;
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
  mocker->SimulatePendingWrite();

  mocker->d->StartConnect(mocker->stream, mocker->endpoint);
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
