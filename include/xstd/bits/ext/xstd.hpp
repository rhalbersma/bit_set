//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_BITS_EXT_XSTD_HPP
#define XSTD_BITS_EXT_XSTD_HPP

// IWYU pragma: always_keep

// Our own bitset made a range by ADL, which xstd/bits/bitset.hpp deliberately
// does not do: iterating a bitset is opt-in, so it is asked for by name too.

#include <xstd/bits/ext/xstd/bitset.hpp> // IWYU pragma: export; begin, end, rbegin, rend

#endif // XSTD_BITS_EXT_XSTD_HPP
