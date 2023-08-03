#include "spiderweb/type/spiderweb_variant.h"

#include "gtest/gtest.h"

struct St {};

struct Metric {
  spiderweb::Variant       v;
  spiderweb::Variant::Type expected_type;
  std::string              expected_string;
  bool                     expected_bool;
  float                    expected_float;
  float                    expected_int;
};

TEST(Variant, Default) {
  spiderweb::Variant v;

  EXPECT_TRUE(v.IsNull());
  EXPECT_EQ(v.GetType(), spiderweb::Variant::Type::kNull);
  EXPECT_EQ("", v.ToString());
}

TEST(Variant, ContructInt) {
  {
    spiderweb::Variant v(static_cast<int16_t>(123));

    EXPECT_EQ("123", v.ToString());
    EXPECT_EQ(v.GetType(), spiderweb::Variant::Type::kInt16);
  }
  {
    spiderweb::Variant v(static_cast<uint16_t>(123));

    EXPECT_EQ("123", v.ToString());
    EXPECT_EQ(v.GetType(), spiderweb::Variant::Type::kUint16);
  }
  {
    spiderweb::Variant v(static_cast<int32_t>(123));

    EXPECT_EQ("123", v.ToString());
    EXPECT_EQ(v.GetType(), spiderweb::Variant::Type::kInt32);
  }
  {
    spiderweb::Variant v(static_cast<uint32_t>(123));

    EXPECT_EQ("123", v.ToString());
    EXPECT_EQ(v.GetType(), spiderweb::Variant::Type::kUint32);
  }
  {
    spiderweb::Variant v(static_cast<int64_t>(123));

    EXPECT_EQ("123", v.ToString());
    EXPECT_EQ(v.GetType(), spiderweb::Variant::Type::kInt64);
  }
  {
    spiderweb::Variant v(static_cast<uint64_t>(123));

    EXPECT_EQ("123", v.ToString());
    EXPECT_EQ(v.GetType(), spiderweb::Variant::Type::kUint64);
  }
}

TEST(Variant, ContructFloat) {
  spiderweb::Variant v(123.f);

  EXPECT_EQ("123.000000", v.ToString());
  EXPECT_EQ(v.GetType(), spiderweb::Variant::Type::kFloat);
}

TEST(Variant, ContructAny) {
  spiderweb::Variant v(St{});

  EXPECT_EQ("", v.ToString());
  EXPECT_EQ(v.GetType(), spiderweb::Variant::Type::kAny);
}

TEST(Variant, ToOtherTypes) {
  std::vector<Metric> metrics = {
      /**
       * @brief int
       */
      Metric{spiderweb::Variant(static_cast<int16_t>(123)), spiderweb::Variant::Type::kInt16, "123",
             true, 123.f, 123},
      Metric{spiderweb::Variant(static_cast<uint16_t>(123)), spiderweb::Variant::Type::kUint16,
             "123", true, 123.f, 123},
      Metric{spiderweb::Variant(static_cast<int32_t>(123)), spiderweb::Variant::Type::kInt32, "123",
             true, 123.f, 123},
      Metric{spiderweb::Variant(static_cast<uint32_t>(123)), spiderweb::Variant::Type::kUint32,
             "123", true, 123.f, 123},
      Metric{spiderweb::Variant(static_cast<int64_t>(123)), spiderweb::Variant::Type::kInt64, "123",
             true, 123.f, 123},
      Metric{spiderweb::Variant(static_cast<uint64_t>(123)), spiderweb::Variant::Type::kUint64,
             "123", true, 123.f, 123},

      /**
       * @brief int
       */
      Metric{spiderweb::Variant(static_cast<int16_t>(-123)), spiderweb::Variant::Type::kInt16,
             "-123", true, -123.f, -123},
      Metric{spiderweb::Variant(static_cast<int32_t>(-123)), spiderweb::Variant::Type::kInt32,
             "-123", true, -123.f, -123},
      Metric{spiderweb::Variant(static_cast<int64_t>(-123)), spiderweb::Variant::Type::kInt64,
             "-123", true, -123.f, -123},

      /**
       * @brief bool
       */
      Metric{spiderweb::Variant(true), spiderweb::Variant::Type::kBool, "1", true, 1.f, 1},
      Metric{spiderweb::Variant(false), spiderweb::Variant::Type::kBool, "0", false, 0.f, 0},
      /**
       * @brief float point
       */
      Metric{spiderweb::Variant(static_cast<float>(123.f)), spiderweb::Variant::Type::kFloat,
             "123.000000", true, 123.f, 123},
      Metric{spiderweb::Variant(static_cast<float>(-123.f)), spiderweb::Variant::Type::kFloat,
             "-123.000000", true, -123.f, -123},
      /**
       * @brief string
       */
      Metric{spiderweb::Variant("123"), spiderweb::Variant::Type::kString, "123", true, 123.f, 123},
      Metric{spiderweb::Variant("-123"), spiderweb::Variant::Type::kString, "-123", true, -123.f,
             -123},
      Metric{spiderweb::Variant("0"), spiderweb::Variant::Type::kString, "0", false, 0.f, 0},
      /**
       * @brief any
       */
      Metric{spiderweb::Variant(St{}), spiderweb::Variant::Type::kAny, "", false, 0.f},
  };

  for (const auto &metric : metrics) {
    EXPECT_EQ(metric.v.ToString(), metric.expected_string);
    EXPECT_EQ(metric.v.GetType(), metric.expected_type) << static_cast<int>(metric.v.GetType());
    EXPECT_EQ(metric.v.ToBool(), metric.expected_bool);
    EXPECT_FLOAT_EQ(metric.v.ToFloat(), metric.expected_float);
    EXPECT_FLOAT_EQ(metric.v.ToBool(), metric.expected_bool);
  }
}

