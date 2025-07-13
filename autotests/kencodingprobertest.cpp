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
    auto ep = std::make_unique<KEncodingProber>();

    // utf-8
    ep->setProberType(KEncodingProber::Universal);
    ep->feed(QByteArray::fromHex("e998bfe5b094e58d91e696afe5b1b1e88489"));
    QCOMPARE(ep->encoding().toLower(), QByteArray("utf-8"));
    ep->reset();

    // gb18030
    ep->setProberType(KEncodingProber::ChineseSimplified);
    ep->feed(QByteArray::fromHex("d7d4d3c9b5c4b0d9bfc6c8abcae9"));
    QCOMPARE(ep->encoding().toLower(), QByteArray("gb18030"));
    ep->reset();

    // shift_jis
    ep->setProberType(KEncodingProber::Japanese);
    ep->feed(QByteArray::fromHex("8374838a815b955389c88e969354"));
    QCOMPARE(ep->encoding().toLower(), QByteArray("shift_jis"));
    ep->reset();

    // big5
    ep->setProberType(KEncodingProber::ChineseTraditional);
    ep->feed(QByteArray::fromHex("aefcafc7a6caa474a141a6b3ae65a444a46a"));
    QCOMPARE(ep->encoding().toLower(), QByteArray("big5"));
    ep->reset();

    // binary data, just make sure we do not crash (cf. crash in bug #357341)
    const auto binaryData = QByteArray::fromBase64( //
        "4QEAAAAOAAAAgVBYVIp1X0cQSZ67QGBARKLmgwFdRqxVgwJbyCougwNVrEZdiARNdogFmAScBkph"
        "Y2sgQXVkaW9DRIEHK4JhWg0OvPK2SjYuwNZv1ogI/xOICf8TgxUZjO5WiREAnBJIZWxsbyB2MC41"
        "MGGIE1DD");
    ep->setProberType(KEncodingProber::Universal);
    ep->feed(binaryData);
    QCOMPARE(ep->encoding().toLower(), QByteArray("utf-8"));
    ep->reset();
}

QTEST_MAIN(KEncodingProberTest)

#include "kencodingprobertest.moc"
