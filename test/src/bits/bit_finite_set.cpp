//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END
#include <test/block_types.hpp>         // graded_extents
#include <test/set/concepts.hpp>        // bit_set
#include <test/value_reference.hpp>     // value_reference
#include <xstd/bits/bit_finite_set.hpp> // bit_finite_set
#include <concepts>                     // regular, totally_ordered
#include <cstddef>                      // size_t
#include <cstdint>                      // uint8_t, uint64_t
#include <iterator>                     // bidirectional_iterator
#include <ranges>                       // bidirectional_range, iota, to

BOOST_AUTO_TEST_SUITE(BitFiniteSet)

// Every Block model within one block and the narrow ones across boundaries; the grading is in test/block_types.hpp.
using Types = test::graded_extents<xstd::bit_finite_set>;

// The clauses one at a time, so a failure names which one; the umbrella asserts the composite.
BOOST_AUTO_TEST_CASE_TEMPLATE(IsRegular, T, Types)
{
        static_assert(std::regular<T>);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(IsTotallyOrdered, T, Types)
{
        static_assert(std::totally_ordered<T>);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(IsABidirectionalRange, T, Types)
{
        static_assert(std::ranges::bidirectional_range<T>);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ItsIteratorIsBidirectional, T, Types)
{
        using I = T::iterator;
        static_assert(std::bidirectional_iterator<I>);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ItsConstReferenceIsAValue, T, Types)
{
        static_assert(test::value_reference<typename T::const_reference>);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(IsABitSet, T, Types)
{
        static_assert(test::set::bit_set<T>);
}

// std::set's lookups are total over key_type, and so are these: a key naming no possible
// element answers "absent" rather than reaching into the blocks. Kept here rather than in
// the exhaustive suite because that suite draws every key from [0, N), which is exactly
// why this went unseen.
//
// The width x key matrix is not decoration. Run against the unguarded header, no single
// width exposes all six operations and no single key does either, so any one cell alone
// would have let the bug through. Two cells are worth naming: erase past the last block
// is an out-of-bounds *write*, and upper_bound fails on the returned value rather than on
// memory at all -- find_next's ++n wrapped before it could test the bound, so it answered
// with a real element where end() was due. That second one is what keeps this a gate on
// the jobs that build without sanitizers.
template<std::size_t N, class Block>
auto check_keys_outside_the_domain() -> void
{
        using X = xstd::bit_finite_set<N, Block>;
        auto const full = std::views::iota(0uz, N) | std::ranges::to<X>();

        // Just past the end, past the last block, and the value that would wrap any n + 1.
        for (auto const x : { N, N + 1, (2 * N) + 1, std::size_t(-1) }) {
                for (auto const& original : { X(), full }) {
                        auto a = original;

                        BOOST_CHECK(not a.contains(x));
                        BOOST_CHECK_EQUAL(a.count(x), 0uz);
                        BOOST_CHECK(a.find(x)        == a.end());
                        BOOST_CHECK(a.lower_bound(x) == a.end());
                        BOOST_CHECK(a.upper_bound(x) == a.end());

                        auto const [ first, last ] = a.equal_range(x);
                        BOOST_CHECK(first == a.end());
                        BOOST_CHECK(last  == a.end());

                        // A no-op that must stay one: this is the write.
                        BOOST_CHECK_EQUAL(a.erase(x), 0uz);
                        BOOST_CHECK(a == original);
                }
        }
}

BOOST_AUTO_TEST_CASE(LookupIsTotalOverKeyType)
{
        // The wide and multi-block widths carry the test. Measured against the unguarded
        // header, N = 8 over uint8_t is clean for all six at key == N, so a narrow
        // single-block set on its own proves nothing either way.
        check_keys_outside_the_domain<  0, std::uint8_t >();
        check_keys_outside_the_domain<  8, std::uint8_t >();
        check_keys_outside_the_domain< 24, std::uint8_t >();
        check_keys_outside_the_domain<  1, std::uint64_t>();
        check_keys_outside_the_domain< 64, std::uint64_t>();
        check_keys_outside_the_domain<129, std::uint64_t>();
}

BOOST_AUTO_TEST_SUITE_END()
