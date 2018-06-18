#pragma once

//          Copyright Rein Halbersma 2014-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <traits.hpp>                   // has_op_minus_assign_v, has_forward_iterator_v, has_resize_v, has_hinted_insert_v
#include <boost/test/unit_test.hpp>     // BOOST_CHECK, BOOST_CHECK_EQUAL, BOOST_CHECK_EQUAL_COLLECTIONS, BOOST_CHECK_LE, BOOST_CHECK_NE, BOOST_CHECK_THROW
#include <algorithm>                    // copy_if, equal, find, includes, lexicographical_compare,
                                        // set_difference, set_intersection, set_symmetric_difference, set_union, transform
#include <iterator>                     // distance, inserter
#include <memory>                       // addressof
#include <set>                          // set
#include <stdexcept>                    // out_of_range

namespace xstd {

template<class IntSet>
struct constructor
{
        constexpr auto operator()() const noexcept
        {
                IntSet is;
                BOOST_CHECK(empty(is));                                         // [bitset.cons]/1
        }

        // [bitset.cons]/2-7 describe constructors taking unsigned long long, basic_string and char const*
};

struct op_bitand_assign
{
        template<class IntSet>
        constexpr auto operator()(IntSet& lhs, IntSet const& rhs) const noexcept
        {
                BOOST_CHECK_EQUAL(max_size(lhs), max_size(rhs));
                auto const src = lhs;
                auto const& dst = lhs &= rhs;

                for (auto N = max_size(src), i = decltype(N){0}; i < N; ++i) {
                                                                                // [bitset.members]/1
                        BOOST_CHECK_EQUAL(contains(dst, i), !contains(rhs, i) ? false : contains(src, i));
                }
                BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(lhs));    // [bitset.members]/2
        }
};

struct op_bitor_assign
{
        template<class IntSet>
        constexpr auto operator()(IntSet& lhs, IntSet const& rhs) const noexcept
        {
                BOOST_CHECK_EQUAL(max_size(lhs), max_size(rhs));
                auto const src = lhs;
                auto const& dst = lhs |= rhs;

                for (auto N = max_size(src), i = decltype(N){0}; i < N; ++i) {
                                                                                // [bitset.members]/3
                        BOOST_CHECK_EQUAL(contains(dst, i), contains(rhs, i) ? true : contains(src, i));
                }
                BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(lhs));    // [bitset.members]/4
        }
};

struct op_xor_assign
{
        template<class IntSet>
        constexpr auto operator()(IntSet& lhs, IntSet const& rhs) const noexcept
        {
                BOOST_CHECK_EQUAL(max_size(lhs), max_size(rhs));
                auto const src = lhs;
                auto const& dst = lhs ^= rhs;

                for (auto N = max_size(src), i = decltype(N){0}; i < N; ++i) {
                                                                                // [bitset.members]/5
                        BOOST_CHECK_EQUAL(contains(dst, i), contains(rhs, i) ? !contains(src, i) : contains(src, i));
                }
                BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(lhs));    // [bitset.members]/6
        }
};

struct op_minus_assign
{
        template<class IntSet>
        constexpr auto operator()(IntSet& lhs [[maybe_unused]], IntSet const& rhs [[maybe_unused]]) const noexcept
        {
                if constexpr (tti::has_op_minus_assign_v<IntSet>) {
                        BOOST_CHECK_EQUAL(max_size(lhs), max_size(rhs));
                        auto const src = lhs;
                        auto const& dst = lhs -= rhs;

                        for (auto N = max_size(src), i = decltype(N){0}; i < N; ++i) {
                                BOOST_CHECK_EQUAL(contains(dst, i), contains(rhs, i) ? false : contains(src, i));
                        }
                        BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(lhs));
                }
        }
};

struct op_shift_left_assign
{
        template<class IntSet, class SizeType>
        auto operator()(IntSet& is, SizeType const pos) const
        {
                BOOST_CHECK_LE(pos, max_size(is));
                auto const src = is;
                auto const& dst = is <<= pos;

                for (auto N = max_size(src), I = decltype(N){0}; I < N; ++I) {
                        if (I < pos) {
                                BOOST_CHECK(!contains(dst, I));                 // [bitset.members]/7.1
                        } else {                                                // [bitset.members]/7.2
                                BOOST_CHECK_EQUAL(contains(dst, I), contains(src, I - pos));
                        }
                }
                BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(is));     // [bitset.members]/8
        }
};

