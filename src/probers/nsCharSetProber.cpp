/*  -*- C++ -*-
    SPDX-FileCopyrightText: 1998 Netscape Communications Corporation <developer@mozilla.org>

    SPDX-License-Identifier: MIT
*/

#include "nsCharSetProber.h"

#include <stdlib.h>

namespace kencodingprober
{
// This filter applies to all scripts which do not use English characters
bool nsCharSetProber::FilterWithoutEnglishLetters(const char *aBuf, unsigned int aLen, char **newBuf, unsigned int &newLen)
{
    char *newptr;
    char *prevPtr;
    char *curPtr;

    bool meetMSB = false;
    newptr = *newBuf = (char *)malloc(aLen);
    if (!newptr) {
        return false;
    }

    for (curPtr = prevPtr = (char *)aBuf; curPtr < aBuf + aLen; ++curPtr) {
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

// This filter applies to all scripts which contain both English characters and upper ASCII characters.
bool nsCharSetProber::FilterWithEnglishLetters(const char *aBuf, unsigned int aLen, char **newBuf, unsigned int &newLen)
{
    // do filtering to reduce load to probers
    char *newptr;
    char *prevPtr;
    char *curPtr;
    bool isInTag = false;

    newptr = *newBuf = (char *)malloc(aLen);
    if (!newptr) {
        return false;
    }

    for (curPtr = prevPtr = (char *)aBuf; curPtr < aBuf + aLen; ++curPtr) {
        if (*curPtr == '>') {
            isInTag = false;
        } else if (*curPtr == '<') {
            isInTag = true;
        }

        if (!(*curPtr & 0x80) //
            && (*curPtr < 'A' || (*curPtr > 'Z' && *curPtr < 'a') || *curPtr > 'z')) {
            if (curPtr > prevPtr && !isInTag) // Current segment contains more than just a symbol
                                              // and it is not inside a tag, keep it.
            {
                while (prevPtr < curPtr) {
                    *newptr++ = *prevPtr++;
                }
                prevPtr++;
                *newptr++ = ' ';
            } else {
                prevPtr = curPtr + 1;
            }
        }
    }

    // If the current segment contains more than just a symbol
    // and it is not inside a tag then keep it.
    if (!isInTag) {
        while (prevPtr < curPtr) {
            *newptr++ = *prevPtr++;
        }
    }

    newLen = newptr - *newBuf;

    return true;
}
}
