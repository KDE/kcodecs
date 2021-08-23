/*
    SPDX-FileCopyrightText: 2011 Romain Perier <bambi@kubuntu.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KCHARSETSTEST_H
#define KCHARSETSTEST_H

#include <QObject>

class KCharsetsTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testSingleton();
    void testCodecForName_data();
    void testCodecForName();
    void testFromEntity();
    void testToEntity();
    void testResolveEntities();
    void testEncodingNames();
    void testUsAsciiEncoding_data();
    void testUsAsciiEncoding();
    void testUsAsciiDecoding_data();
    void testUsAsciiDecoding();
};

#endif /* KCHARSETSTEST_H */
