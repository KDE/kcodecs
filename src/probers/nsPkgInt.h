/*  -*- C++ -*-
    SPDX-FileCopyrightText: 1998 Netscape Communications Corporation <developer@mozilla.org>

    SPDX-License-Identifier: MIT
*/

#ifndef nsPkgInt_h__
#define nsPkgInt_h__

#include <array>
#include <concepts>
#include <cstdint>
#include <span>

namespace kencodingprober
{
using nsPackedTable = std::span<const uint32_t>;
}

constexpr auto PCKXBITS(std::convertible_to<uint8_t> auto... il)
{
    constexpr auto N = sizeof...(il);
    std::array<uint32_t, N / 8> val = {0};

    int pos = 0;
    for (auto i : {uint8_t(il)...}) {
        val[pos / 8] |= (i << (4 * (pos % 8)));
        pos++;
    }
    return val;
}
static_assert(PCKXBITS(0, 0, 0, 0, 0, 0, 0, 0) == std::array<uint32_t, 1>{0x00000000});
static_assert(PCKXBITS(0, 0, 0, 0, 0, 0, 0, 1) == std::array<uint32_t, 1>{0x10000000});
static_assert(PCKXBITS(1, 0, 0, 0, 0, 0, 0, 0) == std::array<uint32_t, 1>{0x00000001});
static_assert(PCKXBITS(1, 2, 3, 4, 5, 6, 7, 8) == std::array<uint32_t, 1>{0x87654321});
static_assert(PCKXBITS(8, 7, 6, 5, 4, 3, 2, 1) == std::array<uint32_t, 1>{0x12345678});

constexpr unsigned int GETFROMPCK(int index, const kencodingprober::nsPackedTable model)
{
    const auto high = index >> 3;
    const auto low = (index & 0x7) << 2;

    uint32_t data = model[high] >> low;
    return data & 0xf;
}

namespace
{
using namespace kencodingprober;
constexpr std::array data{
    // clang-format off
    PCKXBITS(1, 2, 3, 4, 5, 6, 7, 8,
             15, 14, 13, 12, 11, 10, 9, 8),
    // clang-format on
};
constexpr nsPackedTable table{data};

static_assert(GETFROMPCK(0, table) == 1);
static_assert(GETFROMPCK(9, table) == 14);
static_assert(GETFROMPCK(15, table) == 8);
}

#endif /* nsPkgInt_h__ */
