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

// VS 2022's MSVC STL has no <flat_set>, and std::flat_set is only a reference
// implementation in the set tests' type lists, so dropping it costs no bit_set
// coverage. The probe lives here because <version> has to precede it, which a
// test doing it itself could only manage by breaking its own include order.
namespace xstd {

template<class T>
inline constexpr bool is_flat_set = false;

#ifdef XSTD_TEST_HAS_FLAT_SET

template<class Key, class Compare, class KeyContainer>
inline constexpr bool is_flat_set<std::flat_set<Key, Compare, KeyContainer>> = true;

#endif

}       // namespace xstd
