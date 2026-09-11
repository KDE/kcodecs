/*  -*- C++ -*-
    SPDX-FileCopyrightText: 1998 Netscape Communications Corporation <developer@mozilla.org>

    SPDX-License-Identifier: MIT
*/

#include "nsMBCSGroupProber.h"

#include "nsBig5Prober.h"
#include "nsEUCJPProber.h"
#include "nsEUCKRProber.h"
#include "nsGB2312Prober.h"
#include "nsSJISProber.h"

#include <format>

namespace kencodingprober
{

namespace
{
using Prober = nsCharSetProber::Prober;
constexpr std::array<Prober, NUM_OF_PROBERS> allProbers{{
    Prober::SJIS,
    Prober::EUCJP,
    Prober::GB18030,
    Prober::EUCKR,
    Prober::Big5,
}};
constexpr std::array<bool, NUM_OF_PROBERS> fromSelectedList(std::span<const Prober> selected)
{
    std::array<bool, NUM_OF_PROBERS> isSelected{false};
    for (auto p : selected) {
        for (uint8_t index = 0; index < allProbers.size(); index++) {
            if (allProbers[index] != p) {
                continue;
            }
            isSelected[index] = true;
            break;
        }
    }
    return isSelected;
}
static_assert(fromSelectedList({}) == std::array<bool, NUM_OF_PROBERS>{false});
static_assert(fromSelectedList(allProbers) == std::array<bool, NUM_OF_PROBERS>{true, true, true, true, true});
static_assert(fromSelectedList(std::array{Prober::Utf8}) == std::array<bool, NUM_OF_PROBERS>{false});
static_assert(fromSelectedList(std::array{Prober::HZ}) == std::array<bool, NUM_OF_PROBERS>{false});
static_assert(fromSelectedList(std::array{Prober::KOI8_R}) == std::array<bool, NUM_OF_PROBERS>{false});
static_assert(fromSelectedList(std::array{Prober::SJIS, Prober::Big5}) == std::array<bool, NUM_OF_PROBERS>{true, false, false, false, true});
} // namespace <anonymous>

nsMBCSGroupProber::nsMBCSGroupProber(std::span<const Prober> selected)
    : mProbers{
          std::make_unique<nsSJISProber>(),
          std::make_unique<nsEUCJPProber>(),
          std::make_unique<nsGB18030Prober>(),
          std::make_unique<nsEUCKRProber>(),
          std::make_unique<nsBig5Prober>(),
      }
    , mIsSelected(fromSelectedList(selected))
{
    for (unsigned int i = 0; i < NUM_OF_PROBERS; i++) {
        if (mProbers[i] && mIsSelected[i]) {
            mIsActive[i] = true;
            ++mActiveNum;
        }
    }
}

nsMBCSGroupProber::nsMBCSGroupProber()
    : nsMBCSGroupProber(allProbers)
{
}

const char *nsMBCSGroupProber::GetCharSetName()
{
    if (mBestGuess >= mProbers.size()) {
        GetConfidence();
        if (mBestGuess >= mProbers.size()) {
            mBestGuess = 0;
        }
    }
    return mProbers[mBestGuess]->GetCharSetName();
}

nsProbingState nsMBCSGroupProber::HandleData(const char *aBuf, unsigned int aLen)
{
    if (mState != eDetecting) {
        return mState;
    }

    // do filtering to reduce load to probers
    char *highbyteBuf;
    char *hptr;
    bool keepNext = true; // assume previous is not ascii, it will do no harm except add some noise
    hptr = highbyteBuf = (char *)malloc(aLen);
    if (!hptr) {
        return mState;
    }
    for (unsigned int i = 0; i < aLen; ++i) {
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

    for (size_t i = 0; i < mProbers.size(); i++) {
        if (!mIsActive[i]) {
            continue;
        }
        nsProbingState st = mProbers[i]->HandleData(highbyteBuf, hptr - highbyteBuf);
        if (st == eFoundIt) {
            mBestGuess = i;
            mState = eFoundIt;
            break;
        } else if (st == eNotMe) {
            mIsActive[i] = false;
            mActiveNum--;
        }
    }

    if (mActiveNum == 0) {
        mState = eNotMe;
    }

    free(highbyteBuf);

    return mState;
}

float nsMBCSGroupProber::GetConfidence(void)
{
    float bestConf = 0.0;

    switch (mState) {
    case eFoundIt:
        return 0.99f;
    case eNotMe:
        return 0.0f;
    default:
        for (size_t i = 0; i < mProbers.size(); i++) {
            if (!mIsActive[i]) {
                continue;
            }
            float cf = mProbers[i]->GetConfidence();
            if (bestConf < cf) {
                bestConf = cf;
                mBestGuess = i;
            }
        }
    }
    return bestConf;
}

std::string nsMBCSGroupProber::StatusOutput(uint8_t indent)
{
    indent += 2;
    std::string output{"  MBCS Group Prober ----"};
    GetConfidence();
    for (size_t i = 0; i < mProbers.size(); i++) {
        char state = !mIsSelected[i] ? '.' : !mIsActive[i] ? '-' : (i == mBestGuess) ? '*' : ' ';
        output += '\n' + std::string(indent, ' ');
        output += std::format("{} #{:02}  MBCS: ", state, i);
        output += mProbers[i]->StatusOutput(indent);
    }
    return output;
}
}
