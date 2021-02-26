/*
    SPDX-FileCopyrightText: 2006 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef KCODECSTEST_H
#define KCODECSTEST_H

#include <QObject>

class KCodecsTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testRFC2047decode();
    void testInvalidDecode();
    void testRFC2047encode();
};

#endif
