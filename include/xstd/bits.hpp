//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_BITS_HPP
#define XSTD_BITS_HPP

// Every container is one header named after the type it declares, so a consumer
// wanting one asks for it by name and this front door is for wanting them all.
//
// Not the ext adaptors: each hard-includes the library it adapts, so a front door
// naming them would put Boost on every consumer path. Ask for those by name.
#include <xstd/bits/bit_array.hpp>      // IWYU pragma: export; bit_array
#include <xstd/bits/bitset.hpp>         // IWYU pragma: export; bitset
#include <xstd/bits/bit_finite_set.hpp> // IWYU pragma: export; bit_finite_set
#include <xstd/bits/ranges.hpp>         // IWYU pragma: export; set_view, array_view

#endif // XSTD_BITS_HPP
