#include "spiderweb/net/spiderweb_uds_server.h"

#include <ghc/filesystem.hpp>

#include "gtest/gtest.h"
#include "spiderweb/core/spiderweb_eventloop.h"
#include "spiderweb/core/spiderweb_notify_spy.h"
#include "spiderweb/core/spiderweb_object.h"
#include "spiderweb/net/spiderweb_uds_socket.h"

namespace spiderweb {

namespace {
constexpr const char* kSockPath = "/tmp/spiderweb_uds_test.sock";
}

TEST(spiderweb_uds_server, Accept) {
  EventLoop      loop;
  net::UdsServer server;

  auto ec = server.ListenAndServ(kSockPath);
  ASSERT_FALSE(ec) << ec.FormatedMessage();

  NotifySpy server_spy(&server, &net::UdsServer::InComingConnection);

  net::UdsSocket client;
  NotifySpy      client_spy(&client, &net::UdsSocket::ConnectionEstablished);
  client.ConnectTo(kSockPath);

  server_spy.Wait();
  EXPECT_EQ(server_spy.Count(), 1);

  net::UdsSocket* accepted = std::get<0>(server_spy.LastResult<net::UdsSocket*>());
  ASSERT_NE(accepted, nullptr);

  // 等待客户端连接确认
  client_spy.Wait();
  EXPECT_EQ(client_spy.Count(), 1);

  accepted->DeleteLater();
  server.Stop();

  loop.Quit();
  loop.ExecEx();
}

TEST(spiderweb_uds_server, ListenAndServFailed) {
  EventLoop      loop;
  net::UdsServer server;

  // 不存在的目录，bind 一定失败
  auto ec = server.ListenAndServ("/tmp/__no_such_dir__/test.sock");
  EXPECT_TRUE(ec) << ec.FormatedMessage();
}

TEST(spiderweb_uds_server, SafeDelete) {
  EventLoop loop;

  auto* server = new net::UdsServer();

  server->ListenAndServ("");

  server->Stop();
  server->DeleteLater();

  loop.Quit();
  loop.ExecEx();
}

TEST(spiderweb_uds_server, Stop) {
  EventLoop      loop;
  net::UdsServer server;

  auto ec = server.ListenAndServ(kSockPath);
  ASSERT_FALSE(ec) << ec.FormatedMessage();

  NotifySpy spy(&server, &net::UdsServer::Stopped);

  server.RunAfter(50, [&server]() { server.Stop(); });

  spy.Wait();
  EXPECT_EQ(spy.Count(), 1);

  loop.Quit();
  loop.ExecEx();
}

}  // namespace spiderweb
