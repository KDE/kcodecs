/*  -*- C++ -*-
    SPDX-FileCopyrightText: 2026 Stefan Brüns <stefan.bruens@rwth-aachen.de>

    SPDX-License-Identifier: MIT
*/

#ifndef nsStateMachineProber_h
#define nsStateMachineProber_h

#include "nsCharSetProber.h"
#include "nsCodingStateMachine.h"

namespace kencodingprober
{
enum class SMProberType : uint8_t {
    Utf8 = 0,
    Utf16LE = 1,
    Utf16BE = 2,
    ISO2022_JP = 3,
    HZ = 4,
};

template<SMProberType MODEL>
constexpr const SMModel &modelForProber();

template<SMProberType MODEL>
class KCODECS_NO_EXPORT StateMachineProber : public nsCharSetProber
{
public:
    StateMachineProber() = default;
    ~StateMachineProber() override = default;

    nsProbingState HandleData(const char *aBuf, unsigned int aLen) override;

    const char *GetCharSetName() override;

    nsProbingState GetState(void) override
    {
        return mState;
    }

    float GetConfidence() override;

protected:
    nsProbingState mState = eDetecting;
    nsSMState mCodingState = 0;
};

using nsUtf8Prober = StateMachineProber<SMProberType::Utf8>;
using nsUtf16LEProber = StateMachineProber<SMProberType::Utf16LE>;
using nsUtf16BEProber = StateMachineProber<SMProberType::Utf16BE>;

} // namespace kencodingprober

#endif // nsStateMachineProber_h
