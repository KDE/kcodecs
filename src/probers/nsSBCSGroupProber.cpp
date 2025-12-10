/*  -*- C++ -*-
    SPDX-FileCopyrightText: 1998 Netscape Communications Corporation <developer@mozilla.org>

    SPDX-License-Identifier: MIT
*/

#include "nsSBCSGroupProber.h"

#include "UnicodeGroupProber.h"
#include "nsHebrewProber.h"
#include "nsSBCharSetProber.h"

#include <stdio.h>
#include <stdlib.h>

namespace kencodingprober
{
nsSBCSGroupProber::nsSBCSGroupProber()
{
    mProbers[0] = new nsSingleByteCharSetProber(&Win1251Model);
    mProbers[1] = new nsSingleByteCharSetProber(&Koi8rModel);
    mProbers[2] = new nsSingleByteCharSetProber(&Latin5Model);
    mProbers[3] = new nsSingleByteCharSetProber(&MacCyrillicModel);
    mProbers[4] = new nsSingleByteCharSetProber(&Ibm866Model);
    mProbers[5] = new nsSingleByteCharSetProber(&Ibm855Model);
    mProbers[6] = new nsSingleByteCharSetProber(&Latin7Model);
    mProbers[7] = new nsSingleByteCharSetProber(&Win1253Model);
    mProbers[8] = new nsSingleByteCharSetProber(&Latin5BulgarianModel);
    mProbers[9] = new nsSingleByteCharSetProber(&Win1251BulgarianModel);

    mProbers[10] = new nsHebrewProber();
    mProbers[11] = new UnicodeGroupProber();

    // disable latin2 before latin1 is available, otherwise all latin1
    // will be detected as latin2 because of their similarity.
    // mProbers[10] = new nsSingleByteCharSetProber(&Latin2HungarianModel);
    // mProbers[11] = new nsSingleByteCharSetProber(&Win1250HungarianModel);

    Reset();
}

nsSBCSGroupProber::~nsSBCSGroupProber()
{
    for (unsigned int i = 0; i < NUM_OF_SBCS_PROBERS; i++) {
        delete mProbers[i];
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

void nsSBCSGroupProber::Reset(void)
{
    mActiveNum = 0;
    for (unsigned int i = 0; i < NUM_OF_SBCS_PROBERS; i++) {
        if (mProbers[i]) { // not null
            mProbers[i]->Reset();
            mIsActive[i] = true;
            ++mActiveNum;
        } else {
            mIsActive[i] = false;
        }
    }
    mBestGuess = -1;
    mState = eDetecting;
}

nsProbingState nsSBCSGroupProber::HandleData(const char *aBuf, unsigned int aLen)
{
    nsProbingState st;
    unsigned int i;
    char *newBuf1 = nullptr;
    unsigned int newLen1 = 0;

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

    for (i = 0; i < NUM_OF_SBCS_PROBERS; ++i) {
        if (!mIsActive[i]) {
            continue;
        }
        st = mProbers[i]->HandleData(newBuf1, newLen1);
        if (st == eFoundIt) {
            mBestGuess = i;
            mState = eFoundIt;
            break;
        } else if (st == eNotMe) {
            mIsActive[i] = false;
            mActiveNum--;
            if (mActiveNum == 0) {
                mState = eNotMe;
                break;
            }
        }
    }

done:
    free(newBuf1);

    return mState;
}

float nsSBCSGroupProber::GetConfidence(void)
{
    unsigned int i;
    float bestConf = 0.0;
    float cf;

    switch (mState) {
    case eFoundIt:
        return (float)0.99; // sure yes
    case eNotMe:
        return (float)0.01; // sure no
    default:
        for (i = 0; i < NUM_OF_SBCS_PROBERS; ++i) {
            if (!mIsActive[i]) {
                continue;
            }
            cf = mProbers[i]->GetConfidence();
            if (bestConf < cf) {
                bestConf = cf;
                mBestGuess = i;
            }
        }
    }
    return bestConf;
}

#ifdef DEBUG_PROBE
void nsSBCSGroupProber::DumpStatus()
{
    unsigned int i;
    float cf;

    cf = GetConfidence();
    printf(" SBCS Group Prober --------begin status \r\n");
    for (i = 0; i < NUM_OF_SBCS_PROBERS; i++) {
        if (!mIsActive[i]) {
            printf("  inactive: [%s] (i.e. confidence is too low).\r\n", mProbers[i]->GetCharSetName());
        } else {
            mProbers[i]->DumpStatus();
        }
    }
    printf(" SBCS Group found best match [%s] confidence %f.\r\n", mProbers[mBestGuess]->GetCharSetName(), cf);
}
#endif
}
