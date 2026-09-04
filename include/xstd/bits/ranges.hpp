//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_BITS_RANGES_HPP
#define XSTD_BITS_RANGES_HPP

// The two things a sequence of bits is: the set of positions that are on, and the sequence of bools at every position.
#include <xstd/bits/ranges/bit_extent.hpp>    // IWYU pragma: export; bit_extent
#include <xstd/bits/ranges/block_access.hpp>  // IWYU pragma: export; block_access, block_range
#include <xstd/bits/ranges/sequence_view.hpp> // IWYU pragma: export; sequence_view, sequence_find, sequence_ops
#include <xstd/bits/ranges/set_view.hpp>      // IWYU pragma: export; set_view, set_find, set_ops, set_compare

#endif // XSTD_BITS_RANGES_HPP
