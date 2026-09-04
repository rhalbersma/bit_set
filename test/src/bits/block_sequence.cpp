//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <boost/test/unit_test.hpp>    // BOOST_CHECK_EQUAL, BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END
#include <test/block_types.hpp>        // graded_extents, word_types
#include <xstd/bits/block_sequence.hpp> // block_array, block_sequence, block_storage, block_vector
#include <algorithm>                   // count
#include <array>                       // array
#include <cstddef>                     // size_t
#include <cstdint>                     // uint8_t, uint64_t
#include <ranges>                      // iota
#include <vector>                      // vector

BOOST_AUTO_TEST_SUITE(BitBlocks)

namespace {

// std::vector<bool> is the reference reading: one bool per bit, no packing, no invariant
// to get wrong. Every answer block_sequence gives is checked against what it says.
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

// The pair under test, its model, and the running disagreement count, held together so
// that each family of checks below reads as what it compares rather than as a parameter
// list. One family per member, because all of them in one function is a cognitive
// complexity clang-tidy rightly objects to.
//
// Disagreements are counted rather than asserted per bit: a passing assertion per bit
// says no more than one, and a failing one drowns the log.
template<class BB>
class checker
{
        BB const& m_x;
        BB const& m_y;
        model m_mx = reference(m_x);
        model m_my = reference(m_y);
        std::size_t m_n = m_x.size();
        std::size_t m_cardinality = static_cast<std::size_t>(std::ranges::count(m_mx, true));
        int& m_disagreements;

        // Two scratch objects, reset before each mutating check, rather than a fresh copy
        // per check. Same-width assignment reuses the storage, so this is two allocations
        // for the whole checker instead of one per operation -- of which positions() alone
        // did five per bit. It is faster, and it leaves the optimizer one object to follow
        // rather than thousands of construct/destroy pairs: three GCC versions have each
        // mis-analysed that shape in a different way (-Wfree-nonheap-object on 15,
        // -Wrestrict on 17-SVN), and moving the copies around only moved the diagnostic.
        //
        // Held by reference, owned by check_ops: by value they would be the only members
        // narrower than a pointer, and -Wpadded reports the tail padding that leaves.
        BB& m_a;
        BB& m_b;

        auto fresh_x() -> BB&
        {
                m_a = m_x;
                return m_a;
        }

        auto fresh_y() -> BB&
        {
                m_b = m_y;
                return m_b;
        }

        // The two places a comparison becomes a count, so the cast that
        // readability-implicit-bool-conversion asks for has two sites and not thirty.
        auto disagree(bool ours, bool theirs) -> void
        {
                m_disagreements += static_cast<int>(ours != theirs);
        }

        auto unequal(std::size_t ours, std::size_t theirs) -> void
        {
                m_disagreements += static_cast<int>(ours != theirs);
        }

        // Position by position against a model of what the operation should have left.
        auto same(model const& m, BB const& got) -> void
        {
                for (auto i = 0UZ; i < m_n; ++i) {
                        disagree(got[i], m[i]);
                }
        }

public:
        checker(BB const& x, BB const& y, BB& a, BB& b, int& disagreements)
        :
                m_x(x),
                m_y(y),
                m_disagreements(disagreements),
                m_a(a),
                m_b(b)
        {}

        auto width() -> void
        {
                unequal(m_x.count(), m_cardinality);
                disagree(m_x.any(),  m_cardinality != 0);
                disagree(m_x.none(), m_cardinality == 0);
                disagree(m_x.all(),  m_cardinality == m_n);
                disagree(m_x == m_y, m_mx == m_my);
        }

