#include "spiderweb/core/spiderweb_complete_queue.h"

#include <absl/memory/memory.h>

#include "gtest/gtest.h"
#include "spiderweb/core/spiderweb_eventloop.h"
#include "spiderweb/core/spiderweb_timer.h"

// class BrowseOperation : public AsyncOperation {
//  public:
//   explicit BrowseOperation(const std::string &node) : node_(node) {
//   }
//
//   void Start() override {
//     auto st = client_->AsyncBrowse();
//     if (st) {
//       Done();
//     }
//   }
//
//   static void HandleResp(void *context) {
//     auto *op = static_cast<BrowseOperation *>(context);
//     op->Done();
//   }
//
//   const std::vector<std::string> &Childs() const {
//     return childs_;
//   }
//
//  private:
//   std::vector<std::string> childs_;
//   std::string              node_;
// };
//
// class Client {
//  public:
//   Client() {
//     q.Open();
//   }
//
//   ~Client() {
//     q.ShutDown();
//   }
//
//   void Start() {
//     q.Commit(new BrowseOperation("i=85"), [this](AsyncOperation *op) { BrowseDone(op); });
//   }
//
//   void BrowseDone(AsyncOperation *op) {
//     auto *bop = static_cast<BrowseOperation *>(op);
//     for (const auto &ch : bop->Childs()) {
//       q.Commit(new BrowseOperation(ch), [this](AsyncOperation *op) { BrowseDone(op); });
//     };
//     delete op;
//   };
//
//  private:
//   AsyncCompleteQueue q;
// };

namespace spiderweb {

TEST(AsyncCompleteQueue, DefaultConstruct) {
  AsyncCompleteQueue queue;

  EXPECT_TRUE(queue.IsShutDown());
}

TEST(AsyncCompleteQueue, CommitShutDownQueue_ShouldDoneWithShutDownResult) {
  class Operation : public AsyncOperation {
   public:
    void Start() override {
      call_times_++;
    }

    uint32_t call_times_ = 0;
  };

  AsyncCompleteQueue queue;
  uint32_t           done_call_times = 0;
  auto               op = absl::make_unique<Operation>();

  const auto done = [&](AsyncOperation::Ptr opt, AsyncOperation::State result) {
    if (result == AsyncOperation::State::kShutDown) {
      done_call_times++;
      auto *v = static_cast<Operation *>(opt.get());
      EXPECT_EQ(v->call_times_, 0);
    }
  };

  queue.Commit(std::move(op), done);

  EXPECT_EQ(done_call_times, 1);
}

TEST(AsyncCompleteQueue, DestructWhenHasRuningOperation) {
  class DeferCompleteOperation : public AsyncOperation {
   public:
    DeferCompleteOperation() {
      timer = std::make_shared<Timer>();
      Object::Connect(timer.get(), &Timer::timeout, timer.get(), [=]() {
        timer->Stop();
        Done();
      });

      timer->SetInterval(1000);
      timer->SetSingalShot(false);
    }

    ~DeferCompleteOperation() override {
      timer->Stop();
    }

    void Start() override {
      timer->Start();
    }

    std::shared_ptr<Timer> timer;
  };

  EventLoop loop;
  int32_t   done_call_times = 0;
  int32_t   shutdown_call_times = 0;

  {
    AsyncCompleteQueue queue;

    queue.Open();

    queue.Commit(absl::make_unique<DeferCompleteOperation>(),
                 [&](AsyncOperation::Ptr, AsyncOperation::State result) {
                   EXPECT_EQ(result, AsyncOperation::State::kDone);
                   done_call_times++;
                 });
    queue.Commit(absl::make_unique<DeferCompleteOperation>(),
                 [&](AsyncOperation::Ptr, AsyncOperation::State result) {
                   EXPECT_EQ(result, AsyncOperation::State::kShutDown);
                   shutdown_call_times++;
                 });
    queue.Commit(absl::make_unique<DeferCompleteOperation>(),
                 [&](AsyncOperation::Ptr, AsyncOperation::State result) {
                   EXPECT_EQ(result, AsyncOperation::State::kShutDown);
                   shutdown_call_times++;
                   loop.Quit();
                 });
  }

  loop.RunAfter(1000, [&]() { loop.Quit(); });

  loop.Exec();
  EXPECT_EQ(done_call_times, 1);
  EXPECT_EQ(shutdown_call_times, 2);
}
}  // namespace spiderweb
