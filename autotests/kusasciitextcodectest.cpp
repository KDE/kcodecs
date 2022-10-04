/*
    SPDX-FileCopyrightText: 2021 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <kusasciitextcodec.h>
// Qt
#include <QTest>

class KUsAsciiTextCodecTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();

    void testBrokenBuiltinEncoding();
    void testEncoding();
    void testDecoding();

private:
    QTextCodec *qtCodec = nullptr;
    QTextCodec *ourCodec = nullptr;
};

void KUsAsciiTextCodecTest::initTestCase()
{
    // default coming from Qt, if built with ICU
    qtCodec = QTextCodec::codecForName("US-ASCII");

    // QTextCodec constructor of the baseclass registers the instance, also for ownership.
    // By the previous query for a US-ASCII codec the default one though
    // is cached for further look-ups, so we have to remember our own explicitely.
    ourCodec = new KUsAsciiTextCodec;
}

void KUsAsciiTextCodecTest::testBrokenBuiltinEncoding()
{
    if (!qtCodec) {
        QSKIP("Qt without built-in US-ASCII codec");
    }

    const QString failUnicodeText = QStringLiteral("Testnäme");

    QTextCodec::ConverterState failConverterState;

    // Only interested in what failConverterState will be changed to
    qtCodec->fromUnicode(failUnicodeText.constData(), failUnicodeText.length(), &failConverterState);

    // Bug seems fixed in Qt6
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 7)
    // confirm the broken result 0, which rather should be 1
    QCOMPARE(failConverterState.invalidChars, 1);
#else
    QCOMPARE(failConverterState.invalidChars, 0);
#endif
}

void KUsAsciiTextCodecTest::testEncoding()
{
    // compatible text
    const QString successUnicodeText = QStringLiteral("Testname");

    QTextCodec::ConverterState successConverterState;

    const QByteArray successEncoded8Bit = ourCodec->fromUnicode(successUnicodeText.constData(), successUnicodeText.length(), &successConverterState);

    const QByteArray successExpected8Bit = QByteArrayLiteral("Testname");
    QCOMPARE(successConverterState.invalidChars, 0);
    QCOMPARE(successEncoded8Bit, successExpected8Bit);

    // incompatible text
    const QString failUnicodeText = QStringLiteral("Testnäme");

    QTextCodec::ConverterState failConverterState;

    const QByteArray failEncoded8Bit = ourCodec->fromUnicode(failUnicodeText.constData(), failUnicodeText.length(), &failConverterState);

    const QByteArray failExpected8Bit = QByteArrayLiteral("Testn?me");
    QCOMPARE(failConverterState.invalidChars, 1);
    QCOMPARE(failEncoded8Bit, failExpected8Bit);
}

void KUsAsciiTextCodecTest::testDecoding()
{
    // compatible text
    const QByteArray success8BitString = QByteArrayLiteral("Testname");

    QTextCodec::ConverterState successConverterState;

    const QString successUnicodeString = ourCodec->toUnicode(success8BitString.constData(), success8BitString.length(), &successConverterState);

    const QString successExpectedString = QStringLiteral("Testname");
    QCOMPARE(successConverterState.invalidChars, 0);
    QCOMPARE(successUnicodeString, successExpectedString);

    // incompatible text, with "ä" in latin1
    /* clang-format off */
    const QByteArray fail8BitString = QByteArrayLiteral("Testn""\xE4""me");
    /* clang-format on */

    QTextCodec::ConverterState failConverterState;

    const QString failUnicodeString = ourCodec->toUnicode(fail8BitString.constData(), fail8BitString.length(), &failConverterState);

    const QString failExpectedString = QStringLiteral("Testn?me");
    QCOMPARE(failConverterState.invalidChars, 1);
    QCOMPARE(failUnicodeString, failExpectedString);
}

QTEST_GUILESS_MAIN(KUsAsciiTextCodecTest)

#include "kusasciitextcodectest.moc"
