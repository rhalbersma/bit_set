#pragma once

//          Copyright Rein Halbersma 2014-2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/bit_set.hpp>     // bit_set
#include <concepts>             // unsigned_integral
#include <iosfwd>               // basic_ostream

namespace xstd {

template<class CharT, class Traits, std::size_t N, std::unsigned_integral Block>
auto& operator<<(std::basic_ostream<CharT, Traits>& ostr, bit_set<N, Block> const& bs)
{
        ostr << ostr.widen('{');
        auto first = true;
        for (auto x : bs) {
                if (!first) {
                        ostr << ostr.widen(',');
                } else {
                        first = false;
                }
                ostr << x;
        }
        ostr << ostr.widen('}');
        return ostr;
}

template<class CharT, class Traits, std::size_t N, std::unsigned_integral Block>
auto& operator>>(std::basic_istream<CharT, Traits>& istr, bit_set<N, Block>& bs)
{
        typename bit_set<N, Block>::value_type x;
        CharT c;

        istr >> c;
        assert(c == istr.widen('{'));
        istr >> c;
        for (auto first = true; c != istr.widen('}'); istr >> c) {
                assert(first == (c != istr.widen(',')));
                if (first) {
                        istr.putback(c);
                        first = false;
                }
                istr >> x;
                assert(0 <= x && x < static_cast<int>(bs.max_size()));
                bs.insert(x);
        }
        return istr;
}

}       // namespace xstd
