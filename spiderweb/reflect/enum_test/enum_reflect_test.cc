#include "gtest/gtest.h"
#include "spiderweb/reflect/reflect_enum.hpp"

enum class State {
  kGood,
  kBad,
  kInvalid,
};
REFLECT_ENUM(State, int, (kGood, 1), (kBad, 2), (kInvalid, 3))
REFLECT_ENUM(State, std::string, (kGood, "good"), (kBad, "bad"), (kInvalid, "invalid"))

TEST(enum_reflect, FromAliasType) {
  {
    const auto result = spiderweb::reflect::FromAliasType(1, State::kBad);
    EXPECT_EQ(result, State::kGood);
  }

  {
    const auto result = spiderweb::reflect::FromAliasType("good", State::kBad);
    EXPECT_EQ(result, State::kGood);
  }

  {
    const auto result = spiderweb::reflect::FromAliasType("100", State::kInvalid);
    EXPECT_EQ(result, State::kInvalid);
  }
}

TEST(enum_reflect, ToAliasType) {
  {
    const auto result = spiderweb::reflect::ToAliasType<int>(State::kBad);
    EXPECT_EQ(result, 2);
  }

  {
    const auto result = spiderweb::reflect::ToAliasType<std::string>(State::kInvalid);
    EXPECT_EQ(result, "invalid");
  }
}
