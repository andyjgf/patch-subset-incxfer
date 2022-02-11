#include "patch_subset/bit_input_buffer.h"

#include <bitset>
#include <string>

#include "gtest/gtest.h"

namespace patch_subset {

using std::bitset;
using std::string;

class BitInputBufferTest : public ::testing::Test {};

TEST_F(BitInputBufferTest, SingleByte4) {
  BitInputBuffer bin(string{0b00000000, 0b00001111});
  //                        ^ d1 bf4 ^
  EXPECT_EQ(BF4, bin.GetBranchFactor());
  EXPECT_EQ(1, bin.Depth());
  uint32_t out = UINT32_MAX;
  EXPECT_TRUE(bin.read(&out));
  EXPECT_EQ(out, 0xF);
  EXPECT_TRUE(bin.read(&out));
  EXPECT_EQ(out, 0x0);
  EXPECT_FALSE(bin.read(&out));
}

TEST_F(BitInputBufferTest, SingleRead8) {
  BitInputBuffer bin(string{0b00000001, 0x2F});
  //                        ^ d1 bf8 ^
  EXPECT_EQ(BF8, bin.GetBranchFactor());
  EXPECT_EQ(1, bin.Depth());
  uint32_t out = UINT32_MAX;
  EXPECT_TRUE(bin.read(&out));
  EXPECT_EQ(out, 0x2F);
  EXPECT_FALSE(bin.read(&out));
}

TEST_F(BitInputBufferTest, SingleRead16) {
  BitInputBuffer bin(string{0b00000010, 0x7F, 0x10});
  //                        ^ d1 bf16^
  EXPECT_EQ(BF16, bin.GetBranchFactor());
  EXPECT_EQ(1, bin.Depth());
  uint32_t out = UINT32_MAX;
  EXPECT_TRUE(bin.read(&out));
  EXPECT_EQ(out, 0x107F);
  EXPECT_FALSE(bin.read(&out));
}

TEST_F(BitInputBufferTest, SingleRead32) {
  BitInputBuffer bin(string{0b00000011, 0x11, 0x22, 0x33, 0x44});
  //                        ^ d1 bf32^
  EXPECT_EQ(BF32, bin.GetBranchFactor());
  EXPECT_EQ(1, bin.Depth());
  uint32_t out = UINT32_MAX;
  EXPECT_TRUE(bin.read(&out));
  EXPECT_EQ(out, 0x44332211u);
  EXPECT_FALSE(bin.read(&out));
}

TEST_F(BitInputBufferTest, Empty) {
  uint32_t out;
  EXPECT_FALSE(BitInputBuffer("").read(&out));
  EXPECT_FALSE(BitInputBuffer(string{0x00}).read(&out));

  EXPECT_FALSE(BitInputBuffer(string{0x01}).read(&out));

  EXPECT_FALSE(BitInputBuffer(string{0x02}).read(&out));
  EXPECT_FALSE(BitInputBuffer(string{0x02, 0x01}).read(&out));

  EXPECT_FALSE(BitInputBuffer(string{0x03}).read(&out));
  EXPECT_FALSE(BitInputBuffer(string{0x03, 0x01}).read(&out));
  EXPECT_FALSE(BitInputBuffer(string{0x03, 0x01, 0x01}).read(&out));
  EXPECT_FALSE(BitInputBuffer(string{0x03, 0x01, 0x01, 0x01}).read(&out));
}

TEST_F(BitInputBufferTest, Null) {
  EXPECT_FALSE(BitInputBuffer(string{0x00}).read(nullptr));
  EXPECT_FALSE(BitInputBuffer(string{0x01}).read(nullptr));
  EXPECT_FALSE(BitInputBuffer(string{0x02}).read(nullptr));
  EXPECT_FALSE(BitInputBuffer(string{0x03}).read(nullptr));
}

TEST_F(BitInputBufferTest, ReservedBitsIgnored) {
  for (string s : {string{0b00000000}, string{0b01000000},
                   string{(char)0b10000000}, string{(char)0b11000000}}) {
    BitInputBuffer bin(s);
    EXPECT_EQ(BF4, bin.GetBranchFactor());
    EXPECT_EQ(1, bin.Depth());
  }
  for (string s : {string{0b00000001}, string{0b01000001},
                   string{(char)0b10000001}, string{(char)0b11000001}}) {
    BitInputBuffer bin(s);
    EXPECT_EQ(BF8, bin.GetBranchFactor());
    EXPECT_EQ(1, bin.Depth());
  }
  for (string s : {string{0b00000010}, string{0b01000010},
                   string{(char)0b10000010}, string{(char)0b11000010}}) {
    BitInputBuffer bin(s);
    EXPECT_EQ(BF16, bin.GetBranchFactor());
    EXPECT_EQ(1, bin.Depth());
  }
  for (string s : {string{0b00000011}, string{0b01000011},
                   string{(char)0b10000011}, string{(char)0b11000011}}) {
    BitInputBuffer bin(s);
    EXPECT_EQ(BF32, bin.GetBranchFactor());
    EXPECT_EQ(1, bin.Depth());
  }
}

}  // namespace patch_subset
