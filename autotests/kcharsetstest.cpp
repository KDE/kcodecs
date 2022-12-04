/*
    SPDX-FileCopyrightText: 2011 Romain Perier <bambi@kubuntu.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "kcharsetstest.h"

#include "kcharsets_p.h"
#include <QDebug>
#include <QString>
#include <QTest>
#include <QTextCodec>
#include <kcharsets.h>

static bool encodingNameHasADescription(const QString &encodingName, const QStringList &descriptions)
{
    return std::any_of(descriptions.cbegin(), descriptions.cend(), [&encodingName](const QString &description) {
        return description.contains(encodingName);
    });
}

void KCharsetsTest::testSingleton()
{
    QVERIFY(KCharsets::charsets() != nullptr);
    QCOMPARE(KCharsets::charsets(), KCharsets::charsets());
}

void KCharsetsTest::testCodecForName_data()
{
    QTest::addColumn<QString>("codec");
    QTest::addColumn<QString>("expectedCodecFromKDE");
    QTest::addColumn<QString>("expectedCodecFromQt");

    QTest::newRow("utf8") << "utf8"
                          << "UTF-8"
                          << "UTF-8";
    QTest::newRow("utf-8") << "utf-8"
                           << "UTF-8"
                           << "UTF-8";
    QTest::newRow("UTF8") << "UTF8"
                          << "UTF-8"
                          << "UTF-8";
    QTest::newRow("UTF-8") << "UTF-8"
                           << "UTF-8"
                           << "UTF-8";

    QTest::newRow("Big5") << "Big5"
                          << "Big5"
                          << "Big5";
    QTest::newRow("Big5-HKSCS") << "Big5-HKSCS"
                                << "Big5-HKSCS"
                                << "Big5-HKSCS";
    QTest::newRow("EUC-JP") << "EUC-JP"
                            << "EUC-JP"
                            << "EUC-JP";
    QTest::newRow("EUC-KR") << "EUC-KR"
                            << "EUC-KR"
                            << "EUC-KR";
    QTest::newRow("CP 949") << "CP 949"
                            << "CP 949"
                            << "CP 949";
    QTest::newRow("GB18030") << "GB18030"
                             << "GB18030"
                             << "GB18030";
    QTest::newRow("GB2312") << "GB2312"
                            << "GB2312"
                            << "GB2312";
    QTest::newRow("GBK") << "GBK"
                         << "GBK"
                         << "GBK";
    QTest::newRow("IBM850") << "IBM850"
                            << "IBM850"
                            << "IBM850";
    QTest::newRow("IBM866") << "IBM866"
                            << "IBM866"
                            << "IBM866";
    QTest::newRow("IBM874") << "IBM874"
                            << "IBM874"
                            << "IBM874";
    QTest::newRow("ISO 10646-UCS-2") << "ISO 10646-UCS-2"
                                     << "ISO 10646-UCS-2"
                                     << "ISO 10646-UCS-2";
    QTest::newRow("ISO 8859-1") << "ISO 8859-1"
                                << "ISO 8859-1"
                                << "ISO 8859-1";
    QTest::newRow("ISO 8859-11") << "ISO 8859-11"
                                 << "ISO 8859-11"
                                 << "ISO 8859-11";
    QTest::newRow("ISO 8859-13") << "ISO 8859-13"
                                 << "ISO 8859-13"
                                 << "ISO 8859-13";
    QTest::newRow("ISO 8859-14") << "ISO 8859-14"
                                 << "ISO 8859-14"
                                 << "ISO 8859-14";
    QTest::newRow("ISO 8859-15") << "ISO 8859-15"
                                 << "ISO 8859-15"
                                 << "ISO 8859-15";
    QTest::newRow("ISO 8859-16") << "ISO 8859-16"
                                 << "ISO 8859-16"
                                 << "ISO 8859-16";
    QTest::newRow("ISO 8859-2") << "ISO 8859-2"
                                << "ISO 8859-2"
                                << "ISO 8859-2";
    QTest::newRow("ISO 8859-3") << "ISO 8859-3"
                                << "ISO 8859-3"
                                << "ISO 8859-3";
    QTest::newRow("ISO 8859-4") << "ISO 8859-4"
                                << "ISO 8859-4"
                                << "ISO 8859-4";
    QTest::newRow("ISO 8859-5") << "ISO 8859-5"
                                << "ISO 8859-5"
                                << "ISO 8859-5";
    QTest::newRow("ISO 8859-6") << "ISO 8859-6"
                                << "ISO 8859-6"
                                << "ISO 8859-6";
    QTest::newRow("ISO 8859-7") << "ISO 8859-7"
                                << "ISO 8859-7"
                                << "ISO 8859-7";
    QTest::newRow("ISO 8859-8") << "ISO 8859-8"
                                << "ISO 8859-8"
                                << "ISO 8859-8";
    QTest::newRow("ISO 8859-8-I") << "ISO 8859-8-I"
                                  << "ISO 8859-8-I"
                                  << "ISO 8859-8-I";
    QTest::newRow("ISO 8859-9") << "ISO 8859-9"
                                << "ISO 8859-9"
                                << "ISO 8859-9";
    QTest::newRow("KOI8-R") << "KOI8-R"
                            << "KOI8-R"
                            << "KOI8-R";
    QTest::newRow("KOI8-U") << "KOI8-U"
                            << "KOI8-U"
                            << "KOI8-U";
    QTest::newRow("TIS620") << "TIS620"
                            << "TIS620"
                            << "TIS620";
    QTest::newRow("TSCII") << "TSCII"
                           << "TSCII"
                           << "TSCII";
    QTest::newRow("UTF-16") << "UTF-16"
                            << "UTF-16"
                            << "UTF-16";
    QTest::newRow("UTF-8") << "UTF-8"
                           << "UTF-8"
                           << "UTF-8";
    QTest::newRow("cp 1250") << "cp 1250"
                             << "cp 1250"
                             << "cp 1250";
    QTest::newRow("cp 1251") << "cp 1251"
                             << "cp 1251"
                             << "cp 1251";
    QTest::newRow("cp 1252") << "cp 1252"
                             << "cp 1252"
                             << "cp 1252";
    QTest::newRow("cp 1253") << "cp 1253"
                             << "cp 1253"
                             << "cp 1253";
    QTest::newRow("cp 1254") << "cp 1254"
                             << "cp 1254"
                             << "cp 1254";
    QTest::newRow("cp 1255") << "cp 1255"
                             << "cp 1255"
                             << "cp 1255";
    QTest::newRow("cp 1256") << "cp 1256"
                             << "cp 1256"
                             << "cp 1256";
    QTest::newRow("cp 1257") << "cp 1257"
                             << "cp 1257"
                             << "cp 1257";
    QTest::newRow("jis7") << "jis7"
                          << "jis7"
                          << "jis7";
    QTest::newRow("sjis") << "sjis"
                          << "sjis"
                          << "sjis";
    QTest::newRow("ucs2") << "ucs2"
                          << "ucs2"
                          << "ucs2";
    QTest::newRow("utf7") << "utf7"
                          << "utf7"
                          << "utf7";
    QTest::newRow("windows-1258") << "windows-1258"
                                  << "windows-1258"
                                  << "windows-1258";
    QTest::newRow("winsami2") << "winsami2"
                              << "winsami2"
                              << "winsami2";
    QTest::newRow("US-ASCII") << "US-ASCII"
                              << "US-ASCII"
                              << "US-ASCII";
}

void KCharsetsTest::testCodecForName()
{
    KCharsets *singleton = KCharsets::charsets();

    QFETCH(QString, codec);
    QFETCH(QString, expectedCodecFromKDE);
    QFETCH(QString, expectedCodecFromQt);

    if (QTextCodec::codecForName(codec.toLocal8Bit()) == nullptr) {
        qWarning() << "codec " << codec << "is not supported by QTextCodec !";
        return;
    }

    QVERIFY(QTextCodec::codecForName(expectedCodecFromKDE.toLocal8Bit()) != nullptr);
    QCOMPARE(singleton->d->codecForName(codec)->name(), QTextCodec::codecForName(expectedCodecFromKDE.toLocal8Bit())->name());

    QVERIFY(QTextCodec::codecForName(expectedCodecFromQt.toLocal8Bit()) != nullptr);
    QCOMPARE(QTextCodec::codecForName(codec.toLocal8Bit())->name(), QTextCodec::codecForName(expectedCodecFromQt.toLocal8Bit())->name());
}

void KCharsetsTest::testFromEntity()
{
    KCharsets *singleton = KCharsets::charsets();

    QCOMPARE(singleton->fromEntity(QString::fromLatin1("&#1234")), QChar(1234));
    QCOMPARE(singleton->fromEntity(QString::fromLatin1("&#x1234")), QChar(0x1234));
    QCOMPARE(singleton->fromEntity(QString::fromLatin1("lt")), QChar::fromLatin1('<'));
    QCOMPARE(singleton->fromEntity(QString::fromLatin1("gt")), QChar::fromLatin1('>'));
    QCOMPARE(singleton->fromEntity(QString::fromLatin1("quot")), QChar::fromLatin1('"'));
    QCOMPARE(singleton->fromEntity(QString::fromLatin1("amp")), QChar::fromLatin1('&'));
    QCOMPARE(singleton->fromEntity(QString::fromLatin1("apos")), QChar::fromLatin1('\''));
}

void KCharsetsTest::testToEntity()
{
    QSKIP("KCharsets::toEntity test not implemented.");
}

void KCharsetsTest::testResolveEntities()
{
    KCharsets *singleton = KCharsets::charsets();

    QCOMPARE(singleton->resolveEntities(QString::fromLatin1("&quot;&apos;&lt;Hello &amp;World&gt;&apos;&quot;")),
             QString::fromLatin1("\"\'<Hello &World>\'\""));
}

void KCharsetsTest::testEncodingNames()
{
    KCharsets *singleton = KCharsets::charsets();

    QCOMPARE(singleton->availableEncodingNames().count(), singleton->descriptiveEncodingNames().count());

    for (const QString &encodingName : singleton->availableEncodingNames()) {
        bool ok = false;

        if (encodingName == QLatin1String("ucs2") || encodingName == QLatin1String("ISO 10646-UCS-2")) {
            singleton->d->codecForName(QStringLiteral("UTF-16"), ok);
        } else if (encodingName == QLatin1String("utf7")) {
            continue;
        } else {
            singleton->d->codecForName(encodingName, ok);
        }
        // The availability of some of the charsets below depends on whether Qt was built with ICU...
        if (!ok) {
            if (encodingName == QLatin1String("jis7")) {
                QEXPECT_FAIL("", "jis7 is missing in Qt", Continue);
            }
            if (encodingName == QLatin1String("winsami2")) {
                QEXPECT_FAIL("", "winsami2 is missing in Qt", Continue);
            }
            if (encodingName == QLatin1String("ISO 8859-16")) { // ICU bug?
                QEXPECT_FAIL("", "ISO 8859-16 is missing in Qt", Continue);
            }
        }

        if (!ok) {
            qDebug() << "Error:" << encodingName << "not found";
            QVERIFY(false);
        }
        QVERIFY(encodingNameHasADescription(encodingName, singleton->descriptiveEncodingNames()));
        QVERIFY(!singleton->descriptionForEncoding(encodingName).isEmpty());
        QCOMPARE(singleton->encodingForName(singleton->descriptionForEncoding(encodingName)), encodingName);
    }
}

void KCharsetsTest::testUsAsciiEncoding_data()
{
    QTest::addColumn<QString>("codecName");

    QTest::newRow("normal-name") << QStringLiteral("US-ASCII");
    QTest::newRow("alias-name") << QStringLiteral("IBM367");
}

void KCharsetsTest::testUsAsciiEncoding()
{
    QFETCH(QString, codecName);

    KCharsets *singleton = KCharsets::charsets();

    bool ok = false;
    QTextCodec *codec = singleton->d->codecForName(codecName, ok);
    QVERIFY(ok);

    // compatible text
    const QString successUnicodeText = QStringLiteral("Testname");

    QTextCodec::ConverterState successConverterState;

    const QByteArray successEncoded8Bit = codec->fromUnicode(successUnicodeText.constData(), successUnicodeText.length(), &successConverterState);

    const QByteArray successExpected8Bit = QByteArrayLiteral("Testname");
    QCOMPARE(successConverterState.invalidChars, 0);
    QCOMPARE(successEncoded8Bit, successExpected8Bit);

    // incompatible text
    const QString failUnicodeText = QStringLiteral("Testnäme");

    QTextCodec::ConverterState failConverterState;

    const QByteArray failEncoded8Bit = codec->fromUnicode(failUnicodeText.constData(), failUnicodeText.length(), &failConverterState);

    const QByteArray failExpected8Bit = QByteArrayLiteral("Testn?me");
    QCOMPARE(failConverterState.invalidChars, 1);
    QCOMPARE(failEncoded8Bit, failExpected8Bit);
}

void KCharsetsTest::testUsAsciiDecoding_data()
{
    QTest::addColumn<QString>("codecName");

    QTest::newRow("normal-name") << QStringLiteral("US-ASCII");
    QTest::newRow("alias-name") << QStringLiteral("IBM367");
}

void KCharsetsTest::testUsAsciiDecoding()
{
    QFETCH(QString, codecName);

    KCharsets *singleton = KCharsets::charsets();

    bool ok = false;
    QTextCodec *codec = singleton->d->codecForName(codecName, ok);
    QVERIFY(ok);

    // compatible text
    const QByteArray success8BitString = QByteArrayLiteral("Testname");

    QTextCodec::ConverterState successConverterState;

    const QString successUnicodeString = codec->toUnicode(success8BitString.constData(), success8BitString.length(), &successConverterState);

    const QString successExpectedString = QStringLiteral("Testname");
    QCOMPARE(successConverterState.invalidChars, 0);
    QCOMPARE(successUnicodeString, successExpectedString);

    // incompatible text, with "ä" in latin1
    /* clang-format off */
    const QByteArray fail8BitString = QByteArrayLiteral("Testn""\xE4""me");
    /* clang-format on */

    QTextCodec::ConverterState failConverterState;

    const QString failUnicodeString = codec->toUnicode(fail8BitString.constData(), fail8BitString.length(), &failConverterState);

    const QString failExpectedString = QStringLiteral("Testn?me");
    QCOMPARE(failConverterState.invalidChars, 1);
    QCOMPARE(failUnicodeString, failExpectedString);
}

QTEST_MAIN(KCharsetsTest)
