#ifndef EXT_STD_BITSET_HPP
#define EXT_STD_BITSET_HPP

//          Copyright Rein Halbersma 2014-2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/proxy.hpp>       // begin, end
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
[[nodiscard]] constexpr std::size_t find_first(const bitset<N>& c) noexcept
{
        if constexpr (N == 0) {
                return N;
        } else if constexpr (requires { c._Find_first(); }) {
                return c._Find_first();
        } else {
                return *std::ranges::find_if(std::views::iota(0uz, N), [&](auto i) {
                        return c[i];
                });
        }
}

template<std::size_t N>
[[nodiscard]] constexpr std::size_t find_last(const bitset<N>&) noexcept
{
        return N;
}

template<std::size_t N>
[[nodiscard]] constexpr std::size_t find_next(const bitset<N>& c, std::size_t n) noexcept
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
[[nodiscard]] std::size_t find_prev(const bitset<N>& c, std::size_t n) noexcept
{
        assert(c.any());
        return *std::ranges::find_if(std::views::iota(0uz, n) | std::views::reverse, [&](auto i) {
                return c[i];
        });
}

template<std::size_t N> [[nodiscard]] constexpr auto begin  (      bitset<N>& c) noexcept { return xstd::proxy::bidirectional::begin(c); }
template<std::size_t N> [[nodiscard]] constexpr auto begin  (const bitset<N>& c) noexcept { return xstd::proxy::bidirectional::begin(c); }
template<std::size_t N> [[nodiscard]] constexpr auto end    (      bitset<N>& c) noexcept { return xstd::proxy::bidirectional::end(c); }
template<std::size_t N> [[nodiscard]] constexpr auto end    (const bitset<N>& c) noexcept { return xstd::proxy::bidirectional::end(c); }
template<std::size_t N> [[nodiscard]] constexpr auto cbegin (const bitset<N>& c) noexcept { return std::ranges::begin(c); }
template<std::size_t N> [[nodiscard]] constexpr auto cend   (const bitset<N>& c) noexcept { return std::ranges::end(c);   }
template<std::size_t N> [[nodiscard]] constexpr auto rbegin (      bitset<N>& c) noexcept { return std::make_reverse_iterator(std::ranges::end(c)); }
template<std::size_t N> [[nodiscard]] constexpr auto rbegin (const bitset<N>& c) noexcept { return std::make_reverse_iterator(std::ranges::end(c)); }
template<std::size_t N> [[nodiscard]] constexpr auto rend   (      bitset<N>& c) noexcept { return std::make_reverse_iterator(std::ranges::begin(c)); }
template<std::size_t N> [[nodiscard]] constexpr auto rend   (const bitset<N>& c) noexcept { return std::make_reverse_iterator(std::ranges::begin(c)); }
template<std::size_t N> [[nodiscard]] constexpr auto crbegin(const bitset<N>& c) noexcept { return std::ranges::rbegin(c); }
template<std::size_t N> [[nodiscard]] constexpr auto crend  (const bitset<N>& c) noexcept { return std::ranges::rend(c);   }

template<std::size_t N>
[[nodiscard]] auto operator<=>(const bitset<N>& x, const bitset<N>& y) noexcept
        -> std::strong_ordering
{
        return std::lexicographical_compare_three_way(
                std::ranges::begin(x), std::ranges::end(x),
                std::ranges::begin(y), std::ranges::end(y)
        );
}

template<std::size_t N>
bitset<N>& operator-=(bitset<N>& lhs, const bitset<N>& rhs) noexcept
{
        return lhs &= ~rhs;
}

template<std::size_t N>
bitset<N> operator-(const bitset<N>& lhs, const bitset<N>& rhs) noexcept
{
        auto nrv = lhs; nrv -= rhs; return nrv;
}

template<std::size_t N>
bool is_subset_of(const bitset<N>& lhs, const bitset<N>& rhs) noexcept
{
        if constexpr (N == 0) {
                return true;
        } else {
                return (lhs & ~rhs).none();
        }
}

template<std::size_t N>
bool is_proper_subset_of(const bitset<N>& lhs, const bitset<N>& rhs) noexcept
{
        if constexpr (N == 0) {
                return false;
        } else {
                return is_subset_of(lhs, rhs) and lhs != rhs;
        }
}

template<std::size_t N>
bool intersects(const bitset<N>& lhs, const bitset<N>& rhs) noexcept
{
        if constexpr (N == 0) {
                return false;
        } else {
                return (lhs & rhs).any();
        }
}

}       // namespace std

#endif  // include guard
