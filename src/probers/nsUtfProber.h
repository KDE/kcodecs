/*  -*- C++ -*-
    SPDX-FileCopyrightText: 2026 Stefan Brüns <stefan.bruens@rwth-aachen.de>

    SPDX-License-Identifier: MIT
*/

#ifndef NSUTFPROBER_H
#define NSUTFPROBER_H

#include "nsCharSetProber.h"
#include "nsCodingStateMachine.h"

enum UtfProberType : uint8_t {
    Utf8 = 0,
    Utf16LE = 1,
    Utf16BE = 2,
};

namespace kencodingprober
{
template<UtfProberType MODEL>
class KCODECS_NO_EXPORT nsUtfProber : public nsCharSetProber
{
public:
    nsUtfProber() = default;
    ~nsUtfProber() override = default;

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

using nsUtf8Prober = nsUtfProber<UtfProberType::Utf8>;
using nsUtf16LEProber = nsUtfProber<UtfProberType::Utf16LE>;
using nsUtf16BEProber = nsUtfProber<UtfProberType::Utf16BE>;

} // namespace kencodingprober

#endif // NSUTFPROBER_H
