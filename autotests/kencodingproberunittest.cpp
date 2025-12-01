/*
    SPDX-FileCopyrightText: 2025 Stefan Brüns <stefan.bruens@rwth-aachen.de>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <QTest>

#include "../src/probers/nsCodingStateMachine.h"

class KEncodingProberUnitTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testUtf8();
    void testUtf8_data();
    void testUtf16BE();
    void testUtf16BE_data();
    void testUtf16LE();
    void testUtf16LE_data();
    void testUtf16_common_data();
};

void KEncodingProberUnitTest::testUtf8()
{
    QFETCH(QByteArray, data);
    QFETCH(bool, utf8Valid);

    using namespace kencodingprober;

    nsCodingStateMachine stateMachine{&UTF8SMModel};
    nsSMState state = eStart;

    QEXPECT_FAIL("UTF-8 SMP Symbols", "valid codepoint rejected", Abort);
    QEXPECT_FAIL("UTF-8 len 4 invalid range", "invalid codepoint accepted", Abort);
    QEXPECT_FAIL("UTF-8 len 5 invalid", "len 5 sequence accepted", Abort);
    QEXPECT_FAIL("UTF-8 len 6 invalid", "len 6 sequence accepted", Abort);
    for (auto b : data) {
        state = stateMachine.NextState(b);
    }

    if (utf8Valid) {
        QVERIFY(state != eError);
    } else {
        QVERIFY(state == eError);
    }
}

void KEncodingProberUnitTest::testUtf8_data()
{
    using namespace Qt::StringLiterals;

    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<bool>("utf8Valid");

    QTest::addRow("UTF-8 Latin1") << "abcdxyzABCDXYZ 0129;,"_ba << true;
    QTest::addRow("BOM UTF-8") << "\xef\xbb\xbfZ"_ba << true; // "<UTF-8 BOM>Z"

    // multibyte sequences - length 2
    QTest::addRow("UTF-8 Latin1 Supplement") //
        << "Latin1 Text \xC3\xA4\xC3\xB6\xC3\xBC\xC3\x9F"_ba // "Latin1 Text äöüß"
        << true;
    QTest::addRow("UTF-8 len 2") << "Text \xC3\xA4  "_ba << true;
    QTest::addRow("UTF-8 len 2 short") << "Text \xC3  "_ba << false;
    QTest::addRow("UTF-8 len 2 invalid range") << "Text \xC0\x90   "_ba << false;

    // multibyte sequences - length 3
    QTest::addRow("UTF-8 CJK") //
        << QByteArray::fromHex("e998bfe5b094e58d91e696afe5b1b1e88489") // 阿尔卑斯山脉
        << true;
    QTest::addRow("UTF-8 len 3 a") << "Text \xE2\x80\x90 "_ba << true; // "‐" (HYPHEN)
    QTest::addRow("UTF-8 len 3-1 short") << "Text \xE2\x80 "_ba << false;
    QTest::addRow("UTF-8 len 3-2 short") << "Text \xE2 "_ba << false;

    QTest::addRow("UTF-8 len 3 b") << "Text \xE0\xbf\xbf "_ba << true; // "๏" (THAI CHARACTER FONGMAN)
    QTest::addRow("UTF-8 len 3 invalid range") << "Text \xE0\x9f\x90 "_ba << false;

    QTest::addRow("UTF-8 len 3 c") << "Text \xED\x80\x80 "_ba << true; // "퀀" (HANGUL SYLLABLE KWEON)
    QTest::addRow("UTF-8 invalid CESU") << "Text \xED\xbf\x80 "_ba << false;

    // multibyte sequences - length 4
    QTest::addRow("UTF-8 SMP Symbols") << "\xF0\x9F\x82\xA1 "_ba << true; // "🂡 " (ACE OF SPADES)
    QTest::addRow("UTF-8 len 4-1 short") << "\xF0\x9F\x82  "_ba << false;
    QTest::addRow("UTF-8 len 4-2 short") << "\xF0\x9F   "_ba << false;
    QTest::addRow("UTF-8 len 4-3 short") << "\xF0    "_ba << false;
    QTest::addRow("UTF-8 len 4 invalid long") << "\xF0\x8F\x90\x90 "_ba << false;
    QTest::addRow("UTF-8 len 4 invalid range") << "\xF5\x90\x90\x90 "_ba << false;

    // multibyte sequences - length 5/6 (invalid)
    QTest::addRow("UTF-8 len 5 invalid") << "\xF8\x90\x90\x90\x90 "_ba << false;
    QTest::addRow("UTF-8 len 6 invalid") << "\xFC\x90\x90\x90\x90\x90 "_ba << false;

    QTest::addRow("UTF-8 other invalid") << "\xFE "_ba << false;
}

void KEncodingProberUnitTest::testUtf16BE()
{
    QFETCH(QByteArray, data);
    QFETCH(bool, utf16BEValid);

    using namespace kencodingprober;

    nsCodingStateMachine stateMachine{&UCS2BESMModel};
    nsSMState state = eStart;

    QEXPECT_FAIL("UTF16 Interpunctuation little", "valid codepoint rejected", Abort);
    QEXPECT_FAIL("UTF16 Math supplement big", "valid codepoint rejected", Abort);
    QEXPECT_FAIL("UTF16 ZWNBSP little", "zero width no-break space rejected", Abort);
    for (auto b : data) {
        state = stateMachine.NextState(b);
    }

    if (utf16BEValid) {
        QVERIFY(state != eError);
    } else {
        QVERIFY(state == eError);
    }
}

void KEncodingProberUnitTest::testUtf16LE()
{
    QFETCH(QByteArray, data);
    QFETCH(bool, utf16LEValid);

    using namespace kencodingprober;

    nsCodingStateMachine stateMachine{&UCS2LESMModel};
    nsSMState state = eStart;

    QEXPECT_FAIL("UTF16 Interpunctuation big", "valid codepoint rejected", Abort);
    QEXPECT_FAIL("UTF16 Math supplement little", "valid codepoint rejected", Abort);
    QEXPECT_FAIL("UTF16 ZWNBSP big", "zero width no-break space rejected", Abort);
    for (auto b : data) {
        state = stateMachine.NextState(b);
    }

    if (utf16LEValid) {
        QVERIFY(state != eError);
    } else {
        QVERIFY(state == eError);
    }
}

void KEncodingProberUnitTest::testUtf16_common_data()
{
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<bool>("utf16BEValid");
    QTest::addColumn<bool>("utf16LEValid");

    QTest::addRow("empty") << QByteArray() << true << true;
    // BOM must be detected
    QTest::addRow("BE BOM") << QByteArray("\xFE\xFF") << true << false;
    QTest::addRow("LE BOM") << QByteArray("\xFF\xFE") << false << true;
    // swapped endianess does not cause an error, as the codepoint is still valid
    QTest::addRow("BE HS+LS") << QByteArray("\xDC\x00\xD8\x00") << true << true;
    QTest::addRow("LE HS+LS") << QByteArray("\x00\xDC\x00\xD8") << true << true;

    struct Utf16TestData {
        const char *name;
        const std::span<const char16_t> data;
        bool validBig;
        bool validLittle;
    };
    using namespace std::string_view_literals;
    constexpr std::array<Utf16TestData, 7> utf16TestData = {
        // syntactically correct even with wrong endianess
        Utf16TestData{"UTF16 XY", u"XY"sv, true, true},
        Utf16TestData{"UTF16 ab", u"ab"sv, true, true},
        Utf16TestData{"UTF16 äöü", u"äöü"sv, true, true},
        Utf16TestData{"UTF16 BOM", u"\xFEFF"sv, true, false},
        // "‛" or "ᬠ" (U+1B20 BALINESE LETTER DA MURDA MAHAPRANA)
        Utf16TestData{"UTF16 Interpunctuation", u"\x201B"sv, true, true},
        // "⨯" or "⼪" (U+2F2A KANGXI RADICAL LAME)
        Utf16TestData{"UTF16 Math supplement", u"\x2A2F"sv, true, true},
        // ZWNBSP aka BOM inside the document is deprecated, but valid
        Utf16TestData{"UTF16 ZWNBSP", u" \xFEFF"sv, true, true},
    };

    for (const auto &tc : utf16TestData) {
        QByteArray data;
        data.resize(tc.data.size() * 2);

        qToBigEndian<quint16>(tc.data.data(), tc.data.size(), data.data());
        QTest::addRow("%s big", tc.name) << data << tc.validBig << tc.validLittle;

        qToLittleEndian<quint16>(tc.data.data(), tc.data.size(), data.data());
        QTest::addRow("%s little", tc.name) << data << tc.validLittle << tc.validBig;
    }
}

void KEncodingProberUnitTest::testUtf16BE_data()
{
    testUtf16_common_data();
}

void KEncodingProberUnitTest::testUtf16LE_data()
{
    testUtf16_common_data();
}

QTEST_MAIN(KEncodingProberUnitTest)

#include "kencodingproberunittest.moc"
