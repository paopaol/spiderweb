#include "spiderweb/reflect/enum_reflect.h"

#include "gtest/gtest.h"

enum class State {
  kGood,
  kBad,
  kInvalid,
};
REFLECT_ENUM(State, int, (kGood, 1), (kBad, 2), (kInvalid, 3))
REFLECT_ENUM(State, std::string, (kGood, "good"), (kBad, "bad"), (kInvalid, "invalid"))

TEST(enum_reflect, FromAliasType) {
  {
    const auto result = spiderweb::reflect::EnumFrom(1, State::kBad);
    EXPECT_EQ(result, State::kGood);
  }

  {
    const auto result = spiderweb::reflect::EnumFrom("good", State::kBad);
    EXPECT_EQ(result, State::kGood);
  }

  {
    const auto result = spiderweb::reflect::EnumFrom("100", State::kInvalid);
    EXPECT_EQ(result, State::kInvalid);
  }
}

TEST(enum_reflect, ToAliasType) {
  {
    int result;
    spiderweb::reflect::EnumTo(State::kBad, result);
    EXPECT_EQ(result, 2);
  }

  {
    std::string result;
    spiderweb::reflect::EnumTo(State::kInvalid, result);
    EXPECT_EQ(result, "invalid");
  }
}
