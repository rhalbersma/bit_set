//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_BITS_RANGES_BIT_EXTENT_HPP
#define XSTD_BITS_RANGES_BIT_EXTENT_HPP

#include <cstddef> // size_t
#include <span>    // dynamic_extent

namespace xstd::ranges {

// How many positions a bit sequence has, where the type itself settles it.
//
// Both views need this and neither should ask the user for it: std::bitset<N>
// and xstd::bitset<N, Block> carry their width in the type, boost::dynamic_bitset<>
// carries it in the object, and which of the two a Bits is, is the Bits' business.
// So the extent is a property of the type, specialized beside that type's other
// hooks -- see xstd/bits/ext/std/bitset.hpp and its neighbours.
//
// A static extent makes array_view<Bits>::size() and set_view<Bits>::max_size()
// constant expressions; dynamic_extent sends both to the object at run time.
template<class Bits>
inline constexpr std::size_t bit_extent = std::dynamic_extent;

template<class Bits>
concept static_bit_extent = bit_extent<Bits> != std::dynamic_extent;

} // namespace xstd::ranges

#endif // XSTD_BITS_RANGES_BIT_EXTENT_HPP
