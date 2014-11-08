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

#ifndef nsLatin1Prober_h__
#define nsLatin1Prober_h__

#include "nsCharSetProber.h"

#define FREQ_CAT_NUM    4
namespace kencodingprober
{
class KCODECS_NO_EXPORT nsLatin1Prober: public nsCharSetProber
{
public:
    nsLatin1Prober(void)
    {
        Reset();
    };
    virtual ~nsLatin1Prober(void) {};
    nsProbingState HandleData(const char *aBuf, unsigned int aLen) Q_DECL_OVERRIDE;
    const char *GetCharSetName() Q_DECL_OVERRIDE
    {
        return "windows-1252";
    };
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
    char mLastCharClass;
    unsigned int mFreqCounter[FREQ_CAT_NUM];
};
}

#endif /* nsLatin1Prober_h__ */

