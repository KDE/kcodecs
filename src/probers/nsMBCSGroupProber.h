/*  -*- C++ -*-
*  Copyright (C) 1998 <developer@mozilla.org>
*
*
*  Permission is hereby granted, free of charge, to any person obtaining
*  a copy of this software and associated documentation files (the
*  "Software"), to deal in the Software without restriction, including
*  without limitation the rights to use, copy, modify, merge, publish,
*  distribute, sublicense, and/or sell copies of the Software, and to
*  permit persons to whom the Software is furnished to do so, subject to
*  the following conditions:
*
*  The above copyright notice and this permission notice shall be included
*  in all copies or substantial portions of the Software.
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
*  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
*  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
*  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
*  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef nsMBCSGroupProber_h__
#define nsMBCSGroupProber_h__

#include "nsSJISProber.h"
#include "UnicodeGroupProber.h"
#include "nsEUCJPProber.h"
#include "nsGB2312Prober.h"
#include "nsEUCKRProber.h"
#include "nsBig5Prober.h"
#include "nsEUCTWProber.h"

#define NUM_OF_PROBERS    7
namespace kencodingprober
{
class KCODECS_NO_EXPORT nsMBCSGroupProber: public nsCharSetProber
{
public:
    nsMBCSGroupProber();
    virtual ~nsMBCSGroupProber();
    nsProbingState HandleData(const char *aBuf, unsigned int aLen) Q_DECL_OVERRIDE;
    const char *GetCharSetName() Q_DECL_OVERRIDE;
    nsProbingState GetState(void) Q_DECL_OVERRIDE
    {
        return mState;
    };
    void      Reset(void) Q_DECL_OVERRIDE;
    float     GetConfidence(void) Q_DECL_OVERRIDE;
    void      SetOpion() Q_DECL_OVERRIDE {};

#ifdef DEBUG_PROBE
    void  DumpStatus() Q_DECL_OVERRIDE;
#endif

protected:
    nsProbingState mState;
    nsCharSetProber *mProbers[NUM_OF_PROBERS];
    bool          mIsActive[NUM_OF_PROBERS];
    int mBestGuess;
    unsigned int mActiveNum;
};
}

#endif /* nsMBCSGroupProber_h__ */

