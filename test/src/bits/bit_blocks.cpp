//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <boost/test/unit_test.hpp>    // BOOST_CHECK_EQUAL, BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END
#include <test/block_types.hpp>        // graded_extents, word_types
#include <xstd/bits/bit_blocks.hpp>    // bit_blocks, block_storage, dynamic_bits, static_bits
#include <array>                       // array
#include <cstddef>                     // size_t
#include <cstdint>                     // uint8_t, uint64_t
#include <vector>                      // vector

BOOST_AUTO_TEST_SUITE(BitBlocks)

namespace {

// std::vector<bool> is the reference reading: one bool per bit, no packing, no invariant
// to get wrong. Every answer bit_blocks gives is checked against what it says.
using model = std::vector<bool>;

template<class BB>
auto reference(BB const& b) -> model
{
        auto m = model(b.size());
        for (auto i = 0UZ; i < b.size(); ++i) {
                m[i] = b[i];
        }
        return m;
}

// Every operation against the model, for one ordered pair. Disagreements are counted
// rather than asserted per bit: a passing assertion per bit says no more than one, and a
// failing one drowns the log.
template<class BB>
auto check_ops(BB const& x, BB const& y, int& disagreements) -> void
{
        auto const n  = x.size();
        auto const mx = reference(x);
        auto const my = reference(y);
        auto const disagree = [&](bool ours, bool theirs) -> void { disagreements += ours != theirs; };
        auto const same = [&](model const& m, BB const& got) -> void {
                for (auto i = 0UZ; i < n; ++i) {
                        disagreements += got[i] != m[i];
                }
        };

        auto cardinality = 0UZ;
        for (auto const b : mx) {
                cardinality += b ? 1UZ : 0UZ;
        }

        disagreements += x.count() != cardinality;
        disagree(x.any(),  cardinality != 0);
        disagree(x.none(), cardinality == 0);
        disagree(x.all(),  cardinality == n);
        disagree(x == y,   mx == my);

        // The scans. find_front and find_back assert any(), so they only answer for a
        // non-empty x; find_first and find_last are total and answer size() for an empty one.
        if (cardinality != 0) {
                auto front = 0UZ;
                while (not mx[front]) { ++front; }
                auto back = n - 1;
                while (not mx[back]) { --back; }
                disagreements += x.find_front()  != front;
                disagreements += x.find_back()   != back;
                disagreements += x.find_prev(n)  != back;
        }
        {
                auto first = 0UZ;
                while (first < n and not mx[first]) { ++first; }
                disagreements += x.find_first() != first;
                disagreements += x.find_last()  != n;
        }
        for (auto i = 0UZ; i < n; ++i) {
                auto next = i + 1;
                while (next < n and not mx[next]) { ++next; }
                disagreements += x.find_next(i) != next;
        }
        for (auto i = 1UZ; i <= n and cardinality != 0; ++i) {
                auto j = i;
                while (j-- > 0) {
                        if (mx[j]) {
                                disagreements += x.find_prev(i) != j;
                                break;
                        }
                }
        }

        // The set-relational trio, which is where the block-at-a-time shortcuts live.
        {
                auto subset = true;
                auto differs = false;
                auto meets = false;
                for (auto i = 0UZ; i < n; ++i) {
                        subset  = subset and (not mx[i] or my[i]);
                        differs = differs or  (mx[i] != my[i]);
                        meets   = meets   or  (mx[i] and my[i]);
                }
                disagree(x.is_subset_of(y),        subset);
                disagree(x.is_proper_subset_of(y), subset and differs);
                disagree(x.intersects(y),          meets);
        }

        // The six compound assignments. On packed bits the set operation and the pointwise
        // sequence operation are the same instruction, so one model answers for both.
        { auto a = x; a &= y; auto m = model(n); for (auto i = 0UZ; i < n; ++i) { m[i] = mx[i] and     my[i]; } same(m, a); }
        { auto a = x; a |= y; auto m = model(n); for (auto i = 0UZ; i < n; ++i) { m[i] = mx[i] or      my[i]; } same(m, a); }
        { auto a = x; a ^= y; auto m = model(n); for (auto i = 0UZ; i < n; ++i) { m[i] = mx[i] !=      my[i]; } same(m, a); }
        { auto a = x; a -= y; auto m = model(n); for (auto i = 0UZ; i < n; ++i) { m[i] = mx[i] and not my[i]; } same(m, a); }
        for (auto s = 0UZ; s < n; ++s) {
                { auto a = x; a <<= s; auto m = model(n); for (auto i = s;  i < n;     ++i) { m[i] = mx[i - s]; } same(m, a); }
                { auto a = x; a >>= s; auto m = model(n); for (auto i = 0UZ; i + s < n; ++i) { m[i] = mx[i + s]; } same(m, a); }
        }

        // Whole-container mutators, each of which has to leave the unused tail zero.
        { auto a = x; a.set();   disagree(a.all(),  true); disagreements += a.count() != n; }
        { auto a = x; a.reset(); disagree(a.none(), true); disagreements += a.count() != 0; }
        { auto a = x; a.flip();  disagreements += a.count() != n - cardinality; for (auto i = 0UZ; i < n; ++i) { disagree(a[i], not mx[i]); } }
        { auto a = x; auto b = y; a.swap(b); disagree(a == y, true); disagree(b == x, true); }

        // Per-bit mutators, and the two that report whether the bit was already there.
        for (auto i = 0UZ; i < n; ++i) {
                { auto a = x; a.set(i);   disagree(a[i], true);  }
                { auto a = x; a.reset(i); disagree(a[i], false); }
                { auto a = x; a.flip(i);  disagree(a[i], not mx[i]); }
                { auto a = x; disagree(a.insert(i), not mx[i]); disagree(a[i], true);  }
                { auto a = x; disagree(a.erase(i),      mx[i]);  disagree(a[i], false); }
        }

        // Block access both ways. Writing every block back must be the identity, and
        // writing all-ones must stop at size(), which is the unused-tail invariant itself.
        {
                disagree(x.num_blocks() * BB::bits_per_block >= n, true);
                auto a = x;
                for (auto i = 0UZ; i < x.num_blocks(); ++i) {
                        a.set_block(i, x.block(i));
                }
                disagree(a == x, true);
                auto b = x;
                for (auto i = 0UZ; i < x.num_blocks(); ++i) {
                        b.set_block(i, static_cast<typename BB::block_type>(-1));
                }
                disagree(b.all(), true);
                disagreements += b.count() != n;
        }
}

// Seven patterns, chosen so that every pair of them lands on both sides of each branch:
// all clear and all set for the whole-container shortcuts, the strided ones for partial
// blocks, and the endpoint ones for the first and last block specifically. The last one
// fills every block but the last, which is the only way to reach the right operand of
// all()'s short circuit with a false: every other pattern either fails a block below the
// last, or fills the last one too.
template<class BB, class Make>
auto sweep(std::size_t n, Make make) -> int
{
        // Every bit below the last block, which is where the whole-container shortcuts split.
        auto const below_last = (make().num_blocks() - 1UZ) * BB::bits_per_block;

        auto values = std::vector<BB>();
        auto const push = [&](auto fill) -> void {
                auto b = make();
                for (auto i = 0UZ; i < n; ++i) {
                        if (fill(i)) { b.set(i); }
                }
                values.push_back(b);
        };
        push([  ](std::size_t  ) -> bool { return false;                });
        push([  ](std::size_t  ) -> bool { return true;                 });
        push([  ](std::size_t i) -> bool { return i % 2 == 0;           });
        push([  ](std::size_t i) -> bool { return i % 3 == 0;           });
        push([ n](std::size_t i) -> bool { return i == 0 or i + 1 == n; });
        push([ n](std::size_t i) -> bool { return i + 1 == n;           });
        push([below_last](std::size_t i) -> bool { return i < below_last; });

        auto disagreements = 0;
        for (auto const& x : values) {
                for (auto const& y : values) {
                        check_ops(x, y, disagreements);
                }
        }
        return disagreements;
}

} // namespace

