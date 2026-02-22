/*  -*- C++ -*-
    SPDX-FileCopyrightText: 2008 Wang Kai <wkai@gmail.com>

    SPDX-License-Identifier: MIT
*/

#ifndef UNICODEGROUPPROBER_H
#define UNICODEGROUPPROBER_H

#include "nsCharSetProber.h"
#include "nsCodingStateMachine.h"

#include <memory>

#define NUM_OF_UNICODE_CHARSETS 3
namespace kencodingprober
{
class KCODECS_NO_EXPORT UnicodeGroupProber : public nsCharSetProber
{
public:
    UnicodeGroupProber();
    ~UnicodeGroupProber() override = default;

    nsProbingState HandleData(const char *aBuf, unsigned int aLen) override;
    const char *GetCharSetName() override
    {
        return mDetectedCharset;
    }
    nsProbingState GetState(void) override
    {
        return mState;
    }
    void Reset(void) override;
    float GetConfidence() override;
#ifdef DEBUG_PROBE
    void DumpStatus() override;
#endif

protected:
    std::unique_ptr<nsCodingStateMachine> mCodingSM[NUM_OF_UNICODE_CHARSETS];
    unsigned int mActiveSM;
    nsProbingState mState;
    const char *mDetectedCharset;
};
}
#endif /* UNICODEGROUPPROBER_H */
