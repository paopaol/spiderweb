#include "spiderweb_synchronized.h"

#include <thread>
#include <vector>

#include "gtest/gtest.h"

class My {
 public:
  void Do() const {
    printf("Do\n");
    std::this_thread::sleep_for(std::chrono::seconds(5));
    ++i;
  }

  void Dump() {
    printf("i=%d\n", i);
  }

  int Value() const {
    return i;
  }

 private:
  mutable int i = 0;
};

TEST(Synchronized, Lock) {
  spiderweb::Synchronized<My> syn;

  std::thread thread1([&]() { syn.Lock()->Do(); });

  std::thread thread2([&]() {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    syn.Lock()->Dump();
  });

  thread1.join();
  thread2.join();
  EXPECT_EQ(syn.Lock()->Value(), 1);
}
