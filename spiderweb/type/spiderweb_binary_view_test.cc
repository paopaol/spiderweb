#include "spiderweb/type/spiderweb_binary_view.h"

#include "gtest/gtest.h"

class BinaryViewTest : public testing::Test {};

using spiderweb::arch::ArchType;
using spiderweb::type::BinaryView;

TEST_F(BinaryViewTest, ViewAs_Uint16) {
  uint8_t buf[2] = {0x01, 0x02};

  auto value = BinaryView::ViewAs<uint16_t>(buf, 2, ArchType::kBig);
  EXPECT_EQ(value, 258);
}

TEST_F(BinaryViewTest, ViewAs_Float1) {
  uint8_t buf[4] = {0x47, 0xf1, 0x20, 0x00};

  auto value = BinaryView::ViewAs<float>(buf, 4, ArchType::kBig);
  EXPECT_FLOAT_EQ(value, 123456.00);
}

TEST_F(BinaryViewTest, ViewAs_Float2) {
  uint8_t buf[4] = {0x42, 0xf7, 0x00, 0x00};

  auto value = BinaryView::ViewAs<float>(buf, 4, ArchType::kBig);
  EXPECT_FLOAT_EQ(value, 123.5);
}
