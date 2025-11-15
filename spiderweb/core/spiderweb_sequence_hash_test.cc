#include "spiderweb/core/spiderweb_sequence_hash.h"

#include <utility>

#include "gtest/gtest.h"

struct User {
  std::string id;
  int         age = 0;
};

namespace spiderweb {
class SequenceHashTest : public testing::Test {
 public:
  void SetUp() override {
    users.PushBack("123", User{"123", 19});
    users.PushBack("abc", User{"abc", 23});
  }

  SequenceHash<std::string, User> users;
};

TEST_F(SequenceHashTest, Construct) {
  SequenceHash<std::string, User> v;
  EXPECT_TRUE(v.Empty());
}

TEST_F(SequenceHashTest, MoveConstruct) {
  SequenceHash<std::string, User> v(std::move(users));

  EXPECT_EQ(v.Size(), 2);
  EXPECT_TRUE(users.Empty());
}

TEST_F(SequenceHashTest, MoveAssgin) {
  SequenceHash<std::string, User> v;

  v.PushBack("front", User{"front", 123});

  v = std::move(users);
  EXPECT_EQ(v.Size(), 2);
  EXPECT_TRUE(users.Empty());
}

TEST_F(SequenceHashTest, PushBack) {
  EXPECT_EQ(users.Size(), 2);
  EXPECT_FALSE(users.Empty());
}

TEST_F(SequenceHashTest, PushFront) {
  EXPECT_TRUE(users.PushFront("front", User{"front", 123}));

  std::vector<std::string> result{"front", "123", "abc"};
  int                      i = 0;
  for (auto it = users.begin(); it != users.end(); ++it) {
    EXPECT_EQ(it->id, result[i]);
    ++i;
  }
}

TEST_F(SequenceHashTest, PushFrontwhenEmpty) {
  SequenceHash<std::string, User> v;
  EXPECT_TRUE(v.PushFront("front", User{"front", 123}));
}

TEST_F(SequenceHashTest, PushFailed) {
  EXPECT_FALSE(users.PushBack("123", User{}));
}

TEST_F(SequenceHashTest, PushValue) {
  SequenceHash<std::string, User> v;
  User                            u;

  EXPECT_TRUE(v.PushBack("front", u));
}

TEST_F(SequenceHashTest, PushAfter) {
  EXPECT_TRUE(users.PushAfter("123", "x", User{"x", 33}));

  std::vector<std::string> result{"123", "x", "abc"};
  int                      i = 0;
  for (auto it = users.begin(); it != users.end(); ++it) {
    EXPECT_EQ(it->id, result[i]);
    ++i;
  }
}

TEST_F(SequenceHashTest, PushBefore) {
  EXPECT_TRUE(users.PushBefore("123", "before", User{"before", 123}));

  std::vector<std::string> result{"before", "123", "abc"};
  int                      i = 0;
  for (auto it = users.begin(); it != users.end(); ++it) {
    EXPECT_EQ(it->id, result[i]) << i;
    ++i;
  }
}

TEST_F(SequenceHashTest, Remove) {
  users.Remove("123");

  EXPECT_EQ(users.Size(), 1);
  EXPECT_EQ(users.Find("123"), users.end());
  EXPECT_EQ(users.Find("abc")->id, "abc");
}

TEST_F(SequenceHashTest, Clear) {
  users.Clear();
  EXPECT_TRUE(users.Empty());
}

TEST_F(SequenceHashTest, Begin) {
  auto beg = users.begin();
  EXPECT_EQ(beg->id, "123");
}

TEST_F(SequenceHashTest, For) {
  std::vector<std::string> result{"123", "abc"};
  int                      i = 0;
  for (auto it = users.begin(); it != users.end(); ++it) {
    EXPECT_EQ(it->id, result[i]);
    ++i;
  }
}

TEST_F(SequenceHashTest, ForRange) {
  std::vector<std::string> result{"123", "abc"};
  int                      i = 0;
  for (auto& it : users) {
    EXPECT_EQ(it.id, result[i]);
    ++i;
  }
}

TEST_F(SequenceHashTest, ConstForRange) {
  std::vector<std::string> result{"123", "abc"};
  int                      i = 0;
  for (const auto& it : users) {
    EXPECT_EQ(it.id, result[i]);
    ++i;
  }
}

TEST_F(SequenceHashTest, Front) {
  EXPECT_EQ(users.Front()->id, "123");
  const auto it = users.Front();
  EXPECT_EQ(it->id, "123");
}

TEST_F(SequenceHashTest, Back) {
  EXPECT_EQ(users.Back()->id, "abc");
  const auto it = users.Back();
  EXPECT_EQ(it->id, "abc");

  const SequenceHash<std::string, User> us;
}

TEST_F(SequenceHashTest, Find) {
  EXPECT_NE(users.Find("123"), users.end());
  EXPECT_NE(users.Find("abc"), users.end());
  EXPECT_EQ(users.Find(""), users.end());

  EXPECT_EQ(users.Find("123")->id, "123");
}

TEST_F(SequenceHashTest, ForRangePrint) {
  for (auto& it : users) {
    printf("id: %s age: %d\n", it.id.c_str(), it.age);
  }
}

TEST_F(SequenceHashTest, Forward) {
  auto it = users.Forward("123");
  EXPECT_NE(it, users.end());
  EXPECT_EQ(it->id, "abc");
}

// 123 abc
TEST_F(SequenceHashTest, ForwardNotFound) {
  auto it = users.Forward("bad-key");
  EXPECT_EQ(it, users.end());
}

TEST_F(SequenceHashTest, Backward) {
  auto it = users.Backward("abc");
  EXPECT_NE(it, users.end());
  EXPECT_EQ(it->id, "123");
}

TEST_F(SequenceHashTest, BackwardNotFound) {
  auto it = users.Backward("bad-key");
  EXPECT_EQ(it, users.end());
}

TEST_F(SequenceHashTest, BackRangePrint) {
  auto current = users.Back();

  while (current != users.end()) {
    printf("id: %s age: %d\n", current->id.c_str(), current->age);
    current--;
  }
}

TEST_F(SequenceHashTest, InitList) {
  SequenceHash<std::string, User> oldusers{
      {"123", User{"123", 19}},
      {"abc", User{"abc", 23}},
  };

  EXPECT_EQ(oldusers.Size(), 2);
  EXPECT_TRUE(oldusers.Find("123") != oldusers.end());
  EXPECT_TRUE(oldusers.Find("abc") != oldusers.end());

  auto _123 = oldusers.Find("123");
  auto abc = oldusers.Find("abc");

  EXPECT_EQ(_123->age, 19);
  EXPECT_EQ(_123->id, "123");

  EXPECT_EQ(abc->age, 23);
  EXPECT_EQ(abc->id, "abc");
}

}  // namespace spiderweb
