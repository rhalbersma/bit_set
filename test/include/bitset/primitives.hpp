#pragma once

//          Copyright Rein Halbersma 2014-2019.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/bit_set.hpp>
#include <traits.hpp>                   // has_forward_iterator_v, has_hinted_insert_v, has_op_minus_assign_v, has_resize_v
#include <boost/test/unit_test.hpp>     // BOOST_CHECK, BOOST_CHECK_EQUAL, BOOST_CHECK_EQUAL_COLLECTIONS, BOOST_CHECK_LE, BOOST_CHECK_NE, BOOST_CHECK_THROW
#include <algorithm>                    // copy_if, equal, find, includes, lexicographical_compare,
                                        // set_difference, set_intersection, set_symmetric_difference, set_union, transform
#include <cstddef>                      // ptrdiff_t
#include <iterator>                     // distance, inserter
#include <memory>                       // addressof
#include <set>                          // set
#include <sstream>                      // stringstream
#include <stdexcept>                    // out_of_range
#include <iostream>

namespace xstd {

template<class BitSet>
struct constructor
{
        constexpr auto operator()() const noexcept
        {
                BitSet bs;
                BOOST_CHECK(none(bs));                                          // [bitset.cons]/1
        }

        // [bitset.cons]/2-7 describe constructors taking unsigned long long, basic_string and char const*
};

struct op_bitand_assign
{
        template<class BitSet>
        constexpr auto operator()(BitSet& lhs, BitSet const& rhs) const noexcept
        {
                BOOST_CHECK_EQUAL(fn_size(lhs), fn_size(rhs));
                auto const src = lhs;
                auto const& dst = lhs &= rhs;

                for (auto N = fn_size(src), i = decltype(N){0}; i < N; ++i) {
                                                                                // [bitset.members]/1
                        BOOST_CHECK_EQUAL(at(dst, i), !at(rhs, i) ? false : at(src, i));
                }
                BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(lhs));    // [bitset.members]/2
        }
};

struct op_bitor_assign
{
        template<class BitSet>
        constexpr auto operator()(BitSet& lhs, BitSet const& rhs) const noexcept
        {
                BOOST_CHECK_EQUAL(fn_size(lhs), fn_size(rhs));
                auto const src = lhs;
                auto const& dst = lhs |= rhs;

                for (auto N = fn_size(src), i = decltype(N){0}; i < N; ++i) {
                                                                                // [bitset.members]/3
                        BOOST_CHECK_EQUAL(at(dst, i), at(rhs, i) ? true : at(src, i));
                }
                BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(lhs));    // [bitset.members]/4
        }
};

struct op_xor_assign
{
        template<class BitSet>
        constexpr auto operator()(BitSet& lhs, BitSet const& rhs) const noexcept
        {
                BOOST_CHECK_EQUAL(fn_size(lhs), fn_size(rhs));
                auto const src = lhs;
                auto const& dst = lhs ^= rhs;

                for (auto N = fn_size(src), i = decltype(N){0}; i < N; ++i) {
                                                                                // [bitset.members]/5
                        BOOST_CHECK_EQUAL(at(dst, i), at(rhs, i) ? !at(src, i) : at(src, i));
                }
                BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(lhs));    // [bitset.members]/6
        }
};

struct op_minus_assign
{
        template<class BitSet>
        constexpr auto operator()(BitSet& lhs [[maybe_unused]], BitSet const& rhs [[maybe_unused]]) const noexcept
        {
                if constexpr (tti::has_op_minus_assign_v<BitSet>) {
                        BOOST_CHECK_EQUAL(fn_size(lhs), fn_size(rhs));
                        auto const src = lhs;
                        auto const& dst = lhs -= rhs;

                        for (auto N = fn_size(src), i = decltype(N){0}; i < N; ++i) {
                                BOOST_CHECK_EQUAL(at(dst, i), at(rhs, i) ? false : at(src, i));
                        }
                        BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(lhs));
                }
        }
};

