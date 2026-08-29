/*  -*- C++ -*-
    SPDX-FileCopyrightText: 1998 Netscape Communications Corporation <developer@mozilla.org>

    SPDX-License-Identifier: MIT
*/

#ifndef nsCharSetProber_h__
#define nsCharSetProber_h__

#include <kcodecs_export.h>

#include <cstdint>
#include <string>

namespace kencodingprober
{
typedef enum {
    eDetecting = 0, // We are still detecting, no sure answer yet, but caller can ask for confidence.
    eFoundIt = 1, // That's a positive answer
    eNotMe = 2, // Negative answer
} nsProbingState;

#define SHORTCUT_THRESHOLD 0.95f

class KCODECS_NO_EXPORT nsCharSetProber
{
public:
    enum class Prober : uint8_t {
        Utf8 = 0, //< 8 bit variable length (1/2/3/4 byte) Unicode
        Utf16LE = 1, //< 16 bit Unicode with surrogates, little endian
        Utf16BE = 2, //< 16 bit Unicode with surrogates, big endian

        SJIS = 3, //< JIS-X-0201/0208
        EUCJP = 4, //< JIS-X-0201/0208
        ISO2022_JP = 5, // 7bit ISO-2022 encoding for JIS-X-0201/0208

        GB18030 = 6, //< GB18030 and GB2313 predecessor/subset, 8bit, 1/2/4 byte encoding
        Big5 = 7, //< Big5 Traditional Chinese
        HZ = 8, // 7 bit GB2313 encoding, "Hanzi"

        EUCKR = 9, //< Korean/Hangul KS X 1001/1003

        Latin7_Greek = 10, //< ISO-8859-7 Latin/Greek
        Windows1253_Greek = 11, //< Windows-1253 Latin/Greek

        ISO8859_8_HebrewVisual = 12, //< ISO-8859-7 Hebrew in visual order
        Windows1255_HebrewLogical = 12, //< Windows-1255 Hebrew in logical order

        ISO8859_5 = 14, //< ISO-8859-5 Latin/Cyrillic
        Windows1251 = 15, //< Windows-1251 Latin/Cyrillic
        KOI8_R = 16, //< KOI8-R Latin/Cyrillic
        MAC_Cyrillic = 17, //< MAC OS Cyrillic charset
        IBM866 = 18, //< Code page 866 Latin/Cyrillic
        IBM855 = 19, //< Code page 855 Latin/Cyrillic

        ISO8859_5_Bulgarian = 20, //< ISO-8859-5 Latin/Cyrillic (Bulgarian language model)
        Windows1251_Bulgarian = 21, //< Windows-1251 Latin/Cyrillic (Bulgarian language model)

        Windows1252_Latin1 = 22, //< Windows-1252 Latin1
    };

    virtual ~nsCharSetProber() = default;
    virtual const char *GetCharSetName() = 0;
    virtual nsProbingState HandleData(const char *aBuf, unsigned int aLen) = 0;
    virtual nsProbingState GetState(void) = 0;
    virtual float GetConfidence(void) = 0;

    void DumpStatus();
    virtual std::string StatusOutput(uint8_t indent);

    // Helper functions used in the Latin1 and Group probers.
    // both functions Allocate a new buffer for newBuf. This buffer should be
    // freed by the caller using PR_FREEIF.
    // Both functions return false in case of memory allocation failure.
    static bool FilterWithoutEnglishLetters(const char *aBuf, unsigned int aLen, char **newBuf, unsigned int &newLen);
    static bool FilterWithEnglishLetters(const char *aBuf, unsigned int aLen, char **newBuf, unsigned int &newLen);
};
}
#endif /* nsCharSetProber_h__ */
