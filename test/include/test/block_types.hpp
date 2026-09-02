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

// The Block models and the extents worth instantiating, assembled once: all three containers take <size_t N, class Block>.
namespace test {

template<class Block>
inline constexpr auto digits_v = static_cast<std::size_t>(xstd::numeric_limits<Block>::digits);

// Every Block the library is instantiated over; __uint128_t needs GNU extensions, <bit> rejecting it under -std=c++23.
using word_types = std::tuple
<       std::uint8_t
,       std::uint16_t
,       std::uint32_t
,       std::uint64_t
#if defined(__GNUG__)
,       __uint128_t
#endif
>;

// The Blocks narrow enough to straddle block boundaries exhaustively; a bit-precise unsigned _BitInt(4) belongs here.
using narrow_word_types = std::tuple
<       std::uint8_t
>;

// One block's worth of extents: empty, a single bit, and exactly one full block -- the same cost at any width.
template<template<std::size_t, class> class C, class Block>
using in_block_extents = std::tuple
<       C<0, Block>
,       C<1, Block>
,       C<digits_v<Block>, Block>
>;

// The extents that straddle a block boundary, at the narrowest word only: the arithmetic follows digits, not the carrier.
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

// Every word type at the extents it can afford: all within one block, and the narrow ones across boundaries too.
template<template<std::size_t, class> class C>
using graded_extents = decltype(std::tuple_cat(
        std::declval<decltype(detail::expand<C, in_block_extents>(std::declval<word_types>()))>(),
        std::declval<decltype(detail::expand<C, straddling_extents>(std::declval<narrow_word_types>()))>()
));

} // namespace test

#endif // TEST_BLOCK_TYPES_HPP