        // find_front and find_back assert any(), so they answer only for a non-empty x;
        // find_first and find_last are total and answer size() for an empty one.
        auto scans() -> void
        {
                if (m_cardinality != 0) {
                        auto front = 0UZ;
                        while (not m_mx[front]) { ++front; }
                        auto back = m_n - 1;
                        while (not m_mx[back]) { --back; }
                        unequal(m_x.find_front(), front);
                        unequal(m_x.find_back(),  back);
                        unequal(m_x.find_prev(m_n), back);
                }

                auto first = 0UZ;
                while (first < m_n and not m_mx[first]) { ++first; }
                unequal(m_x.find_first(), first);
                unequal(m_x.find_last(),  m_n);

                for (auto i = 0UZ; i < m_n; ++i) {
                        auto next = i + 1;
                        while (next < m_n and not m_mx[next]) { ++next; }
                        unequal(m_x.find_next(i), next);
                }

                // lower_bound is the primitive; find_first is its 0 and find_next its n + 1. Check
                // it directly over its whole precondition domain -- n == size() included, which is
                // the one value the scan cannot start from and the reason the guard is == at all.
                for (auto i = 0UZ; i <= m_n; ++i) {
                        auto bound = i;
                        while (bound < m_n and not m_mx[bound]) { ++bound; }
                        unequal(m_x.lower_bound(i), bound);
                }
                for (auto i = 1UZ; i <= m_n and m_cardinality != 0; ++i) {
                        auto j = i;
                        while (j-- > 0) {
                                if (m_mx[j]) {
                                        unequal(m_x.find_prev(i), j);
                                        break;
                                }
                        }
                }
        }

        // Where the block-at-a-time shortcuts live.
        auto relational() -> void
        {
                auto subset = true;
                auto differs = false;
                auto meets = false;
                for (auto i = 0UZ; i < m_n; ++i) {
                        subset  = subset and (not m_mx[i] or m_my[i]);
                        differs = differs or  (m_mx[i] != m_my[i]);
                        meets   = meets   or  (m_mx[i] and m_my[i]);
                }
                disagree(m_x.is_subset_of(m_y),        subset);
                disagree(m_x.is_proper_subset_of(m_y), subset and differs);
                disagree(m_x.intersects(m_y),          meets);
        }

        // On packed bits the set operation and the pointwise sequence operation are the
        // same instruction, so one model answers for both readings.
        auto bitwise() -> void
        {
                { auto& a = fresh_x(); a &= m_y; auto m = model(m_n); for (auto i = 0UZ; i < m_n; ++i) { m[i] = m_mx[i] and     m_my[i]; } same(m, a); }
                { auto& a = fresh_x(); a |= m_y; auto m = model(m_n); for (auto i = 0UZ; i < m_n; ++i) { m[i] = m_mx[i] or      m_my[i]; } same(m, a); }
                { auto& a = fresh_x(); a ^= m_y; auto m = model(m_n); for (auto i = 0UZ; i < m_n; ++i) { m[i] = m_mx[i] !=      m_my[i]; } same(m, a); }
                { auto& a = fresh_x(); a -= m_y; auto m = model(m_n); for (auto i = 0UZ; i < m_n; ++i) { m[i] = m_mx[i] and not m_my[i]; } same(m, a); }
        }

        auto shifts() -> void
        {
                for (auto s = 0UZ; s < m_n; ++s) {
                        { auto& a = fresh_x(); a <<= s; auto m = model(m_n); for (auto i = s;  i < m_n;     ++i) { m[i] = m_mx[i - s]; } same(m, a); }
                        { auto& a = fresh_x(); a >>= s; auto m = model(m_n); for (auto i = 0UZ; i + s < m_n; ++i) { m[i] = m_mx[i + s]; } same(m, a); }
                }
        }

        // Each of these has to leave the unused tail zero. One method apiece rather than
        // four scopes in one: at -O3 GCC 15 inlines the whole sweep into a single function
        // and then reports -Wfree-nonheap-object on the vector copies, which ASan, LSan and
        // UBSan all say is not there.
        auto whole_set() -> void
        {
                auto& a = fresh_x();
                a.set();
                disagree(a.all(), true);
                unequal(a.count(), m_n);
        }

        auto whole_reset() -> void
        {
                auto& a = fresh_x();
                a.reset();
                disagree(a.none(), true);
                unequal(a.count(), 0UZ);
        }

        auto whole_flip() -> void
        {
                auto& a = fresh_x();
                a.flip();
                unequal(a.count(), m_n - m_cardinality);
                for (auto const i : std::views::iota(0UZ, m_n)) {
                        disagree(a[i], not m_mx[i]);
                }
        }

        auto whole_swap() -> void
        {
                auto& a = fresh_x();
                auto& b = fresh_y();
                a.swap(b);
                disagree(a == m_y, true);
                disagree(b == m_x, true);
        }

