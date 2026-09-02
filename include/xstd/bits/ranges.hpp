//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_BITS_RANGES_HPP
#define XSTD_BITS_RANGES_HPP

// The two things a sequence of bits is: the set of positions that are on, and the
// sequence of bools at every position. Everything else about a bit sequence is a
// third vocabulary for one of these two.
#include <xstd/bits/ranges/array_view.hpp> // IWYU pragma: export; array_view, array_find, array_ops
#include <xstd/bits/ranges/bit_extent.hpp> // IWYU pragma: export; bit_extent
#include <xstd/bits/ranges/set_view.hpp>   // IWYU pragma: export; set_view, set_find, set_ops, set_compare

#endif // XSTD_BITS_RANGES_HPP
