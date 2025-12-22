#ifndef EXT_STD_BITSET_HPP
#define EXT_STD_BITSET_HPP

//          Copyright Rein Halbersma 2014-2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/proxy/forward.hpp>       // begin, end
#include <algorithm>                    // lexicographical_compare_three_way
#include <bitset>                       // bitset
#include <cassert>                      // assert
#include <compare>                      // strong_ordering
#include <cstddef>                      // size_t
#include <ranges>                       // begin, end

namespace std {

template<std::size_t N>
[[nodiscard]] constexpr std::size_t find_first(const bitset<N>& c) noexcept
        requires (N == 0) or (requires { c._Find_first(); })
{
        if constexpr (N == 0) {
                return N;
        } else {
                return c._Find_first();
        }
}

template<std::size_t N>
[[nodiscard]] constexpr std::size_t find_last(const bitset<N>&) noexcept
{
        return N;
}

template<std::size_t N>
[[nodiscard]] constexpr std::size_t find_next(const bitset<N>& c, std::size_t n) noexcept
        requires requires { c._Find_next(n); }
{
        return c._Find_next(n);
}

template<std::size_t N> [[nodiscard]] constexpr auto begin (      bitset<N>& c) noexcept { return xstd::proxy::forward::begin(c); }
template<std::size_t N> [[nodiscard]] constexpr auto begin (const bitset<N>& c) noexcept { return xstd::proxy::forward::begin(c); }
template<std::size_t N> [[nodiscard]] constexpr auto end   (      bitset<N>& c) noexcept { return xstd::proxy::forward::end(c); }
template<std::size_t N> [[nodiscard]] constexpr auto end   (const bitset<N>& c) noexcept { return xstd::proxy::forward::end(c); }
template<std::size_t N> [[nodiscard]] constexpr auto cbegin(const bitset<N>& c) noexcept { return std::ranges::begin(c); }
template<std::size_t N> [[nodiscard]] constexpr auto cend  (const bitset<N>& c) noexcept { return std::ranges::end  (c); }

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