        // Per bit, including the two that report whether the bit was already there.
        auto positions() -> void
        {
                for (auto i = 0UZ; i < m_n; ++i) {
                        { auto& a = fresh_x(); a.set(i);   disagree(a[i], true);  }
                        { auto& a = fresh_x(); a.reset(i); disagree(a[i], false); }
                        { auto& a = fresh_x(); a.flip(i);  disagree(a[i], not m_mx[i]); }
                        { auto& a = fresh_x(); disagree(a.insert(i), not m_mx[i]); disagree(a[i], true);  }
                        { auto& a = fresh_x(); disagree(a.erase(i),      m_mx[i]); disagree(a[i], false); }
                }
        }

        // Writing every block back must be the identity, and writing all-ones must stop at
        // size(), which is the unused-tail invariant itself.
        auto blocks() -> void
        {
                disagree(m_x.num_blocks() * BB::bits_per_block >= m_n, true);

                {
                        auto& a = fresh_x();
                        for (auto i = 0UZ; i < m_x.num_blocks(); ++i) {
                                a.set_block(i, m_x.block(i));
                        }
                        disagree(a == m_x, true);
                }
                {
                        auto& a = fresh_x();
                        for (auto i = 0UZ; i < m_x.num_blocks(); ++i) {
                                a.set_block(i, static_cast<BB::block_type>(-1));
                        }
                        disagree(a.all(), true);
                        unequal(a.count(), m_n);
                }
        }
};

template<class BB>
auto check_ops(BB const& x, BB const& y, int& disagreements) -> void
{
        auto a = x;
        auto b = y;
        auto c = checker<BB>(x, y, a, b, disagreements);
        c.width();
        c.scans();
        c.relational();
        c.bitwise();
        c.shifts();
        c.whole_set();
        c.whole_reset();
        c.whole_flip();
        c.whole_swap();
        c.positions();
        c.blocks();
}

// Seven patterns, chosen so that every pair of them lands on both sides of each branch:
// all clear and all set for the whole-container shortcuts, the strided ones for partial
// blocks, and the endpoint ones for the first and last block specifically. The last one
// fills every block but the last, which is the only way to reach the right operand of
// all()'s short circuit with a false: every other pattern either fails a block below the
// last, or fills the last one too. It compares block indices rather than a precomputed
// bit count, so that a single-block width -- for which it selects nothing -- does not fold
// into an unsigned comparison against zero, which -Wtype-limits reports.
// graded_extents parameterizes on <N, Block>, as the three containers do; block_array is
// <Block, N>, after std::array. Adapting here keeps the alias ordered by what it holds.
template<std::size_t N, class Block>
using graded_block_array = xstd::block_array<Block, N>;

template<class BB>
auto sweep(BB const& empty) -> int
{
        auto const n = empty.size();

        auto values = std::vector<BB>();
        // views::iota rather than i < n: n is empty.size(), which folds to a constant, and
        // at a width of zero that leaves an unsigned comparison against zero -- -Wtype-limits
        // on GCC, and the same C4296 the header's is_valid() carries a comment about.
        auto const push = [&](auto fill) -> void {
                auto b = empty;
                for (auto const i : std::views::iota(0UZ, n)) {
                        if (fill(i)) { b.set(i); }
                }
                values.push_back(b);
        };
        // Captured by reference throughout rather than by name: under a static width the
        // compiler folds these to constants, and naming something usable in a constant
        // expression is exactly what -Wunused-lambda-capture reports.
        push([&](std::size_t  ) -> bool { return false;                });
        push([&](std::size_t  ) -> bool { return true;                 });
        push([&](std::size_t i) -> bool { return i % 2 == 0;           });
        push([&](std::size_t i) -> bool { return i % 3 == 0;           });
        push([&](std::size_t i) -> bool { return i == 0 or i + 1 == n; });
        push([&](std::size_t i) -> bool { return i + 1 == n;           });
        push([&](std::size_t i) -> bool { return (i / BB::bits_per_block) + 1UZ < empty.num_blocks(); });

        auto disagreements = 0;
        for (auto const& x : values) {
                for (auto const& y : values) {
                        check_ops(x, y, disagreements);
                }
        }
        return disagreements;
}

// Named rather than immediately-invoked lambdas: an empty parameter list before a trailing
// return type is redundant, and dropping it would lean on P1102 for no reason.
constexpr auto a_static_width_is_constexpr() -> bool
{
        auto b = xstd::block_array<std::uint8_t, 9>();
        b.set(8);
        return b.count() == 1 and b.find_first() == 8;
}

constexpr auto a_run_time_width_is_constexpr() -> bool
{
        auto b = xstd::block_vector<std::uint8_t>(9);
        b.set(8);
        return b.count() == 1 and b.find_first() == 8;
}

} // namespace