struct op_shift_left_assign
{
        template<class BitSet, class SizeType>
        auto operator()(BitSet& bs, SizeType const pos) const
        {
                assert(0 <= pos); assert(pos < fn_size(bs));
                auto const src = bs;
                auto const& dst = bs <<= pos;

                for (auto N = fn_size(src), I = decltype(N){0}; I < N; ++I) {
                        if (I < pos) {
                                BOOST_CHECK(!at(dst, I));                       // [bitset.members]/7.1
                        } else {                                                // [bitset.members]/7.2
                                BOOST_CHECK_EQUAL(at(dst, I), at(src, I - pos));
                        }
                }
                BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(bs));     // [bitset.members]/8
        }
};

struct op_shift_right_assign
{
        template<class BitSet, class SizeType>
        auto operator()(BitSet& bs, SizeType const pos) const
        {
                assert(0 <= pos); assert(pos < fn_size(bs));
                auto const src = bs;
                auto const& dst = bs >>= pos;

                for (auto N = fn_size(src), I = decltype(N){0}; I < N; ++I) {
                        if (pos >= N - I) {
                                BOOST_CHECK(!at(dst, I));                       // [bitset.members]/9.1
                        } else {                                                // [bitset.members]/9.2
                                BOOST_CHECK_EQUAL(at(dst, I), at(src, I + pos));
                        }
                }
                BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(bs));     // [bitset.members]/10
        }
};

struct mem_set
{
        template<class BitSet>
        auto operator()(BitSet& bs) const noexcept
        {
                auto const& dst = set(bs);

                BOOST_CHECK(all(bs));                                           // [bitset.members]/11
                BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(bs));     // [bitset.members]/12
                BOOST_CHECK_THROW(set(bs, fn_size(bs)), std::out_of_range);     // [bitset.members]/13
        }

        template<class BitSet, class SizeType>
        auto operator()(BitSet& bs, SizeType const pos) const
        {
                assert(0 <= pos); assert(pos < fn_size(bs));
                auto const src = bs;
                auto const& dst = set(bs, pos);

                for (auto N = fn_size(src), i = decltype(N){0}; i < N; ++i) {
                                                                                // [bitset.members]/14
                        BOOST_CHECK_EQUAL(at(dst, i), i == pos ? true : at(src, i));
                }
                BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(bs));     // [bitset.members]/15
        }

        template<class BitSet, class SizeType>
        auto operator()(BitSet& bs, SizeType const pos, bool const val) const
        {
                auto const src = bs;
                auto const& dst = set(bs, pos, val);

                for (auto N = fn_size(src), i = decltype(N){0}; i < N; ++i) {
                                                                                // [bitset.members]/14
                        BOOST_CHECK_EQUAL(at(dst, i), i == pos ? val : at(src, i));
                }
                BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(bs));     // [bitset.members]/15
        }
};

struct mem_reset
{
        template<class BitSet>
        auto operator()(BitSet& bs) const noexcept
        {
                auto const& dst = reset(bs);

                BOOST_CHECK(none(bs));                                          // [bitset.members]/16
                BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(bs));     // [bitset.members]/17
                BOOST_CHECK_THROW(reset(bs, fn_size(bs)), std::out_of_range);   // [bitset.members]/18
        }

        template<class BitSet, class SizeType>
        auto operator()(BitSet& bs, SizeType const pos) const
        {
                assert(0 <= pos); assert(pos < fn_size(bs));
                auto const src = bs;
                auto const& dst = reset(bs, pos);

                for (auto N = fn_size(src), i = decltype(N){0}; i < N; ++i) {
                                                                                // [bitset.members]/19
                        BOOST_CHECK_EQUAL(at(dst, i), i == pos ? false : at(src, i));
                }
                BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(bs));     // [bitset.members]/20
        }
};

struct op_compl
{
        template<class BitSet>
        constexpr auto operator()(BitSet const& a) const noexcept
        {
                auto expected = a; flip(expected);
                auto const& ret = ~a;

                BOOST_CHECK_NE(std::addressof(ret), std::addressof(expected));  // [bitset.members]/21
                BOOST_CHECK_EQUAL(ret, expected);                               // [bitset.members]/22
                BOOST_CHECK_EQUAL(~ret, a);                                     // involution
        }

        template<class BitSet>
        constexpr auto operator()(BitSet const& a, BitSet const& b) const noexcept
        {
                BOOST_CHECK_EQUAL(~(a | b), (~a & ~b));                         // De Morgan's Laws
                BOOST_CHECK_EQUAL(~(a & b), (~a | ~b));                         // De Morgan's Laws
        }
};

