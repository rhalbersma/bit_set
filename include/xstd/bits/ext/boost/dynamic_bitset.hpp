//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_BITS_EXT_BOOST_DYNAMIC_BITSET_HPP
#define XSTD_BITS_EXT_BOOST_DYNAMIC_BITSET_HPP

#include <xstd/ints/concepts/unsigned_integer.hpp> // unsigned_integer
#include <xstd/bits/ranges/set_view.hpp> // find, view
#include <xstd/bits/ranges/array_view.hpp> // find, view
#include <boost/dynamic_bitset.hpp>     // dynamic_bitset
#include <algorithm>                    // lexicographical_compare_three_way
#include <cassert>                      // assert
#include <compare>                      // strong_ordering
#include <cstddef>                      // std::size_t
#include <ranges>                       // find_if, min
                                        // iota, reverse

// boost::dynamic_bitset<> now provides its own member begin()/end() (added
// upstream after this ADL customization was originally written), which
// always take priority over an ADL free function of the same name in
// ordinary range-for and std::ranges algorithms alike - so an ADL
// customization declared in namespace boost is liable to silently stop
// being used the moment Boost adds a same-named member. Specializing
// xstd::ranges::set_find here instead avoids that: unlike ADL,
// template specialization matching considers specializations visible
// before the point of use, wherever declared, and nothing
// boost::dynamic_bitset<> adds to its own namespace or its own members can
// shadow it.
namespace xstd::ranges {

template<xstd::unsigned_integer Block, class Allocator>
struct set_find<boost::dynamic_bitset<Block, Allocator>>
{
        [[nodiscard]] static constexpr std::size_t first(boost::dynamic_bitset<Block, Allocator> const& c) noexcept
        {
                return c.find_first();
        }

        [[nodiscard]] static constexpr std::size_t last(boost::dynamic_bitset<Block, Allocator> const& c) noexcept
        {
                return c.npos;
        }

        [[nodiscard]] static constexpr std::size_t next(boost::dynamic_bitset<Block, Allocator> const& c, std::size_t n) noexcept
        {
                return c.find_next(n);
        }

        [[nodiscard]] static constexpr std::size_t prev(boost::dynamic_bitset<Block, Allocator> const& c, std::size_t n) noexcept
        {
                assert(c.any());
                return *std::ranges::find_if(std::views::iota(0UZ, std::ranges::min(n, c.size())) | std::views::reverse, [&](auto i) {
                        return c[i];
                });
        }
};

// boost::dynamic_bitset<> may add its own <=> upstream at some point (as it
// already did for begin()/end() - see set_find<> above), with no guarantee its
// semantics would match std::set<int>'s ordering (the same concern as
// std::bitset<N> - see set_set_compare<Bits>'s primary template). Opt in to the
// safe, iteration-based ordering explicitly rather than risk inheriting
// whatever a future native <=> decides to mean. This is what
// set_view<boost::dynamic_bitset<...>>::operator<=> uses; unlike set_find<>, this
// isn't reachable via infix x <=> y - boost::dynamic_bitset<> is a real
// (non-std) namespace so an ADL operator<=> here would be legal, but
// ordering isn't unambiguous enough to be worth adding one - use
// set_view(x) <=> set_view(y).
template<xstd::unsigned_integer Block, class Allocator>
struct set_compare<boost::dynamic_bitset<Block, Allocator>>
{
        [[nodiscard]] static constexpr std::strong_ordering lexicographical_three_way(boost::dynamic_bitset<Block, Allocator> const& x, boost::dynamic_bitset<Block, Allocator> const& y) noexcept
        {
                auto const xv = set_view(x);
                auto const yv = set_view(y);
                return std::lexicographical_compare_three_way(
                        xv.begin(), xv.end(),
                        yv.begin(), yv.end()
                );
        }
};

}       // namespace xstd::ranges

// Same reasoning as set_find above, for the array-of-bool
// interpretation: boost::dynamic_bitset<> already has operator[] for every
// index, so this specialization is trivial - it exists purely so a future
// upstream begin()/end()/at() addition can't shadow it, same as set_find<>
// above.
namespace xstd::ranges {

template<xstd::unsigned_integer Block, class Allocator>
struct array_find<boost::dynamic_bitset<Block, Allocator>>
{
        [[nodiscard]] static constexpr std::size_t first(boost::dynamic_bitset<Block, Allocator> const&) noexcept { return 0UZ; }
        [[nodiscard]] static constexpr std::size_t last (boost::dynamic_bitset<Block, Allocator> const& c) noexcept { return c.size(); }
        [[nodiscard]] static constexpr bool         at  (boost::dynamic_bitset<Block, Allocator> const& c, std::size_t n) noexcept { return c[n]; }
};


}       // namespace xstd::ranges

#endif // XSTD_BITS_EXT_BOOST_DYNAMIC_BITSET_HPP
