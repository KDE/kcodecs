/*
    SPDX-FileCopyrightText: 2021 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KUSASCIITEXTCODEC_H
#define KUSASCIITEXTCODEC_H

#include <QTextCodec>

// TODO KF6, remove this class, bug seems to be fixed in Qt6

// Custom variant due to Qt-built-in not reporting encoding errors, see QTBUG-83081
// Bug seems present since Qt 5.0 when ICU-based codecs were added
// in 88d2e92b39ffd4a6ea9446498ad5a1cb208022a6 to the qtbase repo.
// See KUsAsciiTextCodecTest::testBrokenBuiltinEncoding() for a test checking
// the presence of the bug.
class KUsAsciiTextCodec : public QTextCodec
{
public:
    KUsAsciiTextCodec() = default;
    ~KUsAsciiTextCodec() override = default;

public: // QTextCodec API
    QByteArray name() const override;
    QList<QByteArray> aliases() const override;
    int mibEnum() const override;

protected: // QTextCodec API
    QByteArray convertFromUnicode(const QChar *input, int number, QTextCodec::ConverterState *state) const override;
    QString convertToUnicode(const char *chars, int len, QTextCodec::ConverterState *state) const override;
};

#endif