struct mem_flip
{
        template<class BitSet>
        auto operator()(BitSet& bs) const noexcept
        {
                auto const src = bs;
                auto const& dst = flip(bs);

                for (auto N = fn_size(src), i = decltype(N){0}; i < N; ++i) {
                        BOOST_CHECK_NE(at(dst, i), at(src, i));                 // [bitset.members]/23
                }
                BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(bs));     // [bitset.members]/24

                flip(bs);
                BOOST_CHECK_EQUAL(bs, src);                                     // involution
                BOOST_CHECK_THROW(flip(bs, fn_size(bs)), std::out_of_range);    // [bitset.members]/25
        }

        template<class BitSet, class SizeType>
        auto operator()(BitSet& bs, SizeType const pos) const
        {
                assert(0 <= pos); assert(pos < fn_size(bs));
                auto const src = bs;
                auto const& dst = flip(bs, pos);
                for (auto N = fn_size(src), i = decltype(N){0}; i < N; ++i) {
                                                                                // [bitset.members]/26
                        BOOST_CHECK_EQUAL(at(dst, i), i == pos ? !at(src, i) : at(src, i));
                }
                BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(bs));     // [bitset.members]/27
        }
};

// [bitset.members]/28-33 describe to_ulong, to_ullong, to_string

struct mem_count
{
        template<class BitSet>
        auto operator()(BitSet const& bs) const noexcept
        {
                auto expected = decltype(count(bs)){0};
                for (auto N = fn_size(bs), i = decltype(N){0}; i < N; ++i) {
                        expected += at(bs, i);
                }
                BOOST_CHECK_EQUAL(count(bs), expected);                         // [bitset.members]/34

                if constexpr (tti::has_forward_iterator_v<BitSet>) {
                        BOOST_CHECK_EQUAL(static_cast<std::ptrdiff_t>(count(bs)), std::distance(begin(bs), end(bs)));
                }
        }
};

struct mem_size
{
        template<class BitSet>
        auto operator()(BitSet const& bs [[maybe_unused]]) const noexcept
        {
                if constexpr (!tti::has_resize_v<BitSet>) {
                        BOOST_CHECK_EQUAL(fn_size(bs), fn_size(BitSet{}));      // [bitset.members]/35
                }
        }
};

struct op_equal_to
{
        template<class BitSet>
        auto operator()(BitSet const& a, BitSet const& b) const noexcept
        {
                BOOST_CHECK_EQUAL(fn_size(a), fn_size(b));
                auto expected = true;
                for (auto N = fn_size(a), i = decltype(N){0}; i < N; ++i) {
                        expected &= at(a, i) == at(b, i);
                }
                BOOST_CHECK_EQUAL(a == b, expected);                            // [bitset.members]/36

                if constexpr (tti::has_forward_iterator_v<BitSet>) {
                        BOOST_CHECK_EQUAL(a == b, std::equal(begin(a), end(a), begin(b), end(b)));
                }
        }
};

struct op_not_equal_to
{
        template<class BitSet>
        auto operator()(BitSet const& a, BitSet const& b) const noexcept
        {
                BOOST_CHECK_EQUAL(a != b, !(a == b));                           // [bitset.members]/37
        }
};

struct mem_test
{
        template<class BitSet>
        auto operator()(BitSet const& bs) const noexcept
        {
                BOOST_CHECK_THROW(test(bs, fn_size(bs)), std::out_of_range);    // [bitset.members]/38
        }

        template<class BitSet, class SizeType>
        auto operator()(BitSet const& bs, SizeType const pos) const noexcept
        {
                assert(0 <= pos); assert(pos < fn_size(bs));
                auto value = bs; reset(value); set(value, pos);
                for (auto N = fn_size(bs), i = decltype(N){0}; i < N; ++i) {
                        BOOST_CHECK_EQUAL(test(value, i), i == pos);            // [bitset.members]/39
                }

                if constexpr (tti::has_forward_iterator_v<BitSet>) {
                        BOOST_CHECK_EQUAL(test(bs, pos), std::find(begin(bs), end(bs), pos) != end(bs));
                }
        }
};

