/*  -*- C++ -*-
    SPDX-FileCopyrightText: 2026 Stefan Brüns <stefan.bruens@rwth-aachen.de>

    SPDX-License-Identifier: MIT
*/

#include "StateMachineProber.inl"
#include "nsEscSM.h"

namespace kencodingprober
{
template<>
constexpr const SMModel &modelForProber<SMProberType::ISO2022_JP>()
{
    return kencodingprober::ISO2022JPSMModel;
}

template<>
constexpr const SMModel &modelForProber<SMProberType::HZ>()
{
    return kencodingprober::HZSMModel;
}

template class StateMachineProber<SMProberType::ISO2022_JP>;
template class StateMachineProber<SMProberType::HZ>;

} // namespace kencodingprober
