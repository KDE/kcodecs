/*  -*- C++ -*-
    SPDX-FileCopyrightText: 1998 Netscape Communications Corporation <developer@mozilla.org>
    SPDX-FileCopyrightText: 2008 Wang Kai <wkai@gmail.com>

    SPDX-License-Identifier: MIT
*/

#include "nsUniversalDetector.h"

#include "CharacterStatistics.h"
#include "StateMachineProber.h"
#include "nsLatin1Prober.h"
#include "nsMBCSGroupProber.h"
#include "nsSBCSGroupProber.h"

#include <algorithm>
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
        } else if (auto it2 = std::find(allSBCSProbers.begin(), allSBCSProbers.end(), p); it2 != allSBCSProbers.end()) {
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

    // Initialization helper
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

public:
    explicit ProberState(std::span<const nsCharSetProber::Prober> selected)
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

    nsProbingState ProcessInput(const char *aBuf, unsigned int aLen);
    std::pair<const char *, float> GetBestProber() const;
    std::array<float, 8> CalculateBias() const;

    std::array<Entry, 8> mStates;

    CharacterStats mCharacterStats = {};

    bool mDone = false;
    bool mHas8Bit = false;
    const char *mDetectedCharset = nullptr;
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
    return mProberState->ProcessInput(aBuf, aLen);
}

nsProbingState ProberState::ProcessInput(const char *aBuf, unsigned int aLen)
{
    if (mDone) {
        return eFoundIt;
    }

    if (aLen == 0) {
        return eDetecting;
    }

    mCharacterStats.collectInput({reinterpret_cast<const uint8_t *>(aBuf), aLen});

    if (!mHas8Bit) {
        if (mCharacterStats.max() >= 0x80) {
            mHas8Bit = true;
            // kill mEscCharSetProber if it is active
            mStates[4].active = false;
            mStates[5].active = false;

            // start multibyte and singlebyte charset prober
            mStates[0].active = mStates[0].selected;
            mStates[1].active = mStates[1].selected;
            mStates[2].active = mStates[2].selected;
            mStates[3].active = mStates[3].selected;
        } else {
            bool hasEsc = (mCharacterStats.count(0x1b) > 0);
            bool hasHZ = (mCharacterStats.count(0x7e) > 0);
            if (hasEsc) {
                mStates[4].active = mStates[4].selected;
            }
            if (hasHZ) {
                mStates[5].active = mStates[5].selected;
            }
        }
    }

    for (auto &state : mStates) {
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
    auto [name, confidence] = mProberState->GetBestProber();
    return name;
}

std::array<float, 8> ProberState::CalculateBias() const
{
    const float isBE16 = mCharacterStats.isBigEndian16();

    std::array<float, 8> bias{};
    bias[0] = 0.9 * mCharacterStats.isUtf8();
    bias[6] = std::max(0.9f * isBE16, 0.0f);
    bias[7] = std::max(-0.9f * isBE16, 0.0f);
    if (auto mbcsConf = mStates[1].active ? mStates[1].prober->GetConfidence() : 0.0f; mbcsConf > 0.0f) {
        // MBCS often looks like UTF-16
        bias[6] *= bias[6] / (bias[6] + mbcsConf);
        bias[7] *= bias[7] / (bias[7] + mbcsConf);
    }
    return bias;
}

std::pair<const char *, float> ProberState::GetBestProber() const
{
    if (mCharacterStats.totalCount == 0) {
        return {"", MINIMUM_THRESHOLD};
    }

    if (mDetectedCharset) {
        return {mDetectedCharset, 0.99f};
    }

    const float isBE16 = mCharacterStats.isBigEndian16();
    if (!mHas8Bit) {
        if (isBE16 > 0.2f) {
            return {"UTF-16BE", isBE16};
        } else if (isBE16 < -0.2f) {
            return {"UTF-16LE", -isBE16};
        }
        return {"UTF-8", 0.99f};
    }

    const auto bias = CalculateBias();

    const char *bestCharSet = nullptr;
    float maxProberConfidence = 0.0f;
    for (size_t i = 0; const auto &state : mStates) {
        if (state.active) {
            float proberConfidence = state.prober->GetConfidence();
            proberConfidence += bias[i];
            if (proberConfidence > maxProberConfidence) {
                maxProberConfidence = proberConfidence;
                bestCharSet = state.prober->GetCharSetName();
            }
        }
        i++;
    }
    // do not report anything because we are not confident of it, that's in fact a negative answer
    if (maxProberConfidence > MINIMUM_THRESHOLD) {
        return {bestCharSet, maxProberConfidence};
    } else if (mStates[0].active) {
        // Default to UTF-8, but only if valid
        return {"UTF-8", MINIMUM_THRESHOLD};
    }
    return {bestCharSet, MINIMUM_THRESHOLD};
}

//---------------------------------------------------------------------
float nsUniversalDetector::GetConfidence()
{
    auto [name, confidence] = mProberState->GetBestProber();
    return confidence;
}

nsProbingState nsUniversalDetector::GetState()
{
    if (mProberState->mDone) {
        return eFoundIt;
    } else {
        return eDetecting;
    }
}

std::string nsUniversalDetector::StatusOutput(uint8_t indent)
{
    const auto bias = (*mProberState).CalculateBias();
    indent += 2;
    std::string output = std::format("  Universal Prober ---- (7Bit: {})", mProberState->mHas8Bit ? "0" : "1");
    for (size_t i = 0; const auto &prober : (*mProberState).mStates) {
        char state = !prober.selected ? '.' : !prober.active ? '-' : ' ';
        output += '\n' + std::string(indent, ' ');
        output += std::format("{} {:1.3f} ", state, bias[i++] + prober.prober->GetConfidence());
        output += prober.prober->StatusOutput(indent);
    }
    return output;
}
}
