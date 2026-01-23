#include "spiderweb/net/spiderweb_udp_socket.h"

#include <fcntl.h>
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
  UdpSocket server;
  server.Open(ec);
}

TEST_F(UdpTest, ReOpen) {
  bool                 called = false;
  std::vector<int64_t> files;

  files.reserve(10240);
  for (;;) {
    auto fd = ::open("/etc/passwd", O_RDONLY);
    if (fd >= 0) {
      files.push_back(fd);
    } else {
      spiderweb::ErrorCode e;
      UdpSocket            socket;

      socket.Open(e);
      EXPECT_TRUE(e) << e.FormatedMessage();
      EXPECT_FALSE(socket.IsOpen());

      ::close(files.front());

      socket.Open(e);
      EXPECT_FALSE(e);
      EXPECT_TRUE(socket.IsOpen());

      called = true;
      break;
    }
  }

  EXPECT_TRUE(called);
}
}  // namespace net
}  // namespace spiderweb