struct op_shift_right_assign
{
        template<class IntSet, class SizeType>
        auto operator()(IntSet& is, SizeType const pos) const
        {
                BOOST_CHECK_LE(pos, max_size(is));
                auto const src = is;
                auto const& dst = is >>= pos;

                for (auto N = max_size(src), I = decltype(N){0}; I < N; ++I) {
                        if (pos >= N - I) {
                                BOOST_CHECK(!contains(dst, I));                 // [bitset.members]/9.1
                        } else {                                                // [bitset.members]/9.2
                                BOOST_CHECK_EQUAL(contains(dst, I), contains(src, I + pos));
                        }
                }
                BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(is));     // [bitset.members]/10
        }
};

struct fn_fill
{
        template<class IntSet>
        auto operator()(IntSet& is) const noexcept
        {
                auto const& dst = fill(is);

                BOOST_CHECK(full(is));                                          // [bitset.members]/11
                BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(is));     // [bitset.members]/12
        }
};

struct fn_insert
{
        template<class IntSet>
        auto operator()(IntSet& is) const noexcept
        {
                BOOST_CHECK_THROW(insert(is, max_size(is)), std::out_of_range); // [bitset.members]/13
        }

        template<class IntSet, class SizeType>
        auto operator()(IntSet& is, SizeType const pos) const
        {
                auto const src = is;
                auto const& dst = insert(is, pos);

                for (auto N = max_size(src), i = decltype(N){0}; i < N; ++i) {
                                                                                // [bitset.members]/14
                        BOOST_CHECK_EQUAL(contains(dst, i), i == pos ? true : contains(src, i));
                }
                BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(is));     // [bitset.members]/15
        }

        template<class IntSet, class SizeType>
        auto operator()(IntSet& is, SizeType const pos, bool const val) const
        {
                auto const src = is;
                auto const& dst = insert(is, pos, val);

                for (auto N = max_size(src), i = decltype(N){0}; i < N; ++i) {
                                                                                // [bitset.members]/14
                        BOOST_CHECK_EQUAL(contains(dst, i), i == pos ? val : contains(src, i));
                }
                BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(is));     // [bitset.members]/15
        }
};

struct fn_clear
{
        template<class IntSet>
        auto operator()(IntSet& is) const noexcept
        {
                auto const& dst = clear(is);

                BOOST_CHECK(empty(is));                                         // [bitset.members]/16
                BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(is));     // [bitset.members]/17
        }
};

struct fn_erase
{
        template<class IntSet>
        auto operator()(IntSet& is) const noexcept
        {
                BOOST_CHECK_THROW(erase(is, max_size(is)), std::out_of_range);  // [bitset.members]/18
        }

        template<class IntSet, class SizeType>
        auto operator()(IntSet& is, SizeType const pos) const
        {
                auto const src = is;
                auto const& dst = erase(is, pos);

                for (auto N = max_size(src), i = decltype(N){0}; i < N; ++i) {
                                                                                // [bitset.members]/19
                        BOOST_CHECK_EQUAL(contains(dst, i), i == pos ? false : contains(src, i));
                }
                BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(is));     // [bitset.members]/20
        }
};

struct op_compl
{
        template<class IntSet>
        constexpr auto operator()(IntSet const& a) const noexcept
        {
                auto expected = a; complement(expected);
                auto const& ret = ~a;

                BOOST_CHECK_NE(std::addressof(ret), std::addressof(expected));  // [bitset.members]/21
                BOOST_CHECK(ret == expected);                                   // [bitset.members]/22
                BOOST_CHECK(~ret == a);                                         // involution
        }

        template<class IntSet>
        constexpr auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                BOOST_CHECK(~(a | b) == (~a & ~b));                             // De Morgan's Laws
                BOOST_CHECK(~(a & b) == (~a | ~b));                             // De Morgan's Laws
        }
};

