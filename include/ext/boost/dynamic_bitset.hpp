#pragma once

//          Copyright Rein Halbersma 2014-2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/proxy/forward.hpp>       // begin, end
#include <boost/dynamic_bitset.hpp>     // dynamic_bitset
#include <algorithm>                    // lexicographical_compare_three_way
#include <compare>                      // strong_ordering
#include <concepts>                     // unsigned_integral
#include <cstddef>                      // std::size_t
#include <ranges>                       // begin, end

namespace boost {

template<std::unsigned_integral Block, class Allocator> [[nodiscard]] constexpr std::size_t find_first(dynamic_bitset<Block, Allocator> const& c)                noexcept { return c.find_first(); }
template<std::unsigned_integral Block, class Allocator> [[nodiscard]] constexpr std::size_t find_last (dynamic_bitset<Block, Allocator> const& c)                noexcept { return c.npos;         }
template<std::unsigned_integral Block, class Allocator> [[nodiscard]] constexpr std::size_t find_next (dynamic_bitset<Block, Allocator> const& c, std::size_t n) noexcept { return c.find_next(n); }

template<std::unsigned_integral Block, class Allocator> [[nodiscard]] constexpr auto begin (      dynamic_bitset<Block, Allocator>& c) noexcept { return xstd::proxy::forward::begin(c); }
template<std::unsigned_integral Block, class Allocator> [[nodiscard]] constexpr auto begin (const dynamic_bitset<Block, Allocator>& c) noexcept { return xstd::proxy::forward::begin(c); }
template<std::unsigned_integral Block, class Allocator> [[nodiscard]] constexpr auto end   (      dynamic_bitset<Block, Allocator>& c) noexcept { return xstd::proxy::forward::end(c); }
template<std::unsigned_integral Block, class Allocator> [[nodiscard]] constexpr auto end   (const dynamic_bitset<Block, Allocator>& c) noexcept { return xstd::proxy::forward::end(c); }
template<std::unsigned_integral Block, class Allocator> [[nodiscard]] constexpr auto cbegin(const dynamic_bitset<Block, Allocator>& c) noexcept { return std::ranges::begin(c); }
template<std::unsigned_integral Block, class Allocator> [[nodiscard]] constexpr auto cend  (const dynamic_bitset<Block, Allocator>& c) noexcept { return std::ranges::end  (c); }

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto operator<=>(dynamic_bitset<Block, Allocator> const& x, dynamic_bitset<Block, Allocator> const& y) noexcept
        -> std::strong_ordering
{
        return std::lexicographical_compare_three_way(
                std::ranges::begin(x), std::ranges::end(x),
                std::ranges::begin(y), std::ranges::end(y)
        );
}

}       // namespace boost
