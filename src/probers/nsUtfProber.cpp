/*  -*- C++ -*-
    SPDX-FileCopyrightText: 2026 Stefan Brüns <stefan.bruens@rwth-aachen.de>

    SPDX-License-Identifier: MIT
*/

#include "StateMachineProber.inl"
#include "nsMBCSSM.h"

namespace kencodingprober
{
template<>
constexpr const SMModel &modelForProber<SMProberType::Utf8>()
{
    return kencodingprober::UTF8SMModel;
}

template<>
constexpr const SMModel &modelForProber<SMProberType::Utf16LE>()
{
    return kencodingprober::UCS2LESMModel;
}

template<>
constexpr const SMModel &modelForProber<SMProberType::Utf16BE>()
{
    return kencodingprober::UCS2BESMModel;
}

template class StateMachineProber<SMProberType::Utf8>;
template class StateMachineProber<SMProberType::Utf16LE>;
template class StateMachineProber<SMProberType::Utf16BE>;

} // namespace kencodingprober