struct fn_complement
{
        template<class IntSet>
        auto operator()(IntSet& is) const noexcept
        {
                auto const src = is;
                auto const& dst = complement(is);

                for (auto N = max_size(src), i = decltype(N){0}; i < N; ++i) {
                        BOOST_CHECK_NE(contains(dst, i), contains(src, i));     // [bitset.members]/23
                }
                BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(is));     // [bitset.members]/24

                complement(is);
                BOOST_CHECK(is == src);                                         // involution
        }
};

struct fn_replace
{
        template<class IntSet>
        auto operator()(IntSet& is) const noexcept
        {                                                                       // [bitset.members]/25
                BOOST_CHECK_THROW(replace(is, max_size(is)), std::out_of_range);
        }

        template<class IntSet, class SizeType>
        auto operator()(IntSet& is, SizeType const pos) const
        {
                auto const src = is;
                auto const& dst = replace(is, pos);
                for (auto N = max_size(src), i = decltype(N){0}; i < N; ++i) {
                                                                                // [bitset.members]/26
                        BOOST_CHECK_EQUAL(contains(dst, i), i == pos ? !contains(src, i) : contains(src, i));
                }
                BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(is));     // [bitset.members]/27
        }
};

// [bitset.members]/28-33 describe to_ulong, to_ullong, to_string

struct fn_size
{
        template<class IntSet>
        auto operator()(IntSet const& is) noexcept
        {
                auto expected = decltype(size(is)){0};
                for (auto N = max_size(is), i = decltype(N){0}; i < N; ++i) {
                        expected += contains(is, i);
                }
                BOOST_CHECK_EQUAL(size(is), expected);                          // [bitset.members]/34

                if constexpr (tti::has_forward_iterator_v<IntSet>) {
                        BOOST_CHECK_EQUAL(static_cast<std::ptrdiff_t>(size(is)), std::distance(begin(is), end(is)));
                }
        }
};

struct fn_max_size
{
        template<class IntSet>
        auto operator()(IntSet const& is [[maybe_unused]]) const noexcept
        {
                if constexpr (!tti::has_resize_v<IntSet>) {
                        BOOST_CHECK_EQUAL(max_size(is), max_size(IntSet{}));    // [bitset.members]/35
                }
        }
};

struct op_equal_to
{
        template<class IntSet>
        auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                BOOST_CHECK_EQUAL(max_size(a), max_size(b));
                auto expected = true;
                for (auto N = max_size(a), i = decltype(N){0}; i < N; ++i) {
                        expected &= contains(a, i) == contains(b, i);
                }
                BOOST_CHECK_EQUAL(a == b, expected);                            // [bitset.members]/36

                if constexpr (tti::has_forward_iterator_v<IntSet>) {
                        BOOST_CHECK_EQUAL(a == b, std::equal(begin(a), end(a), begin(b), end(b)));
                }
        }
};

struct op_not_equal_to
{
        template<class IntSet>
        auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                BOOST_CHECK_EQUAL(a != b, !(a == b));                           // [bitset.members]/37
        }
};

struct op_less
{
        template<class IntSet>
        auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                BOOST_CHECK_EQUAL(max_size(a), max_size(b));
                auto expected = false;
                for (auto N = max_size(a), i = decltype(N){0}; i < N; ++i) {
                        auto const r = N - 1 - i;
                        if (!contains(a, r) && contains(b, r)) { expected = true; break; }
                        if (!contains(b, r) && contains(a, r)) {                  break; }
                }
                BOOST_CHECK_EQUAL(a < b, expected);

                if constexpr (tti::has_forward_iterator_v<IntSet>) {
                        BOOST_CHECK_EQUAL(a < b, std::lexicographical_compare(begin(a), end(a), begin(b), end(b)));
                }
        }
};

struct op_greater
{
        template<class IntSet>
        auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                BOOST_CHECK_EQUAL(a > b, b < a);
        }
};

struct op_less_equal
{
        template<class IntSet>
        auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                BOOST_CHECK_EQUAL(a <= b, !(b < a));
        }
};

