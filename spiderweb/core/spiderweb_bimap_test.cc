#include "spiderweb/core/spiderweb_bimap.h"

#include "gtest/gtest.h"

TEST(BiMap, construct) {
  spiderweb::UnorderedBiMap<int, int> m;

  EXPECT_EQ(m.Size(), 0);
  EXPECT_TRUE(m.Empty());
}

TEST(BiMap, insert) {
  spiderweb::UnorderedBiMap<int, int> m;

  m.Set(1, 1);
  EXPECT_EQ(m.Size(), 1);
  EXPECT_TRUE(!m.Empty());
}

// TEST(BiMap, erase_by_key) {
//   spiderweb::UnorderedBiMap<int, int> m;
//
//   m.insert(1, 1);
//   EXPECT_EQ(m.size(), 1);
//   m.erase(1);
//   EXPECT_EQ(m.size(), 0);
// }
//
// TEST(BiMap, erase_by_iter) {
//   spiderweb::UnorderedBiMap<int, std::string> m;
//
//   m.insert(1, "name");
//   auto it = m.find(1);
//
//   m.erase(it);
//   EXPECT_EQ(m.size(), 0);
// }
//
TEST(BiMap, foreach) {
  spiderweb::UnorderedBiMap<int, std::string> m;

  m.Set(1, "name1");
  m.Set(2, "name2");

  for (const auto &it : m) {
    printf("%d %s\n", it.first, it.second->right_value.c_str());
  }
}

struct Item {
  int32_t     id;
  std::string name;
  int32_t     age;
};

struct ItemUniqueKey {
  int32_t operator()(const Item &item) {
    return item.id;
  }
};

TEST(BiMap, Find) {
  spiderweb::UnorderedBiMap<int, Item, ItemUniqueKey> m;

  m.Set(1, {1, "xiaoming", 23});
  m.Set(2, {2, "lilei", 23});
  m.Set(3, {3, "dapeng", 23});

  EXPECT_EQ(m.Size(), 3);

  auto it = m.Find(2);
  EXPECT_EQ(it->first, 2);

  EXPECT_EQ(it->second->right_value.id, 2);
  EXPECT_EQ(it->second->right_value.name, "lilei");
  EXPECT_EQ(it->second->right_value.age, 23);
}

TEST(BiMap, InverseFind) {
  spiderweb::UnorderedBiMap<int, Item, ItemUniqueKey> m;

  m.Set(1, {1, "xiaoming", 23});
  m.Set(2, {2, "lilei", 23});
  m.Set(3, {3, "dapeng", 24});

  EXPECT_EQ(m.Size(), 3);

  auto inverse = m.Inserse();

  auto it = inverse.Find(3);
  EXPECT_EQ(it->first, 3);

  EXPECT_EQ(it->second->right_value.id, 3);
  EXPECT_EQ(it->second->right_value.name, "dapeng");
  EXPECT_EQ(it->second->right_value.age, 24);
}

TEST(BiMap, Range) {
  spiderweb::UnorderedBiMap<int, Item, ItemUniqueKey> m;

  m.Set(1, {1, "xiaoming", 23});
  m.Set(2, {2, "lilei", 23});
  m.Set(3, {3, "dapeng", 23});

  EXPECT_EQ(m.Size(), 3);

  for (const auto &it : m) {
    printf("%d %d %s %d\n", it.first, it.second->right_value.id,
           it.second->right_value.name.c_str(), it.second->right_value.age);
  }
}

TEST(BiMap, Insert) {
  spiderweb::UnorderedBiMap<int, Item, ItemUniqueKey> m;

  m.Set(5, {1, "xiaoming", 23});
  m.Set(5, {2, "lilei", 23});   // change 1 to 2
  m.Set(6, {2, "dapeng", 23});  // change 5 to 6

  EXPECT_EQ(m.Size(), 1);
}
