#include "spiderweb/type/spiderweb_variant.h"

#include "gtest/gtest.h"
#include "spiderweb/type/spiderweb_variant_match.h"

struct St {};

struct Metric {
  spiderweb::Variant v;
  std::string        expected_string;
  bool               expected_bool;
  float              expected_float;
  float              expected_int;
};

TEST(Variant, Default) {
  spiderweb::Variant v;

  EXPECT_TRUE(v.IsNull());
  EXPECT_EQ("", v.ToString());
  v = 1;
  EXPECT_FALSE(v.IsNull());
}

TEST(Variant, ContructInt) {
  {
    spiderweb::Variant v(static_cast<int16_t>(123));

    EXPECT_EQ("123", v.ToString());
  }
  {
    spiderweb::Variant v(static_cast<uint16_t>(123));

    EXPECT_EQ("123", v.ToString());
  }
  {
    spiderweb::Variant v(static_cast<int32_t>(123));

    EXPECT_EQ("123", v.ToString());
  }
  {
    spiderweb::Variant v(static_cast<uint32_t>(123));

    EXPECT_EQ("123", v.ToString());
  }
  {
    spiderweb::Variant v(static_cast<int64_t>(123));

    EXPECT_EQ("123", v.ToString());
  }
  {
    spiderweb::Variant v(static_cast<uint64_t>(123));

    EXPECT_EQ("123", v.ToString());
  }
  {
    spiderweb::Variant v = 123;

    EXPECT_EQ("123", v.ToString());
  }
  {
    spiderweb::Variant v = std::string("123");

    v = 123;
    v = "123";

    EXPECT_EQ("123", v.ToString());
  }
}

TEST(Variant, ContructString) {
  spiderweb::Variant v("123");

  EXPECT_EQ("123", v.ToString());
}

TEST(Variant, ContructFloat) {
  spiderweb::Variant v(123.f);

  EXPECT_EQ("123.000000", v.ToString());
}

TEST(Variant, ContructAny) {
  spiderweb::Variant v(St{});  //(St{});
  v = St{};

  EXPECT_EQ("", v.ToString());
}

TEST(Variant, ToOtherTypes) {
  std::vector<Metric> metrics = {
      /**
       * @brief int
       */
      Metric{spiderweb::Variant(static_cast<int16_t>(123)), "123", true, 123.f, 123},
      Metric{spiderweb::Variant(static_cast<uint16_t>(123)), "123", true, 123.f, 123},
      Metric{spiderweb::Variant(static_cast<int32_t>(123)), "123", true, 123.f, 123},
      Metric{spiderweb::Variant(static_cast<uint32_t>(123)), "123", true, 123.f, 123},
      Metric{spiderweb::Variant(static_cast<int64_t>(123)), "123", true, 123.f, 123},
      Metric{spiderweb::Variant(static_cast<uint64_t>(123)), "123", true, 123.f, 123},

      /**
       * @brief int
       */
      Metric{spiderweb::Variant(static_cast<int16_t>(-123)), "-123", true, -123.f, -123},
      Metric{spiderweb::Variant(static_cast<int32_t>(-123)), "-123", true, -123.f, -123},
      Metric{spiderweb::Variant(static_cast<int64_t>(-123)), "-123", true, -123.f, -123},

      /**
       * @brief bool
       */
      Metric{spiderweb::Variant(true), "1", true, 1.f, 1},
      Metric{spiderweb::Variant(false), "0", false, 0.f, 0},
      /**
       * @brief float point
       */
      Metric{spiderweb::Variant(static_cast<float>(123.f)), "123.000000", true, 123.f, 123},
      Metric{spiderweb::Variant(static_cast<float>(-123.f)), "-123.000000", true, -123.f, -123},
      /**
       * @brief string
       */
      Metric{spiderweb::Variant("123"), "123", true, 123.f, 123},
      Metric{spiderweb::Variant("-123"), "-123", true, -123.f, -123},
      Metric{spiderweb::Variant("0"), "0", false, 0.f, 0},
  };

  for (const auto &metric : metrics) {
    EXPECT_EQ(metric.v.ToString(), metric.expected_string);
    EXPECT_EQ(metric.v.ToBool(), metric.expected_bool);
    EXPECT_FLOAT_EQ(metric.v.ToFloat(), metric.expected_float);
    EXPECT_FLOAT_EQ(metric.v.ToBool(), metric.expected_bool);
  }
}

