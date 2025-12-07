/*  -*- C++ -*-
    SPDX-FileCopyrightText: 1998 Netscape Communications Corporation <developer@mozilla.org>

    SPDX-License-Identifier: MIT
*/

#include "nsMBCSGroupProber.h"

#include "UnicodeGroupProber.h"
#include "nsBig5Prober.h"
#include "nsEUCJPProber.h"
#include "nsEUCKRProber.h"
#include "nsGB2312Prober.h"
#include "nsSJISProber.h"

#include <stdio.h>
#include <stdlib.h>

namespace kencodingprober
{
#ifdef DEBUG_PROBE
static const char *const ProberName[] = {
    "Unicode",
    "SJIS",
    "EUCJP",
    "GB18030",
    "EUCKR",
    "Big5",
};

#endif

namespace
{
using Prober = nsMBCSGroupProber::Prober;
constexpr std::array<bool, 6> fromSelectedList(std::span<const Prober> selected)
{
    std::array<bool, 6> isSelected{false};
    for (auto p : selected) {
        const auto i = static_cast<std::underlying_type_t<Prober>>(p);
        if (i >= NUM_OF_PROBERS) {
            continue;
        }
        isSelected[i] = true;
    }
    return isSelected;
}
static_assert(fromSelectedList({})[0] == false);
static_assert(fromSelectedList({})[5] == false);
static_assert(fromSelectedList(std::array{Prober::Unicode})[0] == true);
static_assert(fromSelectedList(std::array{Prober::Unicode})[5] == false);
static_assert(fromSelectedList(std::array{Prober::SJIS, Prober::Big5})[0] == false);
static_assert(fromSelectedList(std::array{Prober::SJIS, Prober::Big5})[1] == true);
static_assert(fromSelectedList(std::array{Prober::SJIS, Prober::Big5})[2] == false);
static_assert(fromSelectedList(std::array{Prober::SJIS, Prober::Big5})[3] == false);
static_assert(fromSelectedList(std::array{Prober::SJIS, Prober::Big5})[4] == false);
static_assert(fromSelectedList(std::array{Prober::SJIS, Prober::Big5})[5] == true);

} // namespace <anonymous>

nsMBCSGroupProber::nsMBCSGroupProber(std::span<const Prober> selected)
    : mIsSelected(fromSelectedList(selected))
{
    mProbers[0] = new UnicodeGroupProber();
    mProbers[1] = new nsSJISProber();
    mProbers[2] = new nsEUCJPProber();
    mProbers[3] = new nsGB18030Prober();
    mProbers[4] = new nsEUCKRProber();
    mProbers[5] = new nsBig5Prober();
    Reset();
}

nsMBCSGroupProber::nsMBCSGroupProber()
    : nsMBCSGroupProber(std::array{
          Prober::Unicode,
          Prober::SJIS,
          Prober::EUCJP,
          Prober::GB18030,
          Prober::EUCKR,
          Prober::Big5,
      })
{
}

nsMBCSGroupProber::~nsMBCSGroupProber()
{
    for (unsigned int i = 0; i < NUM_OF_PROBERS; i++) {
        delete mProbers[i];
    }
}

const char *nsMBCSGroupProber::GetCharSetName()
{
    if (mBestGuess == -1) {
        GetConfidence();
        if (mBestGuess == -1) {
            mBestGuess = 0;
        }
    }
    return mProbers[mBestGuess]->GetCharSetName();
}

void nsMBCSGroupProber::Reset(void)
{
    mActiveNum = 0;
    for (unsigned int i = 0; i < NUM_OF_PROBERS; i++) {
        if (mProbers[i] && mIsSelected[i]) {
            mProbers[i]->Reset();
            mIsActive[i] = true;
            ++mActiveNum;
        } else {
            mIsActive[i] = false;
        }
    }
    mBestGuess = -1;
    mState = eDetecting;
}

nsProbingState nsMBCSGroupProber::HandleData(const char *aBuf, unsigned int aLen)
{
    nsProbingState st;
    unsigned int i;

    // do filtering to reduce load to probers
    char *highbyteBuf;
    char *hptr;
    bool keepNext = true; // assume previous is not ascii, it will do no harm except add some noise
    hptr = highbyteBuf = (char *)malloc(aLen);
    if (!hptr) {
        return mState;
    }
    for (i = 0; i < aLen; ++i) {
        if (aBuf[i] & 0x80) {
            *hptr++ = aBuf[i];
            keepNext = true;
        } else {
            // if previous is highbyte, keep this even it is a ASCII
            if (keepNext) {
                *hptr++ = aBuf[i];
                keepNext = false;
            }
        }
    }

    for (i = 0; i < NUM_OF_PROBERS; ++i) {
        if (!mIsActive[i]) {
            continue;
        }
        st = mProbers[i]->HandleData(highbyteBuf, hptr - highbyteBuf);
        if (st == eFoundIt) {
            mBestGuess = i;
            mState = eFoundIt;
            break;
        } else if (st == eNotMe) {
            mIsActive[i] = false;
            mActiveNum--;
            if (mActiveNum == 0) {
                mState = eNotMe;
                break;
            }
        }
    }

    free(highbyteBuf);

    return mState;
}

float nsMBCSGroupProber::GetConfidence(void)
{
    unsigned int i;
    float bestConf = 0.0;
    float cf;

    switch (mState) {
    case eFoundIt:
        return (float)0.99;
    case eNotMe:
        return (float)0.01;
    default:
        for (i = 0; i < NUM_OF_PROBERS; ++i) {
            if (!mIsActive[i]) {
                continue;
            }
            cf = mProbers[i]->GetConfidence();
            if (bestConf < cf) {
                bestConf = cf;
                mBestGuess = i;
            }
        }
    }
    return bestConf;
}

#ifdef DEBUG_PROBE
void nsMBCSGroupProber::DumpStatus()
{
    unsigned int i;
    float cf;

    GetConfidence();
    for (i = 0; i < NUM_OF_PROBERS; i++) {
        if (!mIsSelected[i]) {
            printf("  MBCS deselected: [%s][%s]\r\n", ProberName[i], mProbers[i]->GetCharSetName());
        } else if (!mIsActive[i]) {
            printf("  MBCS inactive: [%s][%s] (confidence is too low).\r\n", ProberName[i], mProbers[i]->GetCharSetName());
        } else {
            cf = mProbers[i]->GetConfidence();
            printf("  MBCS %1.3f: [%s][%s]\r\n", cf, ProberName[i], mProbers[i]->GetCharSetName());
            mProbers[i]->DumpStatus();
        }
    }
}
#endif
}
