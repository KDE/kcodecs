/*  -*- C++ -*-
    SPDX-FileCopyrightText: 1998 Netscape Communications Corporation <developer@mozilla.org>
    SPDX-FileCopyrightText: 2008 Wang Kai <zealot.kai@gmail.com>

    SPDX-License-Identifier: MIT
*/

#ifndef nsUniversalDetector_h__
#define nsUniversalDetector_h__

#include "nsCharSetProber.h"

#include <array>
#include <memory>

namespace kencodingprober
{
class KCODECS_NO_EXPORT nsUniversalDetector : public nsCharSetProber
{
public:
    nsUniversalDetector();
    ~nsUniversalDetector() override = default;
    nsProbingState HandleData(const char *aBuf, unsigned int aLen) override;
    const char *GetCharSetName() override;
    float GetConfidence(void) override;
    nsProbingState GetState() override;

    std::string StatusOutput(uint8_t indent) override;

protected:
    bool mDone = false;
    bool mGotData = false;
    bool mHas8Bit = false;
    char mLastChar = '\0';
    const char *mDetectedCharset = nullptr;

    std::array<std::unique_ptr<nsCharSetProber>, 8> mCharSetProbers = {};
};
}

#endif
