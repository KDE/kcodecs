/*  -*- C++ -*-
    SPDX-FileCopyrightText: 1998 Netscape Communications Corporation <developer@mozilla.org>
    SPDX-FileCopyrightText: 2008 Wang Kai <wkai@gmail.com>

    SPDX-License-Identifier: MIT
*/

#include "nsUniversalDetector.h"

#include "StateMachineProber.h"
#include "nsLatin1Prober.h"
#include "nsMBCSGroupProber.h"
#include "nsSBCSGroupProber.h"

#include <format>

namespace kencodingprober
{
//---------------------------------------------------------------------
#define MINIMUM_THRESHOLD 0.20f

nsProbingState nsUniversalDetector::HandleData(const char *aBuf, unsigned int aLen)
{
    if (mDone) {
        return eFoundIt;
    }

    if (aLen == 0) {
        return eDetecting;
    }
    mGotData = true;

    if (!mHas8Bit) {
        bool hasEsc{false}; // ASCII 0x1b "ESCAPE
        bool hasHZ{false}; // HZ "~{" sequence

        for (unsigned int i = 0; i < aLen; i++) {
            if (aBuf[i] & '\x80') {
                mHas8Bit = true;
                break;
            } else if (aBuf[i] == '\x1b') {
                hasEsc = true;
            } else if ((aBuf[i] == '{') && (mLastChar == '~')) {
                hasHZ = true;
            }
            mLastChar = aBuf[i];
        }

        if (mHas8Bit) {
            // kill mEscCharSetProber if it is active
            mCharSetProbers[3] = nullptr;
            mCharSetProbers[4] = nullptr;

            // start multibyte and singlebyte charset prober
            mCharSetProbers[0] = std::make_unique<nsMBCSGroupProber>();
            mCharSetProbers[1] = std::make_unique<nsSBCSGroupProber>();
            mCharSetProbers[2] = std::make_unique<nsLatin1Prober>();
        } else {
            if (hasEsc && !mCharSetProbers[3]) {
                mCharSetProbers[3] = std::make_unique<StateMachineProber<SMProberType::ISO2022_JP>>();
            }
            if (hasHZ && !mCharSetProbers[4]) {
                mCharSetProbers[4] = std::make_unique<StateMachineProber<SMProberType::HZ>>();
            }
        }
    }

    for (auto &prober : mCharSetProbers) {
        if (prober) {
            if (const auto st = prober->HandleData(aBuf, aLen); st == eFoundIt) {
                mDone = true;
                mDetectedCharset = prober->GetCharSetName();
            }
        }
    }

    return eDetecting;
}

//---------------------------------------------------------------------
const char *nsUniversalDetector::GetCharSetName()
{
    if (mDetectedCharset) {
        return mDetectedCharset;
    } else if (!mHas8Bit) {
        return "UTF-8";
    }

    float maxProberConfidence = 0.0f;
    int maxProber = 0;
    for (int i = 0; i < NUM_OF_CHARSET_PROBERS; i++) {
        if (mCharSetProbers[i]) {
            float proberConfidence = mCharSetProbers[i]->GetConfidence();
            if (proberConfidence > maxProberConfidence) {
                maxProberConfidence = proberConfidence;
                maxProber = i;
            }
        }
    }
    // do not report anything because we are not confident of it, that's in fact a negative answer
    if (maxProberConfidence > MINIMUM_THRESHOLD) {
        return mCharSetProbers[maxProber]->GetCharSetName();
    }
    return "UTF-8";
}

//---------------------------------------------------------------------
float nsUniversalDetector::GetConfidence()
{
    if (!mGotData) {
        // we haven't got any data yet, return immediately
        // caller program sometimes call DataEnd before anything has been sent to detector
        return MINIMUM_THRESHOLD;
    }
    if (mDetectedCharset) {
        return 0.99f;
    } else if (!mHas8Bit) {
        return 0.99f;
    }

    float maxProberConfidence = 0.0f;
    int maxProber = 0;
    for (int i = 0; i < NUM_OF_CHARSET_PROBERS; i++) {
        if (mCharSetProbers[i]) {
            float proberConfidence = mCharSetProbers[i]->GetConfidence();
            if (proberConfidence > maxProberConfidence) {
                maxProberConfidence = proberConfidence;
                maxProber = i;
            }
        }
    }
    // do not report anything because we are not confident of it, that's in fact a negative answer
    if (maxProberConfidence > MINIMUM_THRESHOLD) {
        return mCharSetProbers[maxProber]->GetConfidence();
    }
    return MINIMUM_THRESHOLD;
}

nsProbingState nsUniversalDetector::GetState()
{
    if (mDone) {
        return eFoundIt;
    } else {
        return eDetecting;
    }
}

std::string nsUniversalDetector::StatusOutput(uint8_t indent)
{
    indent += 2;
    std::string output{"  Universal Prober ----"};
    for (unsigned int i = 0; i < NUM_OF_CHARSET_PROBERS; i++) {
        if (!mCharSetProbers[i]) {
            continue;
        }
        output += '\n' + std::string(indent, ' ');
        output += mCharSetProbers[i]->StatusOutput(indent);
    }
    return output;
}
}
