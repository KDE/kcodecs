/*
    SPDX-FileCopyrightText: 2021 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "kusasciitextcodec.h"

QByteArray KUsAsciiTextCodec::name() const
{
    return QByteArrayLiteral("US-ASCII");
}

QList<QByteArray> KUsAsciiTextCodec::aliases() const
{
    return {
        // aliases by https://www.iana.org/assignments/character-sets/character-sets.xml, update 2021-01-04
        QByteArrayLiteral("iso-ir-6"),
        QByteArrayLiteral("ANSI_X3.4-1968"),
        QByteArrayLiteral("ANSI_X3.4-1986"),
        QByteArrayLiteral("ISO_646.irv:1991"),
        QByteArrayLiteral("ISO646-US"),
        QByteArrayLiteral("us"),
        QByteArrayLiteral("IBM367"),
        QByteArrayLiteral("cp367"),
        QByteArrayLiteral("csASCII"),
        // further aliases as used by ICU 69.1
        QByteArrayLiteral("ibm-367"),
        QByteArrayLiteral("ASCII"),
        QByteArrayLiteral("ISO_646.irv:1991"),
        QByteArrayLiteral("iso_646.irv:1983"),
        QByteArrayLiteral("ascii7"),
        QByteArrayLiteral("646"),
        QByteArrayLiteral("windows-20127"),
    };
}
int KUsAsciiTextCodec::mibEnum() const
{
    return 3;
}

QByteArray KUsAsciiTextCodec::convertFromUnicode(const QChar *input, int number, QTextCodec::ConverterState *state) const
{
    const char invalidSubstituteChar = (state && state->flags & ConvertInvalidToNull) ? 0 : '?';

    QByteArray encoded(number, Qt::Uninitialized);

    char *d = encoded.data();
    int invalidChars = 0;

    for (int i = 0; i < number; ++i) {
        const ushort unicodeCodePoint = input[i].unicode();
        if (unicodeCodePoint > 0x7f) {
            d[i] = invalidSubstituteChar;
            ++invalidChars;
        } else {
            d[i] = static_cast<char>(unicodeCodePoint & 0xff);
        }
    }

    if (state) {
        state->invalidChars += invalidChars;
    }

    return encoded;
}

QString KUsAsciiTextCodec::convertToUnicode(const char *chars, int len, QTextCodec::ConverterState *state) const
{
    const QChar invalidSubstituteChar = (state && state->flags & ConvertInvalidToNull) ? QChar(0) : QLatin1Char('?');

    QString decoded(len, invalidSubstituteChar);

    QChar *d = decoded.data();
    int invalidChars = 0;

    for (int i = 0; i < len; ++i) {
        const char c = chars[i];
        // signedness of char depends on the compiler and the target platform,
        // so for comparison use explicit variant
        if (static_cast<unsigned char>(c) > 0x7f) {
            ++invalidChars;
        } else {
            d[i] = QLatin1Char(c);
        }
    }

    if (state) {
        state->invalidChars += invalidChars;
    }

    return decoded;
}
