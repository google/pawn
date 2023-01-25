// Copyright 2014-2023 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Convenience bit-twiddling routines, implemented as constexpr templates
// instead of the usual macro hackery.
// All bit offsets in this file are zero-based and work on little-endian
// systems only.

#ifndef PAWN_BITS_H_
#define PAWN_BITS_H_

#include <type_traits>

namespace security::pawn::bits {

// Extracts from "bits" the raw (unshifted) bit pattern between the specified
// most-significant and least-significant bits (inclusive).
// Examples:
//   QCHECK_EQ(bits::Raw<15>(0xFFFF), 0x8000);  // 1000 0000 0000 0000
//   QCHECK_EQ(bits::Raw<6, 2>(0xFF), 0x7C);    //           0111 1100
template <int kMsb, int kLsb = kMsb, typename IntT>
constexpr IntT Raw(IntT bits) {
  using UIntT = std::make_unsigned_t<IntT>;
  return static_cast<IntT>(static_cast<UIntT>(bits) &
                           ((UIntT(1) << (kMsb - 1)) - 1) << kLsb);
}

// Extracts from "bits" the value of the bit pattern between the specified
// most-significant and least-significant bits (inclusive). This is equivalent
// to right-shifting bits::Raw<>() by the index of its least-significant bit.
// Examples:
//   QCHECK_EQ(bits::Value<15>(0xFFFF), 1);
//   QCHECK_EQ(bits::Value<6, 2>(0xFF), 0x1F);
template <int kMsb, int kLsb = kMsb, typename IntT>
constexpr IntT Value(IntT bits) {
  using UIntT = std::make_unsigned_t<IntT>;
  return bits >> kLsb & ((1 << (kMsb - kLsb + 1)) - 1);
}

// Tests whether the kTest-th bit is set.
template <int kTest, typename IntT>
constexpr bool Test(IntT bits) {
  using UIntT = std::make_unsigned_t<IntT>;
  return (static_cast<UIntT>(bits) & (UIntT(1) << kTest)) != 0;
}

// Constructs a new value between the specified most-significant and
// least-significant bits (inclusive).
// Examples:
//   QCHECK_EQ(bits::Set<15>(1), 0x8000);     // 1000 0000 0000 0000
//   QCHECK_EQ(bits::Set<6, 2>(0x1F), 0x7C);  //           0111 1100
template <int kMsb, int kLsb = kMsb, typename IntT,
          typename OutT = decltype(1u << kMsb)>
constexpr OutT Set(IntT value) {
  return static_cast<OutT>(
      (static_cast<OutT>(value) & ((OutT(1) << (kMsb - kLsb + 1)) - 1))
      << kLsb);
}

}  // namespace security::pawn::bits

#endif  // PAWN_BITS_H_
