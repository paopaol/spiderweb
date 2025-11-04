#include "spiderweb/net/spiderweb_udp_socket.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "spiderweb/core/spiderweb_eventloop.h"
#include "spiderweb/core/spiderweb_notify_spy.h"

namespace spiderweb {
namespace net {

class UdpServer : public Object {
 public:
  explicit UdpServer(uint16_t port, Object* parent = nullptr) : Object(parent), socket(parent) {
    ErrorCode ec;
    socket.Open(ec);
    EXPECT_FALSE(ec) << ec.FormatedMessage();
    socket.Bind(EndPoint("0.0.0.0", port), ec);
    EXPECT_FALSE(ec) << ec.FormatedMessage();
  }

  UdpSocket socket;
};

class UdpTest : public testing::Test {
 public:
  UdpTest() : server(9998) {
  }

  EventLoop            loop;
  UdpServer            server;
  spiderweb::ErrorCode ec;
};

TEST_F(UdpTest, Bind) {
}

TEST_F(UdpTest, ClientSendServerRecv) {
  UdpSocket client;
  NotifySpy spy(&server.socket, &UdpSocket::DatagramArrived);

  client.Open(ec);

  client.SendTo(EndPoint("127.0.0.1", 9998), reinterpret_cast<const uint8_t*>("abc"), 3);

  spy.Wait();
  EXPECT_EQ(spy.Count(), 1);
  auto recved = spy.LastResult<Datagram>();
  auto datagram = std::get<0>(recved);

  EXPECT_THAT(datagram.Data(), testing::ElementsAre('a', 'b', 'c'));
}

TEST_F(UdpTest, OpenClose) {
  {
    UdpServer serve1(30001);
  }
  {
    UdpServer server2(30001);
  }
}
}  // namespace net
}  // namespace spiderweb
