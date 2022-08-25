#include "spiderweb/net/spiderweb_tcp_socket.h"

#include "gtest/gtest.h"
#include "spiderweb/core/spiderweb_event_spy.h"
#include "spiderweb/core/spiderweb_eventloop.h"

TEST(spiderweb_tcp_socket, ConnectToHostSuccess) {
  spiderweb::EventLoop loop;

  spiderweb::net::TcpSocket socket;

  spiderweb::EventSpy spy(&socket, &spiderweb::net::TcpSocket::ConnectionEstablished);

  socket.ConnectToHost("127.0.0.1", 6666);

  spy.Wait(1000);

  EXPECT_EQ(spy.Count(), 1);
  loop.QueueTask([&]() { loop.Quit(); });

  loop.Exec();
}

TEST(spiderweb_tcp_socket, ConnectToHostFailed1) {
  spiderweb::EventLoop loop;

  spiderweb::net::TcpSocket socket;

  spiderweb::EventSpy spy(&socket, &spiderweb::net::TcpSocket::Error);

  socket.ConnectToHost("127.0.0.1", 5555);

  spy.Wait(1000);

  EXPECT_EQ(spy.Count(), 1);

  std::cout << std::get<0>(spy.LastResult<const std::error_code &>()).message() << std::endl;
  // output should: Connection refused

  loop.QueueTask([&]() { loop.Quit(); });

  loop.Exec();
}
