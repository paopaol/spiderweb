#include "spiderweb/net/spiderweb_tcp_server.h"

#include "core/spiderweb_eventloop.h"
#include "gtest/gtest.h"
#include "spiderweb/core/spiderweb_event_spy.h"
#include "spiderweb/net/spiderweb_tcp_socket.h"

TEST(spiderweb_tcp_server, Accept) {
  spiderweb::EventLoop      loop;
  spiderweb::net::TcpServer server(1234);
  spiderweb::EventSpy       spy(&server, &spiderweb::net::TcpServer::InComingConnection);

  server.ListenAndServ();

  spy.Wait();
  EXPECT_EQ(spy.Count(), 1);
  auto *conn = std::get<0>(spy.LastResult<spiderweb::net::TcpSocket *&>());
  std::cout << "local " << conn->LocalStringAddress() << " remote " << conn->RemoteStringAddress()
            << std::endl;
  loop.QueueTask([&]() { loop.Quit(); });

  loop.Exec();
}
