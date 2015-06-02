/*
    Copyright (c) 2010 Volker Krause <vkrause@kde.org>

    This library is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This library is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to the
    Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301, USA.
*/

#include <QObject>
#include <QtTest>
#include <KCodecs>
#include <QByteArray>
#include <qtest.h>

#include "../src/kcodecsbase64.h"

class Base64Benchmark : public QObject
{
    Q_OBJECT
private:
    static QByteArray fillByteArray(int size)
    {
        char c = 0;
        QByteArray result;
        result.reserve(size);
        while (result.size() < size) {
            result.append(c++);
        }
        return result;
    }

    void createTestSet()
    {
        QTest::addColumn<QByteArray>("output");
        QTest::addColumn<QByteArray>("input");
        QTest::newRow("empty") << QByteArray() << QByteArray();
        QTest::newRow("128") << fillByteArray(128) << KCodecs::base64Encode(fillByteArray(128), true);
        QTest::newRow("1k") << fillByteArray(1 << 10) << KCodecs::base64Encode(fillByteArray(1 << 10), true);
        QTest::newRow("1M") << fillByteArray(1 << 20) << KCodecs::base64Encode(fillByteArray(1 << 20), true);
    }
private Q_SLOTS:
    void benchmarkKCodecDecode_data()
    {
        createTestSet();
    }

    void benchmarkKCodecDecode()
    {
        QFETCH(QByteArray, input);
        QFETCH(QByteArray, output);
        QByteArray result;
        QBENCHMARK {
            result = KCodecs::base64Decode(input);
        }
        QCOMPARE(result, output);
    }

    void benchmarkQByteArrayDecode_data()
    {
        createTestSet();
    }

    void benchmarkQByteArrayDecode()
    {
        QFETCH(QByteArray, input);
        QFETCH(QByteArray, output);
        QByteArray result;
        QBENCHMARK {
            result = QByteArray::fromBase64(input);
        }
        QCOMPARE(result, output);
    }

    void benchmarkKMimeBase64Decoder_data()
    {
        createTestSet();
    }

    void benchmarkKMimeBase64Decoder()
    {
        QFETCH(QByteArray, input);
        QFETCH(QByteArray, output);
        QByteArray result;
        QBENCHMARK {
            KCodecs::Codec *codec = KCodecs::Codec::codecForName("base64");
            QVERIFY(codec);
            result.resize(codec->maxDecodedSizeFor(input.size()));
            KCodecs::Decoder *decoder = codec->makeDecoder();
            QByteArray::const_iterator inputIt = input.constBegin();
            QByteArray::iterator resultIt = result.begin();
            decoder->decode(inputIt, input.constEnd(), resultIt, result.end());
            result.truncate(resultIt - result.begin());
        }
        QCOMPARE(result, output);
    }
};

QTEST_MAIN(Base64Benchmark)

#include "base64benchmark.moc"
