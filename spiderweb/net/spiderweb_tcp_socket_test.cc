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

  spy.Wait(1000, 10);

  EXPECT_EQ(spy.Count(), 1);

  std::cout << std::get<0>(spy.LastResult<const std::error_code &>()).message() << std::endl;
  // output should: Connection refused

  loop.QueueTask([&]() { loop.Quit(); });

  loop.Exec();
}

TEST(spiderweb_tcp_socket, WriteSuccess) {
  spiderweb::EventLoop loop;

  spiderweb::net::TcpSocket socket;

  spiderweb::EventSpy spy(&socket, &spiderweb::net::TcpSocket::BytesWritten);

  spiderweb::Object::Connect(&socket, &spiderweb::net::TcpSocket::ConnectionEstablished, &loop,
                             [&] {
                               std::vector<uint8_t> data{0x30, 0x31, 0x32, 0x33, 0x34};
                               socket.Write(data.data(), data.size());
                             });

  spiderweb::Object::Connect(&socket, &spiderweb::net::TcpSocket::BytesRead, &loop,
                             [&](const spiderweb::io::BufferReader &buffer) {
                               std::cout << buffer.Len() << std::endl;

                               std::vector<char> data;
                               buffer.PeekAt(data, 0, buffer.Len());

                               socket.Write(reinterpret_cast<const uint8_t *>(data.data()),
                                            data.size());
                             });
  spiderweb::Object::Connect(
      &socket, &spiderweb::net::TcpSocket::Error, &loop,
      [&](const std::error_code &ec) { std::cout << ec.message() << std::endl; });

  socket.ConnectToHost("127.0.0.1", 6666);

  spy.Wait(3000, 100000);
  EXPECT_TRUE(spy.Count() > 0);

  loop.QueueTask([&]() { loop.Quit(); });

  loop.Exec();
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

TEST(spiderweb_tcp_socket, SafeDelete) {
  spiderweb::EventLoop loop;

  auto               *socket = new spiderweb::net::TcpSocket;
  spiderweb::EventSpy spy(socket, &spiderweb::net::TcpSocket::Error);

  std::vector<uint8_t> data{0x30, 0x31, 0x32, 0x33, 0x34};

  /**
   * @brief the actual delete operation(it's Private) should called after all write operations are
   *
   * done
   */
  socket->ConnectToHost("127.0.0.1", 12345);
  socket->Write(data.data(), data.size());
  socket->DeleteLater();
  socket->Write(data.data(), data.size());
  socket->Write(data.data(), data.size());
  socket = nullptr;

  spy.Wait(3000, 100000);
  /**
   * @brief because DeleteLater will delete the socket,before all write operations done,
   *
   * so, no any Error emited
   */
  EXPECT_EQ(spy.Count(), 0);

  loop.QueueTask([&]() { loop.Quit(); });

  loop.Exec();
}
