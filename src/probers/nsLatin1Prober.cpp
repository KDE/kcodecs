/*  -*- C++ -*-
    SPDX-FileCopyrightText: 1998 Netscape Communications Corporation <developer@mozilla.org>

    SPDX-License-Identifier: MIT
*/

#include "nsLatin1Prober.h"

#include <algorithm>
#include <format>
#include <numeric>
#include <span>

namespace kencodingprober
{
namespace
{
constexpr auto CLASS_COUNT = nsLatin1Prober::CLASS_COUNT;

enum Latin1Class : uint8_t {
    UDF = 0, // undefined
    OTH = 1, // other
    ASC = 2, // ascii capital letter
    ASS = 3, // ascii small letter
    ACV = 4, // accent capital vowel
    ACO = 5, // accent capital other
    ASV = 6, // accent small vowel
    ASO = 7, // accent small other
};

static constexpr std::array<uint8_t, 256> Latin1_CharToClass{
    OTH, OTH, OTH, OTH, OTH, OTH, OTH, OTH, // 00 - 07
    OTH, OTH, OTH, OTH, OTH, OTH, OTH, OTH, // 08 - 0F
    OTH, OTH, OTH, OTH, OTH, OTH, OTH, OTH, // 10 - 17
    OTH, OTH, OTH, OTH, OTH, OTH, OTH, OTH, // 18 - 1F
    OTH, OTH, OTH, OTH, OTH, OTH, OTH, OTH, // 20 - 27
    OTH, OTH, OTH, OTH, OTH, OTH, OTH, OTH, // 28 - 2F
    OTH, OTH, OTH, OTH, OTH, OTH, OTH, OTH, // 30 - 37
    OTH, OTH, OTH, OTH, OTH, OTH, OTH, OTH, // 38 - 3F
    OTH, ASC, ASC, ASC, ASC, ASC, ASC, ASC, // 40 - 47
    ASC, ASC, ASC, ASC, ASC, ASC, ASC, ASC, // 48 - 4F
    ASC, ASC, ASC, ASC, ASC, ASC, ASC, ASC, // 50 - 57
    ASC, ASC, ASC, OTH, OTH, OTH, OTH, OTH, // 58 - 5F
    OTH, ASS, ASS, ASS, ASS, ASS, ASS, ASS, // 60 - 67
    ASS, ASS, ASS, ASS, ASS, ASS, ASS, ASS, // 68 - 6F
    ASS, ASS, ASS, ASS, ASS, ASS, ASS, ASS, // 70 - 77
    ASS, ASS, ASS, OTH, OTH, OTH, OTH, OTH, // 78 - 7F
    OTH, UDF, OTH, OTH, OTH, OTH, OTH, OTH, // 80 - 87
    OTH, OTH, ACO, OTH, ACO, UDF, ACO, UDF, // 88 - 8F
    UDF, OTH, OTH, OTH, OTH, OTH, OTH, OTH, // 90 - 97
    OTH, OTH, ASO, OTH, ASO, UDF, ASO, ACO, // 98 - 9F
    OTH, OTH, OTH, OTH, OTH, OTH, OTH, OTH, // A0 - A7
    OTH, OTH, OTH, OTH, OTH, OTH, OTH, OTH, // A8 - AF
    OTH, OTH, OTH, OTH, OTH, OTH, OTH, OTH, // B0 - B7
    OTH, OTH, OTH, OTH, OTH, OTH, OTH, OTH, // B8 - BF
    ACV, ACV, ACV, ACV, ACV, ACV, ACV, ACO, // C0 - C7
    ACV, ACV, ACV, ACV, ACV, ACV, ACV, ACV, // C8 - CF
    ACO, ACO, ACV, ACV, ACV, ACV, ACV, OTH, // D0 - D7
    ACV, ACV, ACV, ACV, ACV, ACO, ACO, ASO, // D8 - DF
    ASV, ASV, ASV, ASV, ASV, ASV, ASO, ASO, // E0 - E7
    ASV, ASV, ASV, ASV, ASV, ASV, ASV, ASV, // E8 - EF
    ASO, ASO, ASV, ASV, ASV, ASV, ASV, OTH, // F0 - F7
    ASV, ASV, ASV, ASV, ASV, ASO, ASO, ASV, // F8 - FF
};

/* 0 : illegal
   1 : very unlikely
   2 : normal
   3 : very likely
   4 : ASCII letter pair
   5 : ASCII / digit or punctuation pair
*/
static constexpr std::array<uint8_t, CLASS_COUNT * CLASS_COUNT> Latin1ClassModel{
    // clang-format off
    /*      UDF OTH ASC ASS ACV ACO ASV ASO  */
    /*UDF*/   0,  0,  0,  0,  0,  0,  0,  0,
    /*OTH*/   0,  3,  5,  5,  3,  3,  3,  3,
    /*ASC*/   0,  5,  4,  4,  3,  3,  3,  3,
    /*ASS*/   0,  5,  4,  4,  1,  1,  3,  3,
    /*ACV*/   0,  3,  3,  3,  2,  2,  2,  2,
    /*ACO*/   0,  3,  3,  3,  3,  3,  3,  3,
    /*ASV*/   0,  3,  1,  3,  1,  1,  2,  3,
    /*ASO*/   0,  3,  1,  3,  1,  1,  3,  3,
    // clang-format on
};

auto calculateFrequencies(const std::array<uint32_t, CLASS_COUNT * CLASS_COUNT> &seqCount)
{
    std::array<uint64_t, 6> freq{};
    for (size_t i = 0; const auto seq : seqCount) {
        const uint8_t seqQuality = Latin1ClassModel[i++];
        freq[seqQuality] += seq;
    }
    const uint64_t total = std::accumulate(freq.begin(), freq.end(), 0ull);

    // Number of non-letters : * -> OTH transitions
    // Number of letters: * -> Letter transitions
    // total = letters + non-letters
    const uint64_t nonLetters = [&] {
        uint64_t count = 0;
        for (size_t i = 1 * CLASS_COUNT + 1; i < seqCount.size(); i += CLASS_COUNT) {
            count += seqCount[i];
        }
        return count;
    }();
    const uint64_t letters = total - nonLetters;
    // Number of words: OTH -> Letter transitions
    const uint64_t words = [&] {
        uint64_t count = 0;
        for (size_t i = 1 * CLASS_COUNT + 2; i < 2 * CLASS_COUNT; i++) {
            count += seqCount[i];
        }
        return count;
    }();

    return std::array{freq[0], freq[1], freq[2], freq[3], freq[4], freq[5], total, nonLetters, letters, words};
}
} // namespace <anonymous>

nsLatin1Prober::nsLatin1Prober()
    : mLastCharClass(OTH)
{
}

nsProbingState nsLatin1Prober::HandleData(const char *aBuf, unsigned int aLen)
{
    std::span<const uint8_t> buf{reinterpret_cast<const uint8_t *>(aBuf), aLen};
    auto lastCharClass = mLastCharClass;

    for (const auto c : buf) {
        const uint8_t charClass = Latin1_CharToClass[c];
        if (charClass == UDF) {
            mState = eNotMe;
            break;
        }
        mSeqCounter[lastCharClass * CLASS_COUNT + charClass]++;
        lastCharClass = charClass;
    }
    mLastCharClass = lastCharClass;

    return mState;
}

float nsLatin1Prober::GetConfidence(void)
{
    if (mState == eNotMe) {
        return 0.0f;
    }

    const auto frequencies = calculateFrequencies(mSeqCounter);
    const auto total = frequencies[6];

    if (!total) {
        return 0.0f;
    }

    /*
     * Calculate confidence based on the information we have
     * +/- Sufficient average word length
     * +   Pairs of ASCII and codepoint >= 0x80, i.e. accented character
     *     Other SBCS have pairs of ASCII (english words) and
     *     pairs of codepoints >= 0x80
     *     Most frequent codes in MBCS are also pairs of bytes >= 0x80.
     * -   Unlikely pairs, e.g. lower case consonant followed by upper
     *     case consonant. See Latin1ClassModel above.
     * -   High normal to likely ratio
     * -   High accented/accented to accented/unaccented ration (TBI)
     *
     * Adjust low counts for short inputs.
     */
    const auto letters = frequencies[8];
    const auto words = frequencies[9];
    // => 0.2 for short inputs, => 0 for avg. word length <= 1.2, => 1.0 for avg. word length >= 3.2
    float confidence = std::clamp((letters + 7.0f) / (2.0f * words + 9.0f) - 0.6f, 0.0f, 1.0f);

    const auto unlikely = frequencies[1];
    const auto normal = frequencies[2];
    const auto likely = frequencies[3];
    // 1.1 for only likely, 1.0 for 1/150 unlikely/likely, 0.0 for ~1/14 likely/unlikely
    confidence *= (2.0f + likely * 1.1f) / (2.0f + likely + 15.0f * unlikely);
    confidence = std::clamp(confidence, 0.0f, 1.0f);
    // 1.0 for 2/5 normal/likely, 0.5 for 14/5 normal/likely
    confidence *= (1.0f + likely * 1.2f) / (1.0f + likely + 0.5f * normal);

    return std::clamp(confidence, 0.0f, 1.0f);
}

std::string nsLatin1Prober::StatusOutput(uint8_t /* indent */)
{
    const auto freqCounter = calculateFrequencies(mSeqCounter);
    return std::format( //
        "{:1.3f} [{}] [{} {} {} {} {} {} | {}] [{} : {} : {}]",
        GetConfidence(),
        GetCharSetName(),
        freqCounter[0],
        freqCounter[1],
        freqCounter[2],
        freqCounter[3],
        freqCounter[4],
        freqCounter[5],
        freqCounter[6], // total
        freqCounter[7],
        freqCounter[8],
        freqCounter[9]);
}
}
