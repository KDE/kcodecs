/*
    SPDX-FileCopyrightText: 2006 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#include <QTest>

#include "kcodecstest.h"

#include "../src/kcodecs.h"

QTEST_MAIN(KCodecsTest)

void KCodecsTest::testRFC2047decode()
{
    // empty
    QCOMPARE(KCodecs::decodeRFC2047String(QString()), QString());
    // identity
    QCOMPARE(KCodecs::decodeRFC2047String("bla"), QLatin1String("bla"));
    // utf-8
    QCOMPARE(KCodecs::decodeRFC2047String("=?utf-8?q?Ingo=20Kl=C3=B6cker?= <kloecker@kde.org>", "utf-8"), QString::fromUtf8("Ingo Klöcker <kloecker@kde.org>"));
    QCOMPARE(KCodecs::decodeRFC2047String("=?utf-8?q?Ingo=20Kl=C3=B6cker?="), QString::fromUtf8("Ingo Klöcker"));

    // whitespaces between two encoded words
    /*
    QCOMPARE(KCodecs::decodeRFC2047String("=?utf-8?q?Ingo=20Kl=C3=B6cker?=       =?utf-8?q?Ingo=20Kl=C3=B6cker?="),
             QString::fromUtf8("Ingo KlöckerIngo Klöcker"));
    */
    /*
    QCOMPARE(KCodecs::decodeRFC2047String("=?utf-8?q?Ingo=20Kl=C3=B6cker?=  foo  =?utf-8?q?Ingo=20Kl=C3=B6cker?="),
             QString::fromUtf8("Ingo Klöcker  foo  Ingo Klöcker"));
    */

    // iso-8859-x
    QCOMPARE(KCodecs::decodeRFC2047String("=?ISO-8859-1?Q?Andr=E9s_Ot=F3n?=", "utf-8"), QString::fromUtf8("Andrés Otón"));
    QCOMPARE(KCodecs::decodeRFC2047String("=?iso-8859-2?q?Rafa=B3_Rzepecki?=", "utf-8"), QString::fromUtf8("Rafał Rzepecki"));
    QCOMPARE(KCodecs::decodeRFC2047String("=?iso-8859-9?Q?S=2E=C7a=F0lar?= Onur", "utf-8"), QString::fromUtf8("S.Çağlar Onur"));
    QCOMPARE(KCodecs::decodeRFC2047String("Rafael =?iso-8859-15?q?Rodr=EDguez?=", "utf-8"), QString::fromUtf8("Rafael Rodríguez"));

    // language parameter according to RFC 2231, section 5
    QCOMPARE(KCodecs::decodeRFC2047String("From: =?US-ASCII*EN?Q?Keith_Moore?= <moore@cs.utk.edu>"), QString::fromUtf8("From: Keith Moore <moore@cs.utk.edu>"));

    // broken qp endoding (using lowercase)
    QCOMPARE(KCodecs::decodeRFC2047String("Subject: =?iso-8859-1?Q?Belangrijk=3a=20Verhuizing=20FTP=20server?="),
             QString::fromUtf8("Subject: Belangrijk: Verhuizing FTP server"));

    // mixed charsets, based on bug 125542 but pasted from above instead since I'm unable to enter those asian symbols
    QCOMPARE(KCodecs::decodeRFC2047String("Subject: =?utf-8?q?Ingo=20Kl=C3=B6cker?= unencoded words =?iso-8859-9?Q?S=2E=C7a=F0lar?="),
             QString::fromUtf8("Subject: Ingo KlÃ¶cker unencoded words S.ÃaÄlar"));

    // illegal characters which are already encoded in the given encoding but are not ASCII (bug 206417)
    QCOMPARE(KCodecs::decodeRFC2047String("Subject: =?utf-8?Q?Ð¿ÐžÑ¿ÐžÐ»Ð»,=20=D0=B4=D0=BE=D0=B1=D1=80=D1=8B=D0=B9=20=D0=B4=D0=B5=D0=BD=D1=8C?="),
             QString::fromUtf8("Subject: Ð¿ÐžÑ¿ÐžÐ»Ð», ÐŽÐŸÐ±ÑÑÐ¹ ÐŽÐµÐœÑ"));
    QCOMPARE(KCodecs::decodeRFC2047String("Subject: =?iso-8859-1?Q?ÖÄÜöäü?="), QString::fromLatin1("Subject: ÖÄÜöäü"));

    // Small data
    QCOMPARE(KCodecs::decodeRFC2047String("=?iso-8859-1?Q?c?="), QString::fromUtf8("c"));
}

void KCodecsTest::testInvalidDecode()
{
    QByteArray encCharset;

    // invalid / incomplete encoded data
    QCOMPARE(KCodecs::decodeRFC2047String("="), QString::fromUtf8("="));
    QCOMPARE(KCodecs::decodeRFC2047String("=?"), QString::fromUtf8("=?"));
    QCOMPARE(KCodecs::decodeRFC2047String("=?a?b?="), QString::fromUtf8("=?a?b?="));
    QCOMPARE(KCodecs::decodeRFC2047String("=?a?b?c?"), QString::fromUtf8("=?a?b?c?"));
    QCOMPARE(KCodecs::decodeRFC2047String("=?a??c?="), QString::fromUtf8("=?a??c?="));
}

void KCodecsTest::testRFC2047encode()
{
    // empty
    QCOMPARE(KCodecs::encodeRFC2047String(QString(), "utf-8"), QByteArray());
    // identity
    QCOMPARE(KCodecs::encodeRFC2047String(QLatin1String("bla"), "utf-8"), QByteArray("bla"));
    // utf-8
    // expected value is probably wrong, libkmime will chose 'B' instead of 'Q' encoding
    QEXPECT_FAIL("", "libkmime will chose 'B' instead of 'Q' encoding", Continue);
    QCOMPARE(KCodecs::encodeRFC2047String(QString::fromUtf8("Ingo KlÃ¶cker <kloecker@kde.org>"), "utf-8").constData(),
             "=?utf-8?q?Ingo=20Kl=C3=B6cker?= <kloecker@kde.org>");

    // Fallback to UTF-8 for encoding since the given charset can't encode the string
    const QString input = QString::fromUtf8("ÃŠÅ¿Ã°ÄÅÄ§Å");
    const QByteArray result = KCodecs::encodeRFC2047String(input, "latin1");
    QCOMPARE(KCodecs::decodeRFC2047String(result), input);
    QVERIFY(result.contains("utf-8"));
}
