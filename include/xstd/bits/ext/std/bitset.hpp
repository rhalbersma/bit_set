//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_BITS_EXT_STD_BITSET_HPP
#define XSTD_BITS_EXT_STD_BITSET_HPP

#include <xstd/bits/ranges/block_access.hpp> // block_access
#include <xstd/bits/ranges/set_view.hpp> // find, view
#include <xstd/bits/ranges/array_view.hpp> // find, view
#include <algorithm>                    // find_if
#include <bitset>                       // bitset
#include <cassert>                      // assert
#include <compare>                      // strong_ordering
#include <cstddef>                      // size_t
#include <limits>                       // numeric_limits
#include <utility>                      // declval
#include <ranges>                       // find_if
                                        // iota

// std::bitset<N>'s only associated namespace is std, where a program may
// not add declarations [namespace.std] - so its find_first/find_last/
// find_next/find_prev customizations can't legally live where ADL would
// find them. Specializing xstd::ranges::set_find here instead
// works regardless: unlike ADL, template specialization matching considers
// specializations visible before the point of use, wherever declared.
namespace xstd::ranges {

// The width is in the type, so set_view's max_size() and array_view's size() are
// constant expressions over a std::bitset<N>.
template<std::size_t N>
inline constexpr std::size_t bit_extent<std::bitset<N>> = N;

template<std::size_t N>
struct set_find<std::bitset<N>>
{
        [[nodiscard]] static constexpr std::size_t first(const std::bitset<N>& c) noexcept
        {
                if constexpr (N == 0) {
                        return N;
                } else if constexpr (requires { c._Find_first(); }) {
                        return c._Find_first();
                } else {
                        return *std::ranges::find_if(std::views::iota(0UZ, N), [&](auto i) {
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
                return *std::ranges::find_if(std::views::iota(0UZ, n) | std::views::reverse, [&](auto i) {
                        return c[i];
                });
        }
};

// std::bitset<N> has no <=> of its own, so xstd::ranges::set_set_compare<Bits>'s
// default (trust Bits' own <=>) doesn't apply - it must opt
// in to the safe, iteration-based ordering explicitly. This is what
// set_view<std::bitset<N>>::operator<=> uses; there is no infix x <=> y for
// std::bitset<N> itself (see the comment below on why that isn't added).
template<std::size_t N>
struct set_compare<std::bitset<N>>
{
        [[nodiscard]] static constexpr std::strong_ordering lexicographical_three_way(std::bitset<N> const& x, std::bitset<N> const& y) noexcept
        {
                return set_three_way(set_view(x), set_view(y));
        }
};

}       // namespace xstd::ranges

// Same [namespace.std] situation as set_find above, for the
// array-of-bool interpretation instead of the set-of-indices one:
// std::bitset<N> already has operator[] for every index (unlike find_first/
// find_next, which need real bit-scanning), so this specialization is
// trivial - the whole point is just making it reachable without adding
// declarations to namespace std.
namespace xstd::ranges {

// The Microsoft STL publishes its words through a nonstandard but public
// _Getword, so a std::bitset there can be ordered a word at a time like our own
// types. libstdc++ keeps its words in _Base_bitset, which std::bitset inherits
// privately, and exposes only _Find_first/_Find_next; libc++ exposes neither. So
// this is a specialization guarded on the member rather than on the platform:
// where it is absent the primary template leaves block_range unsatisfied and the
// element-wise path stands.
//
// The layout matches what block_access documents -- ascending words, and the
// least significant bit of a word is its lowest position -- and std::bitset must
// keep the padding above N zero for count() and all() to work, which is the
// other thing comparing whole words relies on.
template<std::size_t N>
        requires requires (std::bitset<N> const& c) { { c._Getword(0UZ) } -> xstd::unsigned_integer; }
struct block_access<std::bitset<N>>
{
        using block_type = decltype(std::declval<std::bitset<N> const&>()._Getword(0UZ));

        static constexpr auto digits = static_cast<std::size_t>(std::numeric_limits<block_type>::digits);

        [[nodiscard]] static constexpr std::size_t num_blocks(std::bitset<N> const&) noexcept
        {
                return N == 0UZ ? 1UZ : (N + digits - 1UZ) / digits;
        }

        [[nodiscard]] static constexpr block_type block(std::bitset<N> const& c, std::size_t i) noexcept
        {
                return c._Getword(i);
        }
};

template<std::size_t N>
struct array_find<std::bitset<N>>
{
        [[nodiscard]] static constexpr std::size_t first(const std::bitset<N>&) noexcept { return 0UZ; }
        [[nodiscard]] static constexpr std::size_t last (const std::bitset<N>&) noexcept { return N;   }
        [[nodiscard]] static constexpr bool         at  (const std::bitset<N>& c, std::size_t n) noexcept { return c[n]; }
};


}       // namespace xstd::ranges

// is_subset_of, is_proper_subset_of, intersects, and now <=> used to live
// here too, with the same [namespace.std] problem set_find<std::bitset<N>>
// above solves. None of them need to any more: xstd::set_view provides all
// four directly (preferring a Bits type's own member/<=>
// when it has one, falling back to computing them from iteration otherwise
// - see compare<std::bitset<N>> above for <=> specifically), so
// `set_view(x).is_subset_of(set_view(y))`, `set_view(x) <=> set_view(y)`, etc. work for
// std::bitset<N> without adding anything to namespace std at all. There is
// deliberately no infix x <=> y for std::bitset<N> itself: unlike ==,
// ordering isn't unambiguous enough to be worth resurrecting a [namespace.
// std] exception for - use set_view(x) <=> set_view(y).
//
// operator-=, and operator- remain here: natural infix syntax for them is
// only reachable via ADL or membership in std::bitset<N>'s own namespace
// (std), and there is no legal way to provide that from outside namespace
// std.
namespace std {

// NOLINTBEGIN(bugprone-std-namespace-modification)

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

// NOLINTEND(bugprone-std-namespace-modification)

}       // namespace std

#endif // XSTD_BITS_EXT_STD_BITSET_HPP
