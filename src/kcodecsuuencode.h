/*  -*- c++ -*-
    Copyright (c) 2002 Marc Mutz <mutz@kde.org>

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
  defines a @ref uuencode @ref Codec class.

  @brief
  Defines the UUCodec class.

  @authors Marc Mutz \<mutz@kde.org\>

  @glossary @anchor UUEncode @anchor uuencode @b uuencode:
  a binary to text encoding scheme. For more information, see the
  <a href="http://en.wikipedia.org/wiki/Uuencode"> Wikipedia Uuencode page</a>.
*/

#ifndef KCODECS_UUENCODE_H
#define KCODECS_UUENCODE_H

#include "kcodecs.h"

namespace KCodecs
{

/**
  @brief
  A class representing the @ref UUEncode @ref codec.
*/
class UUCodec : public Codec
{
public:
    /**
      Constructs a UUEncode codec.
    */
    UUCodec()
        : Codec()
    {}

    /**
      Destroys the codec.
    */
    virtual ~UUCodec() {}

    /**
      @copydoc
      Codec::name()
    */
    const char *name() const
    {
        return "x-uuencode";
    }

    /**
      @copydoc
      Codec::maxEncodedSizeFor()
    */
    int maxEncodedSizeFor(int insize, NewlineType newline = Codec::NewlineLF) const
    {
        Q_UNUSED(newline);
        return insize; // we have no encoder!
    }

    /**
      @copydoc
      Codec::maxDecodedSizeFor()
    */
    int maxDecodedSizeFor(int insize, NewlineType newline = Codec::NewlineLF) const
    {
        // assuming all characters are part of the uuencode stream (which
        // does almost never hold due to required linebreaking; but
        // additional non-uu chars don't affect the output size), each
        // 4-tupel of them becomes a 3-tupel in the decoded octet
        // stream. So:
        int result = ((insize + 3) / 4) * 3;
        // but all of them may be \n, so
        if (newline == Codec::NewlineCRLF) {
            result *= 2; // :-o
        }
        return result;
    }

    /**
      @copydoc
      Codec::makeEncoder()
    */
    Encoder *makeEncoder(NewlineType newline = Codec::NewlineLF) const;

    /**
      @copydoc
      Codec::makeEncoder()
    */
    Decoder *makeDecoder(NewlineType newline = Codec::NewlineLF) const;
};

} // namespace KCodecs

#endif // KCODECS_UUENCODE_H
