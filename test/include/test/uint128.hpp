//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef TEST_UINT128_HPP
#define TEST_UINT128_HPP

#include <xstd/ints/cstdint/int128.hpp> // IWYU pragma: export; uint128
#include <concepts>                     // unsigned_integral

// A built-in wide enough to name, a library mode that will own it, and a <bit> that will take it.
#if defined(__SIZEOF_INT128__) && !defined(__STRICT_ANSI__) && !defined(_MSC_VER)
#define TEST_HAS_UINT128
#endif

// Three terms for three facts, and the assert below holds the macro to the concept no #if can spell. [design.md#uint128-support]
namespace test {

#ifdef TEST_HAS_UINT128
inline constexpr bool has_uint128 = true;
#else
inline constexpr bool has_uint128 = false;
#endif

static_assert(has_uint128 == std::unsigned_integral<xstd::uint128>);

} // namespace test

#endif // TEST_UINT128_HPP
