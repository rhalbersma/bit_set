//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_BITS_EXT_BOOST_DYNAMIC_BITSET_HPP
#define XSTD_BITS_EXT_BOOST_DYNAMIC_BITSET_HPP

// IWYU pragma: always_keep

#include <boost/dynamic_bitset.hpp>                // IWYU pragma: export; dynamic_bitset
#include <xstd/bits/ranges/array_view.hpp>         // find, view
#include <xstd/bits/ranges/set_view.hpp>           // find, view
#include <xstd/ints/concepts/unsigned_integer.hpp> // unsigned_integer
#include <algorithm>                               // find_if, min
#include <cassert>                                 // assert
#include <compare>                                 // strong_ordering
#include <cstddef>                                 // std::size_t
#include <ranges>                                  // find_if, min
                                        // iota, reverse

// Specializing set_find rather than using ADL: dynamic_bitset's own members would silently shadow a same-named free function.
namespace xstd::ranges {

template<xstd::unsigned_integer Block, class Allocator>
struct set_find<boost::dynamic_bitset<Block, Allocator>>
{
        [[nodiscard]] static constexpr auto first(boost::dynamic_bitset<Block, Allocator> const& c) noexcept
                -> std::size_t
        {
                return c.find_first();
        }

        [[nodiscard]] static constexpr auto last(boost::dynamic_bitset<Block, Allocator> const& c) noexcept
                -> std::size_t
        {
                return c.npos;
        }

        [[nodiscard]] static constexpr auto next(boost::dynamic_bitset<Block, Allocator> const& c, std::size_t n) noexcept
                -> std::size_t
        {
                return c.find_next(n);
        }

        [[nodiscard]] static constexpr auto prev(boost::dynamic_bitset<Block, Allocator> const& c, std::size_t n) noexcept
                -> std::size_t
        {
                assert(c.any());
                return *std::ranges::find_if(std::views::iota(0UZ, std::ranges::min(n, c.size())) | std::views::reverse, [&](auto i) {
                        return c[i];
                });
        }
};

// Opt in to the iteration-based ordering rather than risk inheriting whatever a future native <=> decides to mean.
template<xstd::unsigned_integer Block, class Allocator>
struct set_compare<boost::dynamic_bitset<Block, Allocator>>
{
        [[nodiscard]] static constexpr auto lexicographical_three_way(boost::dynamic_bitset<Block, Allocator> const& x, boost::dynamic_bitset<Block, Allocator> const& y) noexcept
                -> std::strong_ordering
        {
                return set_three_way(set_view(x), set_view(y));
        }
};

}       // namespace xstd::ranges

// Same reasoning as set_find above, for the array-of-bool reading: trivial, and unshadowable by a future upstream member.
namespace xstd::ranges {

template<xstd::unsigned_integer Block, class Allocator>
struct array_find<boost::dynamic_bitset<Block, Allocator>>
{
        [[nodiscard]] static constexpr auto first(boost::dynamic_bitset<Block, Allocator> const&) noexcept -> std::size_t { return 0UZ; }
        [[nodiscard]] static constexpr auto last (boost::dynamic_bitset<Block, Allocator> const& c) noexcept -> std::size_t { return c.size(); }
        [[nodiscard]] static constexpr auto at   (boost::dynamic_bitset<Block, Allocator> const& c, std::size_t n) noexcept -> bool { return c[n]; }
};


}       // namespace xstd::ranges

#endif // XSTD_BITS_EXT_BOOST_DYNAMIC_BITSET_HPP
