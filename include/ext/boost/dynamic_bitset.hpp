#pragma once

//          Copyright Rein Halbersma 2014-2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/proxy/bidirectional.hpp> // begin, end
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
[[nodiscard]] constexpr std::size_t find_first(dynamic_bitset<Block, Allocator> const& c) noexcept
{
        return c.find_first();
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] constexpr std::size_t find_last(dynamic_bitset<Block, Allocator> const& c) noexcept
{
        return c.npos;
}

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

template<std::unsigned_integral Block, class Allocator> [[nodiscard]] constexpr auto begin  (      dynamic_bitset<Block, Allocator>& c) noexcept { return xstd::proxy::bidirectional::begin(c); }
template<std::unsigned_integral Block, class Allocator> [[nodiscard]] constexpr auto begin  (const dynamic_bitset<Block, Allocator>& c) noexcept { return xstd::proxy::bidirectional::begin(c); }
template<std::unsigned_integral Block, class Allocator> [[nodiscard]] constexpr auto end    (      dynamic_bitset<Block, Allocator>& c) noexcept { return xstd::proxy::bidirectional::end(c); }
template<std::unsigned_integral Block, class Allocator> [[nodiscard]] constexpr auto end    (const dynamic_bitset<Block, Allocator>& c) noexcept { return xstd::proxy::bidirectional::end(c); }
template<std::unsigned_integral Block, class Allocator> [[nodiscard]] constexpr auto cbegin (const dynamic_bitset<Block, Allocator>& c) noexcept { return std::ranges::begin(c); }
template<std::unsigned_integral Block, class Allocator> [[nodiscard]] constexpr auto cend   (const dynamic_bitset<Block, Allocator>& c) noexcept { return std::ranges::end(c);   }
template<std::unsigned_integral Block, class Allocator> [[nodiscard]] constexpr auto rbegin (      dynamic_bitset<Block, Allocator>& c) noexcept { return std::make_reverse_iterator(std::ranges::end(c)); }
template<std::unsigned_integral Block, class Allocator> [[nodiscard]] constexpr auto rbegin (const dynamic_bitset<Block, Allocator>& c) noexcept { return std::make_reverse_iterator(std::ranges::end(c)); }
template<std::unsigned_integral Block, class Allocator> [[nodiscard]] constexpr auto rend   (      dynamic_bitset<Block, Allocator>& c) noexcept { return std::make_reverse_iterator(std::ranges::begin(c)); }
template<std::unsigned_integral Block, class Allocator> [[nodiscard]] constexpr auto rend   (const dynamic_bitset<Block, Allocator>& c) noexcept { return std::make_reverse_iterator(std::ranges::begin(c)); }
template<std::unsigned_integral Block, class Allocator> [[nodiscard]] constexpr auto crbegin(const dynamic_bitset<Block, Allocator>& c) noexcept { return std::ranges::rbegin(c); }
template<std::unsigned_integral Block, class Allocator> [[nodiscard]] constexpr auto crend  (const dynamic_bitset<Block, Allocator>& c) noexcept { return std::ranges::rend(c);   }

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
