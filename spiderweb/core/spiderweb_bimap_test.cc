#include "spiderweb/core/spiderweb_bimap.h"

#include "gtest/gtest.h"

TEST(BiMap, construct) {
  spiderweb::UnorderedBiMap<int, int> m;

  EXPECT_EQ(m.size(), 0);
  EXPECT_TRUE(m.empty());
}

TEST(BiMap, insert) {
  spiderweb::UnorderedBiMap<int, int> m;

  m.insert(1, 1);
  EXPECT_EQ(m.size(), 1);
  EXPECT_TRUE(!m.empty());
}

TEST(BiMap, erase_by_key) {
  spiderweb::UnorderedBiMap<int, int> m;

  m.insert(1, 1);
  EXPECT_EQ(m.size(), 1);
  m.erase(1);
  EXPECT_EQ(m.size(), 0);
}

TEST(BiMap, erase_by_iter) {
  spiderweb::UnorderedBiMap<int, std::string> m;

  m.insert(1, "name");
  auto it = m.find(1);

  m.erase(it);
  EXPECT_EQ(m.size(), 0);
}

TEST(BiMap, foreach) {
  spiderweb::UnorderedBiMap<int, std::string> m;

  m.insert(1, "name1");
  m.insert(2, "name2");

  for (const auto &it : m) {
    printf("%d %s\n", it.first, it.second.c_str());
  }
}

TEST(BiMap, inverse) {
  spiderweb::UnorderedBiMap<int, std::string> m;

  m.insert(1, "name1");
  m.insert(2, "name2");

  auto inverse_map = m.Iverse();
  EXPECT_EQ(inverse_map.size(), 2);

  for (const auto &it : inverse_map) {
    printf("%s %d\n", it.first.c_str(), it.second);
  }

  inverse_map.erase("name2");
  EXPECT_EQ(inverse_map.size(), 1);
  EXPECT_EQ(m.size(), 1);

  puts("after removed from inverse_map:inverse_map:");
  for (const auto &it : inverse_map) {
    printf("%s %d\n", it.first.c_str(), it.second);
  }

  puts("after removed from inverse_map:map:");
  for (const auto &it : m) {
    printf("%d %s\n", it.first, it.second.c_str());
  }
}