struct op_greater_equal
{
        template<class IntSet>
        auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                BOOST_CHECK_EQUAL(a >= b, !(a < b));
        }
};

struct fn_contains
{
        template<class IntSet>
        auto operator()(IntSet const& is) const noexcept
        {                                                                       // [bitset.members]/38
                BOOST_CHECK_THROW(contains(is, max_size(is)), std::out_of_range);
        }

        template<class IntSet, class SizeType>
        auto operator()(IntSet const& is, SizeType const pos) const noexcept
        {
                auto value = is; clear(value); insert(value, pos);
                for (auto N = max_size(is), i = decltype(N){0}; i < N; ++i) {
                        BOOST_CHECK_EQUAL(contains(value, i), i == pos);        // [bitset.members]/39
                }

                if constexpr (tti::has_forward_iterator_v<IntSet>) {
                        BOOST_CHECK_EQUAL(contains(is, pos), std::find(begin(is), end(is), pos) != end(is));
                }
        }
};

struct fn_full
{
        template<class IntSet>
        auto operator()(IntSet const& is) const noexcept
        {
                BOOST_CHECK_EQUAL(full(is), size(is) == max_size(is));          // [bitset.members]/40
        }
};

struct fn_not_empty
{
        template<class IntSet>
        auto operator()(IntSet const& is) const noexcept
        {
                BOOST_CHECK_EQUAL(not_empty(is), size(is) != 0);                // [bitset.members]/41
        }
};

struct fn_empty
{
        template<class IntSet>
        auto operator()(IntSet const& is) const noexcept
        {
                BOOST_CHECK_EQUAL(empty(is), size(is) == 0);                    // [bitset.members]/42
        }
};

struct op_shift_left
{
        template<class IntSet, class SizeType>
        auto operator()(IntSet const& is, SizeType const n) const
        {
                BOOST_CHECK_LE(n, max_size(is));
                auto expected = is; expected <<= n;
                BOOST_CHECK(is << n == expected);                               // [bitset.members]/43

                if constexpr (tti::has_hinted_insert_v<IntSet>) {
                        auto const lhs = is << n;
                        std::set<int> tmp, rhs;
                        std::transform(is.begin(), is.end(), std::inserter(tmp, tmp.end()), [=](auto const x) {
                                return x + n;
                        });
                        std::copy_if(tmp.begin(), tmp.end(), std::inserter(rhs, rhs.end()), [&](auto const x) {
                                return x < is.max_size();
                        });
                        BOOST_CHECK_EQUAL_COLLECTIONS(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
                }
        }
};

struct op_shift_right
{
        template<class IntSet, class SizeType>
        auto operator()(IntSet const& is, SizeType const n) const
        {
                BOOST_CHECK_LE(n, max_size(is));
                auto expected = is; expected >>= n;
                BOOST_CHECK(is >> n == expected);                               // [bitset.members]/44

                if constexpr (tti::has_hinted_insert_v<IntSet>) {
                        auto const lhs = is >> n;
                        std::set<int> tmp, rhs;
                        std::transform(is.begin(), is.end(), std::inserter(tmp, tmp.end()), [=](auto const x) {
                                return x - n;
                        });
                        std::copy_if(tmp.begin(), tmp.end(), std::inserter(rhs, rhs.end()), [](auto const x) {
                                return 0 <= x;
                        });
                        BOOST_CHECK_EQUAL_COLLECTIONS(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
                }
        }
};

struct op_at
{
        template<class IntSet, class SizeType>
        auto operator()(IntSet const& is, SizeType const pos) const
        {
                BOOST_CHECK(0 <= pos && pos < max_size(is));                    // [bitset.members]/45
                BOOST_CHECK_EQUAL(at(is, pos), contains(is, pos));              // [bitset.members]/46
                                                                                // [bitset.members]/47: at(is, max_size(is)) does not throw
        }

        template<class IntSet, class SizeType>
        auto operator()(IntSet& is, SizeType const pos) const
        {
                BOOST_CHECK(0 <= pos && pos < max_size(is));                    // [bitset.members]/48
                BOOST_CHECK_EQUAL(at(is, pos), contains(is, pos));              // [bitset.members]/49
                                                                                // [bitset.members]/50: at(is, max_size(is), val) does not throw
        }

