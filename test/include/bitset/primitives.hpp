#pragma once

//          Copyright Rein Halbersma 2014-2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <concepts.hpp>                 // insertable, resizeable
#include <iostream.hpp>                 // operator<<, operator>>
#include <boost/test/unit_test.hpp>     // BOOST_CHECK, BOOST_CHECK_EQUAL, BOOST_CHECK_EQUAL_COLLECTIONS, BOOST_CHECK_LE, BOOST_CHECK_NE, BOOST_CHECK_THROW
#include <cstddef>                      // ptrdiff_t, size_t
#include <iterator>                     // distance, inserter
#include <memory>                       // addressof
#include <ranges>                       // copy, equal, find, includes, set_difference, set_intersection, set_symmetric_difference, set_union, views::filter, views::transform
#include <set>                          // set
#include <sstream>                      // stringstream
#include <stdexcept>                    // out_of_range

namespace xstd {

template<class BitSet>
struct constructor
{
        auto operator()() const noexcept
        {
                BitSet bs;
                BOOST_CHECK(none(bs));                                          // [bitset.cons]/1
        }

        // [bitset.cons]/2-7 describe constructors taking unsigned long long, basic_string and char const*
};

struct op_bitand_assign
{
        auto operator()(auto& lhs, auto const& rhs) const noexcept
        {
                BOOST_CHECK_EQUAL(fn_size(lhs), fn_size(rhs));
                auto const src = lhs;
                auto const& dst = lhs &= rhs;

                for (auto N = fn_size(src), i = decltype(N)(0); i < N; ++i) {
                                                                                // [bitset.members]/1
                        BOOST_CHECK_EQUAL(at(dst, i), !at(rhs, i) ? false : at(src, i));
                }
                BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(lhs));    // [bitset.members]/2
        }
};

struct op_bitor_assign
{
        auto operator()(auto& lhs, auto const& rhs) const noexcept
        {
                BOOST_CHECK_EQUAL(fn_size(lhs), fn_size(rhs));
                auto const src = lhs;
                auto const& dst = lhs |= rhs;

                for (auto N = fn_size(src), i = decltype(N)(0); i < N; ++i) {
                                                                                // [bitset.members]/3
                        BOOST_CHECK_EQUAL(at(dst, i), at(rhs, i) ? true : at(src, i));
                }
                BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(lhs));    // [bitset.members]/4
        }
};

struct op_xor_assign
{
        auto operator()(auto& lhs, auto const& rhs) const noexcept
        {
                BOOST_CHECK_EQUAL(fn_size(lhs), fn_size(rhs));
                auto const src = lhs;
                auto const& dst = lhs ^= rhs;

                for (auto N = fn_size(src), i = decltype(N)(0); i < N; ++i) {
                                                                                // [bitset.members]/5
                        BOOST_CHECK_EQUAL(at(dst, i), at(rhs, i) ? !at(src, i) : at(src, i));
                }
                BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(lhs));    // [bitset.members]/6
        }
};

struct op_minus_assign
{
        auto operator()(auto& lhs, auto const& rhs) const noexcept
        {
                BOOST_CHECK_EQUAL(fn_size(lhs), fn_size(rhs));
                auto const src = lhs;
                auto const& dst = lhs -= rhs;

                for (auto N = fn_size(src), i = decltype(N)(0); i < N; ++i) {
                        BOOST_CHECK_EQUAL(at(dst, i), at(rhs, i) ? false : at(src, i));
                }
                BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(lhs));
        }
};

