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

constexpr auto fromSelectedList(std::span<const nsCharSetProber::Prober> selected)
{
    std::array<bool, 8> isSelected{false};
    for (auto p : selected) {
        if (p == nsCharSetProber::Prober::Utf8) {
            isSelected[0] = true;
        } else if (auto it = std::find(allMBCSProbers.begin(), allMBCSProbers.end(), p); it != allMBCSProbers.end()) {
            isSelected[1] = true;
        } else if (auto it = std::find(allSBCSProbers.begin(), allSBCSProbers.end(), p); it != allSBCSProbers.end()) {
            isSelected[2] = true;
        } else if (p == nsCharSetProber::Prober::Windows1252_Latin1) {
            isSelected[3] = true;
        } else if (p == nsCharSetProber::Prober::ISO2022_JP) {
            isSelected[4] = true;
        } else if (p == nsCharSetProber::Prober::HZ) {
            isSelected[5] = true;
        } else if (p == nsCharSetProber::Prober::Utf16BE) {
            isSelected[6] = true;
        } else if (p == nsCharSetProber::Prober::Utf16LE) {
            isSelected[7] = true;
        }
    }
    return isSelected;
}
} // namespace <anonymous>

class ProberState
{
    struct Entry {
        const bool selected = true;
        bool active = true;
        std::unique_ptr<nsCharSetProber> prober = nullptr;
    };

    static const auto createStateList(std::span<const nsCharSetProber::Prober> selected)
    {
        auto isSelected = fromSelectedList(selected);
        std::array<Entry, 8> states{
            Entry{isSelected[0], false, std::make_unique<nsUtf8Prober>()},
            Entry{isSelected[1], false, std::make_unique<nsMBCSGroupProber>(selected)},
            Entry{isSelected[2], false, std::make_unique<nsSBCSGroupProber>(selected)},
            Entry{isSelected[3], false, std::make_unique<nsLatin1Prober>()},
            Entry{isSelected[4], false, std::make_unique<StateMachineProber<SMProberType::ISO2022_JP>>()},
            Entry{isSelected[5], false, std::make_unique<StateMachineProber<SMProberType::HZ>>()},
            Entry{isSelected[6], true, std::make_unique<nsUtf16BEProber>()},
            Entry{isSelected[7], true, std::make_unique<nsUtf16LEProber>()},
        };
        return states;
    }

    std::array<Entry, 8> mStates;

public:
    ProberState(std::span<const nsCharSetProber::Prober> selected)
        : mStates{createStateList(selected)}
    {
    }

    ProberState()
        : mStates{
              Entry{true, false, std::make_unique<nsUtf8Prober>()},
              Entry{true, false, std::make_unique<nsMBCSGroupProber>(allMBCSProbers)},
              Entry{true, false, std::make_unique<nsSBCSGroupProber>(allSBCSProbers)},
              Entry{true, false, std::make_unique<nsLatin1Prober>()},
              Entry{true, false, std::make_unique<StateMachineProber<SMProberType::ISO2022_JP>>()},
              Entry{true, false, std::make_unique<StateMachineProber<SMProberType::HZ>>()},
              Entry{true, true, std::make_unique<nsUtf16BEProber>()},
              Entry{true, true, std::make_unique<nsUtf16LEProber>()},
          }
    {
    }

    auto &operator[](size_t i)
    {
        return mStates[i];
    }
    auto begin()
    {
        return mStates.begin();
    }
    auto end()
    {
        return mStates.end();
    }
};

//---------------------------------------------------------------------
#define MINIMUM_THRESHOLD 0.20f

nsUniversalDetector::nsUniversalDetector(std::span<const nsCharSetProber::Prober> selected)
    : mProberState{std::make_unique<ProberState>(selected)}
{
}

nsUniversalDetector::nsUniversalDetector()
    : mProberState{std::make_unique<ProberState>()}
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
            (*mProberState)[4].active = false;
            (*mProberState)[5].active = false;

            // start multibyte and singlebyte charset prober
            (*mProberState)[0].active = (*mProberState)[0].selected;
            (*mProberState)[1].active = (*mProberState)[1].selected;
            (*mProberState)[2].active = (*mProberState)[2].selected;
            (*mProberState)[3].active = (*mProberState)[3].selected;
        } else {
            if (hasEsc) {
                (*mProberState)[4].active = (*mProberState)[4].selected;
            }
            if (hasHZ) {
                (*mProberState)[5].active = (*mProberState)[5].selected;
            }
        }
    }

    for (auto &state : (*mProberState)) {
        if (state.active) {
            if (const auto st = state.prober->HandleData(aBuf, aLen); st == eFoundIt) {
                mDone = true;
                mDetectedCharset = state.prober->GetCharSetName();
            } else if (st == eNotMe) {
                state.active = false;
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
    for (const auto &state : *mProberState) {
        if (state.active) {
            float proberConfidence = state.prober->GetConfidence();
            if (proberConfidence > maxProberConfidence) {
                maxProberConfidence = proberConfidence;
                bestCharSet = state.prober->GetCharSetName();
            }
        }
    }
    // do not report anything because we are not confident of it, that's in fact a negative answer
    if (maxProberConfidence > MINIMUM_THRESHOLD) {
        return bestCharSet;
    } else if ((*mProberState)[0].prober && (*mProberState)[0].prober->GetState() != eNotMe) {
        // Default to UTF-8, but only if valid
        return (*mProberState)[0].prober->GetCharSetName();
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
    for (const auto &state : *mProberState) {
        if (state.active) {
            float proberConfidence = state.prober->GetConfidence();
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
    for (const auto &prober : *mProberState) {
        char state = !prober.selected ? '.' : !prober.active ? '-' : ' ';
        output += '\n' + std::string(indent, ' ');
        output += std::format("{} ", state);
        output += prober.prober->StatusOutput(indent);
    }
    return output;
}
}
