#include "spiderweb_binary_writer.hpp"

#include <gmock/gmock-matchers.h>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "spiderweb_eventloop.h"

TEST(spiderweb_BinaryWriter, Size) {
  spiderweb::DefaultStreamer                          output;
  spiderweb::BinaryWriter<spiderweb::DefaultStreamer> writer(output);

  const auto size = writer.Size(spiderweb::arch::BigEndian<2>(123), "123");
  EXPECT_EQ(size, 5);
}

TEST(spiderweb_BinaryWriter, Write) {
  spiderweb::DefaultStreamer output;

  output.PreAllocate(2);

  spiderweb::BinaryWriter<spiderweb::DefaultStreamer> writer(output);

  const auto size = writer.Write(spiderweb::arch::BigEndian<2>(123), "123");
  EXPECT_EQ(size, 5);

  writer.Preppend(spiderweb::arch::BigEndian<2>(size));

  EXPECT_THAT(output.Stream(),
              testing::ElementsAre(0x00, 0x05, 0x00, 0x7b, 0x31, 0x32, 0x33));
}
