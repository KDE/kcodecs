/*  -*- C++ -*-
    SPDX-FileCopyrightText: 1998 Netscape Communications Corporation <developer@mozilla.org>

    SPDX-License-Identifier: MIT
*/

#include "nsSBCharSetProber.h"

#include <stdio.h>

namespace kencodingprober
{
nsProbingState nsSingleByteCharSetProber::HandleData(const char *aBuf, unsigned int aLen)
{
    for (unsigned int i = 0; i < aLen; i++) {
        const unsigned char order = mModel->charToOrderMap[(unsigned char)aBuf[i]];

        if (order < SYMBOL_CAT_ORDER) {
            mTotalChar++;
        }
        if (order < SAMPLE_SIZE) {
            mFreqChar++;

            if (mLastOrder < SAMPLE_SIZE) {
                mTotalSeqs++;
                if (!mReversed) {
                    ++(mSeqCounters[(int)mModel->precedenceMatrix[mLastOrder * SAMPLE_SIZE + order]]);
                } else { // reverse the order of the letters in the lookup
                    ++(mSeqCounters[(int)mModel->precedenceMatrix[order * SAMPLE_SIZE + mLastOrder]]);
                }
            }
        }
        mLastOrder = order;
    }

    if (mState == eDetecting) {
        if (mTotalSeqs > 1024) {
            float cf = GetConfidence();
            if (cf > 0.95f) {
                mState = eFoundIt;
            } else if (cf < 0.05) {
                mState = eNotMe;
            }
        }
    }

    return mState;
}

void nsSingleByteCharSetProber::Reset(void)
{
    mState = eDetecting;
    mLastOrder = 255;
    for (unsigned int i = 0; i < NUMBER_OF_SEQ_CAT; i++) {
        mSeqCounters[i] = 0;
    }
    mTotalSeqs = 0;
    mTotalChar = 0;
    mFreqChar = 0;
}

//#define NEGATIVE_APPROACH 1

float nsSingleByteCharSetProber::GetConfidence(void)
{
#ifdef NEGATIVE_APPROACH
    if (mTotalSeqs > 0)
        if (mTotalSeqs > mSeqCounters[NEGATIVE_CAT] * 10) {
            return (mTotalSeqs - mSeqCounters[NEGATIVE_CAT] * 10.f) / mTotalSeqs * mFreqChar / mTotalChar;
        }
    return 0.01f;
#else // POSITIVE_APPROACH

    if (mTotalSeqs > 0) {
        float r = 1.0f * mSeqCounters[POSITIVE_CAT] / mTotalSeqs / mModel->mTypicalPositiveRatio;
        r = r * mFreqChar / mTotalChar;
        if (r >= 0.99f) {
            r = 0.99f;
        }
        return r;
    }
    return 0.01f;
#endif
}

const char *nsSingleByteCharSetProber::GetCharSetName()
{
    if (!mNameProber) {
        return mModel->charsetName;
    }
    return mNameProber->GetCharSetName();
}

#ifdef DEBUG_PROBE
void nsSingleByteCharSetProber::DumpStatus()
{
    printf("  SBCS: %1.3f [%s]\r\n", GetConfidence(), GetCharSetName());
}
#endif
}