        template<class IntSet, class SizeType>
        auto operator()(IntSet& is, SizeType const pos, bool const val) const
        {
                BOOST_CHECK(0 <= pos && pos < max_size(is));                    // [bitset.members]/48
                auto src = is; insert(src, pos, val);
                at(is, pos, val);
                BOOST_CHECK(is == src);                                         // [bitset.members]/49
                                                                                // [bitset.members]/50: at(is, max_size(is), val) does not throw
        }
};


// [bitset.hash]/1 describes std::hash specialization

struct op_bitand
{
        template<class IntSet>
        constexpr auto operator()(IntSet const& a) const noexcept
        {
                BOOST_CHECK((a & a) == a);                                      // idempotent
        }

        template<class IntSet>
        auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                auto expected = a; expected &= b;
                BOOST_CHECK((a & b) == expected);                               // [bitset.operators]/1
                BOOST_CHECK((a & b) == (b & a));                                // commutative
                BOOST_CHECK(is_subset_of(a & b, a));
                BOOST_CHECK(is_subset_of(a & b, b));
                BOOST_CHECK_LE(size(a & b), std::min(size(a), size(b)));

                if constexpr (tti::has_hinted_insert_v<IntSet>) {
                        auto const lhs = a & b;
                        IntSet rhs;
                        std::set_intersection(a.cbegin(), a.cend(), b.cbegin(), b.cend(), std::inserter(rhs, rhs.end()));
                        BOOST_CHECK(lhs == rhs);
                }
        }

        template<class IntSet>
        constexpr auto operator()(IntSet const& a, IntSet const& b, IntSet const& c) const noexcept
        {
                BOOST_CHECK(((a & b) & c) == (a & (b & c)));                    // associative
                BOOST_CHECK((a & (b | c)) == ((a & b) | (a & c)));              // distributive
        }
};

struct op_bitor
{
        template<class IntSet>
        constexpr auto operator()(IntSet const& a) const noexcept
        {
                BOOST_CHECK((a | a) == a);                                      // idempotent
        }

        template<class IntSet>
        auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                auto expected = a; expected |= b;
                BOOST_CHECK((a | b) == expected);                               // [bitset.operators]/2
                BOOST_CHECK((a | b) == (b | a));                                // commutative
                BOOST_CHECK(is_subset_of(a, a | b));
                BOOST_CHECK(is_subset_of(b, a | b));
                BOOST_CHECK_EQUAL(size(a | b), size(a) + size(b) - size(a & b));

                if constexpr (tti::has_hinted_insert_v<IntSet>) {
                        auto const lhs = a | b;
                        IntSet rhs;
                        std::set_union(a.begin(), a.end(), b.begin(), b.end(), std::inserter(rhs, rhs.end()));
                        BOOST_CHECK(lhs == rhs);
                }
        }

        template<class IntSet>
        constexpr auto operator()(IntSet const& a, IntSet const& b, IntSet const& c) const noexcept
        {
                BOOST_CHECK(((a | b) | c) == (a | (b | c)));                    // associative
                BOOST_CHECK((a | (b & c)) == ((a | b) & (a | c)));              // distributive
        }
};

struct op_xor
{
        template<class IntSet>
        constexpr auto operator()(IntSet const& a) const noexcept
        {
                BOOST_CHECK(empty(a ^ a));                                      // nilpotent
        }

        template<class IntSet>
        auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                auto expected = a; expected ^= b;
                BOOST_CHECK((a ^ b) == expected);                               // [bitset.operators]/3
                BOOST_CHECK((a ^ b) == (b ^ a));                                // commutative
                BOOST_CHECK((a ^ b) == ((a - b) | (b - a)));
                BOOST_CHECK((a ^ b) == (a | b) - (a & b));

