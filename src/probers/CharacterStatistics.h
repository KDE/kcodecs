/*  -*- C++ -*-
    SPDX-FileCopyrightText: 2026 Stefan Brüns <stefan.bruens@rwth-aachen.de>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef CharacterStatistics_h__
#define CharacterStatistics_h__

#include <algorithm>

#include <QDebug>

namespace kencodingprober
{
struct CharacterStats {
    // Frequency count, one bin for each byte value
    using CCounts = std::array<uint32_t, 256>;
    // Each collected for input offset modulo 4
    std::array<CCounts, 4> counts = {};
    struct MinMaxFill {
        uint8_t min = 255;
        uint8_t max = 0;
    };
    std::array<MinMaxFill, 4> minmax = {};
    uint64_t totalCount = 0;

    void collectInput(std::span<const uint8_t> input);

    constexpr uint8_t min() const
    {
        return std::min({minmax[0].min, minmax[1].min, minmax[2].min, minmax[3].min});
    }

    constexpr uint8_t max() const
    {
        return std::max({minmax[0].max, minmax[1].max, minmax[2].max, minmax[3].max});
    }

    constexpr uint64_t count(uint8_t c) const
    {
        return static_cast<uint64_t>(counts[0][c]) + counts[1][c] //
            + counts[2][c] + counts[3][c];
    }

    constexpr uint64_t countRange(uint8_t min, uint8_t max) const
    {
        uint64_t sum = 0;
        for (uint8_t c = min; c <= max; c++) {
            sum += count(c);
        }
        return sum;
    }

    /**
     * Calculate LE/BE likelihood
     *
     * Most scripts only use one or a few different values for the high byte, while
     * the values of the low byte is distributed more evenly. This even holds when
     * multiple scripts, symbols, or scripts with many different symbols are used.
     *
     * 2 histograms are created for each for each of the high byte and the low byte.
     *
     * The auto correlations of the high byte histograms tends to be higher than
     * for the low byte, and also their cross correlation. The cross correlation
     * between high and low bytes is typically fairly low, often ~ 0.0.
     *
     * For a single byte or variable width coding (e.g. UTF-8), cross and auto
     * correlations are all quite similar, as all histograms are similar.
     *
     * returns ~ +1.0 when input is likely big endian
     * returns ~ -1.0 when input is likely little endian
     */
    float isBigEndian16() const;

    /**
     * Calculate Utf8 likelihood based on number of seen 2/3/4 byte UTF-8 sequences.
     *
     * returns ~ 1.0 if a a sufficient number of sequences were seen.
     */
    float isUtf8() const;
};

QDebug operator<<(QDebug dbg, const CharacterStats &pc);

} // namespace
#endif // CharacterStatistics_h__