struct op_shift_left_assign
{
        auto operator()(auto& bs, std::size_t pos) const
        {
                auto const src = bs;
                auto const& dst = left_shift_assign(bs, pos);

                for (auto N = fn_size(src), I = decltype(N)(0); I < N; ++I) {
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
        auto operator()(auto& bs, std::size_t pos) const
        {
                auto const src = bs;
                auto const& dst = right_shift_assign(bs, pos);

                for (auto N = fn_size(src), I = decltype(N)(0); I < N; ++I) {
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
        auto operator()(auto& bs) const noexcept
        {
                auto const& dst = set(bs);

                BOOST_CHECK(all(bs));                                           // [bitset.members]/11
                BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(bs));     // [bitset.members]/12
        }

        auto operator()(auto& bs, std::size_t pos, bool val = true) const
        {
                if (pos < fn_size(bs)) {
                        auto const src = bs;
                        auto const& dst = set(bs, pos, val);

                        for (auto N = fn_size(src), i = decltype(N)(0); i < N; ++i) {
                                                                                // [bitset.members]/13
                                BOOST_CHECK_EQUAL(at(dst, i), i == pos ? val : at(src, i));
                        }                                                       // [bitset.members]/14
                        BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(bs));
                } else {                                                        // [bitset.members]/15
                        BOOST_CHECK_THROW(set(bs, pos, val), std::out_of_range);                        
                }
        }
};

struct mem_reset
{
        auto operator()(auto& bs) const noexcept
        {
                auto const& dst = reset(bs);

                BOOST_CHECK(none(bs));                                          // [bitset.members]/16
                BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(bs));     // [bitset.members]/17
        }

        auto operator()(auto& bs, std::size_t pos) const
        {
                if (pos < fn_size(bs)) {
                        auto const src = bs;
                        auto const& dst = reset(bs, pos);

                        for (auto N = fn_size(src), i = decltype(N)(0); i < N; ++i) {
                                                                                // [bitset.members]/18
                                BOOST_CHECK_EQUAL(at(dst, i), i == pos ? false : at(src, i));
                        }                                                       // [bitset.members]/19
                        BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(bs));
                } else {                                                        
                        BOOST_CHECK_THROW(reset(bs, pos), std::out_of_range);   // [bitset.members]/20
                }                        
        }
};

struct op_compl
{
        auto operator()(auto const& a) const noexcept
        {
                auto expected = a; flip(expected);
                auto const& ret = ~a;

                BOOST_CHECK_NE(std::addressof(ret), std::addressof(expected));  // [bitset.members]/21
                BOOST_CHECK_EQUAL(ret, expected);                               // [bitset.members]/22
                BOOST_CHECK_EQUAL(~ret, a);                                     // involution
        }

        auto operator()(auto const& a, auto const& b) const noexcept
        {
                BOOST_CHECK_EQUAL(~(a | b), (~a & ~b));                         // De Morgan's Laws
                BOOST_CHECK_EQUAL(~(a & b), (~a | ~b));                         // De Morgan's Laws
        }
};

struct mem_flip
{
        auto operator()(auto& bs) const noexcept
        {
                auto const src = bs;
                auto const& dst = flip(bs);

                for (auto N = fn_size(src), i = decltype(N)(0); i < N; ++i) {
                        BOOST_CHECK_NE(at(dst, i), at(src, i));                 // [bitset.members]/23
                }
                BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(bs));     // [bitset.members]/24

                flip(bs);
                BOOST_CHECK_EQUAL(bs, src);                                     // involution
        }

        auto operator()(auto& bs, std::size_t pos) const
        {
                if (pos < fn_size(bs)) {
                        auto const src = bs;
                        auto const& dst = flip(bs, pos);
                        for (auto N = fn_size(src), i = decltype(N)(0); i < N; ++i) {
                                                                                // [bitset.members]/25
                                BOOST_CHECK_EQUAL(at(dst, i), i == pos ? !at(src, i) : at(src, i));
                        }                                                       // [bitset.members]/26
                        BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(bs));
                } else {
                        BOOST_CHECK_THROW(flip(bs, pos), std::out_of_range);    // [bitset.members]/27
                }
        }
};

// [bitset.members]/28-33 describe to_ulong, to_ullong, to_string

