#include "spiderweb_synchronized.h"

#include <vector>

#include "gtest/gtest.h"

class My {
 public:
  void Do() const {
    printf("Do\n");
    ++i;
  }

  void Dump() {
    printf("i=%d\n", i);
  }

 private:
  mutable int i = 0;
};

TEST(Synchronized, Lock) {
  spiderweb::Synchronized<My> syn;

  syn.Lock()->Do();
  syn.Lock()->Do();
  syn.WithLock([](const My *my) { my->Do(); });
  syn.WithLock([](My *my) { my->Do(); });

  syn.Lock()->Dump();
}
