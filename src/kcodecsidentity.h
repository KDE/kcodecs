/*  -*- c++ -*-
    SPDX-FileCopyrightText: 2004 Marc Mutz <mutz@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
    {
    }

    /**
      Destroys the codec.
    */
    ~IdentityCodec() override
    {
    }

    using Codec::decode;
    using Codec::encode;

    /**
      @copydoc
      QByteArray Codec::encode()
    */
    QByteArray encode(const QByteArray &src, Codec::NewlineType newline = Codec::NewlineLF) const override;

    /**
      @copydoc
      QByteArray Codec::decode()
    */
    QByteArray decode(const QByteArray &src, Codec::NewlineType newline = Codec::NewlineLF) const override;

    /**
      @copydoc
      Codec::maxEncodedSizeFor()
    */
    int maxEncodedSizeFor(int insize, Codec::NewlineType newline = Codec::NewlineLF) const override
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
    int maxDecodedSizeFor(int insize, Codec::NewlineType newline) const override
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
    Encoder *makeEncoder(Codec::NewlineType newline = Codec::NewlineCRLF) const override;

    /**
      @copydoc
      Codec::makeDecoder()
    */
    Decoder *makeDecoder(Codec::NewlineType newline = Codec::NewlineCRLF) const override;
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
    {
    }

    /**
      Destroys the codec.
    */
    ~SevenBitCodec() override
    {
    }

    /**
      @copydoc
      Codec::name()
    */
    const char *name() const override
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
    {
    }

    /**
      Destroys the codec.
    */
    ~EightBitCodec() override
    {
    }

    /**
      @copydoc
      Codec::name()
    */
    const char *name() const override
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
    {
    }

    /**
      Destroys the codec.
    */
    ~BinaryCodec() override
    {
    }

    /**
      @copydoc
      Codec::name()
    */
    const char *name() const override
    {
        return "binary";
    }

    /**
      @copydoc
      Codec::maxEncodedSizeFor()
    */
    int maxEncodedSizeFor(int insize, Codec::NewlineType newline = Codec::NewlineLF) const override
    {
        Q_UNUSED(newline);
        return insize;
    }

    /**
      @copydoc
      Codec::maxDecodedSizeFor()
    */
    int maxDecodedSizeFor(int insize, Codec::NewlineType newline = Codec::NewlineLF) const override
    {
        Q_UNUSED(newline);
        return insize;
    }
};

} // namespace KCodecs

#endif // KCODECS_IDENTITY_H
