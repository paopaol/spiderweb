#include "spiderweb/net/spiderweb_tcp_server.h"

#include <vector>

#include "gtest/gtest.h"
#include "spiderweb/core/spiderweb_eventloop.h"
#include "spiderweb/core/spiderweb_notify_spy.h"
#include "spiderweb/core/spiderweb_object.h"
#include "spiderweb/net/spiderweb_tcp_socket.h"
#include "spiderweb/spiderweb_check.h"

TEST(spiderweb_tcp_server, Accept) {
  spiderweb::EventLoop loop;

  /**
   * @brief 端口传 0 由内核分配，避免用例之间抢端口
   */
  spiderweb::net::TcpServer server(0);
  ASSERT_FALSE(server.ListenAndServ("127.0.0.1"));
  ASSERT_NE(server.LocalPort(), 0);

  spiderweb::NotifySpy spy(&server, &spiderweb::net::TcpServer::InComingConnection);

  spiderweb::net::TcpSocket client;
  client.ConnectToHost("127.0.0.1", server.LocalPort());

  spy.Wait();
  ASSERT_EQ(spy.Count(), 1);

  auto* accepted = std::get<0>(spy.LastResult<spiderweb::net::TcpSocket*>());
  ASSERT_NE(accepted, nullptr);

  /**
   * @brief accept 出来的连接必须可用：客户端写，服务端能收到
   */
  spiderweb::NotifySpy received(accepted, &spiderweb::net::TcpSocket::BytesRead);
  client.Write(std::vector<uint8_t>{'p', 'i', 'n', 'g'});

  received.Wait();
  EXPECT_EQ(received.Count(), 1);

  accepted->DeleteLater();

  /**
   * @brief 停掉 listener：取消重新挂起的 accept，
   * 并把 accept 过程中的 socket / accepted 一起 DeleteLater 掉
   */
  server.Stop();
  loop.Quit();
  loop.ExecEx();
}

TEST(spiderweb_tcp_server, ListenAndServFailed) {
  spiderweb::EventLoop loop;
  auto*                server = new spiderweb::net::TcpServer(2);

  auto ec = server->ListenAndServ();
  EXPECT_TRUE(ec) << ec.FormatedMessage();

  server->Stop();
  server->DeleteLater();
  loop.Quit();
  loop.ExecEx();
}

TEST(spiderweb_tcp_server, SafeDelete) {
  spiderweb::EventLoop loop;
  auto*                server = new spiderweb::net::TcpServer(1234);

  server->ListenAndServ();

  server->Stop();
  server->DeleteLater();
  loop.Quit();
  loop.ExecEx();
}

TEST(spiderweb_tcp_server, Stop) {
  spiderweb::EventLoop loop;
  auto*                server = new spiderweb::net::TcpServer(1234);

  spiderweb::NotifySpy spy(server, &spiderweb::net::TcpServer::Stopped);
  spiderweb::NotifySpy incoming(server, &spiderweb::net::TcpServer::InComingConnection);

  server->ListenAndServ();

  loop.RunAfter(100, [&]() { server->Stop(); });

  spy.Wait();
  EXPECT_EQ(spy.Count(), 1);
  spy.Clear();

  server->ListenAndServ();
  loop.RunAfter(100, [&]() { server->Stop(); });
  spy.Wait();
  EXPECT_EQ(spy.Count(), 1);

  /**
   * @brief Stop 关闭 acceptor 会取消挂起的 accept，不会产生新连接
   */
  EXPECT_EQ(incoming.Count(), 0);

  SPIDERWEB_VERIFY(1 == 1, return);
  SPIDERWEB_VERIFY(1 == 1, spy.Clear(); return);

  server->DeleteLater();
  loop.Quit();
  loop.ExecEx();
}
