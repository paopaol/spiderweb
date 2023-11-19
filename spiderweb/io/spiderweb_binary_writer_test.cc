#include "spiderweb_binary_writer.hpp"

#include <gmock/gmock-matchers.h>

#include "gtest/gtest.h"

TEST(spiderweb_BinaryWriter, Size) {
  spiderweb::io::DefaultStreamer                              output;
  spiderweb::io::BinaryWriter<spiderweb::io::DefaultStreamer> writer(output);

  const auto size = writer.Size(spiderweb::arch::BigEndian<2>(123), "123");
  EXPECT_EQ(size, 5);
}

TEST(spiderweb_BinaryWriter, Write) {
  spiderweb::io::DefaultStreamer output;

  output.PreAllocate(2);

  spiderweb::io::BinaryWriter<spiderweb::io::DefaultStreamer> writer(output);

  const auto size = writer.Write(spiderweb::arch::BigEndian<2>(123), "123");
  EXPECT_EQ(size, 5);

  writer.Preppend(spiderweb::arch::BigEndian<2>(size));

  writer.Write(static_cast<uint8_t>(0x01));

  EXPECT_THAT(output.Stream(),
              testing::ElementsAre(0x00, 0x05, 0x00, 0x7b, 0x31, 0x32, 0x33, 0x01));
}

TEST(spiderweb_BinaryWriter, WriteArray) {
  spiderweb::io::DefaultStreamer output;

  spiderweb::io::BinaryWriter<spiderweb::io::DefaultStreamer> writer(output);

  std::array<uint8_t, 4> data = {0x01, 0x02, 0x03, 0x04};
  writer.Write(data);

  EXPECT_THAT(output.Stream(), testing::ElementsAre(0x01, 0x02, 0x03, 0x04));
}