struct mem_count
{
        template<class BitSet>
        auto operator()(BitSet const& bs) const noexcept
        {
                auto expected = decltype(count(bs))(0);
                for (auto N = fn_size(bs), i = decltype(N)(0); i < N; ++i) {
                        expected += at(bs, i);
                }
                BOOST_CHECK_EQUAL(count(bs), expected);                         // [bitset.members]/34

                if constexpr (std::ranges::range<BitSet>) {
                        BOOST_CHECK_EQUAL(static_cast<std::ptrdiff_t>(count(bs)), std::distance(begin(bs), end(bs)));
                }
        }
};

struct mem_size
{
        template<class BitSet>
        auto operator()(BitSet const& bs [[maybe_unused]]) const noexcept
        {
                if constexpr (!resizeable<BitSet>) {
                        BOOST_CHECK_EQUAL(fn_size(bs), fn_size(BitSet()));      // [bitset.members]/35
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
                for (auto N = fn_size(a), i = decltype(N)(0); i < N; ++i) {
                        expected &= at(a, i) == at(b, i);
                }
                BOOST_CHECK_EQUAL(a == b, expected);                            // [bitset.members]/36

                if constexpr (std::ranges::range<BitSet>) {
                        BOOST_CHECK_EQUAL(a == b, std::ranges::equal(a, b));
                }
        }
};

struct mem_test
{
        template<class BitSet>
        auto operator()(BitSet const& bs, std::size_t pos) const noexcept
        {
                if (pos < fn_size(bs)) {
                        auto value = bs; reset(value); set(value, pos);
                        for (auto N = fn_size(bs), i = decltype(N)(0); i < N; ++i) {
                                BOOST_CHECK_EQUAL(test(value, i), i == pos);    // [bitset.members]/37
                        }

                        if constexpr (std::ranges::range<BitSet>) {
                                using value_type = decltype(begin(bs))::value_type;
                                BOOST_CHECK_EQUAL(test(bs, pos), std::ranges::find(bs, static_cast<value_type>(pos)) != end(bs));
                        }
                } else {
                        BOOST_CHECK_THROW(test(bs, pos), std::out_of_range);    // [bitset.members]/38
                }
                 
        }
};

struct mem_all
{
        auto operator()(auto const& bs) const noexcept
        {
                BOOST_CHECK_EQUAL(all(bs), count(bs) == fn_size(bs));           // [bitset.members]/39
        }
};

struct mem_any
{
        auto operator()(auto const& bs) const noexcept
        {
                BOOST_CHECK_EQUAL(any(bs), count(bs) != 0);                     // [bitset.members]/40
        }
};

struct mem_none
{
        auto operator()(auto const& bs) const noexcept
        {
                BOOST_CHECK_EQUAL(none(bs), count(bs) == 0);                    // [bitset.members]/41
        }
};

struct op_shift_left
{
        template<class BitSet>
        auto operator()(BitSet const& bs, std::size_t pos) const
        {
                auto expected = bs; left_shift_assign(expected, pos);
                BOOST_CHECK_EQUAL(left_shift(bs, pos), expected);               // [bitset.members]/42

                if constexpr (insertable<BitSet>) {
                        auto const lhs = left_shift(bs, pos);
                        BitSet rhs;
                        std::ranges::copy(
                                bs |
                                std::views::transform([=](auto x) {
                                        return x + static_cast<value_t<BitSet>>(pos);
                                }) |
                                std::views::filter([&](auto x) {
                                        return x < static_cast<value_t<BitSet>>(fn_size(bs));
                                }),
                                std::inserter(rhs, rhs.end())
                        );
                        BOOST_CHECK_EQUAL_COLLECTIONS(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
                }
        }
};

struct op_shift_right
{
        template<class BitSet>
        auto operator()(BitSet const& bs, std::size_t pos) const
        {
                auto expected = bs; right_shift_assign(expected, pos);
                BOOST_CHECK_EQUAL(right_shift(bs, pos), expected);              // [bitset.members]/43

                if constexpr (insertable<BitSet>) {
                        auto const lhs = right_shift(bs, pos);
                        BitSet rhs;
                        std::ranges::copy(
                                bs |
                                std::views::transform([=](auto x) {
                                        return x - static_cast<value_t<BitSet>>(pos);
                                }) |
                                std::views::filter([](auto x) {
                                        return 0 <= x;
                                }),
                                std::inserter(rhs, rhs.end())
                        );
                        BOOST_CHECK_EQUAL_COLLECTIONS(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
                }
        }
};

struct op_at
{
        auto operator()(auto const& bs, std::size_t pos) const
        {
                BOOST_CHECK(pos < fn_size(bs));                                 // [bitset.members]/44
                BOOST_CHECK_EQUAL(at(bs, pos), test(bs, pos));                  // [bitset.members]/45
                //BOOST_CHECK_NO_THROW(at(bs, pos));                              // [bitset.members]/46
        }

