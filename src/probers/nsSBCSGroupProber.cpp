/*  -*- C++ -*-
    SPDX-FileCopyrightText: 1998 Netscape Communications Corporation <developer@mozilla.org>

    SPDX-License-Identifier: MIT
*/

#include "nsSBCSGroupProber.h"

#include "nsHebrewProber.h"
#include "nsSBCharSetProber.h"
#include "nsUtfProber.h"

#include <format>

namespace kencodingprober
{
nsSBCSGroupProber::nsSBCSGroupProber()
    : mProbers{
          std::make_unique<nsSingleByteCharSetProber<false>>(&Win1251Model),
          std::make_unique<nsSingleByteCharSetProber<false>>(&Koi8rModel),
          std::make_unique<nsSingleByteCharSetProber<false>>(&Latin5Model),
          std::make_unique<nsSingleByteCharSetProber<false>>(&MacCyrillicModel),
          std::make_unique<nsSingleByteCharSetProber<false>>(&Ibm866Model),
          std::make_unique<nsSingleByteCharSetProber<false>>(&Ibm855Model),
          std::make_unique<nsSingleByteCharSetProber<false>>(&Latin7Model),
          std::make_unique<nsSingleByteCharSetProber<false>>(&Win1253Model),
          std::make_unique<nsSingleByteCharSetProber<false>>(&Latin5BulgarianModel),
          std::make_unique<nsSingleByteCharSetProber<false>>(&Win1251BulgarianModel),
          std::make_unique<nsHebrewProber>(),
          std::make_unique<nsUtf8Prober>(),
          std::make_unique<nsUtf16BEProber>(),
          std::make_unique<nsUtf16LEProber>(),
      }
{
    // disable latin2 before latin1/windows-1252 is available, otherwise all latin1
    // will be detected as latin2 because of their similarity.
    // mProbers[14] = std::make_unique<nsSingleByteCharSetProber<false>>(&Latin2HungarianModel);
    // mProbers[15] = std::make_unique<nsSingleByteCharSetProber<false>>(&Win1250HungarianModel);

    for (unsigned int i = 0; i < NUM_OF_SBCS_PROBERS; i++) {
        if (mProbers[i]) { // not null
            mIsActive[i] = true;
        }
    }
}

const char *nsSBCSGroupProber::GetCharSetName()
{
    // if we have no answer yet
    if (mBestGuess == -1) {
        GetConfidence();
        // no charset seems positive
        if (mBestGuess == -1)
        // we will use default.
        {
            mBestGuess = 0;
        }
    }
    return mProbers[mBestGuess]->GetCharSetName();
}

nsProbingState nsSBCSGroupProber::HandleData(const char *aBuf, unsigned int aLen)
{
    if (mState != eDetecting) {
        return mState;
    }

    char *newBuf1 = nullptr;
    unsigned int newLen1 = 0;

    int activeNum = NUM_OF_SBCS_PROBERS;

    // The UTF16 probers need unmangled data
    for (unsigned int i = NUM_OF_SBCS_PROBERS - 2; i < NUM_OF_SBCS_PROBERS; ++i) {
        if (!mIsActive[i]) {
            activeNum--;
            continue;
        }
        if (const auto st = mProbers[i]->HandleData(aBuf, aLen); st == eFoundIt) {
            mBestGuess = i;
            mState = eFoundIt;
            return mState;
        } else if (st == eNotMe) {
            mIsActive[i] = false;
            activeNum--;
        }
    }

    // apply filter to original buffer, and we got new buffer back
    // depend on what script it is, we will feed them the new buffer
    // we got after applying proper filter
    // this is done without any consideration to KeepEnglishLetters
    // of each prober since as of now, there are no probers here which
    // recognize languages with English characters.
    if (!FilterWithoutEnglishLetters(aBuf, aLen, &newBuf1, newLen1)) {
        goto done;
    }

    if (newLen1 == 0) {
        goto done; // Nothing to see here, move on.
    }

    for (unsigned int i = 0; i < NUM_OF_SBCS_PROBERS - 2; ++i) {
        if (!mIsActive[i]) {
            activeNum--;
            continue;
        }
        auto st = mProbers[i]->HandleData(newBuf1, newLen1);
        if (st == eFoundIt) {
            mBestGuess = i;
            mState = eFoundIt;
            break;
        } else if (st == eNotMe) {
            mIsActive[i] = false;
            activeNum--;
        }
    }

    if (activeNum == 0) {
        mState = eNotMe;
    }

done:
    free(newBuf1);

    return mState;
}

float nsSBCSGroupProber::GetConfidence(void)
{
    float bestConf = 0.0;

    switch (mState) {
    case eFoundIt:
        return 0.99f; // sure yes
    case eNotMe:
        return 0.01f; // sure no
    default:
        for (unsigned int i = 0; i < NUM_OF_SBCS_PROBERS; ++i) {
            if (!mIsActive[i]) {
                continue;
            }
            float cf = mProbers[i]->GetConfidence();
            if (bestConf < cf) {
                bestConf = cf;
                mBestGuess = i;
            }
        }
    }
    return bestConf;
}

std::string nsSBCSGroupProber::StatusOutput(uint8_t indent)
{
    indent += 2;
    std::string output{"  SBCS Group Prober ----"};
    GetConfidence();
    for (int i = 0; i < NUM_OF_SBCS_PROBERS; i++) {
        char state = !mIsActive[i] ? '-' : (i == mBestGuess) ? '*' : ' ';
        output += '\n' + std::string(indent, ' ');
        output += std::format("{} #{:02}  SBCS: ", state, i);
        output += mProbers[i]->StatusOutput(indent);
    }
    return output;
}
}
