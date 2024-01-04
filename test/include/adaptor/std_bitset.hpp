#pragma once

//          Copyright Rein Halbersma 2014-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <bitset>       // bitset
#include <cstddef>      // size_t

namespace xstd {

template<std::size_t N>
auto& operator-=(std::bitset<N>& lhs, std::bitset<N> const& rhs) noexcept
{
        return lhs &= ~rhs;
}

template<std::size_t N>
auto operator-(std::bitset<N> const& lhs, std::bitset<N> const& rhs) noexcept
{
        auto nrv = lhs; nrv -= rhs; return nrv;
}

template<std::size_t N>
auto is_subset_of(std::bitset<N> const& lhs, std::bitset<N> const& rhs) noexcept
{
        return (lhs & ~rhs).none();
}

template<std::size_t N>
auto is_proper_subset_of(std::bitset<N> const& lhs, std::bitset<N> const& rhs) noexcept
{
        return is_subset_of(lhs, rhs) && lhs != rhs;
}

template<std::size_t N>
auto intersects(std::bitset<N> const& lhs, std::bitset<N> const& rhs) noexcept
{
        return (lhs & rhs).any();
}

}       // namespace xstd
