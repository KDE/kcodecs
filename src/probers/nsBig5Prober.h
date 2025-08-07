/*  -*- C++ -*-
    SPDX-FileCopyrightText: 1998 Netscape Communications Corporation <developer@mozilla.org>

    SPDX-License-Identifier: MIT
*/

#ifndef nsBig5Prober_h__
#define nsBig5Prober_h__

#include "CharDistribution.h"
#include "nsCharSetProber.h"
#include "nsCodingStateMachine.h"
namespace kencodingprober
{
class KCODECS_NO_EXPORT nsBig5Prober final : public nsCharSetProber
{
public:
    explicit nsBig5Prober(void)
    {
        mCodingSM = new nsCodingStateMachine(&Big5SMModel);
        Reset();
    }
    ~nsBig5Prober() override
    {
        delete mCodingSM;
    }
    nsProbingState HandleData(const char *aBuf, unsigned int aLen) override;
    const char *GetCharSetName() override
    {
        return "Big5";
    }
    nsProbingState GetState(void) override
    {
        return mState;
    }
    void Reset(void) final;
    float GetConfidence(void) override;

    Q_DISABLE_COPY_MOVE(nsBig5Prober)

protected:
    void GetDistribution(unsigned int aCharLen, const char *aStr);

    nsCodingStateMachine *mCodingSM;
    nsProbingState mState;

    // Big5ContextAnalysis mContextAnalyser;
    Big5DistributionAnalysis mDistributionAnalyser;
    char mLastChar[2];
};
}

#endif /* nsBig5Prober_h__ */
