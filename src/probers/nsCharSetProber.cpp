/*  -*- C++ -*-
    SPDX-FileCopyrightText: 1998 Netscape Communications Corporation <developer@mozilla.org>

    SPDX-License-Identifier: MIT
*/

#include "nsCharSetProber.h"

#include <kcodecs_prober_debug.h>

#include <format>

namespace kencodingprober
{
// This filter applies to all scripts which do not use English characters
bool nsCharSetProber::FilterWithoutEnglishLetters(const char *aBuf, unsigned int aLen, char **newBuf, unsigned int &newLen)
{
    char *newptr;
    const char *prevPtr;
    const char *curPtr;

    bool meetMSB = false;
    newptr = *newBuf = (char *)malloc(aLen);
    if (!newptr) {
        return false;
    }

    for (curPtr = prevPtr = aBuf; curPtr < aBuf + aLen; ++curPtr) {
        if (*curPtr & 0x80) {
            meetMSB = true;
        } else if (*curPtr < 'A' || (*curPtr > 'Z' && *curPtr < 'a') || *curPtr > 'z') {
            // current char is a symbol, most likely a punctuation. we treat it as segment delimiter
            if (meetMSB && curPtr > prevPtr)
            // this segment contains more than single symbol, and it has upper ASCII, we need to keep it
            {
                while (prevPtr < curPtr) {
                    *newptr++ = *prevPtr++;
                }
                prevPtr++;
                *newptr++ = ' ';
                meetMSB = false;
            } else { // ignore current segment. (either because it is just a symbol or just an English word)
                prevPtr = curPtr + 1;
            }
        }
    }
    if (meetMSB && curPtr > prevPtr) {
        while (prevPtr < curPtr) {
            *newptr++ = *prevPtr++;
        }
    }

    newLen = newptr - *newBuf;

    return true;
}

void nsCharSetProber::DumpStatus()
{
    qCInfo(KCODECSPROBER).noquote().nospace() << "\n" << StatusOutput(0);
}

std::string nsCharSetProber::StatusOutput(uint8_t /* indent */)
{
    return std::format("{:1.3f} [{}]", GetConfidence(), GetCharSetName());
}
}
