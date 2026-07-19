#ifndef EXT_STD_BITSET_HPP
#define EXT_STD_BITSET_HPP

//          Copyright Rein Halbersma 2014-2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/proxy/bidirectional.hpp> // find, view
#include <algorithm>                    // lexicographical_compare_three_way
#include <bitset>                       // bitset
#include <cassert>                      // assert
#include <compare>                      // strong_ordering
#include <cstddef>                      // size_t
#include <ranges>                       // find_if
                                        // iota

// std::bitset<N>'s only associated namespace is std, where a program may
// not add declarations [namespace.std] - so its find_first/find_last/
// find_next/find_prev customizations can't legally live where ADL would
// find them. Specializing xstd::proxy::bidirectional::find here instead
// works regardless: unlike ADL, template specialization matching considers
// specializations visible before the point of use, wherever declared.
namespace xstd::proxy::bidirectional {

template<std::size_t N>
struct find<std::bitset<N>>
{
        [[nodiscard]] static constexpr std::size_t first(const std::bitset<N>& c) noexcept
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

        [[nodiscard]] static constexpr std::size_t last(const std::bitset<N>&) noexcept
        {
                return N;
        }

        [[nodiscard]] static constexpr std::size_t next(const std::bitset<N>& c, std::size_t n) noexcept
        {
                if constexpr (requires { c._Find_next(n); }) {
                        return c._Find_next(n);
                } else {
                        return *std::ranges::find_if(std::views::iota(n + 1, N), [&](auto i) {
                                return c[i];
                        });
                }
        }

        [[nodiscard]] static std::size_t prev(const std::bitset<N>& c, std::size_t n) noexcept
        {
                assert(c.any());
                return *std::ranges::find_if(std::views::iota(0uz, n) | std::views::reverse, [&](auto i) {
                        return c[i];
                });
        }
};

}       // namespace xstd::proxy::bidirectional

// TODO: everything below is still declared in namespace std, which has the
// exact same [namespace.std] problem the find<std::bitset<N>> specialization
// above solves - natural infix syntax (x <=> y, x - y) is only found via ADL
// or membership in std::bitset<N>'s own namespace (std), and there is no
// legal way to provide it from outside namespace std. Left as-is (updated
// only to stop depending on the removed std::begin/std::end overloads)
// pending a decision on whether to keep this as documented, deliberate UB,
// drop it, or convert call sites to explicit non-operator syntax.
namespace std {

template<std::size_t N>
[[nodiscard]] auto operator<=>(const bitset<N>& x, const bitset<N>& y) noexcept
        -> std::strong_ordering
{
        auto const xv = xstd::proxy::bidirectional::view(x);
        auto const yv = xstd::proxy::bidirectional::view(y);
        return std::lexicographical_compare_three_way(
                xv.begin(), xv.end(),
                yv.begin(), yv.end()
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
