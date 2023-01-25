#include "pawn/bits.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace security::pawn {
namespace {

using ::testing::Eq;
using ::testing::IsTrue;

TEST(BitsTest, RawBits) {
  EXPECT_THAT(bits::Raw<15>(0xFFFF), Eq(0x8000));  // 1000 0000 0000 0000
  EXPECT_THAT((bits::Raw<6, 2>(0xFF)), Eq(0x7C));  //           0111 1100
}

TEST(BitsTest, ValueOfBits) {
  EXPECT_THAT(bits::Value<15>(0xFFFF), Eq(0x1));
  EXPECT_THAT((bits::Value<6, 2>(0xFF)), Eq(0x1F));
}

TEST(BitsTest, TestBit) {
  EXPECT_THAT(bits::Test<15>(0xFFFF), IsTrue());
}

TEST(BitsTest, SetBits) {
  EXPECT_THAT(bits::Set<15>(1), Eq(0x8000));       // 1000 0000 0000 0000
  EXPECT_THAT((bits::Set<6, 2>(0x1F)), Eq(0x7C));  //           0111 1100
}

}  // namespace
}  // namespace security::pawn
