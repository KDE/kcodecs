/*  -*- C++ -*-
    SPDX-FileCopyrightText: 2026 Stefan Brüns <stefan.bruens@rwth-aachen.de>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "CharacterStatistics.h"

#include <format>

namespace kencodingprober
{
namespace
{
inline constexpr void gatherStats(std::span<const uint8_t> input, kencodingprober::CharacterStats &stats, size_t inputOffset)
{
    stats.totalCount = inputOffset + input.size();
    size_t remainder = std::min<size_t>(input.size(), (4 - inputOffset % 4) % 4);

    // Update histogram buckets
    for (size_t i = 0; i < remainder; i++) {
        uint8_t c = input[i];
        stats.counts[(inputOffset + i) % 4][c] += 1;
    }
    input = input.subspan(remainder);

    for (size_t i = 0; i + 3 < input.size(); i += 4) {
        stats.counts[0][input[i + 0]] += 1;
        stats.counts[1][input[i + 1]] += 1;
        stats.counts[2][input[i + 2]] += 1;
        stats.counts[3][input[i + 3]] += 1;
    }

    size_t processed = input.size() - (input.size() % 4);
    input = input.subspan(processed);
    for (size_t i = 0; i < input.size(); i++) {
        uint8_t c = input[i];
        stats.counts[i][c] += 1;
    }

    // Find highest and lowes used value for each histogram
    for (size_t i = 0; i < stats.minmax.size(); i++) {
        for (size_t j = 0; j < stats.minmax[i].min; j++) {
            if (stats.counts[i][j]) {
                stats.minmax[i].min = j;
                break;
            }
        }
        for (size_t j = 255; j > stats.minmax[i].max; j--) {
            if (stats.counts[i][j]) {
                stats.minmax[i].max = j;
                break;
            }
        }
    }
}
static_assert([]() {
    kencodingprober::CharacterStats s;
    return s.counts;
}() == std::array<CharacterStats::CCounts, 4>({{}, {}, {}, {}}));
static_assert([]() {
    kencodingprober::CharacterStats s;
    gatherStats(std::array<uint8_t, 2>{1, 2}, s, 0);
    return s.totalCount;
}() == 2);
static_assert([]() {
    kencodingprober::CharacterStats s;
    gatherStats(std::array<uint8_t, 2>{1, 2}, s, 0);
    return s.counts;
}() == std::array<CharacterStats::CCounts, 4>{CharacterStats::CCounts{0, 1, 0, 0}, {0, 0, 1, 0}, {}, {}});
static_assert([]() {
    kencodingprober::CharacterStats s;
    gatherStats(std::array<uint8_t, 2>{1, 2}, s, 1);
    return s.counts;
}() == std::array<CharacterStats::CCounts, 4>{CharacterStats::CCounts{}, {0, 1, 0, 0}, {0, 0, 1, 0}, {}});
static_assert([]() {
    kencodingprober::CharacterStats s;
    gatherStats(std::array<uint8_t, 5>{1, 2, 3, 4, 5}, s, 2);
    return s.counts;
}() == std::array<CharacterStats::CCounts, 4>{CharacterStats::CCounts{0, 0, 0, 1, 0}, {0, 0, 0, 0, 1}, {0, 1, 0, 0, 0, 1}, {0, 0, 1, 0, 0}});
static_assert([]() {
    kencodingprober::CharacterStats s;
    gatherStats(std::array<uint8_t, 14>{1, 2, 3, 0, 1, 2, 0, 0, 1, 0, 0, 0, 0, 0}, s, 1);
    return s.counts;
}() == std::array<CharacterStats::CCounts, 4>{CharacterStats::CCounts{3, 0, 0, 0}, {1, 3, 0, 0}, {2, 0, 2, 0}, {2, 0, 0, 1}});
static_assert([]() {
    kencodingprober::CharacterStats s;
    gatherStats(std::array<uint8_t, 14>{1, 2, 3, 0, 1, 2, 0, 0, 1, 0, 0, 0, 0, 0}, s, 1);
    return s.totalCount;
}() == 15);

/**
 * Calculate auto and cross correlations for the histogram of the mod(n, 4)th bytes
 * with the histograms of all other offsets. Likewise for the other 3 histograms.
 */
[[nodiscard]] constexpr auto correlate(const CharacterStats &stats) -> const std::array<float, 16>
{
    const std::array<uint64_t, 4> total = {
        (stats.totalCount + 3) / 4,
        (stats.totalCount + 2) / 4,
        (stats.totalCount + 1) / 4,
        (stats.totalCount + 0) / 4,
    };
    std::array<float, 16> corr{};

    for (const auto m : {0, 1, 2, 3}) {
        if (total[m]) {
            float sum = 0.0f;
            for (size_t i = 0; i < stats.counts[m].size(); i++) {
                sum += (stats.counts[m][i] * stats.counts[m][i]);
            }
            corr[m * 4 + m] = sum / (1.0f * total[m] * total[m]);
        } else {
            corr[m * 4 + m] = 0;
        }
    }
    for (const auto [m, n] : {std::pair{0, 1}, {0, 2}, {0, 3}, {1, 2}, {1, 3}, {2, 3}}) {
        if (total[m] && total[n]) {
            float sum = 0.0f;
            for (size_t i = 0; i < stats.counts[m].size(); i++) {
                sum += (stats.counts[m][i] * stats.counts[n][i]);
            }
            corr[m * 4 + n] = sum / (1.0f * total[m] * total[n]);
        } else {
            corr[m * 4 + n] = 0;
        }
        // Matrix is symmetric
        corr[n * 4 + m] = corr[m * 4 + n];
    }
    return corr;
}

