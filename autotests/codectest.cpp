/*
    Copyright (c) 2010 Thomas McGuire <mcguire@kde.org>

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
#include "codectest.h"

#include <qtest.h>

#include <QDir>

#include "../src/kcodecs.h"

using namespace KCodecs;

QTEST_MAIN(CodecTest)

enum Mode { Decode, Encode };
Q_DECLARE_METATYPE(Mode)

void CodecTest::testCodecs_data()
{
    QTest::addColumn<QByteArray>("input");
    QTest::addColumn<QByteArray>("expResult");
    QTest::addColumn<QByteArray>("codecName");
    QTest::addColumn<QString>("tag");
    QTest::addColumn<Mode>("mode");

    QDir codecBaseDir(QFINDTESTDATA("data"));
    const QStringList lst = codecBaseDir.entryList(QStringList(), QDir::Dirs | QDir::NoDotAndDotDot, QDir::NoSort);
    for (const QString &dir :  lst) {
        if (dir.toLower().startsWith(QLatin1String("codec_"))) {
            const QString codecName = dir.right(dir.size() - 6);
            QDir codecDir(codecBaseDir.path() + QLatin1String("/") + dir);
            const QStringList lst2 = codecDir.entryList(QStringList(), QDir::Files, QDir::NoSort);
            for (const QString &file : lst2) {
                if (file.toLower().endsWith(QLatin1String(".expected"))) {
                    const QString dataFileNameBase = file.left(file.size() - 9);
                    QFile dataFile(codecDir.path() + QLatin1Char('/') + dataFileNameBase);
                    QFile expectedFile(codecDir.path() + QLatin1Char('/') + file);
                    QVERIFY(dataFile.open(QIODevice::ReadOnly));
                    QVERIFY(expectedFile.open(QIODevice::ReadOnly));

                    Mode mode = Decode;
                    if (file.contains(QLatin1String("-decode"))) {
                        mode = Decode;
                    } else if (file.contains(QLatin1String("-encode"))) {
                        mode = Encode;
                    }

                    const QByteArray data = dataFile.readAll();
                    const QByteArray expected = expectedFile.readAll();

                    const QString tag = codecName + QLatin1Char('/') + dataFileNameBase;
                    QTest::newRow(tag.toLatin1().constData()) << data << expected << codecName.toLatin1() << tag  << mode;

                    dataFile.close();
                    expectedFile.close();
                }
            }
        }
    }
}

void CodecTest::testCodecs()
{
    QFETCH(QByteArray, input);
    QFETCH(QByteArray, expResult);
    QFETCH(QByteArray, codecName);
    QFETCH(QString, tag);
    QFETCH(Mode, mode);

    Codec *codec = Codec::codecForName(codecName);
    QVERIFY(codec);

    QStringList blacklistedTags;
    if (blacklistedTags.contains(tag)) {
        QEXPECT_FAIL(tag.toLatin1().constData(), "Codec broken", Continue);
    }

    QByteArray result;
    if (mode == Decode) {
        result = codec->decode(input, Codec::NewlineLF);
    } else {
        result = codec->encode(input, Codec::NewlineLF);
    }

    // More usable version of QCOMPARE(result, expResult), in case the difference is at the end...
    if (result != expResult) {
        const QList<QByteArray> lines = result.split('\n');
        const QList<QByteArray> expLines = expResult.split('\n');
        if (lines.count() == expLines.count())
            QCOMPARE(result.split('\n'), expResult.split('\n'));
    }
    QCOMPARE(result, expResult);
}

