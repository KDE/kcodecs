/*  -*- c++ -*-
    Copyright (c) 2004 Marc Mutz <mutz@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/
/**
  @file
  This file is part of the API for handling @ref MIME data and
  defines the Identity, @ref seven-bit-text, @ref eight-bit-text,
  and @ref eight-bit-binary @ref Codec classes.

  @brief
  Defines the classes IdentityCodec, SevenBitCodec, EightBitCodec,
  and BinaryCodec.

  @authors Marc Mutz \<mutz@kde.org\>
*/

#include "kcodecsidentity.h"

#include <qdebug.h>

#include <QByteArray>

#include <cstring>

using namespace KCodecs;

class IdentityEnDecoder : public Encoder, public Decoder
{
protected:
    friend class KCodecs::IdentityCodec;
    IdentityEnDecoder(Codec::NewlineType newline)
        : Encoder(Codec::NewlineLF)
    {
        if (newline == Codec::NewlineCRLF) {
            qWarning() << "IdentityEnDecoder: CRLF isn't yet supported!";
        }
    }

public:
    ~IdentityEnDecoder() {}

    bool encode(const char *&scursor, const char *const send,
                char *&dcursor, const char *const dend) override
    {
        return decode(scursor, send, dcursor, dend);
    }

    bool decode(const char *&scursor, const char *const send,
                char *&dcursor, const char *const dend) override;

    bool finish(char *&dcursor, const char *const dend) override
    {
        Q_UNUSED(dcursor);
        Q_UNUSED(dend);
        return true;
    }
};

Encoder *IdentityCodec::makeEncoder(Codec::NewlineType newline) const
{
    return new IdentityEnDecoder(newline);
}

Decoder *IdentityCodec::makeDecoder(Codec::NewlineType newline) const
{
    return new IdentityEnDecoder(newline);
}

/********************************************************/
/********************************************************/
/********************************************************/

bool IdentityEnDecoder::decode(const char *&scursor, const char *const send,
                               char *&dcursor, const char *const dend)
{
    const int size = qMin(send - scursor, dcursor - dend);
    if (size > 0) {
        std::memmove(dcursor, scursor, size);
        dcursor += size;
        scursor += size;
    }
    return scursor == send;
}

QByteArray IdentityCodec::encode(const QByteArray &src, Codec::NewlineType newline) const
{
    if (newline == Codec::NewlineCRLF) {
        qWarning() << "IdentityCodec::encode(): CRLF not yet supported!";
    }
    return src;
}

QByteArray IdentityCodec::decode(const QByteArray &src, Codec::NewlineType newline) const
{
    if (newline == Codec::NewlineCRLF) {
        qWarning() << "IdentityCodec::decode(): CRLF not yet supported!";
    }
    return src;
}
