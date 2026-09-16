/*  -*- C++ -*-
    SPDX-FileCopyrightText: 1998 Netscape Communications Corporation <developer@mozilla.org>

    SPDX-License-Identifier: MIT
*/

#ifndef CharDistribution_h__
#define CharDistribution_h__

#include "kcodecs_export.h"

#include <cstdint>
#include <string>

#define ENOUGH_DATA_THRESHOLD 256

namespace kencodingprober
{
class KCODECS_NO_EXPORT CharDistributionAnalysis
{
public:
    CharDistributionAnalysis() = default;
    virtual ~CharDistributionAnalysis() = default;

    // Feed a character with known length
    void HandleOneChar(const char *aStr, unsigned int aCharLen)
    {
        // we only care about 2-bytes character in our distribution analysis
        const int code = (aCharLen == 2) ? GetCode(reinterpret_cast<const uint8_t *>(aStr)) : -1;

        if (code >= 0) {
            mTotalChars++;
            // code is valid
            if ((unsigned int)code < mTableSize) {
                if (mCharToFreqOrder[code] < 512) {
                    mFreqChars++;
                }
            }
        }
    }

    // return confidence base on existing data
    float GetConfidence();

    std::string StatusOutput();

    // It is not necessary to receive all data to draw conclusion. For charset detection,
    // certain amount of data is enough
    bool GotEnoughData()
    {
        return mTotalChars > ENOUGH_DATA_THRESHOLD;
    }

protected:
    // Characters are not handled based on its original encoded value, but
    // converted to an encoding specific unique code.
    // This allows multiple encoding formats (e.g. SJIS and EUCJP) of an
    // encoding (like JIS X 213) to share one frequency table, mapping this
    // code to its frequency.
    virtual int GetCode(const uint8_t str[2]) = 0;

    // The number of characters whose frequency order is less than 512
    unsigned int mFreqChars = 0;

    // Total character encountered.
    unsigned int mTotalChars = 0;

    // Mapping table to get frequency order from code (from GetCode())
    const short *mCharToFreqOrder = nullptr;

    // Size of above table
    unsigned int mTableSize = 0;

    // This is a constant value varies from language to language, it is used in
    // calculating confidence. See my paper for further detail.
    float mTypicalDistributionRatio = 0.0f;
};

class KCODECS_NO_EXPORT EUCKRDistributionAnalysis : public CharDistributionAnalysis
{
public:
    EUCKRDistributionAnalysis();

protected:
    // for euc-KR encoding, we are interested
    //  first  byte range: 0xb0 -- 0xfe
    //  second byte range: 0xa1 -- 0xfe
    // no validation needed here. State machine has done that
    int GetCode(const uint8_t str[2]) override
    {
        if (str[0] >= 0xb0) {
            return 94 * (str[0] - 0xb0) + str[1] - 0xa1;
        } else {
            return -1;
        }
    }
};

class KCODECS_NO_EXPORT GB2312DistributionAnalysis : public CharDistributionAnalysis
{
public:
    GB2312DistributionAnalysis();

protected:
    // for GB2312 encoding, we are interested
    //  first  byte range: 0xb0 -- 0xfe
    //  second byte range: 0xa1 -- 0xfe
    // no validation needed here. State machine has done that
    int GetCode(const uint8_t str[2]) override
    {
        if (str[0] >= 0xb0 && str[1] >= 0xa1) {
            return 94 * (str[0] - 0xb0) + str[1] - 0xa1;
        } else {
            return -1;
        }
    }
};

class KCODECS_NO_EXPORT Big5DistributionAnalysis : public CharDistributionAnalysis
{
public:
    Big5DistributionAnalysis();

protected:
    // for big5 encoding, we are interested
    //  first  byte range: 0xa4 -- 0xfe
    //  second byte range: 0x40 -- 0x7e , 0xa1 -- 0xfe
    // no validation needed here. State machine has done that
    int GetCode(const uint8_t str[2]) override
    {
        if (str[0] >= 0xa4)
            if (str[1] >= 0xa1) {
                return 157 * (str[0] - 0xa4) + str[1] - 0xa1 + 63;
            } else {
                return 157 * (str[0] - 0xa4) + str[1] - 0x40;
            }
        else {
            return -1;
        }
    }
};

class KCODECS_NO_EXPORT SJISDistributionAnalysis : public CharDistributionAnalysis
{
public:
    SJISDistributionAnalysis();

protected:
    // for sjis encoding, we are interested
    //  first  byte range: 0x81 -- 0x9f , 0xe0 -- 0xfe
    //  second byte range: 0x40 -- 0x7e,  0x81 -- oxfe
    // no validation needed here. State machine has done that
    int GetCode(const uint8_t str[2]) override
    {
        int code;
        if (str[0] >= 0x81 && *str <= 0x9f) {
            code = 188 * (str[0] - 0x81);
        } else if (str[0] >= 0xe0 && str[0] <= 0xef) {
            code = 188 * (str[0] - 0xe0 + 31);
        } else {
            return -1;
        }
        code += str[1] - 0x40;
        if (str[1] > 0x7f) {
            code--;
        }
        return code;
    }
};

class KCODECS_NO_EXPORT EUCJPDistributionAnalysis : public CharDistributionAnalysis
{
public:
    EUCJPDistributionAnalysis();

protected:
    // for euc-JP encoding, we are interested
    //  first  byte range: 0xa0 -- 0xfe
    //  second byte range: 0xa1 -- 0xfe
    // no validation needed here. State machine has done that
    int GetCode(const uint8_t str[2]) override
    {
        if (str[0] >= 0xa0) {
            return 94 * (str[0] - 0xa1) + str[1] - 0xa1;
        } else {
            return -1;
        }
    }
};
}
#endif // CharDistribution_h__
