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

// xstd::uint128 names a type on every compiler the matrix runs, but the library can only
// carry it where <bit> will: detail::bits::intrin forwards countl_zero, countr_zero and
// popcount straight through, and those take std::unsigned_integral alone. That is three
// separate facts, and the three terms above are them in order. GCC and Clang have the
// built-in; libstdc++ and libc++ hand it the numeric_limits specialization that carries
// it into the concept only outside __STRICT_ANSI__, which is why the matrix compiles as
// gnu++23; and the Microsoft STL's std::_Unsigned128 is a class type, so <bit> declines
// it whatever the mode -- that block waits on an xstd::countl_zero, not on anything here.
//
// The condition worth testing is the concept, which no #if can spell, so the assert below
// holds the macro to it in both directions. The day that seam grows its own implementation,
// or a new pairing lands on the matrix, the build says so here rather than at fifteen
// instantiation lists or, worse, nowhere.
namespace test {

#ifdef TEST_HAS_UINT128
inline constexpr bool has_uint128 = true;
#else
inline constexpr bool has_uint128 = false;
#endif

static_assert(has_uint128 == std::unsigned_integral<xstd::uint128>);

} // namespace test

#endif // TEST_UINT128_HPP
