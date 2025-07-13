/*
    SPDX-FileCopyrightText: 2012 Ni Hui <shuizhuyuanluo@126.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <QTest>
#include <kencodingprober.h>

class KEncodingProberTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testReset();
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

void KEncodingProberTest::testProbe()
{
    QFETCH(QByteArray, data);
    QFETCH(KEncodingProber::ProberType, proberType);
    QFETCH(QByteArray, encoding);

    KEncodingProber ep(proberType);
    QCOMPARE(ep.proberType(), proberType);
    ep.feed(data);

    QCOMPARE(ep.encoding().toLower(), encoding);
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

    // binary data, just make sure we do not crash (cf. crash in bug #357341)
    const auto binaryData = QByteArray::fromBase64( //
        "4QEAAAAOAAAAgVBYVIp1X0cQSZ67QGBARKLmgwFdRqxVgwJbyCougwNVrEZdiARNdogFmAScBkph"
        "Y2sgQXVkaW9DRIEHK4JhWg0OvPK2SjYuwNZv1ogI/xOICf8TgxUZjO5WiREAnBJIZWxsbyB2MC41"
        "MGGIE1DD");
    QTest::addRow("binaryData") //
        << binaryData << KEncodingProber::Universal << QByteArray("utf-8");
}

QTEST_MAIN(KEncodingProberTest)

#include "kencodingprobertest.moc"