TEST(Variant, CopyConstruct) {
  spiderweb::Variant v1(123);
  spiderweb::Variant v2(v1);

  EXPECT_EQ(v1.ToInt(), v2.ToInt());
  EXPECT_EQ(v1.GetType(), v2.GetType());
}

TEST(Variant, MoveConstruct) {
  spiderweb::Variant v1(123);
  spiderweb::Variant v2(std::move(v1));

  EXPECT_TRUE(v1.IsNull());
  EXPECT_EQ(v2.ToInt(), 123);
  EXPECT_EQ(v2.IsNull(), false);
}

TEST(Variant, CopyAssgin) {
  spiderweb::Variant v1(123);
  spiderweb::Variant v2 = v1;

  v2 = v1;

  EXPECT_EQ(v2.ToInt(), 123);
  EXPECT_EQ(v2.IsNull(), false);
  EXPECT_EQ(v1.ToInt(), 123);
  EXPECT_EQ(v1.IsNull(), false);
  EXPECT_TRUE(v1.GetType() == v2.GetType());
}

TEST(Variant, MoveAssgin) {
  spiderweb::Variant v1(123);
  spiderweb::Variant v2;

  v2 = std::move(v1);

  EXPECT_TRUE(v1.IsNull());
  EXPECT_EQ(v2.ToInt(), 123);
  EXPECT_EQ(v2.IsNull(), false);
}

TEST(Variant, AssginOtherTypes) {
  /**
   * @brief copy construct
   */
  {
    std::string abc = "abc";

    spiderweb::Variant v1 = abc;
  }
  /**
   * @brief copy construct
   */
  {
    int                abc = 123;
    spiderweb::Variant v1 = abc;
  }
  /**
   * @brief move construct
   */
  {
    std::string        abc = "abc";
    spiderweb::Variant v1 = std::move(abc);
    EXPECT_EQ(abc, "");
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
    spiderweb::Variant v1 = std::vector<int>();
    EXPECT_EQ(v1.GetType(), spiderweb::Variant::Type::kAny);
  }

  {
    spiderweb::Variant v1 = std::vector<int>();
    EXPECT_EQ(v1.GetType(), spiderweb::Variant::Type::kAny);

    v1 = 3.f;
    EXPECT_EQ(v1.GetType(), spiderweb::Variant::Type::kFloat);

    v1 = "fff";
    EXPECT_EQ(v1.GetType(), spiderweb::Variant::Type::kString);

    v1 = true;
    EXPECT_EQ(v1.GetType(), spiderweb::Variant::Type::kBool);
  }
}
