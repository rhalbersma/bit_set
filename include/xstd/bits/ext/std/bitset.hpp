//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_BITS_EXT_STD_BITSET_HPP
#define XSTD_BITS_EXT_STD_BITSET_HPP

#include <xstd/bits/ranges/array_view.hpp>   // find, view
#include <xstd/bits/ranges/block_access.hpp> // block_access
#include <xstd/bits/ranges/set_view.hpp>     // find, view
#include <algorithm>                         // find_if
#include <bitset>                            // bitset
#include <cassert>                           // assert
#include <compare>                           // strong_ordering
#include <cstddef>                           // size_t
#include <limits>                            // numeric_limits
#include <ranges>                            // find_if
#include <utility>                           // declval
                                        // iota

// std::bitset's only associated namespace is std, where [namespace.std] forbids adding ADL hooks, so specialize set_find instead.
namespace xstd::ranges {

// The width is in the type, so set_view's max_size() and array_view's size() are constant expressions.
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

// std::bitset has no <=> of its own, so opt in to the iteration-based ordering explicitly.
template<std::size_t N>
struct set_compare<std::bitset<N>>
{
        [[nodiscard]] static constexpr std::strong_ordering lexicographical_three_way(std::bitset<N> const& x, std::bitset<N> const& y) noexcept
        {
                return set_three_way(set_view(x), set_view(y));
        }
};

}       // namespace xstd::ranges

// Same [namespace.std] situation as set_find, for the array-of-bool reading; trivial, since operator[] answers every index.
namespace xstd::ranges {

// Guarded on _Getword rather than on the platform: where it is absent, block_range goes unsatisfied and the element-wise path stands.
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

// is_subset_of, is_proper_subset_of, intersects and <=> now come from set_view; operator-= and operator- stay, reachable only from std.
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