// Both vehicles the library ships satisfy what bit_blocks asks of its storage; growth is
// detected where it exists rather than required, so a fixed one qualifies just as well.
BOOST_AUTO_TEST_CASE(ItsStorageIsAContiguousSizedRangeOfUnsignedIntegers)
{
        static_assert(xstd::block_storage<std::array<std::uint8_t, 4>>);
        static_assert(xstd::block_storage<std::vector<std::uint64_t>>);

        static_assert(not xstd::block_storage<std::vector<bool>>);      // not a contiguous range
        static_assert(not xstd::block_storage<std::vector<int>>);       // nor unsigned integers
}

// A compile-time width costs nothing: the absent size member takes no storage, which is
// the whole point of conditional_data_member_t over a plain size_t.
BOOST_AUTO_TEST_CASE(AStaticWidthAddsNothingToItsBlocks)
{
        static_assert(sizeof(xstd::static_bits< 64, std::uint64_t>) == sizeof(std::array<std::uint64_t,  1>));
        static_assert(sizeof(xstd::static_bits<129, std::uint8_t >) == sizeof(std::array<std::uint8_t,  17>));
        static_assert(sizeof(xstd::static_bits<  0, std::uint8_t >) == sizeof(std::array<std::uint8_t,   1>));

        static_assert(    xstd::static_bits<64>::has_static_size);
        static_assert(not xstd::dynamic_bits<  >::has_static_size);
}

