/*  -*- C++ -*-
    SPDX-FileCopyrightText: 2026 Stefan Brüns <stefan.bruens@rwth-aachen.de>

    SPDX-License-Identifier: MIT
*/

#ifndef StateMachineProber_inl
#define StateMachineProber_inl

#include "StateMachineProber.h"

namespace kencodingprober
{

template<SMProberType TYPE>
struct StateMachine {
    static nsSMState advance(nsSMState oldState, const char c)
    {
        const uint8_t index = static_cast<uint8_t>(c);
        const unsigned int rowIndex = oldState * sModel.classFactor;
        const unsigned int byteCls = sModel.classTable[index];
        return sModel.stateTable[rowIndex + byteCls];
    }
    static constexpr auto sModel = modelForProber<TYPE>();
};

template<SMProberType TYPE>
nsProbingState StateMachineProber<TYPE>::HandleData(const char *aBuf, unsigned int aLen)
{
    if (mState == eNotMe) {
        return mState;
    }

    for (unsigned int i = 0; i < aLen; ++i) {
        // byte is feed to all active state machine
        mCodingState = StateMachine<TYPE>::advance(mCodingState, aBuf[i]);
        if (mCodingState == eError) {
            // got negative answer for this state machine, make it inactive
            mState = eNotMe;
            return mState;
        } else if (mCodingState == eItsMe) {
            mState = eFoundIt;
            return mState;
        }
    }
    return mState;
};

template<SMProberType TYPE>
float StateMachineProber<TYPE>::GetConfidence()
{
    return mState == eNotMe ? 0.0f : mState == eFoundIt ? 0.8f : 0.1f;
}

template<SMProberType TYPE>
const char *StateMachineProber<TYPE>::GetCharSetName()
{
    static constexpr auto sModel = modelForProber<TYPE>();
    return sModel.name;
}

} // namespace kencodingprober

#endif // StateMachineProber_inl