                if constexpr (tti::has_hinted_insert_v<IntSet>) {
                        auto const lhs = a ^ b;
                        IntSet rhs;
                        std::set_symmetric_difference(a.begin(), a.end(), b.begin(), b.end(), std::inserter(rhs, rhs.end()));
                        BOOST_CHECK(lhs == rhs);
                }
        }

        template<class IntSet>
        constexpr auto operator()(IntSet const& a, IntSet const& b, IntSet const& c) const noexcept
        {
                BOOST_CHECK(((a ^ b) ^ c) == (a ^ (b ^ c)));                    // associative
                BOOST_CHECK((a & (b ^ c)) == ((a & b) ^ (a & c)));              // distributive
        }
};

struct op_minus
{
        template<class IntSet>
        constexpr auto operator()(IntSet const& a) const noexcept
        {
                BOOST_CHECK(empty(a - a));                                      // nilpotent
        }

        template<class IntSet>
        auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                if constexpr (tti::has_op_minus_assign_v<IntSet>) {
                        auto expected = a; expected -= b;
                        BOOST_CHECK(a - b == expected);
                }

                BOOST_CHECK(a - b == (a & ~b));
                BOOST_CHECK(~(a - b) == (~a | b));
                BOOST_CHECK(~a - ~b == b - a);
                BOOST_CHECK(a - b == (a | b) - b);
                BOOST_CHECK(a - b == a - (a & b));
                BOOST_CHECK(is_subset_of(a - b, a));
                BOOST_CHECK(disjoint(a - b, b));

                if constexpr (tti::has_hinted_insert_v<IntSet>) {
                        auto const lhs = a - b;
                        IntSet rhs;
                        std::set_difference(a.begin(), a.end(), b.begin(), b.end(), std::inserter(rhs, rhs.end()));
                        BOOST_CHECK(lhs == rhs);
                }
        }
};

struct fn_is_subset_of
{
        template<class IntSet>
        auto operator()(IntSet const& a) const noexcept
        {
                BOOST_CHECK(is_subset_of(a, a));                                // reflexive
        }

        template<class IntSet>
        auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                BOOST_CHECK_EQUAL(max_size(a), max_size(b));
                auto expected = true;
                for (auto N = max_size(a), i = decltype(N){0}; i < N; ++i) {
                        expected &= !contains(a, i) || contains(b, i);
                }
                BOOST_CHECK_EQUAL(is_subset_of(a, b), expected);

                if constexpr (tti::has_forward_iterator_v<IntSet>) {
                        BOOST_CHECK_EQUAL(is_subset_of(a, b), std::includes(begin(a), end(a), begin(b), end(b)));
                }
        }

        template<class IntSet>
        auto operator()(IntSet const& a, IntSet const& b, IntSet const& c) const noexcept
        {                                                                       // transitive
                BOOST_CHECK(!(is_subset_of(a, b) && is_subset_of(b, c)) || is_subset_of(a, c));
        }
};

struct fn_is_superset_of
{
        template<class IntSet>
        auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                BOOST_CHECK_EQUAL(is_superset_of(a, b), is_subset_of(b, a));
        }
};

struct fn_is_proper_subset_of
{
        template<class IntSet>
        auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                BOOST_CHECK_EQUAL(is_proper_subset_of(a, b), is_subset_of(a, b) && !is_subset_of(b, a));
        }
};

struct fn_is_proper_superset_of
{
        template<class IntSet>
        auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                BOOST_CHECK_EQUAL(is_proper_superset_of(a, b), is_proper_subset_of(b, a));
        }
};

struct fn_intersects
{
        template<class IntSet>
        constexpr auto operator()(IntSet const& a) const noexcept
        {
                BOOST_CHECK(intersects(a, a) || empty(a));
        }

        template<class IntSet>
        constexpr auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                BOOST_CHECK_EQUAL(intersects(a, b), not_empty(a & b));
                BOOST_CHECK_EQUAL(intersects(a, b), intersects(b, a));
        }
};

struct fn_disjoint
{
        template<class IntSet>
        constexpr auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                BOOST_CHECK_EQUAL(disjoint(a, b), !intersects(a, b));
        }
};

// [bitset.operators]/4-8 describe operator>> and operator<<

}       // namespace xstd
