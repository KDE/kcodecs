/*
   Copyright (C) 2000-2001 Dawit Alemayehu <adawit@kde.org>
   Copyright (C) 2001 Rik Hemsley (rikkus) <rik@kde.org>
   Copyright (c) 2001-2002 Marc Mutz <mutz@kde.org>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License (LGPL)
   version 2 as published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

   RFC 1321 "MD5 Message-Digest Algorithm" Copyright (C) 1991-1992.             // krazy:exclude=copyright
   RSA Data Security, Inc. Created 1991. All rights reserved.

   The KMD5 class is based on a C++ implementation of
   "RSA Data Security, Inc. MD5 Message-Digest Algorithm" by
   Mordechai T. Abzug,  Copyright (c) 1995.  This implementation                // krazy:exclude=copyright
   passes the test-suite as defined in RFC 1321.

   The encoding and decoding utilities in KCodecs with the exception of
   quoted-printable are based on the java implementation in HTTPClient
   package by Ronald Tschalär Copyright (C) 1996-1999.                          // krazy:exclude=copyright

   The quoted-printable codec as described in RFC 2045, section 6.7. is by
   Rik Hemsley (C) 2001.
*/

#include "kcodecs.h"
#include "kcodecs_p.h"
#include "kcodecsbase64.h"
#include "kcodecsidentity.h"
#include "kcodecsqp.h"
#include "kcodecsuuencode.h"
#include "kcharsets.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QMutex>

#include <cassert>
#include <cstring>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <QtCore/QDebug>
#include <QtCore/QIODevice>
#include <QtCore/QTextCodec>
#include <QtCore/QHash>

#if defined(Q_OS_WIN)
#define strncasecmp _strnicmp
#endif

