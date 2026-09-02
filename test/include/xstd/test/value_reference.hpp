//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TEST_VALUE_REFERENCE_HPP
#define XSTD_TEST_VALUE_REFERENCE_HPP

#include <concepts>    // equality_comparable
#include <type_traits> // is_copy_assignable_v, is_default_constructible_v, is_move_assignable_v,
                       // is_trivially_copy_constructible_v, is_trivially_destructible_v,
                       // is_trivially_move_constructible_v

namespace xstd::test {

// What a container's const_reference must be for a[i] = a[j] to write through it
// rather than rebind it. A packed container's proxy and an unpacked one's
// const bool& or const size_t& all satisfy this, which is the whole point: the
// assignment means the same thing either way, so the caller cannot tell which it
// has. Shared by both contracts -- a set's reference and a sequence's answer to
// the same list.
template<class R>
concept value_reference =
            std::is_trivially_destructible_v<R>
    and not std::is_default_constructible_v<R>
    and     std::is_trivially_copy_constructible_v<R>
    and not std::is_copy_assignable_v<R>
    and     std::is_trivially_move_constructible_v<R>
    and not std::is_move_assignable_v<R>
    and     std::equality_comparable<R>;

} // namespace xstd::test

#endif // XSTD_TEST_VALUE_REFERENCE_HPP
