/*
    SPDX-FileCopyrightText: 2012 Ni Hui <shuizhuyuanluo@126.com>
    SPDX-FileCopyrightText: 2025 Stefan Brüns <stefan.bruens@rwth-aachen.de>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <QTest>
#include <kencodingprober.h>

class KEncodingProberTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testReset();
    void testShort();
    void testShort_data();
    void testProbe();
    void testProbe_data();
};

void KEncodingProberTest::testReset()
{
    auto ep = std::make_unique<KEncodingProber>();
    ep->feed(QByteArray("some random data @*@#&jd"));
    ep->reset();
    QCOMPARE(ep->state(), KEncodingProber::Probing);
    QCOMPARE(ep->encoding().toLower(), QByteArray("utf-8"));
}

void KEncodingProberTest::testShort()
{
    QFETCH(QByteArray, data);

    KEncodingProber ep(KEncodingProber::Universal);
    ep.feed(data);
}

void KEncodingProberTest::testShort_data()
{
    using namespace Qt::StringLiterals;

    QTest::addColumn<QByteArray>("data");

    QTest::addRow("empty") << QByteArray();
    QTest::addRow("Len1") << "a"_ba;
    QTest::addRow("Len2") << "ab"_ba;
    QTest::addRow("Len3") << "abc"_ba;
    QTest::addRow("Len4") << "ab d"_ba;
    QTest::addRow("Len5") << "ab de"_ba;

    QTest::addRow("Short BOM UTF-8") << QByteArray("\xef\xbb", 2);
    QTest::addRow("Short BOM UTF-16BE") << QByteArray("\xfe\xff", 2);
}

void KEncodingProberTest::testProbe()
{
    QFETCH(QByteArray, data);
    QFETCH(KEncodingProber::ProberType, proberType);
    QFETCH(QByteArray, encoding);

    KEncodingProber ep(proberType);
    QCOMPARE(ep.proberType(), proberType);
    ep.feed(data);

    QEXPECT_FAIL("UTF-16LE Unicode", "UTF-16BE preferred unless erroneous", Abort);
    QEXPECT_FAIL("UTF-16LE Unicode definite 1", "UTF-16BE invalid surrogate ignored", Abort);
    QEXPECT_FAIL("UTF-16BE Unicode definite 2", "UTF-16BE valid code misdetected", Abort);
    QEXPECT_FAIL("UTF-16LE Unicode definite 2", "UTF-16LE valid code misdetected", Abort);
    QEXPECT_FAIL("utf-8 Hebrew", "UTF-8 zero confidence", Abort);
    QCOMPARE(ep.encoding().toLower(), encoding);

    QEXPECT_FAIL("UTF-16BE Unicode", "UTF-16 no confidence", Abort);
    QEXPECT_FAIL("UTF-16BE Unicode definite 1", "UTF-16LE invalid surrogate ignored, no confidence", Abort);
    QCOMPARE_GE(ep.confidence(), 0.2);
}

void KEncodingProberTest::testProbe_data()
{
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<KEncodingProber::ProberType>("proberType");
    QTest::addColumn<QByteArray>("encoding");

    QTest::addRow("utf-8") //
        << QByteArray::fromHex("e998bfe5b094e58d91e696afe5b1b1e88489") //
        << KEncodingProber::Universal << QByteArray("utf-8");

    QTest::addRow("gb18030") //
        << QByteArray::fromHex("d7d4d3c9b5c4b0d9bfc6c8abcae9") //
        << KEncodingProber::ChineseSimplified << QByteArray("gb18030");

    QTest::addRow("shift_jis") //
        << QByteArray::fromHex("8374838a815b955389c88e969354") //
        << KEncodingProber::Japanese << QByteArray("shift_jis");

    QTest::addRow("big5") //
        << QByteArray::fromHex("aefcafc7a6caa474a141a6b3ae65a444a46a") //
        << KEncodingProber::ChineseTraditional << QByteArray("big5");

    // "שפן אכל קצת גזר בטעם חסה, ודי" - "A bunny ate some lettuce-flavored carrots, and he had enough"
    QTest::addRow("windows-1255 Hebrew") //
        << QByteArray::fromHex("f9f4ef20e0ebec20f7f6fa20e2e6f820e1e8f2ed20e7f1e42c20e5e3e9") //
        << KEncodingProber::Universal << QByteArray("windows-1255");

    // Same "שפן אכל קצת גזר בטעם חסה, ודי", but using UTF-8
    QTest::addRow("utf-8 Hebrew") << QByteArray::fromHex( //
        "d7a9d7a4d79f20d790d79bd79c20d7a7d7a6d7aa20d792d7"
        "96d7a820d791d798d7a2d79d20d797d7a1d7942c20d795d7"
        "93d799") << //
        KEncodingProber::Universal << QByteArray("utf-8");

    // binary data, just make sure we do not crash (cf. crash in bug #357341)
    const auto binaryData = QByteArray::fromBase64( //
        "4QEAAAAOAAAAgVBYVIp1X0cQSZ67QGBARKLmgwFdRqxVgwJbyCougwNVrEZdiARNdogFmAScBkph"
        "Y2sgQXVkaW9DRIEHK4JhWg0OvPK2SjYuwNZv1ogI/xOICf8TgxUZjO5WiREAnBJIZWxsbyB2MC41"
        "MGGIE1DD");
    QTest::addRow("binaryData") //
        << binaryData << KEncodingProber::Universal << QByteArray("utf-8");

    QTest::addRow("BOM UTF-8") //
        << QByteArray("\xef\xbb\xbfZ", 4) // "<UTF-8 BOM>Z"
        << KEncodingProber::Universal << QByteArray("utf-8");
    QTest::addRow("BOM UTF-16BE") //
        << QByteArray("\xfe\xff\x00Z", 4) // "<UTF-16BE BOM>Z"
        << KEncodingProber::Universal << QByteArray("utf-16be");
    QTest::addRow("BOM UTF-16LE") //
        << QByteArray("\xff\xfeZ\x00", 4) // "<UTF-16LE BOM>Z"
        << KEncodingProber::Universal << QByteArray("utf-16le");

    QTest::addRow("UTF-16BE Unicode") //
        << QByteArray("\x00\x0a\x00\xc4\x00\xd6", 6) // "\nÄÖ"
        << KEncodingProber::Unicode << QByteArray("utf-16be");
    QTest::addRow("UTF-16LE Unicode") //
        << QByteArray("\x0a\x00\xc4\x00\xd6\x00", 6) // "\nÄÖ"
        << KEncodingProber::Unicode << QByteArray("utf-16le");
    QTest::addRow("UTF-16BE Unicode definite 1") //
        << QByteArray("\x00\xdc\x00\x20", 4) // "Ü "
        << KEncodingProber::Unicode << QByteArray("utf-16be");
    QTest::addRow("UTF-16LE Unicode definite 1") //
        << QByteArray("\xdc\x00\x20\x00", 4) // "Ü "
        << KEncodingProber::Unicode << QByteArray("utf-16le");
    QTest::addRow("UTF-16BE Unicode definite 2") //
        << QByteArray("\xc4\x00\x2a\x00\x00\xdc", 6) // "Ä*<inv>" or "쐀⨀Ü"
        << KEncodingProber::Unicode << QByteArray("utf-16be");
    QTest::addRow("UTF-16LE Unicode definite 2") //
        << QByteArray("\x00\xc4\x00\x2a\xdc\x00", 6) // "Ä*<inv>" or "쐀⨀Ü"
        << KEncodingProber::Unicode << QByteArray("utf-16le");
}

QTEST_MAIN(KEncodingProberTest)

#include "kencodingprobertest.moc"