// Both vehicles the library ships satisfy what block_sequence asks of its storage; growth is
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
        static_assert(sizeof(xstd::block_array<std::uint64_t,  64>) == sizeof(std::array<std::uint64_t,  1>));
        static_assert(sizeof(xstd::block_array<std::uint8_t,  129>) == sizeof(std::array<std::uint8_t,  17>));
        static_assert(sizeof(xstd::block_array<std::uint8_t,    0>) == sizeof(std::array<std::uint8_t,   1>));

        static_assert(    xstd::block_array<std::size_t, 64>::has_static_size);
        static_assert(not xstd::block_vector<>::has_static_size);
}

// Both widths in a constant expression; the run-time one needs C++20 constexpr allocation.
BOOST_AUTO_TEST_CASE(BothWidthsAreUsableAtCompileTime)
{
        static_assert(a_static_width_is_constexpr());
        static_assert(a_run_time_width_is_constexpr());
}

// The static width, at every extent the library instantiates. This is the same storage the
// three owners hold, so a disagreement here is a disagreement in all of them.
BOOST_AUTO_TEST_CASE_TEMPLATE(AStaticWidthAgreesWithTheModel, T, test::graded_extents<graded_block_array>)
{
        BOOST_CHECK_EQUAL(sweep(T()), 0);
}

// The run-time width, at the same grading: within one block, and across boundaries either side.
BOOST_AUTO_TEST_CASE_TEMPLATE(ARunTimeWidthAgreesWithTheModel, Block, test::word_types)
{
        using T = xstd::block_vector<Block>;
        constexpr auto D = test::digits_v<Block>;

        auto disagreements = 0;
        for (auto const n : { 0UZ, 1UZ, D - 1, D, D + 1, (2 * D) - 1, 2 * D, (2 * D) + 1, 3 * D, (3 * D) + 1 }) {
                disagreements += sweep(T(n));
        }
        BOOST_CHECK_EQUAL(disagreements, 0);
}

// Two run-time widths are the same type, so == has to answer for a pair that a static
// width can never form. The widths decide it before the blocks are looked at.
BOOST_AUTO_TEST_CASE(RunTimeWidthsOfDifferentSizeAreNotEqual)
{
        using T = xstd::block_vector<std::uint8_t>;

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
BOOST_AUTO_TEST_CASE(AZeroWidthOwnsOneBlockAndReadsEmpty)
{
        auto const b = xstd::block_vector<std::uint8_t>(0);

        BOOST_CHECK_EQUAL(b.size(), 0UZ);
        BOOST_CHECK_EQUAL(b.num_blocks(), 1UZ);
        BOOST_CHECK_EQUAL(b.count(), 0UZ);
        BOOST_CHECK(b.none());
        BOOST_CHECK(b.all());           // vacuously, as std::bitset<0>::all() is
        BOOST_CHECK(not b.any());
}

// The other half of it: that sole block is entirely padding, so the whole-container
// mutators must not reach into it. This is what the last-block mask exists to say.
BOOST_AUTO_TEST_CASE(AZeroWidthsOneBlockIsAllPaddingAndStaysZero)
{
        auto b = xstd::block_vector<std::uint8_t>(0);

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
        auto const b = xstd::block_vector<std::uint8_t>();

        BOOST_CHECK_EQUAL(b.size(), 0UZ);
        BOOST_CHECK_EQUAL(b.num_blocks(), 1UZ);
        BOOST_CHECK(b == xstd::block_vector<std::uint8_t>(0));
}

BOOST_AUTO_TEST_SUITE_END()