namespace KCodecs
{

static const char hexChars[16] = {
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

static QList<QByteArray> charsetCache;


QByteArray cachedCharset(const QByteArray &name)
{
    foreach (const QByteArray &charset, charsetCache) {
        if (qstricmp(name.data(), charset.data()) == 0) {
            return charset;
        }
    }

    charsetCache.append(name.toUpper());
    return charsetCache.last();
}


} // namespace KCodecs

/******************************** KCodecs ********************************/

QByteArray KCodecs::quotedPrintableEncode(const QByteArray &in, bool useCRLF)
{
    Codec *codec = Codec::codecForName("quoted-printable");
    return codec->encode(in, useCRLF ? Codec::NewlineCRLF : Codec::NewlineLF);
}

void KCodecs::quotedPrintableEncode(const QByteArray &in, QByteArray &out, bool useCRLF)
{
    out = quotedPrintableEncode(in, useCRLF ? Codec::NewlineCRLF : Codec::NewlineLF);
}

QByteArray KCodecs::quotedPrintableDecode(const QByteArray &in)
{
    Codec *codec = Codec::codecForName("quoted-printable");
    return codec->decode(in);
}

void KCodecs::quotedPrintableDecode(const QByteArray &in, QByteArray &out)
{
    out = quotedPrintableDecode(in);
}

QByteArray KCodecs::base64Encode(const QByteArray &in)
{
    Codec *codec = Codec::codecForName("base64");
    return codec->encode(in);
}

QByteArray KCodecs::base64Encode(const QByteArray &in, bool insertLFs)
{
    Q_UNUSED(insertLFs);
    return base64Encode(in);
}

void KCodecs::base64Encode(const QByteArray &in, QByteArray &out,
                           bool insertLFs)
{
    Q_UNUSED(insertLFs);
    out = base64Encode(in);
}

QByteArray KCodecs::base64Decode(const QByteArray &in)
{
    Codec *codec = Codec::codecForName("base64");
    return codec->decode(in);
}

void KCodecs::base64Decode(const QByteArray &in, QByteArray &out)
{
    out = base64Decode(in);
}

QByteArray KCodecs::uuencode(const QByteArray &in)
{
    Codec *codec = Codec::codecForName("x-uuencode");
    return codec->encode(in);
}

void KCodecs::uuencode(const QByteArray &in, QByteArray &out)
{
    out = uuencode(in);
}

QByteArray KCodecs::uudecode(const QByteArray &in)
{
    Codec *codec = Codec::codecForName("x-uuencode");
    return codec->decode(in);
}

void KCodecs::uudecode(const QByteArray &in, QByteArray &out)
{
    out = uudecode(in);
}

//@cond PRIVATE

namespace KCodecs
{
// parse the encoded-word (scursor points to after the initial '=')
bool parseEncodedWord(const char *&scursor, const char *const send,
                      QString *result, QByteArray *language,
                      QByteArray *usedCS, const QByteArray &defaultCS,
                      CharsetOption charsetOption)
{
    assert(result);
    assert(language);

    // make sure the caller already did a bit of the work.
    assert(*(scursor - 1) == '=');

    //
    // STEP 1:
    // scan for the charset/language portion of the encoded-word
    //

    char ch = *scursor++;

    if (ch != '?') {
        // qDebug() << "first";
        // qDebug() << "Premature end of encoded word";
        return false;
    }

    // remember start of charset (ie. just after the initial "=?") and
    // language (just after the first '*') fields:
    const char *charsetStart = scursor;
    const char *languageStart = nullptr;

    // find delimiting '?' (and the '*' separating charset and language
    // tags, if any):
    for (; scursor != send ; scursor++) {
        if (*scursor == '?') {
            break;
        } else if (*scursor == '*' && languageStart == nullptr) {
            languageStart = scursor + 1;
        }
    }

    // not found? can't be an encoded-word!
    if (scursor == send || *scursor != '?') {
        // qDebug() << "second";
        // qDebug() << "Premature end of encoded word";
        return false;
    }

    // extract the language information, if any (if languageStart is 0,
    // language will be null, too):
    QByteArray maybeLanguage(languageStart, scursor - languageStart);
    // extract charset information (keep in mind: the size given to the
    // ctor is one off due to the \0 terminator):
    QByteArray maybeCharset(charsetStart,
                            (languageStart ? languageStart - 1 : scursor) - charsetStart);

    //
    // STEP 2:
    // scan for the encoding portion of the encoded-word
    //

    // remember start of encoding (just _after_ the second '?'):
    scursor++;
    const char *encodingStart = scursor;

    // find next '?' (ending the encoding tag):
    for (; scursor != send ; scursor++) {
        if (*scursor == '?') {
            break;
        }
    }

    // not found? Can't be an encoded-word!
    if (scursor == send || *scursor != '?') {
        // qDebug() << "third";
        // qDebug() << "Premature end of encoded word";
        return false;
    }

    // extract the encoding information:
    QByteArray maybeEncoding(encodingStart, scursor - encodingStart);

    // qDebug() << "parseEncodedWord: found charset == \"" << maybeCharset
    //         << "\"; language == \"" << maybeLanguage
    //         << "\"; encoding == \"" << maybeEncoding << "\"";

    //
    // STEP 3:
    // scan for encoded-text portion of encoded-word
    //

    // remember start of encoded-text (just after the third '?'):
    scursor++;
    const char *encodedTextStart = scursor;

    // find the '?=' sequence (ending the encoded-text):
    for (; scursor != send ; scursor++) {
        if (*scursor == '?') {
            if (scursor + 1 != send) {
                if (*(scursor + 1) != '=') {     // We expect a '=' after the '?', but we got something else; ignore
                    // qDebug() << "Stray '?' in q-encoded word, ignoring this.";
                    continue;
                } else { // yep, found a '?=' sequence
                    scursor += 2;
                    break;
                }
            } else { // The '?' is the last char, but we need a '=' after it!
                // qDebug() << "Premature end of encoded word";
                return false;
            }
        }
    }

    if (*(scursor - 2) != '?' || *(scursor - 1) != '=' || scursor < encodedTextStart + 2) {
        // qDebug() << "Premature end of encoded word";
        return false;
    }

    // set end sentinel for encoded-text:
    const char *const encodedTextEnd = scursor - 2;

    //
    // STEP 4:
    // setup decoders for the transfer encoding and the charset
    //

    // try if there's a codec for the encoding found:
    Codec *codec = Codec::codecForName(maybeEncoding);
    if (!codec) {
        // qDebug() << "Unknown encoding" << maybeEncoding;
        return false;
    }

    // get an instance of a corresponding decoder:
    Decoder *dec = codec->makeDecoder();
    assert(dec);

    // try if there's a (text)codec for the charset found:
    bool matchOK = false;
    QByteArray cs;
    QTextCodec *textCodec = nullptr;
    if (charsetOption == KCodecs::ForceDefaultCharset || maybeCharset.isEmpty()) {
        textCodec = KCharsets::charsets()->codecForName(QLatin1String(defaultCS), matchOK);
        cs = cachedCharset(defaultCS);
    } else {
        textCodec = KCharsets::charsets()->codecForName(QLatin1String(maybeCharset), matchOK);
        if (!matchOK) {    //no suitable codec found => use default charset
            textCodec = KCharsets::charsets()->codecForName(QLatin1String(defaultCS), matchOK);
            cs = cachedCharset(defaultCS);
        } else {
            cs = cachedCharset(maybeCharset);
        }
    }
    if (usedCS) {
        *usedCS = cs;
    }

    if (!matchOK || !textCodec) {
        // qDebug() << "Unknown charset" << maybeCharset;
        delete dec;
        return false;
    };

    // qDebug() << "mimeName(): \"" << textCodec->name() << "\"";

    // allocate a temporary buffer to store the 8bit text:
    int encodedTextLength = encodedTextEnd - encodedTextStart;
    QByteArray buffer;
    buffer.resize(codec->maxDecodedSizeFor(encodedTextLength));
    char *bbegin = buffer.data();
    char *bend = bbegin + buffer.length();

    //
    // STEP 5:
    // do the actual decoding
    //

    if (!dec->decode(encodedTextStart, encodedTextEnd, bbegin, bend)) {
        qWarning() << codec->name() << "codec lies about its maxDecodedSizeFor("
                   << encodedTextLength << ")\nresult may be truncated";
    }

    *result = textCodec->toUnicode(buffer.data(), bbegin - buffer.data());

    // qDebug() << "result now: \"" << result << "\"";
    // cleanup:
    delete dec;
    *language = maybeLanguage;

    return true;
}

} // namespace KCodecs

//@endcond

QString KCodecs::decodeRFC2047String(const QString &msg)
{
    QByteArray usedCS;
    return decodeRFC2047String(msg.toUtf8(), &usedCS, "utf-8", NoOption);
}

QString KCodecs::decodeRFC2047String(const QByteArray &src, QByteArray *usedCS,
                                     const QByteArray &defaultCS,
                                     CharsetOption charsetOption)
{
    QByteArray result;
    QByteArray spaceBuffer;
    const char *scursor = src.constData();
    const char *send = scursor + src.length();
    bool onlySpacesSinceLastWord = false;

    while (scursor != send) {
        // space
        if (isspace(*scursor) && onlySpacesSinceLastWord) {
            spaceBuffer += *scursor++;
            continue;
        }

        // possible start of an encoded word
        if (*scursor == '=') {
            QByteArray language;
            QString decoded;
            ++scursor;
            const char *start = scursor;
            if (parseEncodedWord(scursor, send, &decoded, &language, usedCS, defaultCS, charsetOption)) {
                result += decoded.toUtf8();
                onlySpacesSinceLastWord = true;
                spaceBuffer.clear();
            } else {
                if (onlySpacesSinceLastWord) {
                    result += spaceBuffer;
                    onlySpacesSinceLastWord = false;
                }
                result += '=';
                scursor = start; // reset cursor after parsing failure
            }
            continue;
        } else {
            // unencoded data
            if (onlySpacesSinceLastWord) {
                result += spaceBuffer;
                onlySpacesSinceLastWord = false;
            }
            result += *scursor;
            ++scursor;
        }
    }
    // If there are any chars that couldn't be decoded in UTF-8,
    // fallback to local codec
    const QString tryUtf8 = QString::fromUtf8(result);
    if (tryUtf8.contains(0xFFFD)) {
        QTextCodec *codec = QTextCodec::codecForLocale();
        return codec->toUnicode(result);
    } else {
        return tryUtf8;
    }
}


QByteArray KCodecs::encodeRFC2047String(const QString &src, const QByteArray &charset)
{
    QByteArray result;
    int start = 0, end = 0;
    bool nonAscii = false, ok = true, useQEncoding = false;

    // fromLatin1() is safe here, codecForName() uses toLatin1() internally
    const QTextCodec *codec = KCharsets::charsets()->codecForName(QString::fromLatin1(charset), ok);

    QByteArray usedCS;
    if (!ok) {
        //no codec available => try local8Bit and hope the best ;-)
        codec = QTextCodec::codecForLocale();
        usedCS = codec->name();
    } else {
        Q_ASSERT(codec);
        if (charset.isEmpty()) {
            usedCS = codec->name();
        } else {
            usedCS = charset;
        }
    }

    QTextCodec::ConverterState converterState(QTextCodec::IgnoreHeader);
    QByteArray encoded8Bit = codec->fromUnicode(src.constData(), src.length(), &converterState);
    if (converterState.invalidChars > 0) {
        usedCS = "utf-8";
        codec = QTextCodec::codecForName(usedCS);
        encoded8Bit = codec->fromUnicode(src);
    }

    if (usedCS.contains("8859-")) {     // use "B"-Encoding for non iso-8859-x charsets
        useQEncoding = true;
    }

    uint encoded8BitLength = encoded8Bit.length();
    for (unsigned int i = 0; i < encoded8BitLength; i++) {
        if (encoded8Bit[i] == ' ') {   // encoding starts at word boundaries
            start = i + 1;
        }

        // encode escape character, for japanese encodings...
        if (((signed char)encoded8Bit[i] < 0) || (encoded8Bit[i] == '\033')) {
            end = start;   // non us-ascii char found, now we determine where to stop encoding
            nonAscii = true;
            break;
        }
    }

    if (nonAscii) {
        while ((end < encoded8Bit.length()) && (encoded8Bit[end] != ' ')) {
            // we encode complete words
            end++;
        }

        for (int x = end; x < encoded8Bit.length(); x++) {
            if (((signed char)encoded8Bit[x] < 0) || (encoded8Bit[x] == '\033')) {
                end = x;     // we found another non-ascii word

                while ((end < encoded8Bit.length()) && (encoded8Bit[end] != ' ')) {
                    // we encode complete words
                    end++;
                }
            }
        }

        result = encoded8Bit.left(start) + "=?" + usedCS;

        if (useQEncoding) {
            result += "?Q?";

            char c, hexcode;// "Q"-encoding implementation described in RFC 2047
            for (int i = start; i < end; i++) {
                c = encoded8Bit[i];
                if (c == ' ') {   // make the result readable with not MIME-capable readers
                    result += '_';
                } else {
                    if (((c >= 'a') && (c <= 'z')) ||        // paranoid mode, encode *all* special chars to avoid problems
                            ((c >= 'A') && (c <= 'Z')) ||        // with "From" & "To" headers
                            ((c >= '0') && (c <= '9'))) {
                        result += c;
                    } else {
                        result += '=';                 // "stolen" from KMail ;-)
                        hexcode = ((c & 0xF0) >> 4) + 48;
                        if (hexcode >= 58) {
                            hexcode += 7;
                        }
                        result += hexcode;
                        hexcode = (c & 0x0F) + 48;
                        if (hexcode >= 58) {
                            hexcode += 7;
                        }
                        result += hexcode;
                    }
                }
            }
        } else {
            result += "?B?" + encoded8Bit.mid(start, end - start).toBase64();
        }

        result += "?=";
        result += encoded8Bit.right(encoded8Bit.length() - end);
    } else {
        result = encoded8Bit;
    }

    return result;
}

/******************************************************************************/
/*                           KCodecs::Codec                                   */

// global list of KCodecs::Codec's
//@cond PRIVATE
namespace
{
static QHash<QByteArray, KCodecs::Codec*> *allCodecs = nullptr;
Q_GLOBAL_STATIC(QMutex, dictLock)

static void createCodecs()
{
    //all->insert( "7bit", new KCodecs::SevenBitCodec() );
    //all->insert( "8bit", new KCodecs::EightBitCodec() );
    allCodecs->insert("base64", new KCodecs::Base64Codec());
    allCodecs->insert("quoted-printable", new KCodecs::QuotedPrintableCodec());
    allCodecs->insert("b", new KCodecs::Rfc2047BEncodingCodec());
    allCodecs->insert("q", new KCodecs::Rfc2047QEncodingCodec());
    allCodecs->insert("x-kmime-rfc2231", new KCodecs::Rfc2231EncodingCodec());
    allCodecs->insert("x-uuencode", new KCodecs::UUCodec());
    //all->insert( "binary", new KCodecs::BinaryCodec() );
}

static void cleanupCodecs()
{
    for (auto iter = allCodecs->begin(); iter != allCodecs->end(); ++iter) {
        delete (*iter);
    }
    delete allCodecs;
    allCodecs = nullptr;
}

}
//@endcond

KCodecs::Codec *KCodecs::Codec::codecForName(const char *name)
{
    const QByteArray ba(name);
    return codecForName(ba);
}

KCodecs::Codec *KCodecs::Codec::codecForName(const QByteArray &name)
{
    QMutexLocker locker(dictLock); // protect "allCodecs"
    if (!allCodecs) {
        allCodecs = new QHash<QByteArray, Codec*>();
        qAddPostRoutine(cleanupCodecs);
        createCodecs();
    }
    QByteArray lowerName = name.toLower();
    Codec *codec = (*allCodecs).value(lowerName);

    if (!codec) {
        qWarning() << "Unknown codec \"" << name << "\" requested!";
    }

    return codec;
}

bool KCodecs::Codec::encode(const char *&scursor, const char *const send,
                            char *&dcursor, const char *const dend,
                            NewlineType newline) const
{
    // get an encoder:
    QScopedPointer<Encoder> enc(makeEncoder(newline));
    assert(!enc.isNull());

    // encode and check for output buffer overflow:
    while (!enc->encode(scursor, send, dcursor, dend)) {
        if (dcursor == dend) {
            return false; // not enough space in output buffer
        }
    }

    // finish and check for output buffer overflow:
    while (!enc->finish(dcursor, dend)) {
        if (dcursor == dend) {
            return false; // not enough space in output buffer
        }
    }

    return true; // successfully encoded.
}

QByteArray KCodecs::Codec::encode(const QByteArray &src, NewlineType newline) const
{
    // allocate buffer for the worst case:
    QByteArray result;
    result.resize(maxEncodedSizeFor(src.size(), newline));

    // set up iterators:
    QByteArray::ConstIterator iit = src.begin();
    QByteArray::ConstIterator iend = src.end();
    QByteArray::Iterator oit = result.begin();
    QByteArray::ConstIterator oend = result.end();

    // encode
    if (!encode(iit, iend, oit, oend, newline)) {
        qCritical() << name() << "codec lies about it's mEncodedSizeFor()";
    }

    // shrink result to actual size:
    result.truncate(oit - result.begin());

    return result;
}

QByteArray KCodecs::Codec::decode(const QByteArray &src, NewlineType newline) const
{
    // allocate buffer for the worst case:
    QByteArray result;
    result.resize(maxDecodedSizeFor(src.size(), newline));

    // set up iterators:
    QByteArray::ConstIterator iit = src.begin();
    QByteArray::ConstIterator iend = src.end();
    QByteArray::Iterator oit = result.begin();
    QByteArray::ConstIterator oend = result.end();

    // decode
    if (!decode(iit, iend, oit, oend, newline)) {
        qCritical() << name() << "codec lies about it's maxDecodedSizeFor()";
    }

    // shrink result to actual size:
    result.truncate(oit - result.begin());

    return result;
}

bool KCodecs::Codec::decode(const char *&scursor, const char *const send,
                            char *&dcursor, const char *const dend,
                            NewlineType newline) const
{
    // get a decoder:
    QScopedPointer<Decoder> dec(makeDecoder(newline));
    assert(!dec.isNull());

    // decode and check for output buffer overflow:
    while (!dec->decode(scursor, send, dcursor, dend)) {
        if (dcursor == dend) {
            return false; // not enough space in output buffer
        }
    }

    // finish and check for output buffer overflow:
    while (!dec->finish(dcursor, dend)) {
        if (dcursor == dend) {
            return false; // not enough space in output buffer
        }
    }

    return true; // successfully encoded.
}


/******************************************************************************/
/*                          KCodecs::Encoder                                  */

KCodecs::EncoderPrivate::EncoderPrivate(Codec::NewlineType newline)
    : outputBufferCursor(0)
    , newline(newline)
{
}

KCodecs::Encoder::Encoder(Codec::NewlineType newline)
    : d(new KCodecs::EncoderPrivate(newline))
{
}

KCodecs::Encoder::~Encoder()
{
    delete d;
}

bool KCodecs::Encoder::write(char ch, char*& dcursor, const char*const dend)
{
    if (dcursor != dend) {
        // if there's space in the output stream, write there:
        *dcursor++ = ch;
        return true;
    } else {
        // else buffer the output:
        if (d->outputBufferCursor >= maxBufferedChars) {
            qCritical()
                    << "KCodecs::Encoder: internal buffer overflow!";
        } else {
            d->outputBuffer[ d->outputBufferCursor++ ] = ch;
        }
        return false;
    }
}

// write as much as possible off the output buffer. Return true if
// flushing was complete, false if some chars could not be flushed.
bool KCodecs::Encoder::flushOutputBuffer(char *&dcursor, const char *const dend)
{
    int i;
    // copy output buffer to output stream:
    for (i = 0 ; dcursor != dend && i < d->outputBufferCursor ; ++i) {
        *dcursor++ = d->outputBuffer[i];
    }

    // calculate the number of missing chars:
    int numCharsLeft = d->outputBufferCursor - i;
    // push the remaining chars to the begin of the buffer:
    if (numCharsLeft) {
        ::memmove(d->outputBuffer, d->outputBuffer + i, numCharsLeft);
    }
    // adjust cursor:
    d->outputBufferCursor = numCharsLeft;

    return !numCharsLeft;
}

bool KCodecs::Encoder::writeCRLF(char*& dcursor, const char*const dend)
{
    if (d->newline == Codec::NewlineCRLF) {
        write('\r', dcursor, dend);
    }
    return write('\n', dcursor, dend);
}



/******************************************************************************/
/*                           KCodecs::Decoder                                 */

KCodecs::DecoderPrivate::DecoderPrivate(Codec::NewlineType newline)
    : newline(newline)
{
}

KCodecs::Decoder::Decoder(Codec::NewlineType newline)
    : d(new KCodecs::DecoderPrivate(newline))
{
}

KCodecs::Decoder::~Decoder()
{
    delete d;
}

