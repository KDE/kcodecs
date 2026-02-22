/*  -*- C++ -*-
    SPDX-FileCopyrightText: 2008 Wang Kai <wkai@gmail.com>

    SPDX-License-Identifier: MIT
*/

#include "UnicodeGroupProber.h"

namespace kencodingprober
{
UnicodeGroupProber::UnicodeGroupProber(void)
{
    mCodingSM[0] = new nsCodingStateMachine(&UTF8SMModel);
    mCodingSM[1] = new nsCodingStateMachine(&UCS2LESMModel);
    mCodingSM[2] = new nsCodingStateMachine(&UCS2BESMModel);
    mActiveSM = NUM_OF_UNICODE_CHARSETS;
    mState = eDetecting;
    mDetectedCharset = "UTF-8";
}

UnicodeGroupProber::~UnicodeGroupProber(void)
{
    for (unsigned int i = 0; i < NUM_OF_UNICODE_CHARSETS; i++) {
        delete mCodingSM[i];
    }
}

void UnicodeGroupProber::Reset(void)
{
    mState = eDetecting;
    for (unsigned int i = 0; i < NUM_OF_UNICODE_CHARSETS; i++) {
        mCodingSM[i]->Reset();
    }
    mActiveSM = NUM_OF_UNICODE_CHARSETS;
    mDetectedCharset = "UTF-8";
}

nsProbingState UnicodeGroupProber::HandleData(const char *aBuf, unsigned int aLen)
{
    if (mActiveSM == 0) {
        mState = eNotMe;
        return mState;
    }

    for (int j = mActiveSM - 1; j >= 0; --j) {
        for (unsigned int i = 0; i < aLen; ++i) {
            // byte is feed to all active state machine
            nsSMState codingState = mCodingSM[j]->NextState(aBuf[i]);
            if (codingState == eError) {
                // got negative answer for this state machine, make it inactive
                mActiveSM--;
                if (mActiveSM == 0) {
                    mState = eNotMe;
                    return mState;
                } else if (j != (int)mActiveSM) {
                    nsCodingStateMachine *t;
                    t = mCodingSM[mActiveSM];
                    mCodingSM[mActiveSM] = mCodingSM[j];
                    mCodingSM[j] = t;
                }
                break;
            } else if (codingState == eItsMe) {
                mState = eFoundIt;
                mDetectedCharset = mCodingSM[j]->GetCodingStateMachine();
                return mState;
            }
        }
    }
    mDetectedCharset = mCodingSM[0]->GetCodingStateMachine();
    return mState;
}

float UnicodeGroupProber::GetConfidence()
{
    if (mState == eFoundIt) {
        return 0.99f;
    } else {
        return 0.0f;
    }
}

#ifdef DEBUG_PROBE
void UnicodeGroupProber::DumpStatus()
{
    GetConfidence();
    for (uint i = 0; i < mActiveSM; i++) {
        qDebug() << "Unicode group" << mCodingSM[i]->DumpCurrentState() << mCodingSM[i]->GetCodingStateMachine();
    }
}
#endif

}
