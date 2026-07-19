/*  -*- C++ -*-
    SPDX-FileCopyrightText: 2008 Wang Kai <wkai@gmail.com>

    SPDX-License-Identifier: MIT
*/

#ifndef UNICODEGROUPPROBER_H
#define UNICODEGROUPPROBER_H

#include "nsCharSetProber.h"

#include <array>
#include <memory>

#define NUM_OF_UTF_PROBERS 3

namespace kencodingprober
{
class KCODECS_NO_EXPORT UnicodeGroupProber : public nsCharSetProber
{
public:
    UnicodeGroupProber();
    ~UnicodeGroupProber() override = default;

    nsProbingState HandleData(const char *aBuf, unsigned int aLen) override;
    const char *GetCharSetName() override;
    nsProbingState GetState(void) override
    {
        return mState;
    }
    float GetConfidence() override;

    std::string StatusOutput(uint8_t indent) override;

protected:
    nsProbingState mState = eDetecting;
    std::array<std::unique_ptr<nsCharSetProber>, NUM_OF_UTF_PROBERS> mProbers = {nullptr};
    std::array<bool, NUM_OF_UTF_PROBERS> mIsActive = {false};
    int mBestGuess = -1;
};
}
#endif /* UNICODEGROUPPROBER_H */
