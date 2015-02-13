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

#ifndef KCODECS_IDENTITY_H
#define KCODECS_IDENTITY_H

#include "kcodecs.h"

class QByteArray;

namespace KCodecs
{

/**
  @brief
  A class representing the Identify @ref codec.
*/
class IdentityCodec : public Codec
{
public:
    /**
      Constructs the Identity codec.
    */
    IdentityCodec()
        : Codec()
    {}

    /**
      Destroys the codec.
    */
    ~IdentityCodec() {}

    using Codec::encode;
    using Codec::decode;

    /**
      @copydoc
      QByteArray Codec::encode()
    */
    QByteArray encode(const QByteArray &src, Codec::NewlineType newline = Codec::NewlineLF) const Q_DECL_OVERRIDE;

    /**
      @copydoc
      QByteArray Codec::decode()
    */
    QByteArray decode(const QByteArray &src, Codec::NewlineType newline = Codec::NewlineLF) const Q_DECL_OVERRIDE;

    /**
      @copydoc
      Codec::maxEncodedSizeFor()
    */
    int maxEncodedSizeFor(int insize, Codec::NewlineType newline = Codec::NewlineLF) const Q_DECL_OVERRIDE
    {
        if (newline == Codec::NewlineCRLF) {
            return 2 * insize;
        } else {
            return insize;
        }
    }

    /**
      @copydoc
      Codec::maxDecodedSizeFor()
    */
    int maxDecodedSizeFor(int insize, Codec::NewlineType newline) const Q_DECL_OVERRIDE
    {
        if (newline == Codec::NewlineCRLF) {
            return 2 * insize;
        } else {
            return insize;
        }
    }

    /**
      @copydoc
      Codec::makeEncoder()
    */
    Encoder *makeEncoder(Codec::NewlineType newline = Codec::NewlineCRLF) const Q_DECL_OVERRIDE;

    /**
      @copydoc
      Codec::makeDecoder()
    */
    Decoder *makeDecoder(Codec::NewlineType newline = Codec::NewlineCRLF) const Q_DECL_OVERRIDE;
};

/**
  @brief
  A class representing the @ref codec for @ref seven-bit-text.
*/
class SevenBitCodec : public IdentityCodec
{
public:
    /**
      Constructs the 7-bit codec.
    */
    SevenBitCodec()
        : IdentityCodec()
    {}

    /**
      Destroys the codec.
    */
    ~SevenBitCodec() {}

    /**
      @copydoc
      Codec::name()
    */
    const char *name() const Q_DECL_OVERRIDE
    {
        return "7bit";
    }
};

/**
  @brief
  A class representing the @ref codec for @ref eight-bit-text.
*/
class EightBitCodec : public IdentityCodec
{
public:
    /**
      Constructs the 8-bit codec.
    */
    EightBitCodec()
        : IdentityCodec()
    {}

    /**
      Destroys the codec.
    */
    ~EightBitCodec() {}

    /**
      @copydoc
      Codec::name()
    */
    const char *name() const Q_DECL_OVERRIDE
    {
        return "8bit";
    }
};

/**
  @brief
  A class representing the @ref codec for @ref eight-bit-binary.
*/
class BinaryCodec : public IdentityCodec
{
public:
    /**
      Constructs the 8-bit-binary codec.
    */
    BinaryCodec()
        : IdentityCodec()
    {}

    /**
      Destroys the codec.
    */
    ~BinaryCodec() {}

    /**
      @copydoc
      Codec::name()
    */
    const char *name() const Q_DECL_OVERRIDE
    {
        return "binary";
    }

    /**
      @copydoc
      Codec::maxEncodedSizeFor()
    */
    int maxEncodedSizeFor(int insize, Codec::NewlineType newline = Codec::NewlineLF) const Q_DECL_OVERRIDE
    {
        Q_UNUSED(newline);
        return insize;
    }

    /**
      @copydoc
      Codec::maxDecodedSizeFor()
    */
    int maxDecodedSizeFor(int insize, Codec::NewlineType newline = Codec::NewlineLF) const Q_DECL_OVERRIDE
    {
        Q_UNUSED(newline);
        return insize;
    }
};

} // namespace KCodecs

#endif // KCODECS_IDENTITY_H
