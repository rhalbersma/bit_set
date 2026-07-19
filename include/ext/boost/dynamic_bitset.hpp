#pragma once

//          Copyright Rein Halbersma 2014-2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/proxy/bidirectional.hpp> // find, view
#include <boost/dynamic_bitset.hpp>     // dynamic_bitset
#include <algorithm>                    // lexicographical_compare_three_way
#include <cassert>                      // assert
#include <compare>                      // strong_ordering
#include <concepts>                     // unsigned_integral
#include <cstddef>                      // std::size_t
#include <ranges>                       // find_if, min
                                        // iota, reverse

// boost::dynamic_bitset<> now provides its own member begin()/end() (added
// upstream after this ADL customization was originally written), which
// always take priority over an ADL free function of the same name in
// ordinary range-for and std::ranges algorithms alike - so an ADL
// customization declared in namespace boost is liable to silently stop
// being used the moment Boost adds a same-named member. Specializing
// xstd::proxy::bidirectional::find here instead avoids that: unlike ADL,
// template specialization matching considers specializations visible
// before the point of use, wherever declared, and nothing
// boost::dynamic_bitset<> adds to its own namespace or its own members can
// shadow it.
namespace xstd::proxy::bidirectional {

template<std::unsigned_integral Block, class Allocator>
struct find<boost::dynamic_bitset<Block, Allocator>>
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
                return *std::ranges::find_if(std::views::iota(0uz, std::ranges::min(n, c.size())) | std::views::reverse, [&](auto i) {
                        return c[i];
                });
        }
};

}       // namespace xstd::proxy::bidirectional

namespace boost {

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto operator<=>(dynamic_bitset<Block, Allocator> const& x, dynamic_bitset<Block, Allocator> const& y) noexcept
        -> std::strong_ordering
{
        auto const xv = xstd::proxy::bidirectional::view(x);
        auto const yv = xstd::proxy::bidirectional::view(y);
        return std::lexicographical_compare_three_way(
                xv.begin(), xv.end(),
                yv.begin(), yv.end()
        );
}

}       // namespace boost
