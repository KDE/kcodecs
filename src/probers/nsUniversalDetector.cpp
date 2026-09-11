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
namespace
{
constexpr std::array allSBCSProbers{
    nsCharSetProber::Prober::Windows1251,
    nsCharSetProber::Prober::KOI8_R,
    nsCharSetProber::Prober::ISO8859_5,
    nsCharSetProber::Prober::MAC_Cyrillic,
    nsCharSetProber::Prober::IBM866,
    nsCharSetProber::Prober::IBM855,
    nsCharSetProber::Prober::Latin7_Greek,
    nsCharSetProber::Prober::Windows1253_Greek,
    nsCharSetProber::Prober::ISO8859_5_Bulgarian,
    nsCharSetProber::Prober::Windows1251_Bulgarian,
    nsCharSetProber::Prober::ISO8859_8_HebrewVisual,
};

constexpr std::array allMBCSProbers{
    nsCharSetProber::Prober::SJIS,
    nsCharSetProber::Prober::EUCJP,
    nsCharSetProber::Prober::GB18030,
    nsCharSetProber::Prober::EUCKR,
    nsCharSetProber::Prober::Big5,
};
} // namespace <anonymous>

//---------------------------------------------------------------------
#define MINIMUM_THRESHOLD 0.20f

nsUniversalDetector::nsUniversalDetector()
    : mCharSetProbers{
          nullptr, // UTF-8
          nullptr, // MBCS
          nullptr, // SBCS
          nullptr, // Latin1
          nullptr, // ISO-2022-JP
          nullptr, // HZ
          std::make_unique<nsUtf16BEProber>(),
          std::make_unique<nsUtf16LEProber>(),
      }
{
}

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
            mCharSetProbers[4] = nullptr;
            mCharSetProbers[5] = nullptr;

            // start multibyte and singlebyte charset prober
            mCharSetProbers[0] = std::make_unique<nsUtf8Prober>();
            mCharSetProbers[1] = std::make_unique<nsMBCSGroupProber>(allMBCSProbers);
            mCharSetProbers[2] = std::make_unique<nsSBCSGroupProber>(allSBCSProbers);
            mCharSetProbers[3] = std::make_unique<nsLatin1Prober>();
        } else {
            if (hasEsc && !mCharSetProbers[4]) {
                mCharSetProbers[4] = std::make_unique<StateMachineProber<SMProberType::ISO2022_JP>>();
            }
            if (hasHZ && !mCharSetProbers[5]) {
                mCharSetProbers[5] = std::make_unique<StateMachineProber<SMProberType::HZ>>();
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

    const char *bestCharSet = nullptr;
    float maxProberConfidence = 0.0f;
    for (const auto &prober : mCharSetProbers) {
        if (prober) {
            float proberConfidence = prober->GetConfidence();
            if (proberConfidence > maxProberConfidence) {
                maxProberConfidence = proberConfidence;
                bestCharSet = prober->GetCharSetName();
            }
        }
    }
    // do not report anything because we are not confident of it, that's in fact a negative answer
    if (maxProberConfidence > MINIMUM_THRESHOLD) {
        return bestCharSet;
    } else if (mCharSetProbers[0] && mCharSetProbers[0]->GetState() != eNotMe) {
        // Default to UTF-8, but only if valid
        return mCharSetProbers[0]->GetCharSetName();
    }
    return bestCharSet;
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
    for (const auto &prober : mCharSetProbers) {
        if (prober) {
            float proberConfidence = prober->GetConfidence();
            if (proberConfidence > maxProberConfidence) {
                maxProberConfidence = proberConfidence;
            }
        }
    }
    // do not report anything because we are not confident of it, that's in fact a negative answer
    if (maxProberConfidence > MINIMUM_THRESHOLD) {
        return maxProberConfidence;
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
    for (const auto &prober : mCharSetProbers) {
        if (!prober) {
            continue;
        }
        output += '\n' + std::string(indent, ' ');
        output += prober->StatusOutput(indent);
    }
    return output;
}
}
