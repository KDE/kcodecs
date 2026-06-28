/*  -*- C++ -*-
    SPDX-FileCopyrightText: 2026 Stefan Brüns <stefan.bruens@rwth-aachen.de>

    SPDX-License-Identifier: MIT
*/

#include "nsUtfProber.h"
#include "nsMBCSSM.h"

namespace kencodingprober
{
constexpr auto modelForProber(UtfProberType type)
{
    if (type == Utf8) {
        return kencodingprober::UTF8SMModel;
    } else if (type == Utf16LE) {
        return kencodingprober::UCS2LESMModel;
    } else {
        return kencodingprober::UCS2BESMModel;
    }
}

template<UtfProberType TYPE>
struct UtfSM {
    static nsSMState advance(nsSMState oldState, const char c)
    {
        const uint8_t index = static_cast<uint8_t>(c);
        const unsigned int rowIndex = oldState * sModel.classFactor;
        const unsigned int byteCls = sModel.classTable[index];
        return sModel.stateTable[rowIndex + byteCls];
    }
    static constexpr auto sModel = modelForProber(TYPE);
};

template<UtfProberType TYPE>
nsProbingState nsUtfProber<TYPE>::HandleData(const char *aBuf, unsigned int aLen)
{
    if (mState == eNotMe) {
        return mState;
    }

    for (unsigned int i = 0; i < aLen; ++i) {
        // byte is feed to all active state machine
        mCodingState = UtfSM<TYPE>::advance(mCodingState, aBuf[i]);
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

template<UtfProberType TYPE>
float nsUtfProber<TYPE>::GetConfidence()
{
    return mState == eNotMe ? 0.0f : mState == eFoundIt ? 0.8f : 0.1f;
}

template class nsUtfProber<UtfProberType::Utf8>;
template class nsUtfProber<UtfProberType::Utf16LE>;
template class nsUtfProber<UtfProberType::Utf16BE>;

template<>
const char *nsUtf8Prober::GetCharSetName()
{
    return "UTF-8";
}

template<>
const char *nsUtf16LEProber::GetCharSetName()
{
    return "UTF-16LE";
}

template<>
const char *nsUtf16BEProber::GetCharSetName()
{
    return "UTF-16BE";
}

} // namespace kencodingprober