        auto operator()(auto& bs, std::size_t pos) const
        {
                BOOST_CHECK(fn_size(bs));                                       // [bitset.members]/47
                BOOST_CHECK_EQUAL(at(bs, pos), test(bs, pos));                  // [bitset.members]/48
                //BOOST_CHECK_NO_THROW(at(bs, pos));                              // [bitset.members]/49
        }

        auto operator()(auto& bs, std::size_t pos, bool val) const
        {
                BOOST_CHECK(pos < fn_size(bs));                                 // [bitset.members]/47
                auto src = bs; set(src, pos, val);
                at(bs, pos, val);
                BOOST_CHECK_EQUAL(bs, src);                                     // [bitset.members]/48
                //BOOST_CHECK_NO_THROW(at(bs, pos, val));                         // [bitset.members]/49
        }
};

// [bitset.hash]/1 stipulates a std::hash<std::bitset<N>> specialization

struct op_bitand
{
        auto operator()(auto const& a) const noexcept
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

                if constexpr (insertable<BitSet>) {
                        auto const lhs = a & b;
                        BitSet rhs;
                        std::ranges::set_intersection(a, b, std::inserter(rhs, rhs.end()));
                        BOOST_CHECK_EQUAL(lhs, rhs);
                }
        }

        auto operator()(auto const& a, auto const& b, auto const& c) const noexcept
        {
                BOOST_CHECK_EQUAL(((a & b) & c), (a & (b & c)));                // associative
                BOOST_CHECK_EQUAL((a & (b | c)), ((a & b) | (a & c)));          // distributive
        }
};

struct op_bitor
{
        auto operator()(auto const& a) const noexcept
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

                if constexpr (insertable<BitSet>) {
                        auto const lhs = a | b;
                        BitSet rhs;
                        std::ranges::set_union(a, b, std::inserter(rhs, rhs.end()));
                        BOOST_CHECK_EQUAL(lhs,rhs);
                }
        }

        auto operator()(auto const& a, auto const& b, auto const& c) const noexcept
        {
                BOOST_CHECK_EQUAL(((a | b) | c), (a | (b | c)));                // associative
                BOOST_CHECK_EQUAL((a | (b & c)), ((a | b) & (a | c)));          // distributive
        }
};

struct op_xor
{
        auto operator()(auto const& a) const noexcept
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

                if constexpr (insertable<BitSet>) {
                        auto const lhs = a ^ b;
                        BitSet rhs;
                        std::ranges::set_symmetric_difference(a, b, std::inserter(rhs, rhs.end()));
                        BOOST_CHECK_EQUAL(lhs, rhs);
                }
        }

        auto operator()(auto const& a, auto const& b, auto const& c) const noexcept
        {
                BOOST_CHECK_EQUAL(((a ^ b) ^ c), (a ^ (b ^ c)));                // associative
                BOOST_CHECK_EQUAL((a & (b ^ c)), ((a & b) ^ (a & c)));          // distributive
        }
};

struct op_minus
{
        auto operator()(auto const& a) const noexcept
        {
                BOOST_CHECK(none(a - a));                                       // nilpotent
        }

