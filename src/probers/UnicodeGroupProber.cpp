/*  -*- C++ -*-
    SPDX-FileCopyrightText: 2008 Wang Kai <wkai@gmail.com>

    SPDX-License-Identifier: MIT
*/

#include "UnicodeGroupProber.h"
#include "nsUtfProber.h"

#include <format>

namespace kencodingprober
{
UnicodeGroupProber::UnicodeGroupProber()
    : mProbers{
          std::make_unique<nsUtf8Prober>(),
          std::make_unique<nsUtf16BEProber>(),
          std::make_unique<nsUtf16LEProber>(),
      }
{
    for (size_t i = 0; i < NUM_OF_UTF_PROBERS; i++) {
        if (mProbers[i]) { // not null
            mIsActive[i] = true;
        }
    }
}

const char *UnicodeGroupProber::GetCharSetName()
{
    if (mBestGuess == -1) {
        GetConfidence();
        if (mBestGuess == -1) {
            // Default to UTF-8
            mBestGuess = 0;
        }
    }
    return mProbers[mBestGuess]->GetCharSetName();
}

nsProbingState UnicodeGroupProber::HandleData(const char *aBuf, unsigned int aLen)
{
    if (mState != eDetecting) {
        return mState;
    }

    int activeNum = NUM_OF_UTF_PROBERS;

    for (unsigned int i = 0; i < NUM_OF_UTF_PROBERS; ++i) {
        if (!mIsActive[i]) {
            continue;
        }
        if (const auto st = mProbers[i]->HandleData(aBuf, aLen); st == eFoundIt) {
            mBestGuess = i;
            mState = eFoundIt;
            break;
        } else if (st == eNotMe) {
            mIsActive[i] = false;
            activeNum--;
        }
    }

    if (activeNum == 0) {
        mState = eNotMe;
    }

    return mState;
}

float UnicodeGroupProber::GetConfidence()
{
    float bestConf = 0.0;

    switch (mState) {
    case eFoundIt:
        return 0.99f; // sure yes
    case eNotMe:
        return 0.00f; // sure no
    default:
        for (unsigned int i = 0; i < NUM_OF_UTF_PROBERS; ++i) {
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

std::string UnicodeGroupProber::StatusOutput(uint8_t indent)
{
    indent += 2;
    std::string output{"  Unicode Group Prober ----"};
    GetConfidence();
    for (int i = 0; i < NUM_OF_UTF_PROBERS; i++) {
        char state = !mIsActive[i] ? '-' : (i == mBestGuess) ? '*' : ' ';
        output += '\n' + std::string(indent, ' ');
        output += std::format("{} #{:02}   UTF: ", state, i);
        output += mProbers[i]->StatusOutput(indent);
    }
    return output;
}
}