// Both widths in a constant expression; a std::vector one needs C++20 constexpr allocation.
BOOST_AUTO_TEST_CASE(BothWidthsAreUsableAtCompileTime)
{
        static_assert([]() -> bool { auto b = xstd::static_bits<9, std::uint8_t>(); b.set(8); return b.count() == 1 and b.find_first() == 8; }());
        static_assert([]() -> bool { auto b = xstd::dynamic_bits<std::uint8_t>(9); b.set(8); return b.count() == 1 and b.find_first() == 8; }());
}

// The static width, at every extent the library instantiates. This is the same storage the
// three owners hold, so a disagreement here is a disagreement in all of them.
BOOST_AUTO_TEST_CASE_TEMPLATE(AStaticWidthAgreesWithTheModel, T, test::graded_extents<xstd::static_bits>)
{
        BOOST_CHECK_EQUAL(sweep<T>(T().size(), []() -> T { return T(); }), 0);
}

// The run-time width, at the same grading: within one block, and across boundaries either side.
BOOST_AUTO_TEST_CASE_TEMPLATE(ARunTimeWidthAgreesWithTheModel, Block, test::word_types)
{
        using T = xstd::dynamic_bits<Block>;
        constexpr auto D = test::digits_v<Block>;

        auto disagreements = 0;
        for (auto const n : { 0UZ, 1UZ, D - 1, D, D + 1, 2 * D - 1, 2 * D, 2 * D + 1, 3 * D, 3 * D + 1 }) {
                disagreements += sweep<T>(n, [n]() -> T { return T(n); });
        }
        BOOST_CHECK_EQUAL(disagreements, 0);
}

// Two run-time widths are the same type, so == has to answer for a pair that a static
// width can never form. The widths decide it before the blocks are looked at.
BOOST_AUTO_TEST_CASE(RunTimeWidthsOfDifferentSizeAreNotEqual)
{
        using T = xstd::dynamic_bits<std::uint8_t>;

        BOOST_CHECK(T(8) != T(9));
        BOOST_CHECK(T(8) == T(8));

        auto x = T(9);
        auto y = T(9);
        x.set(0);
        BOOST_CHECK(x != y);
        y.set(0);
        BOOST_CHECK(x == y);
}

// A width of zero still owns one block, so that last_block() names something; every
// operation on it has to see through that block to the width, which is what says empty.
BOOST_AUTO_TEST_CASE(AZeroWidthOwnsOneBlockAndStillReadsEmpty)
{
        auto b = xstd::dynamic_bits<std::uint8_t>(0);

        BOOST_CHECK_EQUAL(b.size(), 0UZ);
        BOOST_CHECK_EQUAL(b.num_blocks(), 1UZ);
        BOOST_CHECK_EQUAL(b.count(), 0UZ);
        BOOST_CHECK(b.none());
        BOOST_CHECK(b.all());           // vacuously, as std::bitset<0>::all() is
        BOOST_CHECK(not b.any());

        // set() and flip() must not reach into the block that is entirely padding.
        b.set();
        BOOST_CHECK(b.none());
        b.flip();
        BOOST_CHECK(b.none());
        BOOST_CHECK_EQUAL(b.block(0), 0U);
}

// A default-constructed run-time width is the zero-width one, not a block-less one: the
// at-least-one-block invariant has to hold before any constructor of ours runs.
BOOST_AUTO_TEST_CASE(ADefaultConstructedRunTimeWidthIsZeroWidthWithOneBlock)
{
        auto const b = xstd::dynamic_bits<std::uint8_t>();

        BOOST_CHECK_EQUAL(b.size(), 0UZ);
        BOOST_CHECK_EQUAL(b.num_blocks(), 1UZ);
        BOOST_CHECK(b == xstd::dynamic_bits<std::uint8_t>(0));
}

BOOST_AUTO_TEST_SUITE_END()
