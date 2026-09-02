//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/bits.hpp>                   // the whole bits surface
#include <test/block_types.hpp>            // graded_extents
#include <test/flat_set.hpp>               // TEST_HAS_FLAT_SET
#include <test/set/concepts.hpp>           // bit_set
#include <test/sequence/concepts.hpp>      // bit_sequence
#include <boost/test/unit_test.hpp>        // BOOST_AUTO_TEST_CASE
#include <array>                           // array
#include <cstddef>                         // size_t
#include <ranges>                          // bidirectional_range, random_access_range
#include <set>                             // set
#include <tuple>                           // tuple_element_t, tuple_size_v
#include <utility>                         // declval, index_sequence, make_index_sequence


// Every entity the front door promises, reached through it alone, and each one
// doing the thing it exists to do. No leaf test can state this: each includes
// only the one header it is named after, so none of them sees the door at all.
BOOST_AUTO_TEST_CASE(EveryContainerArrivesThroughTheOneDoor)
{
        // The two containers that are ranges on their own terms: one indexed by
        // position, one iterating its elements.
        static_assert(std::ranges::random_access_range<xstd::bit_array<8>>);
        static_assert(std::ranges::bidirectional_range<xstd::bit_finite_set<8>>);

        // xstd::bitset is deliberately not a range -- it reproduces std::bitset,
        // which is not one either. That is the whole reason the views exist, so
        // the door has to deliver a working one over it.
        static_assert(not std::ranges::range<xstd::bitset<8>>);

        auto legacy = xstd::bitset<8>();
        static_assert(std::ranges::bidirectional_range<decltype(xstd::set_view(legacy))>);

        auto packed = xstd::bit_array<8>();
        static_assert(std::ranges::random_access_range<decltype(xstd::array_view(packed))>);
}

// The claim the whole library rests on, and the one no header's own test can
// make: a packed container is not merely well formed, it satisfies the same
// interface as the standard container it packs. Asserting it over ours alone
// would say nothing -- the interface is only meaningful because std::array
// answers to it too, so both sides are checked against the one concept.
BOOST_AUTO_TEST_CASE(APackedArrayIsTheArrayItPacks)
{
        using namespace test::sequence;

        // The standard's side, at the extents a packed array grades over.
        static_assert(bit_sequence<std::array<bool,  0>>);
        static_assert(bit_sequence<std::array<bool,  1>>);
        static_assert(bit_sequence<std::array<bool,  8>>);
        static_assert(bit_sequence<std::array<bool, 64>>);

        // And ours, over every Block model and extent the grading names.
        using packed = test::graded_extents<xstd::bit_array>;
        [] <std::size_t... I> (std::index_sequence<I...>) {
                static_assert((bit_sequence<std::tuple_element_t<I, packed>> and ...));
        }(std::make_index_sequence<std::tuple_size_v<packed>>{});
}

// The same claim on the other reading, and the reason there are two contracts
// rather than one: a set of keys and a sequence of bools are different
// interfaces, and a packed container answers to whichever one it is packing.
BOOST_AUTO_TEST_CASE(APackedSetIsTheSetItPacks)
{
        using namespace test::set;

        // The standard's side. std::flat_set is here for the same reason
        // std::array is above: a second, differently built reference keeps the
        // concept from quietly describing one implementation.
        static_assert(bit_set<std::set<std::size_t>>);
#ifdef TEST_HAS_FLAT_SET
        static_assert(bit_set<std::flat_set<std::size_t>>);
#endif

        using packed = test::graded_extents<xstd::bit_finite_set>;
        [] <std::size_t... I> (std::index_sequence<I...>) {
                static_assert((bit_set<std::tuple_element_t<I, packed>> and ...));
        }(std::make_index_sequence<std::tuple_size_v<packed>>{});
}
