//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_BITS_HPP
#define XSTD_BITS_HPP

// Not the ext adaptors: each hard-includes the library it adapts, so a front door
// naming them would put Boost on every consumer path. Ask for those by name.

#include <xstd/bits/array.hpp>  // IWYU pragma: export; bit_array
#include <xstd/bits/bit.hpp>    // IWYU pragma: export; the block vehicle and the block predicates
#include <xstd/bits/bitset.hpp> // IWYU pragma: export; bitset
#include <xstd/bits/ranges.hpp>  // IWYU pragma: export; the views that make a bitset a range
#include <xstd/bits/set.hpp>    // IWYU pragma: export; finite_bit_set

#endif // XSTD_BITS_HPP
