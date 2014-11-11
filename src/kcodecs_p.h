/*
   Copyright (C) 2014 Daniel Vrátil <dvratil@redhat.com>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License (LGPL)
   version 2 as published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef KCODECS_P_H
#define KCODECS_P_H

#include "kcodecs.h"

namespace KCodecs
{

class EncoderPrivate
{
public:
    EncoderPrivate(Codec::NewlineType newline);

    /**
      An output buffer to simplify some codecs.
      Used with write() and flushOutputBuffer().
    */
    char outputBuffer[ Encoder::maxBufferedChars ];

    uchar outputBufferCursor;
    const Codec::NewlineType newline;
};


class DecoderPrivate
{
public:
    DecoderPrivate(Codec::NewlineType newline);

    const Codec::NewlineType newline;
};

}

#endif // KCODECS_P_H
