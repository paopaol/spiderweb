#include "spiderweb/core/spiderweb_unbounded_queue.h"

#include <atomic>

#include "gtest/gtest.h"

template <typename Child>
class Count {
 public:
  Count() {
    ++n;
  }

  static std::size_t GetCount() {
    return n;
  }

 private:
  static std::atomic<uint32_t> n;
};

template <typename Child>
std::atomic<uint32_t> Count<Child>::n{0};

TEST(UnboundedQueue, Operate) {
  class Element : public Count<Element> {};

  spiderweb::SyncUnboundedQueue<Element> queue;

  EXPECT_TRUE(queue.Empty());
  queue.PushBack(Element());
  EXPECT_EQ(Element::GetCount(), 1);
  EXPECT_EQ(queue.Size(), 1);

  Element out;

  queue.PopFront(out);
  EXPECT_EQ(Element::GetCount(), 2);
}

TEST(UnboundedQueue, Swap) {
  struct Element {};

  spiderweb::SyncUnboundedQueue<Element> queue;

  queue.PushBack(Element());
  queue.PushBack(Element());
  queue.PushBack(Element());
  EXPECT_EQ(queue.Size(), 3);

  std::queue<Element> all;

  queue.Swap(all);
  EXPECT_TRUE(queue.Empty());
  EXPECT_EQ(all.size(), 3);
}

TEST(UnboundedQueue, CopyConstruct) {
  struct Element {
    int value = 0;
  };

  spiderweb::SyncUnboundedQueue<Element> queue;

  queue.PushBack(Element());
  queue.PushBack(Element());
  queue.PushBack(Element());

  spiderweb::SyncUnboundedQueue<Element> queue2(queue);

  EXPECT_EQ(queue.Size(), queue2.Size());
}

TEST(UnboundedQueue, FrontDo) {
  struct Element {
    int value = 0;
  };

  spiderweb::SyncUnboundedQueue<Element> queue;

  queue.PushBack(Element());

  queue.FrontDo([](Element& e) { e.value = 10; });
  Element first;

  queue.PopFront(first);
  EXPECT_EQ(first.value, 10);
}

TEST(UnboundedQueue, BackDo) {
  struct Element {
    int value = 0;
  };

  spiderweb::SyncUnboundedQueue<Element> queue;

  queue.PushBack(Element());

  queue.BackDo([](Element& e) { e.value = 10; });
  Element first;

  queue.PopFront(first);
  EXPECT_EQ(first.value, 10);
}
