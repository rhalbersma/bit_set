#pragma once

//          Copyright Rein Halbersma 2014-2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <version>                      // __cpp_lib_flat_set
#if defined(__cpp_lib_flat_set)
#define XSTD_TEST_HAS_FLAT_SET
#include <flat_set>                     // flat_set
#endif

// std::flat_set is one of the reference implementations the set tests measure
// bit_set against; it is not itself under test. The MSVC STL shipped with
// Visual Studio 2022 has no <flat_set> at all, so an unconditional include is
// a C1083 that fails every test in this directory before a single assertion
// runs, for a reason that says nothing about bit_set. VS 2022 is the 17.x
// line, in maintenance, so it will not be gaining the header.
//
// The probe lives here rather than in each test because <version> has to
// arrive before anything can ask the question, and a test putting it first in
// its own standard-library group would have to break that group's order to do
// it. A translation unit takes XSTD_TEST_HAS_FLAT_SET for the entry in its
// type list, and is_flat_set for the two cases that exempt flat_set by name.
//
// Dropping the entry costs a reference implementation, not coverage: every
// bit_set specialization in those lists is tested exactly as before.
namespace xstd {

template<class T>
inline constexpr bool is_flat_set = false;

#ifdef XSTD_TEST_HAS_FLAT_SET

template<class Key, class Compare, class KeyContainer>
inline constexpr bool is_flat_set<std::flat_set<Key, Compare, KeyContainer>> = true;

#endif

}       // namespace xstd
