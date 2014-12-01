/*
   This file is part of the KDE project
   Copyright (C) 2004 David Faure <faure@kde.org>
   Copyright (C) 2009 Thomas McGuire <mcguire@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/
#ifndef TESTEMAIL_H
#define TESTEMAIL_H

#include <QtCore/QObject>

class KEmailAddressTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testGetNameAndEmail();
    void testGetNameAndEmail_data();
    void testIsValidEmailAddress();
    void testIsValidEmailAddress_data();
    void testIsValidAddressList();
    void testIsValidAddressList_data();
    void testIsValidSimpleEmailAddress();
    void testIsValidSimpleEmailAddress_data();
    void testGetEmailAddress();
    void testGetEmailAddress_data();
    void testCheckSplitEmailAddrList();
    void testCheckSplitEmailAddrList_data();
    void testNormalizeAddressesAndEncodeIDNs();
    void testNormalizeAddressesAndEncodeIDNs_data();
    void testNormalizeAddressesAndDecodeIDNs();
    void testNormalizeAddressesAndDecodeIDNs_data();
    void testQuoteIfNecessary();
    void testQuoteIfNecessary_data();
    void testMailtoUrls();
    void testMailtoUrls_data();
};

#endif
