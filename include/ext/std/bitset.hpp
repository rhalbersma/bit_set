#ifndef EXT_STD_BITSET_HPP
#define EXT_STD_BITSET_HPP

//          Copyright Rein Halbersma 2014-2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/proxy/bidirectional.hpp> // find, view
#include <xstd/proxy/random_access.hpp> // find, view
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

// std::bitset<N> has no <=> of its own, so xstd::proxy::bidirectional::
// compare<Bits>'s default (trust Bits' own <=>) doesn't apply - it must opt
// in to the safe, iteration-based ordering explicitly. This is what
// view<std::bitset<N>>::operator<=> uses; there is no infix x <=> y for
// std::bitset<N> itself (see the comment below on why that isn't added).
template<std::size_t N>
struct compare<std::bitset<N>>
{
        [[nodiscard]] static constexpr std::strong_ordering lexicographical_three_way(std::bitset<N> const& x, std::bitset<N> const& y) noexcept
        {
                auto const xv = view(x);
                auto const yv = view(y);
                return std::lexicographical_compare_three_way(
                        xv.begin(), xv.end(),
                        yv.begin(), yv.end()
                );
        }
};

}       // namespace xstd::proxy::bidirectional

// Same [namespace.std] situation as bidirectional::find above, for the
// array-of-bool interpretation instead of the set-of-indices one:
// std::bitset<N> already has operator[] for every index (unlike find_first/
// find_next, which need real bit-scanning), so this specialization is
// trivial - the whole point is just making it reachable without adding
// declarations to namespace std.
namespace xstd::proxy::random_access {

template<std::size_t N>
struct find<std::bitset<N>>
{
        [[nodiscard]] static constexpr std::size_t first(const std::bitset<N>&) noexcept { return 0uz; }
        [[nodiscard]] static constexpr std::size_t last (const std::bitset<N>&) noexcept { return N;   }
        [[nodiscard]] static constexpr bool         at  (const std::bitset<N>& c, std::size_t n) noexcept { return c[n]; }
};

// std::bitset<N> has no <=> at all (real std::bitset has only ==), so
// compare<Bits>'s default (trust Bits' own <=>) can't apply here either -
// same opt-in as bidirectional::compare<std::bitset<N>> above, just
// producing the fixed-length sequence-of-bool order (index 0 first)
// instead of the set-of-indices one.
template<std::size_t N>
struct compare<std::bitset<N>>
{
        [[nodiscard]] static constexpr std::strong_ordering lexicographical_three_way(std::bitset<N> const& x, std::bitset<N> const& y) noexcept
        {
                auto const xv = view(x);
                auto const yv = view(y);
                return std::lexicographical_compare_three_way(
                        xv.begin(), xv.end(),
                        yv.begin(), yv.end()
                );
        }
};

}       // namespace xstd::proxy::random_access

// is_subset_of, is_proper_subset_of, intersects, and now <=> used to live
// here too, with the same [namespace.std] problem find<std::bitset<N>>
// above solves. None of them need to any more: xstd::proxy::bidirectional::
// view provides all four directly (preferring a Bits type's own member/<=>
// when it has one, falling back to computing them from iteration otherwise
// - see compare<std::bitset<N>> above for <=> specifically), so
// `view(x).is_subset_of(view(y))`, `view(x) <=> view(y)`, etc. work for
// std::bitset<N> without adding anything to namespace std at all. There is
// deliberately no infix x <=> y for std::bitset<N> itself: unlike ==,
// ordering isn't unambiguous enough to be worth resurrecting a [namespace.
// std] exception for - use view(x) <=> view(y).
//
// operator-=, and operator- remain here: natural infix syntax for them is
// only reachable via ADL or membership in std::bitset<N>'s own namespace
// (std), and there is no legal way to provide that from outside namespace
// std.
namespace std {

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

}       // namespace std

#endif  // include guard
