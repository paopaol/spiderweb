#include "spiderweb/io/spiderweb_bitmap_readwriter.h"

#include "gtest/gtest.h"

TEST(BitmapReader, ReadOneBit) {
  {
    uint32_t value = 0x80000000;

    spiderweb::io::BitmapReader<uint32_t, 31, 31> r(value);
    EXPECT_EQ(r.Value(), 1);
  }
  {
    uint32_t value = 0x40000;

    spiderweb::io::BitmapReader<uint32_t, 18, 18> r(value);
    EXPECT_EQ(r.Value(), 1);
  }
  {
    uint8_t value = 0x01;

    spiderweb::io::BitmapReader<uint8_t, 0, 0> r(value);
    EXPECT_EQ(r.Value(), 1);
  }
  {
    uint64_t value = 0x01;

    spiderweb::io::BitmapReader<uint64_t, 0, 0> r(value);
    EXPECT_EQ(r.Value(), 1);
  }
}

TEST(BitmapReader, ReadSomeBit) {
  {
    uint32_t value = 0x80000000;

    spiderweb::io::BitmapReader<uint32_t, 28, 31> r(value);
    EXPECT_EQ(r.Value(), 8);
  }
  {
    uint32_t value = 0x40000;

    spiderweb::io::BitmapReader<uint32_t, 16, 18> r(value);
    EXPECT_EQ(r.Value(), 4);
  }
  {
    uint8_t value = 0x01;

    spiderweb::io::BitmapReader<uint8_t, 0, 1> r(value);
    EXPECT_EQ(r.Value(), 1);
  }
  {
    uint64_t value = 0x01;

    spiderweb::io::BitmapReader<uint64_t, 0, 1> r(value);
    EXPECT_EQ(r.Value(), 1);
  }
}

TEST(BitmapReader, WriteOneBit) {
  {
    uint32_t value = 0x00000000;

    spiderweb::io::BitmapWriter<uint32_t, 31, 31> w(value);
    w.Set(1);
    EXPECT_EQ(value, 0x80000000);
  }
  {
    uint32_t value = 0x00000;

    spiderweb::io::BitmapWriter<uint32_t, 18, 18> w(value);
    w.Set(1);
    EXPECT_EQ(value, 0x40000);
  }
  {
    uint8_t value = 0x00;

    spiderweb::io::BitmapWriter<uint8_t, 0, 0> w(value);
    w.Set(1);
    EXPECT_EQ(value, 1);
  }
}

TEST(BitmapReader, WriteSomeBit) {
  {
    uint32_t value = 0x00000000;

    spiderweb::io::BitmapWriter<uint32_t, 28, 31> w(value);
    w.Set(8);
    EXPECT_EQ(value, 0x80000000);
  }
  {
    uint32_t value = 0x00000;

    spiderweb::io::BitmapWriter<uint32_t, 16, 18> w(value);
    w.Set(4);
    EXPECT_EQ(value, 0x40000);
  }
  {
    uint8_t value = 0x00;

    spiderweb::io::BitmapWriter<uint8_t, 0, 1> w(value);
    w.Set(1);
    EXPECT_EQ(value, 1);
  }
}