struct mem_all
{
        template<class BitSet>
        auto operator()(BitSet const& bs) const noexcept
        {
                BOOST_CHECK_EQUAL(all(bs), count(bs) == fn_size(bs));           // [bitset.members]/40
        }
};

struct mem_any
{
        template<class BitSet>
        auto operator()(BitSet const& bs) const noexcept
        {
                BOOST_CHECK_EQUAL(any(bs), count(bs) != 0);                     // [bitset.members]/41
        }
};

struct mem_none
{
        template<class BitSet>
        auto operator()(BitSet const& bs) const noexcept
        {
                BOOST_CHECK_EQUAL(none(bs), count(bs) == 0);                    // [bitset.members]/42
        }
};

struct op_shift_left
{
        template<class BitSet, class SizeType>
        auto operator()(BitSet const& bs, SizeType const pos) const
        {
                assert(0 <= pos); assert(pos < fn_size(bs));
                auto expected = bs; expected <<= pos;
                BOOST_CHECK_EQUAL(bs << pos, expected);                         // [bitset.members]/43

                if constexpr (tti::has_hinted_insert_v<BitSet>) {
                        auto const lhs = bs << pos;
                        std::set<int> tmp; BitSet rhs;
                        std::transform(bs.begin(), bs.end(), std::inserter(tmp, tmp.end()), [=](auto const x) {
                                return x + pos;
                        });
                        std::copy_if(tmp.begin(), tmp.end(), std::inserter(rhs, rhs.end()), [&](auto const x) {
                                return x < fn_size(bs);
                        });
                        BOOST_CHECK_EQUAL_COLLECTIONS(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
                }
        }
};

struct op_shift_right
{
        template<class BitSet, class SizeType>
        auto operator()(BitSet const& bs, SizeType const pos) const
        {
                assert(0 <= pos); assert(pos < fn_size(bs));
                auto expected = bs; expected >>= pos;
                BOOST_CHECK_EQUAL(bs >> pos, expected);                         // [bitset.members]/44

                if constexpr (tti::has_hinted_insert_v<BitSet>) {
                        auto const lhs = bs >> pos;
                        std::set<int> tmp; BitSet rhs;
                        std::transform(bs.begin(), bs.end(), std::inserter(tmp, tmp.end()), [=](auto const x) {
                                return x - pos;
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
        template<class BitSet, class SizeType>
        auto operator()(BitSet const& bs, SizeType const pos) const
        {
                BOOST_CHECK(0 <= pos && pos < fn_size(bs));                     // [bitset.members]/45
                BOOST_CHECK_EQUAL(at(bs, pos), test(bs, pos));                  // [bitset.members]/46
                                                                                // [bitset.members]/47: at(bs, fn_size(bs)) does not throw
        }

        template<class BitSet, class SizeType>
        auto operator()(BitSet& bs, SizeType const pos) const
        {
                BOOST_CHECK(0 <= pos && pos < fn_size(bs));                     // [bitset.members]/48
                BOOST_CHECK_EQUAL(at(bs, pos), test(bs, pos));                  // [bitset.members]/49
                                                                                // [bitset.members]/50: at(bs, fn_size(bs), val) does not throw
        }

        template<class BitSet, class SizeType>
        auto operator()(BitSet& bs, SizeType const pos, bool const val) const
        {
                BOOST_CHECK(0 <= pos && pos < fn_size(bs));                     // [bitset.members]/48
                auto src = bs; set(src, pos, val);
                at(bs, pos, val);
                BOOST_CHECK_EQUAL(bs, src);                                     // [bitset.members]/49
                                                                                // [bitset.members]/50: at(bs, fn_size(bs), val) does not throw
        }
};

// [bitset.hash]/1 stipulates a std::hash<std::bitset<N>> specialization

struct op_bitand
{
        template<class BitSet>
        constexpr auto operator()(BitSet const& a) const noexcept
        {
                BOOST_CHECK_EQUAL((a & a), a);                                  // idempotent
        }

        template<class BitSet>
        auto operator()(BitSet const& a, BitSet const& b) const noexcept
        {
                auto expected = a; expected &= b;
                BOOST_CHECK_EQUAL((a & b), expected);                           // [bitset.operators]/1
                BOOST_CHECK_EQUAL((a & b), (b & a));                            // commutative
                BOOST_CHECK(is_subset_of(a & b, a));
                BOOST_CHECK(is_subset_of(a & b, b));
                BOOST_CHECK_LE(count(a & b), std::min(count(a), count(b)));

                if constexpr (tti::has_hinted_insert_v<BitSet>) {
                        auto const lhs = a & b;
                        BitSet rhs;
                        std::set_intersection(a.cbegin(), a.cend(), b.cbegin(), b.cend(), std::inserter(rhs, rhs.end()));
                        BOOST_CHECK_EQUAL(lhs, rhs);
                }
        }

        template<class BitSet>
        constexpr auto operator()(BitSet const& a, BitSet const& b, BitSet const& c) const noexcept
        {
                BOOST_CHECK_EQUAL(((a & b) & c), (a & (b & c)));                // associative
                BOOST_CHECK_EQUAL((a & (b | c)), ((a & b) | (a & c)));          // distributive
        }
};

struct op_bitor
{
        template<class BitSet>
        constexpr auto operator()(BitSet const& a) const noexcept
        {
                BOOST_CHECK_EQUAL((a | a), a);                                  // idempotent
        }

        template<class BitSet>
        auto operator()(BitSet const& a, BitSet const& b) const noexcept
        {
                auto expected = a; expected |= b;
                BOOST_CHECK_EQUAL((a | b), expected);                           // [bitset.operators]/2
                BOOST_CHECK_EQUAL((a | b), (b | a));                            // commutative
                BOOST_CHECK(is_subset_of(a, a | b));
                BOOST_CHECK(is_subset_of(b, a | b));
                BOOST_CHECK_EQUAL(count(a | b), count(a) + count(b) - count(a & b));

                if constexpr (tti::has_hinted_insert_v<BitSet>) {
                        auto const lhs = a | b;
                        BitSet rhs;
                        std::set_union(a.begin(), a.end(), b.begin(), b.end(), std::inserter(rhs, rhs.end()));
                        BOOST_CHECK_EQUAL(lhs,rhs);
                }
        }

        template<class BitSet>
        constexpr auto operator()(BitSet const& a, BitSet const& b, BitSet const& c) const noexcept
        {
                BOOST_CHECK_EQUAL(((a | b) | c), (a | (b | c)));                // associative
                BOOST_CHECK_EQUAL((a | (b & c)), ((a | b) & (a | c)));          // distributive
        }
};

struct op_xor
{
        template<class BitSet>
        constexpr auto operator()(BitSet const& a) const noexcept
        {
                BOOST_CHECK(none(a ^ a));                                       // nilpotent
        }

        template<class BitSet>
        auto operator()(BitSet const& a, BitSet const& b) const noexcept
        {
                auto expected = a; expected ^= b;
                BOOST_CHECK_EQUAL((a ^ b), expected);                           // [bitset.operators]/3
                BOOST_CHECK_EQUAL((a ^ b), (b ^ a));                            // commutative
                BOOST_CHECK_EQUAL((a ^ b), ((a - b) | (b - a)));
                BOOST_CHECK_EQUAL((a ^ b), (a | b) - (a & b));

                if constexpr (tti::has_hinted_insert_v<BitSet>) {
                        auto const lhs = a ^ b;
                        BitSet rhs;
                        std::set_symmetric_difference(a.begin(), a.end(), b.begin(), b.end(), std::inserter(rhs, rhs.end()));
                        BOOST_CHECK_EQUAL(lhs, rhs);
                }
        }

        template<class BitSet>
        constexpr auto operator()(BitSet const& a, BitSet const& b, BitSet const& c) const noexcept
        {
                BOOST_CHECK_EQUAL(((a ^ b) ^ c), (a ^ (b ^ c)));                // associative
                BOOST_CHECK_EQUAL((a & (b ^ c)), ((a & b) ^ (a & c)));          // distributive
        }
};

struct op_minus
{
        template<class BitSet>
        constexpr auto operator()(BitSet const& a) const noexcept
        {
                BOOST_CHECK(none(a - a));                                       // nilpotent
        }

        template<class BitSet>
        auto operator()(BitSet const& a, BitSet const& b) const noexcept
        {
                if constexpr (tti::has_op_minus_assign_v<BitSet>) {
                        auto expected = a; expected -= b;
                        BOOST_CHECK_EQUAL(a - b, expected);
                }

                BOOST_CHECK_EQUAL(a - b, (a & ~b));
                BOOST_CHECK_EQUAL(~(a - b), (~a | b));
                BOOST_CHECK_EQUAL(~a - ~b, b - a);
                BOOST_CHECK_EQUAL(a - b, (a | b) - b);
                BOOST_CHECK_EQUAL(a - b, a - (a & b));
                BOOST_CHECK(is_subset_of(a - b, a));
                BOOST_CHECK(disjoint(a - b, b));

                if constexpr (tti::has_hinted_insert_v<BitSet>) {
                        auto const lhs = a - b;
                        BitSet rhs;
                        std::set_difference(a.begin(), a.end(), b.begin(), b.end(), std::inserter(rhs, rhs.end()));
                        BOOST_CHECK_EQUAL(lhs, rhs);
                }
        }
};

struct fn_is_subset_of
{
        template<class BitSet>
        auto operator()(BitSet const& a) const noexcept
        {
                BOOST_CHECK(is_subset_of(a, a));                                // reflexive
        }

        template<class BitSet>
        auto operator()(BitSet const& a, BitSet const& b) const noexcept
        {
                BOOST_CHECK_EQUAL(fn_size(a), fn_size(b));
                auto expected = true;
                for (auto N = fn_size(a), i = decltype(N){0}; i < N; ++i) {
                        expected &= !at(a, i) || at(b, i);
                }
                BOOST_CHECK_EQUAL(is_subset_of(a, b), expected);

                if constexpr (tti::has_forward_iterator_v<BitSet>) {
                        BOOST_CHECK_EQUAL(is_subset_of(a, b), std::includes(begin(a), end(a), begin(b), end(b)));
                }
        }

        template<class BitSet>
        auto operator()(BitSet const& a, BitSet const& b, BitSet const& c) const noexcept
        {                                                                       // transitive
                BOOST_CHECK(!(is_subset_of(a, b) && is_subset_of(b, c)) || is_subset_of(a, c));
        }
};

struct fn_is_superset_of
{
        template<class BitSet>
        auto operator()(BitSet const& a, BitSet const& b) const noexcept
        {
                BOOST_CHECK_EQUAL(is_superset_of(a, b), is_subset_of(b, a));
        }
};

struct fn_is_proper_subset_of
{
        template<class BitSet>
        auto operator()(BitSet const& a, BitSet const& b) const noexcept
        {
                BOOST_CHECK_EQUAL(is_proper_subset_of(a, b), is_subset_of(a, b) && !is_subset_of(b, a));
        }
};

struct fn_is_proper_superset_of
{
        template<class BitSet>
        auto operator()(BitSet const& a, BitSet const& b) const noexcept
        {
                BOOST_CHECK_EQUAL(is_proper_superset_of(a, b), is_proper_subset_of(b, a));
        }
};

struct fn_intersects
{
        template<class BitSet>
        constexpr auto operator()(BitSet const& a) const noexcept
        {
                BOOST_CHECK(intersects(a, a) || none(a));
        }

        template<class BitSet>
        constexpr auto operator()(BitSet const& a, BitSet const& b) const noexcept
        {
                BOOST_CHECK_EQUAL(intersects(a, b), any(a & b));
                BOOST_CHECK_EQUAL(intersects(a, b), intersects(b, a));
        }
};

struct fn_disjoint
{
        template<class BitSet>
        constexpr auto operator()(BitSet const& a, BitSet const& b) const noexcept
        {
                BOOST_CHECK_EQUAL(disjoint(a, b), none(a & b));
                BOOST_CHECK_EQUAL(disjoint(a, b), !intersects(a, b));
        }
};

struct fn_iostream
{
        template<class BitSet>
        auto operator()(BitSet const& a) const noexcept
        {
                std::stringstream sstr; sstr << a;
                BitSet b;               sstr >> b;
                BOOST_CHECK_EQUAL(a, b);                                        // [bitset.operators]/4-8
        }
};

}       // namespace xstd
