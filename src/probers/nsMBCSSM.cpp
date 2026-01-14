/*  -*- C++ -*-
    SPDX-FileCopyrightText: 1998 Netscape Communications Corporation <developer@mozilla.org>

    SPDX-License-Identifier: MIT
*/

#include "nsCodingStateMachine.h"

/*
Modification from frank tang's original work:
. 0x00 is allowed as a legal character. Since some web pages contains this char in
  text stream.
*/

// BIG5

namespace kencodingprober
{
constexpr std::array<unsigned int, 256 / 8> BIG5_cls = PCKXBITS(
    // clang-format off
    // 0,1,1,1,1,1,1,1,  // 00 - 07
    1, 1, 1, 1, 1, 1, 1, 1, // 00 - 07    //allow 0x00 as legal value
    1, 1, 1, 1, 1, 1, 0, 0, // 08 - 0f
    1, 1, 1, 1, 1, 1, 1, 1, // 10 - 17
    1, 1, 1, 0, 1, 1, 1, 1, // 18 - 1f
    1, 1, 1, 1, 1, 1, 1, 1, // 20 - 27
    1, 1, 1, 1, 1, 1, 1, 1, // 28 - 2f
    1, 1, 1, 1, 1, 1, 1, 1, // 30 - 37
    1, 1, 1, 1, 1, 1, 1, 1, // 38 - 3f
    2, 2, 2, 2, 2, 2, 2, 2, // 40 - 47
    2, 2, 2, 2, 2, 2, 2, 2, // 48 - 4f
    2, 2, 2, 2, 2, 2, 2, 2, // 50 - 57
    2, 2, 2, 2, 2, 2, 2, 2, // 58 - 5f
    2, 2, 2, 2, 2, 2, 2, 2, // 60 - 67
    2, 2, 2, 2, 2, 2, 2, 2, // 68 - 6f
    2, 2, 2, 2, 2, 2, 2, 2, // 70 - 77
    2, 2, 2, 2, 2, 2, 2, 1, // 78 - 7f
    4, 4, 4, 4, 4, 4, 4, 4, // 80 - 87
    4, 4, 4, 4, 4, 4, 4, 4, // 88 - 8f
    4, 4, 4, 4, 4, 4, 4, 4, // 90 - 97
    4, 4, 4, 4, 4, 4, 4, 4, // 98 - 9f
    4, 3, 3, 3, 3, 3, 3, 3, // a0 - a7
    3, 3, 3, 3, 3, 3, 3, 3, // a8 - af
    3, 3, 3, 3, 3, 3, 3, 3, // b0 - b7
    3, 3, 3, 3, 3, 3, 3, 3, // b8 - bf
    3, 3, 3, 3, 3, 3, 3, 3, // c0 - c7
    3, 3, 3, 3, 3, 3, 3, 3, // c8 - cf
    3, 3, 3, 3, 3, 3, 3, 3, // d0 - d7
    3, 3, 3, 3, 3, 3, 3, 3, // d8 - df
    3, 3, 3, 3, 3, 3, 3, 3, // e0 - e7
    3, 3, 3, 3, 3, 3, 3, 3, // e8 - ef
    3, 3, 3, 3, 3, 3, 3, 3, // f0 - f7
    3, 3, 3, 3, 3, 3, 3, 0 // f8 - ff
    // clang-format on
);

constexpr std::array<unsigned int, 3> BIG5_st = PCKXBITS(
    // clang-format off
    eError, eStart, eStart,      3, eError, // eStart
    eError, eError, eError, eError, eError, // eError
    eItsMe, eItsMe, eItsMe, eItsMe, eItsMe, // eItsMe
    eError, eError, eStart, eStart, eStart, // 3
    eStart, eStart, eStart, eStart // Padding
    // clang-format on
);

static const unsigned int Big5CharLenTable[] = {0, 1, 1, 2, 0};

const SMModel Big5SMModel = {
    {eIdxSft4bits, eSftMsk4bits, eBitSft4bits, eUnitMsk4bits, BIG5_cls.data()},
    5,
    {eIdxSft4bits, eSftMsk4bits, eBitSft4bits, eUnitMsk4bits, BIG5_st.data()},
    Big5CharLenTable,
    "Big5",
};

constexpr std::array<unsigned int, 256 / 8> EUCJP_cls = PCKXBITS(
    // clang-format off
    // 5,4,4,4,4,4,4,4,  // 00 - 07
    4, 4, 4, 4, 4, 4, 4, 4, // 00 - 07
    4, 4, 4, 4, 4, 4, 5, 5, // 08 - 0f
    4, 4, 4, 4, 4, 4, 4, 4, // 10 - 17
    4, 4, 4, 5, 4, 4, 4, 4, // 18 - 1f
    4, 4, 4, 4, 4, 4, 4, 4, // 20 - 27
    4, 4, 4, 4, 4, 4, 4, 4, // 28 - 2f
    4, 4, 4, 4, 4, 4, 4, 4, // 30 - 37
    4, 4, 4, 4, 4, 4, 4, 4, // 38 - 3f
    4, 4, 4, 4, 4, 4, 4, 4, // 40 - 47
    4, 4, 4, 4, 4, 4, 4, 4, // 48 - 4f
    4, 4, 4, 4, 4, 4, 4, 4, // 50 - 57
    4, 4, 4, 4, 4, 4, 4, 4, // 58 - 5f
    4, 4, 4, 4, 4, 4, 4, 4, // 60 - 67
    4, 4, 4, 4, 4, 4, 4, 4, // 68 - 6f
    4, 4, 4, 4, 4, 4, 4, 4, // 70 - 77
    4, 4, 4, 4, 4, 4, 4, 4, // 78 - 7f
    5, 5, 5, 5, 5, 5, 5, 5, // 80 - 87
    5, 5, 5, 5, 5, 5, 1, 3, // 88 - 8f
    5, 5, 5, 5, 5, 5, 5, 5, // 90 - 97
    5, 5, 5, 5, 5, 5, 5, 5, // 98 - 9f
    5, 2, 2, 2, 2, 2, 2, 2, // a0 - a7
    2, 2, 2, 2, 2, 2, 2, 2, // a8 - af
    2, 2, 2, 2, 2, 2, 2, 2, // b0 - b7
    2, 2, 2, 2, 2, 2, 2, 2, // b8 - bf
    2, 2, 2, 2, 2, 2, 2, 2, // c0 - c7
    2, 2, 2, 2, 2, 2, 2, 2, // c8 - cf
    2, 2, 2, 2, 2, 2, 2, 2, // d0 - d7
    2, 2, 2, 2, 2, 2, 2, 2, // d8 - df
    0, 0, 0, 0, 0, 0, 0, 0, // e0 - e7
    0, 0, 0, 0, 0, 0, 0, 0, // e8 - ef
    0, 0, 0, 0, 0, 0, 0, 0, // f0 - f7
    0, 0, 0, 0, 0, 0, 0, 5  // f8 - ff
    // clang-format on
);

constexpr std::array<unsigned int, 5> EUCJP_st = PCKXBITS(
    // clang-format off
         3,      4,      3,      5, eStart, eError, // eStart
    eError, eError, eError, eError, eError, eError, // eError
    eItsMe, eItsMe, eItsMe, eItsMe, eItsMe, eItsMe, // eItsMe
    eStart, eError, eStart, eError, eError, eError, // 3
    eError, eError, eStart, eError, eError, eError, // 4
         3, eError,      3, eError, eError, eError, // 5
    eStart, eStart, eStart, eStart // Padding
    // clang-format on
);

static const unsigned int EUCJPCharLenTable[] = {2, 2, 2, 3, 1, 0};

const SMModel EUCJPSMModel = {
    {eIdxSft4bits, eSftMsk4bits, eBitSft4bits, eUnitMsk4bits, EUCJP_cls.data()},
    6,
    {eIdxSft4bits, eSftMsk4bits, eBitSft4bits, eUnitMsk4bits, EUCJP_st.data()},
    EUCJPCharLenTable,
    "EUC-JP",
};

constexpr std::array<unsigned int, 256 / 8> EUCKR_cls = PCKXBITS(
    // clang-format off
    // 0,1,1,1,1,1,1,1,  // 00 - 07
    1, 1, 1, 1, 1, 1, 1, 1, // 00 - 07
    1, 1, 1, 1, 1, 1, 0, 0, // 08 - 0f
    1, 1, 1, 1, 1, 1, 1, 1, // 10 - 17
    1, 1, 1, 0, 1, 1, 1, 1, // 18 - 1f
    1, 1, 1, 1, 1, 1, 1, 1, // 20 - 27
    1, 1, 1, 1, 1, 1, 1, 1, // 28 - 2f
    1, 1, 1, 1, 1, 1, 1, 1, // 30 - 37
    1, 1, 1, 1, 1, 1, 1, 1, // 38 - 3f
    1, 1, 1, 1, 1, 1, 1, 1, // 40 - 47
    1, 1, 1, 1, 1, 1, 1, 1, // 48 - 4f
    1, 1, 1, 1, 1, 1, 1, 1, // 50 - 57
    1, 1, 1, 1, 1, 1, 1, 1, // 58 - 5f
    1, 1, 1, 1, 1, 1, 1, 1, // 60 - 67
    1, 1, 1, 1, 1, 1, 1, 1, // 68 - 6f
    1, 1, 1, 1, 1, 1, 1, 1, // 70 - 77
    1, 1, 1, 1, 1, 1, 1, 1, // 78 - 7f
    0, 0, 0, 0, 0, 0, 0, 0, // 80 - 87
    0, 0, 0, 0, 0, 0, 0, 0, // 88 - 8f
    0, 0, 0, 0, 0, 0, 0, 0, // 90 - 97
    0, 0, 0, 0, 0, 0, 0, 0, // 98 - 9f
    0, 2, 2, 2, 2, 2, 2, 2, // a0 - a7
    2, 2, 2, 2, 2, 3, 3, 3, // a8 - af
    2, 2, 2, 2, 2, 2, 2, 2, // b0 - b7
    2, 2, 2, 2, 2, 2, 2, 2, // b8 - bf
    2, 2, 2, 2, 2, 2, 2, 2, // c0 - c7
    2, 3, 2, 2, 2, 2, 2, 2, // c8 - cf
    2, 2, 2, 2, 2, 2, 2, 2, // d0 - d7
    2, 2, 2, 2, 2, 2, 2, 2, // d8 - df
    2, 2, 2, 2, 2, 2, 2, 2, // e0 - e7
    2, 2, 2, 2, 2, 2, 2, 2, // e8 - ef
    2, 2, 2, 2, 2, 2, 2, 2, // f0 - f7
    2, 2, 2, 2, 2, 2, 2, 0  // f8 - ff
    // clang-format on
);

constexpr std::array<unsigned int, 2> EUCKR_st = PCKXBITS(
    // clang-format off
    eError, eStart,      3, eError, // eStart
    eError, eError, eError, eError, // eError
    eItsMe, eItsMe, eItsMe, eItsMe, // eItsMe
    eError, eError, eStart, eStart  // 3
    // clang-format on
);

static const unsigned int EUCKRCharLenTable[] = {0, 1, 2, 0};

const SMModel EUCKRSMModel = {
    {eIdxSft4bits, eSftMsk4bits, eBitSft4bits, eUnitMsk4bits, EUCKR_cls.data()},
    4,
    {eIdxSft4bits, eSftMsk4bits, eBitSft4bits, eUnitMsk4bits, EUCKR_st.data()},
    EUCKRCharLenTable,
    "EUC-KR",
};

// the following state machine data was created by perl script in
// intl/chardet/tools. It should be the same as in PSM detector.
constexpr std::array<unsigned int, 256 / 8> GB18030_cls = PCKXBITS(
    // clang-format off
    1, 1, 1, 1, 1, 1, 1, 1, // 00 - 07
    1, 1, 1, 1, 1, 1, 0, 0, // 08 - 0f
    1, 1, 1, 1, 1, 1, 1, 1, // 10 - 17
    1, 1, 1, 0, 1, 1, 1, 1, // 18 - 1f
    1, 1, 1, 1, 1, 1, 1, 1, // 20 - 27
    1, 1, 1, 1, 1, 1, 1, 1, // 28 - 2f
    3, 3, 3, 3, 3, 3, 3, 3, // 30 - 37
    3, 3, 1, 1, 1, 1, 1, 1, // 38 - 3f
    2, 2, 2, 2, 2, 2, 2, 2, // 40 - 47
    2, 2, 2, 2, 2, 2, 2, 2, // 48 - 4f
    2, 2, 2, 2, 2, 2, 2, 2, // 50 - 57
    2, 2, 2, 2, 2, 2, 2, 2, // 58 - 5f
    2, 2, 2, 2, 2, 2, 2, 2, // 60 - 67
    2, 2, 2, 2, 2, 2, 2, 2, // 68 - 6f
    2, 2, 2, 2, 2, 2, 2, 2, // 70 - 77
    2, 2, 2, 2, 2, 2, 2, 4, // 78 - 7f
    5, 6, 6, 6, 6, 6, 6, 6, // 80 - 87
    6, 6, 6, 6, 6, 6, 6, 6, // 88 - 8f
    6, 6, 6, 6, 6, 6, 6, 6, // 90 - 97
    6, 6, 6, 6, 6, 6, 6, 6, // 98 - 9f
    6, 6, 6, 6, 6, 6, 6, 6, // a0 - a7
    6, 6, 6, 6, 6, 6, 6, 6, // a8 - af
    6, 6, 6, 6, 6, 6, 6, 6, // b0 - b7
    6, 6, 6, 6, 6, 6, 6, 6, // b8 - bf
    6, 6, 6, 6, 6, 6, 6, 6, // c0 - c7
    6, 6, 6, 6, 6, 6, 6, 6, // c8 - cf
    6, 6, 6, 6, 6, 6, 6, 6, // d0 - d7
    6, 6, 6, 6, 6, 6, 6, 6, // d8 - df
    6, 6, 6, 6, 6, 6, 6, 6, // e0 - e7
    6, 6, 6, 6, 6, 6, 6, 6, // e8 - ef
    6, 6, 6, 6, 6, 6, 6, 6, // f0 - f7
    6, 6, 6, 6, 6, 6, 6, 0  // f8 - ff
    // clang-format on
);

constexpr std::array<unsigned int, 6> GB18030_st = PCKXBITS(
    // clang-format off
    eError, eStart, eStart, eStart, eStart, eStart,      3, // eStart
    eError, eError, eError, eError, eError, eError, eError, // eError
    eItsMe, eItsMe, eItsMe, eItsMe, eItsMe, eItsMe, eItsMe, // eItsMe
    eError, eError, eStart,      4, eError, eStart, eStart, // 3
    eError, eError, eError, eError, eError, eError,      5, // 4
    eError, eError, eError, eItsMe, eError, eError, eError, // 5
    eStart, eStart, eStart, eStart, eStart, eStart // Padding
    // clang-format on
);

// To be accurate, the length of class 6 can be either 2 or 4.
// But it is not necessary to discriminate between the two since
// it is used for frequency analysis only, and we are validating
// each code range there as well. So it is safe to set it to be
// 2 here.
static const unsigned int GB18030CharLenTable[] = {0, 1, 1, 1, 1, 1, 2};

const SMModel GB18030SMModel = {
    {eIdxSft4bits, eSftMsk4bits, eBitSft4bits, eUnitMsk4bits, GB18030_cls.data()},
    7,
    {eIdxSft4bits, eSftMsk4bits, eBitSft4bits, eUnitMsk4bits, GB18030_st.data()},
    GB18030CharLenTable,
    "GB18030",
};

// sjis

constexpr std::array<unsigned int, 256 / 8> SJIS_cls = PCKXBITS(
    // clang-format off
    // 0,1,1,1,1,1,1,1,  // 00 - 07
    1, 1, 1, 1, 1, 1, 1, 1, // 00 - 07
    1, 1, 1, 1, 1, 1, 0, 0, // 08 - 0f
    1, 1, 1, 1, 1, 1, 1, 1, // 10 - 17
    1, 1, 1, 0, 1, 1, 1, 1, // 18 - 1f
    1, 1, 1, 1, 1, 1, 1, 1, // 20 - 27
    1, 1, 1, 1, 1, 1, 1, 1, // 28 - 2f
    1, 1, 1, 1, 1, 1, 1, 1, // 30 - 37
    1, 1, 1, 1, 1, 1, 1, 1, // 38 - 3f
    2, 2, 2, 2, 2, 2, 2, 2, // 40 - 47
    2, 2, 2, 2, 2, 2, 2, 2, // 48 - 4f
    2, 2, 2, 2, 2, 2, 2, 2, // 50 - 57
    2, 2, 2, 2, 2, 2, 2, 2, // 58 - 5f
    2, 2, 2, 2, 2, 2, 2, 2, // 60 - 67
    2, 2, 2, 2, 2, 2, 2, 2, // 68 - 6f
    2, 2, 2, 2, 2, 2, 2, 2, // 70 - 77
    2, 2, 2, 2, 2, 2, 2, 1, // 78 - 7f
    3, 3, 3, 3, 3, 3, 3, 3, // 80 - 87
    3, 3, 3, 3, 3, 3, 3, 3, // 88 - 8f
    3, 3, 3, 3, 3, 3, 3, 3, // 90 - 97
    3, 3, 3, 3, 3, 3, 3, 3, // 98 - 9f
    // 0xa0 is illegal in sjis encoding, but some pages does
    // contain such byte. We need to be more error forgiven.
    2, 2, 2, 2, 2, 2, 2, 2, // a0 - a7
    2, 2, 2, 2, 2, 2, 2, 2, // a8 - af
    2, 2, 2, 2, 2, 2, 2, 2, // b0 - b7
    2, 2, 2, 2, 2, 2, 2, 2, // b8 - bf
    2, 2, 2, 2, 2, 2, 2, 2, // c0 - c7
    2, 2, 2, 2, 2, 2, 2, 2, // c8 - cf
    2, 2, 2, 2, 2, 2, 2, 2, // d0 - d7
    2, 2, 2, 2, 2, 2, 2, 2, // d8 - df
    3, 3, 3, 3, 3, 3, 3, 3, // e0 - e7
    3, 3, 3, 3, 3, 4, 4, 4, // e8 - ef
    4, 4, 4, 4, 4, 4, 4, 4, // f0 - f7
    4, 4, 4, 4, 4, 0, 0, 0  // f8 - ff
    // clang-format on
);

constexpr std::array<unsigned int, 3> SJIS_st = PCKXBITS(
    // clang-format off
    eError, eStart, eStart,      3, eError, eError, // eStart
    eError, eError, eError, eError, eError, eError, // eError
    eItsMe, eItsMe, eItsMe, eItsMe, eItsMe, eItsMe, // eItsMe
    eError, eError, eStart, eStart, eStart, eStart  // 3
    // clang-format on
);

static const unsigned int SJISCharLenTable[] = {0, 1, 1, 2, 0, 0};

const SMModel SJISSMModel = {
    {eIdxSft4bits, eSftMsk4bits, eBitSft4bits, eUnitMsk4bits, SJIS_cls.data()},
    6,
    {eIdxSft4bits, eSftMsk4bits, eBitSft4bits, eUnitMsk4bits, SJIS_st.data()},
    SJISCharLenTable,
    "Shift_JIS",
};

// Class table shared by UCS2LE and UCS2BE
constexpr std::array<unsigned int, 256 / 8> UCS2_cls = PCKXBITS(
    // clang-format off
    0, 0, 0, 0, 0, 0, 0, 0, // 00 - 07
    0, 0, 1, 0, 0, 2, 0, 0, // 08 - 0f
    0, 0, 0, 0, 0, 0, 0, 0, // 10 - 17
    0, 0, 0, 0, 0, 0, 0, 0, // 18 - 1f
    0, 0, 0, 0, 0, 0, 0, 0, // 20 - 27
    0, 0, 0, 0, 0, 0, 0, 0, // 28 - 2f
    0, 0, 0, 0, 0, 0, 0, 0, // 30 - 37
    0, 0, 0, 0, 0, 0, 0, 0, // 38 - 3f
    0, 0, 0, 0, 0, 0, 0, 0, // 40 - 47
    0, 0, 0, 0, 0, 0, 0, 0, // 48 - 4f
    0, 0, 0, 0, 0, 0, 0, 0, // 50 - 57
    0, 0, 0, 0, 0, 0, 0, 0, // 58 - 5f
    0, 0, 0, 0, 0, 0, 0, 0, // 60 - 67
    0, 0, 0, 0, 0, 0, 0, 0, // 68 - 6f
    0, 0, 0, 0, 0, 0, 0, 0, // 70 - 77
    0, 0, 0, 0, 0, 0, 0, 0, // 78 - 7f
    0, 0, 0, 0, 0, 0, 0, 0, // 80 - 87
    0, 0, 0, 0, 0, 0, 0, 0, // 88 - 8f
    0, 0, 0, 0, 0, 0, 0, 0, // 90 - 97
    0, 0, 0, 0, 0, 0, 0, 0, // 98 - 9f
    0, 0, 0, 0, 0, 0, 0, 0, // a0 - a7
    0, 0, 0, 0, 0, 0, 0, 0, // a8 - af
    0, 0, 0, 0, 0, 0, 0, 0, // b0 - b7
    0, 0, 0, 0, 0, 0, 0, 0, // b8 - bf
    0, 0, 0, 0, 0, 0, 0, 0, // c0 - c7
    0, 0, 0, 0, 0, 0, 0, 0, // c8 - cf
    0, 0, 0, 0, 0, 0, 0, 0, // d0 - d7
    6, 6, 6, 6, 7, 7, 7, 7, // d8 - df
    0, 0, 0, 0, 0, 0, 0, 0, // e0 - e7
    0, 0, 0, 0, 0, 0, 0, 0, // e8 - ef
    0, 0, 0, 0, 0, 0, 0, 0, // f0 - f7
    0, 0, 0, 0, 0, 0, 4, 5 // f8 - ff
    // clang-format on
);

// eStart and "6" are MSB states, "5" and "7" are LSB
// 9 is High Surrogate low byte
// 10 is Low Surrogate high byte
constexpr std::array<unsigned int, 11> UCS2BE_st = PCKXBITS(
    // clang-format off
         5,      7,      7, eError,      4,      3,      9, eError, // 0
    eError, eError, eError, eError, eError, eError, eError, eError, // 1
    eItsMe, eItsMe, eItsMe, eItsMe, eItsMe, eItsMe, eItsMe, eItsMe, // 2
         6,      6,      6,      6, eError, eError,      9,      6, // 3
         6,      6,      6,      6,      6, eItsMe,      6,      6, // 4
         6,      6,      6,      6,      6,      6,      6,      6, // 5
         5,      7,      7, eError,      5,      8,      9, eError, // 6
         6,      6, eError,      6,      6,      6,      9,      6, // 7
         6,      6,      6,      6, eError, eError,      9,      6, // 8
        10,     10,     10,     10,     10,     10,     10,     10, // 9
    eError, eError, eError, eError, eError, eError, eError,      6 // 10
    // clang-format on
);

static const unsigned int UCS2BECharLenTable[] = {2, 2, 2, 0, 2, 2, 4, 4};

const SMModel UCS2BESMModel = {
    {eIdxSft4bits, eSftMsk4bits, eBitSft4bits, eUnitMsk4bits, UCS2_cls.data()},
    8,
    {eIdxSft4bits, eSftMsk4bits, eBitSft4bits, eUnitMsk4bits, UCS2BE_st.data()},
    UCS2BECharLenTable,
    "UTF-16BE",
};

// eStart and "5" are LSB states, "3", "4", "6", "7" and "8" are MSB
// 9 is Low Surrogate LSB
// 10 is Low Surrogate MSB
constexpr std::array<unsigned int, 11> UCS2LE_st = PCKXBITS(
    // clang-format off
         6,      6,      7,      6,      4,      3,      6,      6, // 0
    eError, eError, eError, eError, eError, eError, eError, eError, // 1
    eItsMe, eItsMe, eItsMe, eItsMe, eItsMe, eItsMe, eItsMe, eItsMe, // 2
         5,      5,      5, eError, eItsMe, eError,      9, eError, // 3
         5,      5,      5, eError,      5, eError,      9, eError, // 4
         6,      6,      7,      6,      8,      8,      6,      6, // 5
         5,      5,      5, eError,      5,      5,      9, eError, // 6
         5, eError, eError, eError,      5,      5,      9, eError, // 7
         5,      5,      5, eError,      5, eError,      9, eError, // 8
        10,     10,     10,     10,     10,     10,     10,     10, // 9
    eError, eError, eError, eError, eError, eError, eError,      5 // 10
    // clang-format on
);


static const unsigned int UCS2LECharLenTable[] = {2, 2, 2, 2, 2, 2, 4, 4};

const SMModel UCS2LESMModel = {
    {eIdxSft4bits, eSftMsk4bits, eBitSft4bits, eUnitMsk4bits, UCS2_cls.data()},
    8,
    {eIdxSft4bits, eSftMsk4bits, eBitSft4bits, eUnitMsk4bits, UCS2LE_st.data()},
    UCS2LECharLenTable,
    "UTF-16LE",
};

constexpr std::array<unsigned int, 256 / 8> UTF8_cls = PCKXBITS(
    // clang-format off
    // 0,1,1,1,1,1,1,1,  // 00 - 07
    1, 1, 1, 1, 1, 1, 1, 1, // 00 - 07  //allow 0x00 as a legal value
    1, 1, 1, 1, 1, 1, 0, 0, // 08 - 0f
    1, 1, 1, 1, 1, 1, 1, 1, // 10 - 17
    1, 1, 1, 0, 1, 1, 1, 1, // 18 - 1f
    1, 1, 1, 1, 1, 1, 1, 1, // 20 - 27
    1, 1, 1, 1, 1, 1, 1, 1, // 28 - 2f
    1, 1, 1, 1, 1, 1, 1, 1, // 30 - 37
    1, 1, 1, 1, 1, 1, 1, 1, // 38 - 3f
    1, 1, 1, 1, 1, 1, 1, 1, // 40 - 47
    1, 1, 1, 1, 1, 1, 1, 1, // 48 - 4f
    1, 1, 1, 1, 1, 1, 1, 1, // 50 - 57
    1, 1, 1, 1, 1, 1, 1, 1, // 58 - 5f
    1, 1, 1, 1, 1, 1, 1, 1, // 60 - 67
    1, 1, 1, 1, 1, 1, 1, 1, // 68 - 6f
    1, 1, 1, 1, 1, 1, 1, 1, // 70 - 77
    1, 1, 1, 1, 1, 1, 1, 1, // 78 - 7f
    2, 2, 2, 2, 2, 2, 2, 2, // 80 - 87
    2, 2, 2, 2, 2, 2, 2, 2, // 88 - 8f
    3, 3, 3, 3, 3, 3, 3, 3, // 90 - 97
    3, 3, 3, 3, 3, 3, 3, 3, // 98 - 9f
    4, 4, 4, 4, 4, 4, 4, 4, // a0 - a7
    4, 4, 4, 4, 4, 4, 4, 4, // a8 - af
    4, 4, 4, 4, 4, 4, 4, 4, // b0 - b7
    4, 4, 4, 4, 4, 4, 4, 4, // b8 - bf
    0, 0, 5, 5, 5, 5, 5, 5, // c0 - c7
    5, 5, 5, 5, 5, 5, 5, 5, // c8 - cf
    5, 5, 5, 5, 5, 5, 5, 5, // d0 - d7
    5, 5, 5, 5, 5, 5, 5, 5, // d8 - df
    6, 7, 7, 7, 7, 7, 7, 7, // e0 - e7
    7, 7, 7, 7, 7, 8, 7, 7, // e7 - ef
    9, 10, 10, 10, 11, 0, 0, 0, // f0 - f7
    0, 0, 0, 0, 0, 0, 0, 0 // f8 - ff
    // clang-format on
);

constexpr std::array<unsigned int, 10 * 12 / 8> UTF8_st = PCKXBITS(
    // clang-format off
    // byteclass:
    //   0       1       2       3       4       5       6       7       8       9      10      11
    eError, eStart, eError, eError, eError,      3,      4,      5,      6,      7,      8,      9, // eStart
    eError, eError, eError, eError, eError, eError, eError, eError, eError, eError, eError, eError, // eError
    eItsMe, eItsMe, eItsMe, eItsMe, eItsMe, eItsMe, eItsMe, eItsMe, eItsMe, eItsMe, eItsMe, eItsMe, // eItsMe
    eError, eError, eStart, eStart, eStart, eError, eError, eError, eError, eError, eError, eError, // 3
    eError, eError, eError, eError,      3, eError, eError, eError, eError, eError, eError, eError, // 4
    eError, eError,      3,      3,      3, eError, eError, eError, eError, eError, eError, eError, // 5
    eError, eError,      3,      3, eError, eError, eError, eError, eError, eError, eError, eError, // 6
    eError, eError, eError,      5,      5, eError, eError, eError, eError, eError, eError, eError, // 7
    eError, eError,      5,      5,      5, eError, eError, eError, eError, eError, eError, eError, // 8
    eError, eError,      5, eError, eError, eError, eError, eError, eError, eError, eError, eError  // 9
    // clang-format on
);

static const unsigned int UTF8CharLenTable[] = {0, 1, 1, 1, 1, 1, 2, 3, 3, 3, 4, 4};

const SMModel UTF8SMModel = {
    {eIdxSft4bits, eSftMsk4bits, eBitSft4bits, eUnitMsk4bits, UTF8_cls.data()},
    12,
    {eIdxSft4bits, eSftMsk4bits, eBitSft4bits, eUnitMsk4bits, UTF8_st.data()},
    UTF8CharLenTable,
    "UTF-8",
};
}