static_assert([]() {
    kencodingprober::CharacterStats s;
    gatherStats(std::array<uint8_t, 6>{1, 2, 3, 4, 1, 5}, s, 0);
    return correlate(s);
}() == std::array<float, 16>{1, 0, 0, 0, /**/ 0, 0.5f, 0, 0, /**/ 0, 0, 1, 0, /**/ 0, 0, 0, 1});
static_assert([]() {
    kencodingprober::CharacterStats s;
    gatherStats(std::array<uint8_t, 3>{1, 2, 3}, s, 0);
    return correlate(s);
}() == std::array<float, 16>{1, 0, 0, 0, /**/ 0, 1, 0, 0, /**/ 0, 0, 1, 0, /**/ 0, 0, 0, 0});

float calculateBigEndian16(const std::array<float, 16> &corr)
{
    // average of even auto correlations and cross correlation
    float evenSum = (corr[0] + corr[10] + corr[2]) / 3.0f; // 0:0, 2:2, 0:2
    // average of odd auto correlations and cross correlation
    float oddSum = (corr[5] + corr[15] + corr[7]) / 3.0f; // 1:1, 3:3, 1:3
    // average of cross correlations between even and odd histograms
    float crossSum = (corr[1] + corr[3] + corr[9] + corr[11]) / 4.0f; // 0:1, 0:3, 2:1, 2:3

    // confidence for 16 bit vs 8 bit / variable length encoding
    // ~ 1.0 for 16 bit, ~ 0.5 for 8 bit
    float combinedSimilarity = (corr[2] + corr[7]) <= 0.0f ? 0.0f : //
        (corr[2] + corr[7]) / (corr[2] + corr[7] + (crossSum * 2.0f));

    // Normalize to [-1.0...1.0] for 16 bit little ... 16 bit big endian
    if (evenSum > oddSum) {
        return (2.0f * combinedSimilarity - 1.0f);
    } else {
        return (-2.0f * combinedSimilarity + 1.0f);
    }
}

} // anonymous namespace

void CharacterStats::collectInput(std::span<const uint8_t> input)
{
    gatherStats(input, *this, totalCount);
}

float CharacterStats::isBigEndian16() const
{
    auto isBE16 = calculateBigEndian16(correlate(*this));
    // lower confidence for short inputs
    return (isBE16 * totalCount) / (totalCount + 8);
}

float CharacterStats::isUtf8() const
{
    uint64_t len2Count = countRange(0xC2, 0xDF);
    uint64_t len3Count = countRange(0xE0, 0xEF);
    uint64_t len4Count = countRange(0xF0, 0xF4);

    float weightedSum = 2.0f * len2Count + 3.0f * len3Count + 4.0f * len4Count;
    // low confidence for a single 2 byte sequence
    weightedSum -= 1.0f;
    // lower confidence for short inputs
    weightedSum = (weightedSum * totalCount) / (totalCount + 8);
    return std::clamp(weightedSum, 0.0f, 1.0f);
}

QDebug operator<<(QDebug dbg, const CharacterStats &pc)
{
    QDebugStateSaver saver(dbg);

    auto toTable = [](const CharacterStats::CCounts c) -> std::string {
        std::string out;
        for (size_t i = 0; i < c.size(); i++) {
            if (i % 16 == 0) {
                out += "╎";
            }
            if (c[i] == 0) {
                out += ' ';
            } else if (i <= 0x20) {
                // Control character symbols, ␀ ... ␠
                auto a = std::array<char, 4>{"␀"};
                a[2] += i;
                out += std::string(a.begin(), a.begin() + 3);
            } else if (i <= 0x7e) {
                out += char(i);
            } else {
                out += "�";
            }
        }
        return out;
    };

    dbg << "Histogram:";
    CharacterStats::CCounts ones;
    std::ranges::fill(ones, 1);
    dbg.noquote() << "\n  " << toTable(ones) //
                  << "\n 0" << toTable(pc.counts[0]) //
                  << "\n 1" << toTable(pc.counts[1]) //
                  << "\n 2" << toTable(pc.counts[2]) //
                  << "\n 3" << toTable(pc.counts[3]);

    const auto corr = correlate(pc);

    dbg.noquote() << "\n  Min/Max | XCorr:";
    for (size_t i = 0; auto minmax : pc.minmax) {
        // clang-format off
        dbg.noquote() << std::format(
            " [{}: {} {} | {:.3f} {:.3f} {:.3f} {:.3f} ]",
            i, minmax.min, minmax.max,
            corr[4 * i + 0], corr[4 * i + 1], corr[4 * i + 2], corr[4 * i + 3]);
        // clang-format on
        i++;
    }

    dbg.noquote() << "Is BE16:" << calculateBigEndian16(corr);

    return dbg;
}
} // namespace
