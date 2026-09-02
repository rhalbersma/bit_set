//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef TEST_BLOCK_TYPES_HPP
#define TEST_BLOCK_TYPES_HPP

#include <xstd/ints/limits.hpp> // numeric_limits
#include <cstddef>              // size_t
#include <cstdint>              // uint8_t, uint16_t, uint32_t, uint64_t
#include <tuple>                // tuple, tuple_cat
#include <utility>              // declval

// The Block models a packed container is instantiated over, and the extents each
// one is worth instantiating at. Assembled here rather than spelled out per
// container: bit_array, bit_finite_set and xstd::bitset all take <size_t N,
// class Block>, so one list serves all three and widens in one place.
namespace test {

template<class Block>
inline constexpr auto digits_v = static_cast<std::size_t>(numeric_limits<Block>::digits);

// Every Block the library is instantiated over.
//
// __uint128_t is behind __GNUG__ because MSVC has no such type. It is also, less
// visibly, behind GNU extensions: libstdc++ constrains std::popcount and friends
// to the standard unsigned integer types, so <bit> rejects __uint128_t under
// -std=c++23 and accepts it under -std=gnu++23. CMake's CXX_EXTENSIONS defaults
// ON, which is the only reason these rows compile. An xstd::popcount over
// xstd::unsigned_integer would remove that dependency, and is what the models
// this list does not yet name -- MSVC's 128-bit type, boost::uint128,
// absl::uint128, unsigned _BitInt(N) -- are waiting on.
using word_types = std::tuple
<       std::uint8_t
,       std::uint16_t
,       std::uint32_t
,       std::uint64_t
#if defined(__GNUG__)
,       __uint128_t
#endif
>;

// The Blocks narrow enough to straddle block boundaries exhaustively. Widening
// this is the point of the list above: a bit-precise unsigned _BitInt(4) belongs
// here rather than beside uint64_t, being cheaper to straddle than uint8_t is.
using narrow_word_types = std::tuple
<       std::uint8_t
>;

// One block's worth of extents: empty, a single bit, and exactly one full block.
// This is what a word type costs to cover, and it costs the same whatever the
// word's width -- which is why every word gets it.
template<template<std::size_t, class> class C, class Block>
using in_block_extents = std::tuple
<       C<0, Block>
,       C<1, Block>
,       C<digits_v<Block>, Block>
>;

// The extents that straddle a block boundary: one short of a block, exactly a
// block, one past it, and the same again at the second and third. Block-boundary
// arithmetic is a function of digits, not of which type carries them, so
// straddling at the narrowest word exercises the same paths as at the widest for
// a fraction of the N -- and N is what these sweeps are polynomial in.
// 0, 1 and one full block are left to in_block_extents above, which every word
// already gets; repeating them here would instantiate the narrow words twice.
template<template<std::size_t, class> class C, class Block>
using straddling_extents = std::tuple
<       C<    digits_v<Block> - 1, Block>
,       C<    digits_v<Block> + 1, Block>
,       C<2 * digits_v<Block> - 1, Block>
,       C<2 * digits_v<Block>,     Block>
,       C<2 * digits_v<Block> + 1, Block>
,       C<3 * digits_v<Block>,     Block>
>;

namespace detail {

template<template<std::size_t, class> class C, template<template<std::size_t, class> class, class> class Extents, class... Blocks>
auto expand(std::tuple<Blocks...>) -> decltype(std::tuple_cat(std::declval<Extents<C, Blocks>>()...));

} // namespace detail

// Every word type at the extents it can afford: all of them within one block,
// and the narrow ones across block boundaries as well.
template<template<std::size_t, class> class C>
using graded_extents = decltype(std::tuple_cat(
        std::declval<decltype(detail::expand<C, in_block_extents>(std::declval<word_types>()))>(),
        std::declval<decltype(detail::expand<C, straddling_extents>(std::declval<narrow_word_types>()))>()
));

} // namespace test

#endif // TEST_BLOCK_TYPES_HPP
