#include "spiderweb/net/spiderweb_tcp_socket_connector.h"

#include <gtest/gtest.h>

#include <asio/error.hpp>

#include "spiderweb/core/spiderweb_eventloop.h"
#include "spiderweb/core/spiderweb_notify_spy.h"

namespace spiderweb {
namespace net {

TEST(TcpSocketConnector, Lifetime) {
  EventLoop loop;
  auto*     connector = new TcpSocketConnector();
  NotifySpy spy(connector, &TcpSocketConnector::ConnectError);

  connector->SetDeadline(300);
  connector->ConnectToHost("100.100.100.100", 1111);

  spy.Wait();

  EXPECT_EQ(spy.Count(), 1);
  auto error_code = std::get<0>(spy.LastResult<std::error_code>());
  EXPECT_EQ(error_code, asio::error::operation_aborted);
  std::cout << error_code.message() << '\n';

  connector->DeleteLater();

  loop.RunAfter(0, [&]() { loop.Quit(); });
  loop.Exec();
}

TEST(TcpSocketConnector, ConnectTimeout) {
  EventLoop          loop;
  TcpSocketConnector connector;
  NotifySpy          spy(&connector, &TcpSocketConnector::ConnectError);

  connector.SetDeadline(300);
  connector.ConnectToHost("100.100.100.100", 1111);

  spy.Wait();

  EXPECT_EQ(spy.Count(), 1);
  auto error_code = std::get<0>(spy.LastResult<std::error_code>());
  EXPECT_EQ(error_code, asio::error::operation_aborted);
  std::cout << error_code.message() << '\n';

  loop.RunAfter(0, [&]() { loop.Quit(); });
  loop.Exec();
}

TEST(TcpSocketConnector, ConnectCanceled) {
  EventLoop loop;
  auto*     connector = new TcpSocketConnector();
  NotifySpy spy(connector, &TcpSocketConnector::ConnectError);

  connector->SetDeadline(6000);
  connector->ConnectToHost("100.100.100.100", 1111);

  loop.RunAfter(10, [&]() {
    connector->CancelConnect();
    connector->DeleteLater();
  });

  spy.Wait();

  EXPECT_EQ(spy.Count(), 1);
  auto error_code = std::get<0>(spy.LastResult<std::error_code>());
  EXPECT_EQ(error_code, asio::error::operation_aborted);
  std::cout << error_code.message() << '\n';

  loop.RunAfter(0, [&]() { loop.Quit(); });
  loop.Exec();
}

TEST(TcpSocketConnector, ConnectRefused) {
  EventLoop          loop;
  TcpSocketConnector connector;
  NotifySpy          spy(&connector, &TcpSocketConnector::ConnectError);

  connector.SetDeadline(2000);
  connector.ConnectToHost("192.168.101.165", 1111);

  spy.Wait();

  EXPECT_EQ(spy.Count(), 1);
  auto error_code = std::get<0>(spy.LastResult<std::error_code>());
  EXPECT_EQ(error_code, asio::error::connection_refused);
  std::cout << error_code.message() << '\n';

  loop.RunAfter(0, [&]() { loop.Quit(); });
  loop.Exec();
}

}  // namespace net
}  // namespace spiderweb
