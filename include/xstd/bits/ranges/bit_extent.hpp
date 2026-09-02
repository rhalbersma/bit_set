//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_BITS_RANGES_BIT_EXTENT_HPP
#define XSTD_BITS_RANGES_BIT_EXTENT_HPP

#include <cstddef> // size_t
#include <span>    // dynamic_extent

namespace xstd::ranges {

// How many positions a bit sequence has, settled by the type rather than by the caller, and static where the width is in the type.
template<class Bits>
inline constexpr std::size_t bit_extent = std::dynamic_extent;

template<class Bits>
concept static_bit_extent = bit_extent<Bits> != std::dynamic_extent;

} // namespace xstd::ranges

#endif // XSTD_BITS_RANGES_BIT_EXTENT_HPP
