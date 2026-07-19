/*  -*- C++ -*-
    SPDX-FileCopyrightText: 1998 Netscape Communications Corporation <developer@mozilla.org>

    SPDX-License-Identifier: MIT
*/

#ifndef __JPCNTX_H__
#define __JPCNTX_H__

#include "kcodecs_export.h"

#define NUM_OF_CATEGORY 6

#define ENOUGH_REL_THRESHOLD 100
#define MAX_REL_THRESHOLD 1000
namespace kencodingprober
{
// hiragana frequency category table
extern const char jp2CharContext[83][83];

class KCODECS_NO_EXPORT JapaneseContextAnalysis
{
public:
    JapaneseContextAnalysis() = default;
    virtual ~JapaneseContextAnalysis() = default;

    void HandleOneChar(const char *aStr, unsigned int aCharLen)
    {
        // if we received enough data, stop here
        if (mTotalRel > MAX_REL_THRESHOLD) {
            mDone = true;
        }
        if (mDone) {
            return;
        }

        // Only 2-bytes characters are of our interest
        const int code = (aCharLen == 2) ? GetCode(aStr) : -1;
        if (code != -1 && mLastCharCode != -1) {
            mTotalRel++;
            // count this sequence to its category counter
            mRelSample[(int)jp2CharContext[mLastCharCode][code]]++;
        }
        mLastCharCode = code;
    }

    float GetConfidence();
    bool GotEnoughData()
    {
        return mTotalRel > ENOUGH_REL_THRESHOLD;
    }

protected:
    // Get cell code in Hiragana row, or -1 for non-Hiragana
    virtual int GetCode(const char *str) = 0;

    // category counters, each integer counts sequence in its category
    unsigned int mRelSample[NUM_OF_CATEGORY] = {0};

    // total sequence received
    unsigned int mTotalRel = 0;

    // The code of previous char
    int mLastCharCode = -1;

    // If this flag is set to true, detection is done and conclusion has been made
    bool mDone = false;
};

class KCODECS_NO_EXPORT SJISContextAnalysis : public JapaneseContextAnalysis
{
protected:
    int GetCode(const char *str) override
    {
        // We only interested in Hiragana, so first byte is '\202'
        if (*str == '\202' && (unsigned char)*(str + 1) >= (unsigned char)0x9f && (unsigned char)*(str + 1) <= (unsigned char)0xf1) {
            return (unsigned char)*(str + 1) - (unsigned char)0x9f;
        }
        return -1;
    }
};

class KCODECS_NO_EXPORT EUCJPContextAnalysis : public JapaneseContextAnalysis
{
protected:
    int GetCode(const char *str) override
    // We only interested in Hiragana, so first byte is '\244'
    {
        if (*str == '\244' //
            && (unsigned char)*(str + 1) >= (unsigned char)0xa1 //
            && (unsigned char)*(str + 1) <= (unsigned char)0xf3) {
            return (unsigned char)*(str + 1) - (unsigned char)0xa1;
        }
        return -1;
    }
};
}
#endif /* __JPCNTX_H__ */