        template<class BitSet>
        auto operator()(BitSet const& a, BitSet const& b) const noexcept
        {
                auto expected = a; expected -= b;
                BOOST_CHECK_EQUAL(a - b, expected);

                BOOST_CHECK_EQUAL(a - b, (a & ~b));
                BOOST_CHECK_EQUAL(~(a - b), (~a | b));
                BOOST_CHECK_EQUAL(~a - ~b, b - a);
                BOOST_CHECK_EQUAL(a - b, (a | b) - b);
                BOOST_CHECK_EQUAL(a - b, a - (a & b));
                BOOST_CHECK(is_subset_of(a - b, a));
                BOOST_CHECK(is_disjoint(a - b, b));

                if constexpr (insertable<BitSet>) {
                        auto const lhs = a - b;
                        BitSet rhs;
                        std::ranges::set_difference(a, b, std::inserter(rhs, rhs.end()));
                        BOOST_CHECK_EQUAL(lhs, rhs);
                }
        }
};

struct mem_is_subset_of
{
        auto operator()(auto const& a) const noexcept
        {
                BOOST_CHECK(is_subset_of(a, a));                                // reflexive
        }

        template<class BitSet>
        auto operator()(BitSet const& a, BitSet const& b) const noexcept
        {
                BOOST_CHECK_EQUAL(fn_size(a), fn_size(b));
                auto expected = true;
                for (auto N = fn_size(a), i = decltype(N)(0); i < N; ++i) {
                        expected &= !at(a, i) || at(b, i);
                }
                BOOST_CHECK_EQUAL(is_subset_of(a, b), expected);
                BOOST_CHECK_EQUAL(is_subset_of(a, b), none(a - b));
                BOOST_CHECK_EQUAL(is_subset_of(a, b), (a & b) == a);

                if constexpr (std::ranges::range<BitSet>) {
                        BOOST_CHECK_EQUAL(is_subset_of(a, b), std::ranges::includes(a, b));
                }
        }

        auto operator()(auto const& a, auto const& b, auto const& c) const noexcept
        {                                                                       // transitive
                BOOST_CHECK(!(is_subset_of(a, b) && is_subset_of(b, c)) || is_subset_of(a, c));
        }
};

struct mem_is_superset_of
{
        auto operator()(auto const& a, auto const& b) const noexcept
        {
                BOOST_CHECK_EQUAL(is_superset_of(a, b), is_subset_of(b, a));
        }
};

struct mem_is_proper_subset_of
{
        auto operator()(auto const& a, auto const& b) const noexcept
        {
                BOOST_CHECK_EQUAL(is_proper_subset_of(a, b), is_subset_of(a, b) && !is_subset_of(b, a));
                BOOST_CHECK_EQUAL(is_proper_subset_of(a, b), is_subset_of(a, b) && a != b);
        }
};

struct mem_is_proper_superset_of
{
        auto operator()(auto const& a, auto const& b) const noexcept
        {
                BOOST_CHECK_EQUAL(is_proper_superset_of(a, b), is_proper_subset_of(b, a));
        }
};

struct mem_intersects
{
        auto operator()(auto const& a) const noexcept
        {
                BOOST_CHECK(intersects(a, a) || none(a));
        }

        auto operator()(auto const& a, auto const& b) const noexcept
        {
                BOOST_CHECK_EQUAL(intersects(a, b), any(a & b));
                BOOST_CHECK_EQUAL(intersects(a, b), intersects(b, a));
        }
};

struct fn_is_disjoint
{
        auto operator()(auto const& a) const noexcept
        {
                BOOST_CHECK(!is_disjoint(a, a) || none(a));
        }

        auto operator()(auto const& a, auto const& b) const noexcept
        {
                BOOST_CHECK_EQUAL(is_disjoint(a, b), none(a & b));
                BOOST_CHECK_EQUAL(is_disjoint(a, b), !intersects(a, b));
        }
};

struct fn_iostream
{
        template<class BitSet>
        auto operator()(BitSet const& a) const noexcept
        {
                std::stringstream sstr;
                BitSet b;
                sstr << a;
                sstr >> b;
                BOOST_CHECK_EQUAL(a, b);                                        // [bitset.operators]/4-8
        }
};

}       // namespace xstd
