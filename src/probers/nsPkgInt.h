/*  -*- C++ -*-
    SPDX-FileCopyrightText: 1998 Netscape Communications Corporation <developer@mozilla.org>

    SPDX-License-Identifier: MIT
*/

#ifndef nsPkgInt_h__
#define nsPkgInt_h__

#include <concepts>
#include <cstdint>

namespace kencodingprober
{
typedef enum {
    eIdxSft4bits = 3,
    eIdxSft8bits = 2,
    eIdxSft16bits = 1,
} nsIdxSft;

typedef enum {
    eSftMsk4bits = 7,
    eSftMsk8bits = 3,
    eSftMsk16bits = 1,
} nsSftMsk;

typedef enum {
    eBitSft4bits = 2,
    eBitSft8bits = 3,
    eBitSft16bits = 4,
} nsBitSft;

typedef enum {
    eUnitMsk4bits = 0x0000000FL,
    eUnitMsk8bits = 0x000000FFL,
    eUnitMsk16bits = 0x0000FFFFL,
} nsUnitMsk;

typedef struct nsPkgInt {
    nsIdxSft idxsft;
    nsSftMsk sftmsk;
    nsBitSft bitsft;
    nsUnitMsk unitmsk;
    const unsigned int *data;
} nsPkgInt;
}

constexpr uint32_t PCK4BITS(std::convertible_to<uint8_t> auto... il)
{
    uint32_t val = 0;

    int pos = 0;
    for (auto i : {uint8_t(il)...}) {
        val |= (i << (4 * pos));
        pos++;
    }
    return val;
}
static_assert(PCK4BITS(1, 2, 3, 4, 5, 6, 7, 8) == 0x87654321);
static_assert(PCK4BITS(8, 7, 6, 5, 4, 3, 2, 1) == 0x12345678);

constexpr unsigned int GETFROMPCK(int index, const kencodingprober::nsPkgInt &table)
{
    const auto high = index >> table.idxsft;
    const auto low = (index & table.sftmsk) << table.bitsft;

    uint32_t data = table.data[high] >> low;
    return data & table.unitmsk;
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
constexpr nsPkgInt table{eIdxSft4bits, eSftMsk4bits, eBitSft4bits, eUnitMsk4bits, data.data()};

static_assert(GETFROMPCK(0, table) == 1);
static_assert(GETFROMPCK(9, table) == 14);
static_assert(GETFROMPCK(15, table) == 8);
}

#endif /* nsPkgInt_h__ */