TEST(Variant, CopyConstruct) {
  spiderweb::Variant v1(123);
  spiderweb::Variant v2(v1);

  EXPECT_EQ(v1.ToInt(), v2.ToInt());
}

TEST(Variant, MoveConstruct) {
  spiderweb::Variant v1(123);
  spiderweb::Variant v2(std::move(v1));

  EXPECT_EQ(v2.ToInt(), 123);
  EXPECT_EQ(v2.IsNull(), false);
}

TEST(Variant, CopyAssgin) {
  spiderweb::Variant v1(123);
  spiderweb::Variant v2(v1);

  v2 = v1;

  EXPECT_EQ(v2.ToInt(), 123);
  EXPECT_EQ(v2.IsNull(), false);
  EXPECT_EQ(v1.ToInt(), 123);
  EXPECT_EQ(v1.IsNull(), false);
}

TEST(Variant, MoveAssgin) {
  spiderweb::Variant v1(123);
  spiderweb::Variant v2;

  v2 = std::move(v1);

  EXPECT_EQ(v2.ToInt(), 123);
  EXPECT_EQ(v2.IsNull(), false);
}

TEST(Variant, AssginOtherTypes) {
  /**
   * @brief copy construct
   */
  {
    std::string abc = "abc";

    spiderweb::Variant v1(abc);
  }
  /**
   * @brief copy construct
   */
  {
    int abc = 123;

    spiderweb::Variant v2;

    v2 = abc;
    spiderweb::Variant v1(abc);
  }
  /**
   * @brief move construct
   */
  {
    std::string        abc = "abc";
    spiderweb::Variant v1(abc);
    EXPECT_EQ(abc, "abc");
  }

  /**
   * @brief copy assgin
   */
  {
    spiderweb::Variant v1;
    std::string        abc = "abc";

    EXPECT_EQ(abc, "abc");
    v1 = abc;
    EXPECT_EQ(abc, "abc");
  }
  {
    spiderweb::Variant v1;
    int32_t            abc = 5;

    EXPECT_EQ(abc, 5);
    v1 = abc;
    EXPECT_EQ(abc, 5);
  }

  /**
   * @brief move assgin
   */
  {
    spiderweb::Variant v1;
    std::string        abc = "abc";

    EXPECT_EQ(abc, "abc");
    v1 = std::move(abc);
    EXPECT_EQ(abc, "");
  }

  /**
   * @brief move assgin
   */
  {
    spiderweb::Variant v1;  // = std::vector<int>();
    v1.SetValue(std::vector<int>());
  }

  {
    spiderweb::Variant v1;  // = std::vector<int>();
    v1.SetValue(std::vector<int>());

    v1 = 3.f;
    v1 = "fff";
    v1 = true;
    v1 = "zzz";
    EXPECT_EQ(v1.ToString(), "zzz");
  }
}

TEST(Variant, Match) {
  absl::variant<int, std::string> var;

  var = 123;

  int         int_v = 0;
  std::string str_v = "";

  spiderweb::Match<int, std::string>(
      var, [&](int v) { int_v = v; }, [&](const std::string &v) { str_v = v; });
  EXPECT_EQ(int_v, 123);
  EXPECT_EQ(str_v, "");

  var = "abc";

  int_v = 0;
  str_v = "";
  spiderweb::Match<int, std::string>(
      var, [&](int v) { int_v = v; }, [&](const std::string &v) { str_v = v; });
  EXPECT_EQ(int_v, 0);
  EXPECT_EQ(str_v, "abc");
}
