#ifndef EXT_STD_BITSET_HPP
#define EXT_STD_BITSET_HPP

//          Copyright Rein Halbersma 2014-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/proxy.hpp>       // bidirectional_proxy_iterator
#include <algorithm>            // lexicographical_compare_three_way
#include <bitset>               // bitset
#include <cassert>              // assert
#include <compare>              // strong_ordering
#include <cstddef>              // size_t
#include <iterator>             // make_reverse_iterator
#include <ranges>               // begin, end, find_if, rbegin, rend
                                // iota, reverse

namespace std {

template<std::size_t N>
bitset<N>& operator-=(bitset<N>& lhs, bitset<N> const& rhs) noexcept
{
        return lhs &= ~rhs;
}

template<std::size_t N>
bitset<N> operator-(bitset<N> const& lhs, bitset<N> const& rhs) noexcept
{
        auto nrv = lhs; nrv -= rhs; return nrv;
}

template<std::size_t N>
bool is_subset_of(bitset<N> const& lhs, bitset<N> const& rhs) noexcept
{
        if constexpr (N == 0) {
                return true;
        } else {
                return (lhs & ~rhs).none();
        }
}

template<std::size_t N>
bool is_proper_subset_of(bitset<N> const& lhs, bitset<N> const& rhs) noexcept
{
        if constexpr (N == 0) {
                return false;
        } else {
                return is_subset_of(lhs, rhs) && lhs != rhs;
        }
}

template<std::size_t N>
bool intersects(bitset<N> const& lhs, bitset<N> const& rhs) noexcept
{
        if constexpr (N == 0) {
                return false;
        } else {
                return (lhs & rhs).any();
        }
}

template<std::size_t N>
[[nodiscard]] constexpr std::size_t find_first(bitset<N> const& c) noexcept
{
        if constexpr (N == 0) {
                return N;
        } else if constexpr (requires { c._Find_first(); }) {
                return c._Find_first();
        } else {
                return *std::ranges::find_if(std::views::iota(std::size_t(0), N), [&](auto i) {
                        return c[i];
                });
        }
}

template<std::size_t N>
[[nodiscard]] constexpr std::size_t find_next(bitset<N> const& c, std::size_t n) noexcept
{
        if constexpr (requires { c._Find_next(n); }) {
                return c._Find_next(n);
        } else {
                return *std::ranges::find_if(std::views::iota(n + 1, N), [&](auto i) {
                        return c[i];
                });
        }
}

template<std::size_t N>
[[nodiscard]] std::size_t find_prev(bitset<N> const& c, std::size_t n) noexcept
{
        assert(c.any());
        return *std::ranges::find_if(std::views::iota(std::size_t(0), n) | std::views::reverse, [&](auto i) {
                return c[i];
        });
}

// range access
template<std::size_t N>
[[nodiscard]] constexpr auto begin(bitset<N>& c) noexcept
        -> xstd::bidirectional_proxy_iterator<bitset<N>>
{
        return { &c, find_first(c) };
}

template<std::size_t N>
[[nodiscard]] constexpr auto begin(bitset<N> const& c) noexcept
        -> xstd::bidirectional_proxy_iterator<bitset<N>>
{
        return { &c, find_first(c) };
}

template<std::size_t N>
[[nodiscard]] constexpr auto end(bitset<N>& c) noexcept
        -> xstd::bidirectional_proxy_iterator<bitset<N>>
{
        return { &c, N };
}

template<std::size_t N>
[[nodiscard]] constexpr auto end(bitset<N> const& c) noexcept
        -> xstd::bidirectional_proxy_iterator<bitset<N>>
{
        return { &c, N };
}

template<std::size_t N>
[[nodiscard]] constexpr auto cbegin(bitset<N> const& c) noexcept
{
        return std::ranges::begin(c);
}

template<std::size_t N>
[[nodiscard]] constexpr auto cend(bitset<N> const& c) noexcept
{
        return std::ranges::end(c);
}

template<std::size_t N>
[[nodiscard]] constexpr auto rbegin(bitset<N>& c) noexcept
{
        return std::make_reverse_iterator(std::ranges::end(c));
}

template<std::size_t N>
[[nodiscard]] constexpr auto rbegin(bitset<N> const& c) noexcept
{
        return std::make_reverse_iterator(std::ranges::end(c));
}

template<std::size_t N>
[[nodiscard]] constexpr auto rend(bitset<N>& c) noexcept
{
        return std::make_reverse_iterator(std::ranges::begin(c));
}

template<std::size_t N>
[[nodiscard]] constexpr auto rend(bitset<N> const& c) noexcept
{
        return std::make_reverse_iterator(std::ranges::begin(c));
}

template<std::size_t N>
[[nodiscard]] constexpr auto crbegin(bitset<N> const& c) noexcept
{
        return std::ranges::rbegin(c);
}

template<std::size_t N>
[[nodiscard]] constexpr auto crend(bitset<N> const& c)  noexcept
{
        return std::ranges::rend(c);
}

template<std::size_t N>
[[nodiscard]] auto operator<=>(bitset<N> const& x, bitset<N> const& y) noexcept
        -> std::strong_ordering
{
        return std::lexicographical_compare_three_way(
                std::ranges::begin(x), std::ranges::end(x),
                std::ranges::begin(y), std::ranges::end(y)
        );
}

}       // namespace std

#endif  // include guard
