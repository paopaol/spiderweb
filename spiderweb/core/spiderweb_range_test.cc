#include "spiderweb/core/spiderweb_range.h"

#include <vector>

#include "gtest/gtest.h"

TEST(Range, AllOf) {
  std::vector<int> vec = {1, 2, 3, 5};

  EXPECT_TRUE(spiderweb::AllOf(vec, [&](int k) { return k > 0; }));
}

TEST(Range, AnyOf) {
  std::vector<int> vec = {1, 2, 3, 5};

  EXPECT_TRUE(spiderweb::AnyOf(vec, [&](int k) { return k > 0; }));
}

TEST(Range, NoneOf) {
  std::vector<int> vec = {1, 2, 3, 5};

  EXPECT_FALSE(spiderweb::NoneOf(vec, [&](int k) { return k > 0; }));
}

TEST(Range, ForEach) {
  std::vector<int> vec = {1, 2, 3, 5};
  int              called = 0;

  spiderweb::ForEach(vec, [&](int /*k*/) { ++called; });
  EXPECT_EQ(called, 4);
}

TEST(Range, FindIf) {
  std::vector<int> vec = {1, 2, 3, 5};

  EXPECT_TRUE(spiderweb::FindIf(vec, [&](int k) { return k == 1; }) != vec.end());
}

TEST(Range, Count) {
  std::vector<int> vec = {1, 2, 2, 5};
  auto             cnt = spiderweb::Count(vec, 2);
  EXPECT_EQ(cnt, 2);
}

TEST(Range, CountIf) {
  std::vector<int> vec = {1, 2, 3, 5};
  auto             cnt = spiderweb::CountIf(vec, [&](int k) { return k >= 2; });
  EXPECT_EQ(cnt, 3);
}
