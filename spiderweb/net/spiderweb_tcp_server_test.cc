#include "spiderweb/net/spiderweb_tcp_server.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "spiderweb/core/spiderweb_async_wait_group.h"
#include "spiderweb/core/spiderweb_eventloop.h"
#include "spiderweb/core/spiderweb_notify_spy.h"
#include "spiderweb/core/spiderweb_timer.h"
#include "spiderweb/net/private/spiderweb_tcp_server_private.h"
#include "spiderweb/net/spiderweb_tcp_socket.h"
#include "spiderweb/spiderweb_check.h"

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
  delete server;

  SPIDERWEB_VERIFY(1 == 1, spy.Clear(); return);
}

class MyClient : public spiderweb::Object {
 public:
  enum class State {
    kStopping,
    kStopped,
  };
  MyClient() {
  }

  void Start() {
    assert(!timer_a);
    assert(!timer_b);

    timer_a = std::make_shared<spiderweb::Timer>();
    timer_b = std::make_shared<spiderweb::Timer>();

    timer_a->Start();
    timer_b->Start();
  }

  void Stop() {
    assert(!stop_waiter_);

    state_ = State::kStopping;
    stop_waiter_ = std::make_shared<spiderweb::AsyncWaitGroup>();

    auto hande_done = [this]() {
      QueueTask([this]() {
        state_ = State::kStopped;
        spider_emit Stopped();
      });
    };

    spiderweb::Object::Connect(stop_waiter_.get(), &spiderweb::AsyncWaitGroup::AllDone,
                               stop_waiter_.get(), hande_done);

    if (timer_a->IsRunning()) {
      stop_waiter_->Add(1);
      timer_a->Stop();
    }
    if (timer_b->IsRunning()) {
      stop_waiter_->Add(1);
      timer_b->Stop();
    }

    stop_waiter_->Start();
  }

  spiderweb::Notify<> Stopped;

  spiderweb::Notify<> Started;

  spiderweb::Notify<> Error;

 private:
  std::shared_ptr<spiderweb::AsyncWaitGroup> stop_waiter_;
  std::shared_ptr<spiderweb::Timer>          timer_a;
  std::shared_ptr<spiderweb::Timer>          timer_b;
  State                                      state_ = State::kStopped;
};
