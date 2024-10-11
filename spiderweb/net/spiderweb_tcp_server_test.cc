#include "spiderweb/net/spiderweb_tcp_server.h"

#include "asio.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "spiderweb/core/spiderweb_eventloop.h"
#include "spiderweb/core/spiderweb_notify_spy.h"
#include "spiderweb/net/private/spiderweb_tcp_server_private.h"
#include "spiderweb/net/spiderweb_tcp_socket.h"

class tcp_acceptor {
 public:
  using AcceptHandler = std::function<void(const asio::error_code &)>;

  MOCK_METHOD(void, async_accept, (asio::ip::tcp::socket & socket, AcceptHandler &&handler));
};

struct MockTcpServer {
  explicit MockTcpServer(spiderweb::EventLoop &loop)
      : loop(loop), d(std::make_shared<spiderweb::net::TcpServer::Private>(6666, &server)) {
  }

  ~MockTcpServer() {
    d->q = nullptr;
  }

  spiderweb::EventLoop                               &loop;
  tcp_acceptor                                        acceptor;
  spiderweb::net::TcpServer                           server{1234};
  std::shared_ptr<spiderweb::net::TcpServer::Private> d;
};

TEST(spiderweb_tcp_server, Accept) {
  using ::testing::_;

  spiderweb::EventLoop loop;
  MockTcpServer        mocker(loop);

  spiderweb::NotifySpy spy(&mocker.server, &spiderweb::net::TcpServer::InComingConnection);

  EXPECT_CALL(mocker.acceptor, async_accept(_, _))
      .WillOnce([&](asio::ip::tcp::socket & /*socket*/, tcp_acceptor::AcceptHandler &&handler) {
        loop.QueueTask([handler = std::move(handler)]() { handler(asio::error_code()); });
      })
      .WillOnce([&](asio::ip::tcp::socket & /*socket*/, tcp_acceptor::AcceptHandler &&handler) {});

  mocker.d->StartAccept(mocker.acceptor);

  spy.Wait();
  EXPECT_EQ(spy.Count(), 1);

  spiderweb::net::TcpSocket *client = std::get<0>(spy.LastResult<spiderweb::net::TcpSocket *>());
  assert(client);
  delete client;
}

TEST(spiderweb_tcp_server, AcceptFailed) {
  using ::testing::_;

  spiderweb::EventLoop loop;
  MockTcpServer        mocker(loop);

  spiderweb::NotifySpy spy(&mocker.server, &spiderweb::net::TcpServer::InComingConnection);

  EXPECT_CALL(mocker.acceptor, async_accept(_, _))
      .WillOnce([&](asio::ip::tcp::socket & /*socket*/, tcp_acceptor::AcceptHandler &&handler) {
        loop.QueueTask([handler = std::move(handler)]() { handler(asio::error::address_in_use); });
      })
      .WillOnce([&](asio::ip::tcp::socket & /*socket*/, tcp_acceptor::AcceptHandler &&handler) {});

  mocker.d->StartAccept(mocker.acceptor);

  spy.Wait(500);
  EXPECT_EQ(spy.Count(), 0);
}

TEST(spiderweb_tcp_server, SafeDelete) {
  using ::testing::_;

  spiderweb::EventLoop loop;
  auto                *server = new spiderweb::net::TcpServer(1234);

  server->ListenAndServ();
  delete server;
}

TEST(spiderweb_tcp_server, Stop) {
  using ::testing::_;

  spiderweb::EventLoop loop;
  auto                *server = new spiderweb::net::TcpServer(1234);

  spiderweb::NotifySpy spy(server, &spiderweb::net::TcpServer::Stopped);

  server->ListenAndServ();

  loop.RunAfter(100, [&]() { server->Stop(); });

  spy.Wait();
  EXPECT_EQ(spy.Count(), 1);
  spy.Clear();

  server->ListenAndServ();
  loop.RunAfter(100, [&]() { server->Stop(); });
  spy.Wait(100000);
  EXPECT_EQ(spy.Count(), 1);
}
