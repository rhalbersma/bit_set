#pragma once

//          Copyright Rein Halbersma 2014-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/proxy.hpp>               // bidirectional_bit_iterator
#include <boost/dynamic_bitset.hpp>     // dynamic_bitset
#include <algorithm>                    // lexicographical_compare_three_way
#include <cassert>                      // assert
#include <compare>                      // strong_ordering
#include <concepts>                     // unsigned_integral
#include <cstddef>                      // std::size_t
#include <iterator>                     // make_reverse_iterator
#include <ranges>                       // begin, end, find_if, min, rbegin, rend
                                        // iota, reverse

namespace boost {

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] constexpr std::size_t find_next(dynamic_bitset<Block, Allocator> const& c, std::size_t n) noexcept
{
        return c.find_next(n);
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] constexpr std::size_t find_prev(dynamic_bitset<Block, Allocator> const& c, std::size_t n) noexcept
{
        assert(c.any());
        return *std::ranges::find_if(std::views::iota(0uz, std::ranges::min(n, c.size())) | std::views::reverse, [&](auto i) {
                return c[i];
        });
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto begin(dynamic_bitset<Block, Allocator>& c) noexcept
        -> xstd::bit::bidirectional::const_iterator<dynamic_bitset<Block, Allocator>>
{
        return { &c, c.find_first() };
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto begin(dynamic_bitset<Block, Allocator> const& c) noexcept
        -> xstd::bit::bidirectional::const_iterator<dynamic_bitset<Block, Allocator>>
{
        return { &c, c.find_first() };
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto end(dynamic_bitset<Block, Allocator>& c) noexcept
        -> xstd::bit::bidirectional::const_iterator<dynamic_bitset<Block, Allocator>>
{
        return { &c, c.npos };
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto end(dynamic_bitset<Block, Allocator> const& c) noexcept
        -> xstd::bit::bidirectional::const_iterator<dynamic_bitset<Block, Allocator>>
{
        return { &c, c.npos };
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto cbegin(dynamic_bitset<Block, Allocator> const& c) noexcept
{
        return std::ranges::begin(c);
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto cend(dynamic_bitset<Block, Allocator> const& c) noexcept
{
        return std::ranges::end(c);
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto rbegin(dynamic_bitset<Block, Allocator>& c) noexcept
{
        return std::make_reverse_iterator(std::ranges::end(c));
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto rbegin(dynamic_bitset<Block, Allocator> const& c) noexcept
{
        return std::make_reverse_iterator(std::ranges::end(c));
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto rend(dynamic_bitset<Block, Allocator>& c) noexcept
{
        return std::make_reverse_iterator(std::ranges::begin(c));
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto rend(dynamic_bitset<Block, Allocator> const& c) noexcept
{
        return std::make_reverse_iterator(std::ranges::begin(c));
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto crbegin(dynamic_bitset<Block, Allocator> const& bs) noexcept
{
        return std::ranges::rbegin(bs);
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto crend(dynamic_bitset<Block, Allocator> const& c) noexcept
{
        return std::ranges::rend(c);
}

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
