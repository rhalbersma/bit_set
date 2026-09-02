//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_TEST_SEQUENCE_CONCEPTS_HPP
#define XSTD_TEST_SEQUENCE_CONCEPTS_HPP

#include <concepts>    // equality_comparable, regular, totally_ordered
#include <iterator>    // random_access_iterator
#include <ranges>      // random_access_range
#include <type_traits> // is_copy_assignable_v, is_default_constructible_v, is_move_assignable_v,
                       // is_trivially_copy_constructible_v, is_trivially_destructible_v,
                       // is_trivially_move_constructible_v

namespace xstd::test::sequence {

// What a container's const_reference must be for a[i] = a[j] to write through it
// rather than rebind it. A packed container's proxy and an unpacked one's
// const bool& both satisfy this, which is the whole point: the assignment means
// the same thing either way, so the caller cannot tell which it has.
template<class R>
concept value_reference =
            std::is_trivially_destructible_v<R>
    and not std::is_default_constructible_v<R>
    and     std::is_trivially_copy_constructible_v<R>
    and not std::is_copy_assignable_v<R>
    and     std::is_trivially_move_constructible_v<R>
    and not std::is_move_assignable_v<R>
    and     std::equality_comparable<R>;

// The interface a bit-packed sequence shares with the sequence it packs. Named
// here rather than restated per container, so the umbrella can say the two
// satisfy the same one instead of comparing two lists and hoping they stay equal.
template<class C>
concept bit_sequence =
        std::regular<C>
    and std::totally_ordered<C>
    and std::ranges::random_access_range<C>
    and std::random_access_iterator<typename C::iterator>
    and value_reference<typename C::const_reference>;

} // namespace xstd::test::sequence

#endif // XSTD_TEST_SEQUENCE_CONCEPTS_HPP
