/*  -*- C++ -*-
    SPDX-FileCopyrightText: 2008 Wang Kai <wkai@gmail.com>

    SPDX-License-Identifier: MIT
*/

#include "UnicodeGroupProber.h"
#include "nsMBCSSM.h"

#include <format>

namespace kencodingprober
{
UnicodeGroupProber::UnicodeGroupProber()
    : mCodingSM{
          std::make_unique<nsCodingStateMachine>(&UTF8SMModel),
          std::make_unique<nsCodingStateMachine>(&UCS2LESMModel),
          std::make_unique<nsCodingStateMachine>(&UCS2BESMModel),
      }
{
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
                    std::swap(mCodingSM[mActiveSM], mCodingSM[j]);
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

std::string UnicodeGroupProber::StatusOutput(uint8_t indent)
{
    indent += 2;
    GetConfidence();
    std::string output{"  Unicode Group Prober ----"};
    for (unsigned int i = 0; i < mActiveSM; i++) {
        output += '\n' + std::string(indent, ' ');
        output += std::format("  [{}] {}", //
                              mCodingSM[i]->GetCodingStateMachine(),
                              mCodingSM[i]->DumpCurrentState());
    }
    return output;
}
}
