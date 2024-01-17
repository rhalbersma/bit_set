#pragma once

//          Copyright Rein Halbersma 2014-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <concepts.hpp>                 // resizeable
#include <boost/test/unit_test.hpp>     // BOOST_CHECK, BOOST_CHECK_EQUAL, BOOST_CHECK_EQUAL_COLLECTIONS, BOOST_CHECK_LE, BOOST_CHECK_NE, BOOST_CHECK_THROW
#include <cstddef>                      // ptrdiff_t, size_t
#include <iterator>                     // distance, inserter
#include <memory>                       // addressof
#include <set>                          // set
#include <sstream>                      // stringstream
#include <stdexcept>                    // out_of_range
#include <type_traits>                  // remove_cvref_t

namespace xstd {

template<class BitSet>
struct constructor
{
        auto operator()() const noexcept
        {
                BitSet bs;
                BOOST_CHECK(bs.none());                                         // [bitset.cons]/1
        }

        // [bitset.cons]/2-7 describe constructors taking unsigned long long, basic_string and char const*
};

struct op_bit_and_assign
{
        auto operator()(auto& lhs, auto const& rhs) const noexcept
        {
                BOOST_CHECK_EQUAL(lhs.size(), rhs.size());
                auto const src = lhs;
                auto const& dst = lhs &= rhs;
                for (auto i = 0uz, N = src.size(); i < N; ++i) {
                        BOOST_CHECK_EQUAL(dst[i], !rhs[i] ? false : src[i]);    // [bitset.members]/1
                }
                BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(lhs));    // [bitset.members]/2
        }
};

struct op_bit_or_assign
{
        auto operator()(auto& lhs, auto const& rhs) const noexcept
        {
                BOOST_CHECK_EQUAL(lhs.size(), rhs.size());
                auto const src = lhs;
                auto const& dst = lhs |= rhs;
                for (auto i = 0uz, N = src.size(); i < N; ++i) {
                        BOOST_CHECK_EQUAL(dst[i], rhs[i] ? true : src[i]);      // [bitset.members]/3
                }
                BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(lhs));    // [bitset.members]/4
        }
};

struct op_bit_xor_assign
{
        auto operator()(auto& lhs, auto const& rhs) const noexcept
        {
                BOOST_CHECK_EQUAL(lhs.size(), rhs.size());
                auto const src = lhs;
                auto const& dst = lhs ^= rhs;
                for (auto i = 0uz, N = src.size(); i < N; ++i) {
                        BOOST_CHECK_EQUAL(dst[i], rhs[i] ? !src[i] : src[i]);   // [bitset.members]/5
                }
                BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(lhs));    // [bitset.members]/6
        }
};

struct op_minus_assign
{
        auto operator()(auto& lhs, auto const& rhs) const noexcept
        {
                BOOST_CHECK_EQUAL(lhs.size(), rhs.size());
                auto const src = lhs;
                auto const& dst = lhs -= rhs;
                for (auto i = 0uz, N = src.size(); i < N; ++i) {
                        BOOST_CHECK_EQUAL(dst[i], rhs[i] ? false : src[i]);
                }
                BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(lhs));
        }
};

struct op_shift_left_assign
{
        auto operator()(auto& bs, std::size_t pos) const noexcept
        {
                auto const src = bs;
                auto const& dst = bs <<= pos;
                for (auto I = 0uz, N = src.size(); I < N; ++I) {
                        if (I < pos) {
                                BOOST_CHECK(!dst[I]);                           // [bitset.members]/7.1
                        } else {
                                BOOST_CHECK_EQUAL(dst[I], src[I - pos]);        // [bitset.members]/7.2
                        }
                }
                BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(bs));     // [bitset.members]/8
        }
};

struct op_shift_right_assign
{
        auto operator()(auto& bs, std::size_t pos) const noexcept
        {
                auto const src = bs;
                auto const& dst = bs >>= pos;
                for (auto I = 0uz, N = src.size(); I < N; ++I) {
                        if (pos >= N - I) {
                                BOOST_CHECK(!dst[I]);                           // [bitset.members]/9.1
                        } else {
                                BOOST_CHECK_EQUAL(dst[I], src[I + pos]);        // [bitset.members]/9.2
                        }
                }
                BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(bs));     // [bitset.members]/10
        }
};

struct mem_set
{
        auto operator()(auto& bs) const noexcept
        {
                auto const& dst = bs.set();
                BOOST_CHECK(bs.all());                                          // [bitset.members]/11
                BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(bs));     // [bitset.members]/12
        }

        auto operator()(auto& bs, std::size_t pos, bool val = true) const noexcept
        {
                if (pos < bs.size()) {
                        auto const src = bs;
                        auto const& dst = bs.set(pos, val);
                        for (auto i = 0uz, N = src.size(); i < N; ++i) {
                                                                                // [bitset.members]/13
                                BOOST_CHECK_EQUAL(dst[i], i == pos ? val : src[i]);
                        }                                                       // [bitset.members]/14
                        BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(bs));
                } else {
                        BOOST_CHECK_THROW(bs.set(pos, val), std::out_of_range); // [bitset.members]/15
                }
        }
};

struct mem_reset
{
        auto operator()(auto& bs) const noexcept
        {
                auto const& dst = bs.reset();
                BOOST_CHECK(bs.none());                                         // [bitset.members]/16
                BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(bs));     // [bitset.members]/17
        }

        auto operator()(auto& bs, std::size_t pos) const noexcept
        {
                if (pos < bs.size()) {
                        auto const src = bs;
                        auto const& dst = bs.reset(pos);
                        for (auto i = 0uz, N = src.size(); i < N; ++i) {
                                                                                // [bitset.members]/18
                                BOOST_CHECK_EQUAL(dst[i], i == pos ? false : src[i]);
                        }                                                       // [bitset.members]/19
                        BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(bs));
                } else {
                        BOOST_CHECK_THROW(bs.reset(pos), std::out_of_range);    // [bitset.members]/20
                }
        }
};

struct op_bit_not
{
        auto operator()(auto const& a) const noexcept
        {
                auto expected = a; expected.flip();
                auto const& ret = ~a;
                BOOST_CHECK_NE(std::addressof(ret), std::addressof(expected));  // [bitset.members]/21
                BOOST_CHECK_EQUAL(ret, expected);                               // [bitset.members]/22
        }
};

struct mem_flip
{
        auto operator()(auto& bs) const noexcept
        {
                auto const src = bs;
                auto const& dst = bs.flip();
                for (auto i = 0uz, N = src.size(); i < N; ++i) {
                        BOOST_CHECK_NE(dst[i], src[i]);                         // [bitset.members]/23
                }
                BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(bs));     // [bitset.members]/24
        }

        auto operator()(auto& bs, std::size_t pos) const noexcept
        {
                if (pos < bs.size()) {
                        auto const src = bs;
                        auto const& dst = bs.flip(pos);
                        for (auto i = 0uz, N = src.size(); i < N; ++i) {
                                                                                // [bitset.members]/25
                                BOOST_CHECK_EQUAL(dst[i], i == pos ? !src[i] : src[i]);
                        }                                                       // [bitset.members]/26
                        BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(bs));
                } else {
                        BOOST_CHECK_THROW(bs.flip(pos), std::out_of_range);     // [bitset.members]/27
                }
        }
};

// [bitset.members]/28-33 describe conversion functions to_ulong, to_ullong, to_string

struct mem_count
{
        auto operator()(auto const& bs) const noexcept
        {
                auto expected = 0uz;
                for (auto i = 0uz, N = bs.size(); i < N; ++i) {
                        expected += bs[i];
                }
                BOOST_CHECK_EQUAL(bs.count(), expected);                        // [bitset.members]/34
        }
};

struct mem_size
{
        auto operator()(auto const& bs) const noexcept
        {
                using set_type = std::remove_cvref_t<decltype(bs)>;
                if constexpr (!resizeable<set_type>) {
                        BOOST_CHECK_EQUAL(bs.size(), set_type().size());        // [bitset.members]/35
                }
        }
};

struct op_equal_to
{
        auto operator()(auto const& a, auto const& b) const noexcept
        {
                auto expected = true;
                for (auto i = 0uz, N = a.size(); i < N; ++i) {
                        expected &= a[i] == b[i];
                }
                BOOST_CHECK_EQUAL(a == b, expected);                            // [bitset.members]/36
        }
};

struct mem_test
{
        auto operator()(auto const& bs, std::size_t pos) const noexcept
        {
                if (pos < bs.size()) {
                        BOOST_CHECK_EQUAL(bs.test(pos), bs[pos]);               // [bitset.members]/37
                } else {                                                        // [bitset.members]/38
                        BOOST_CHECK_THROW(static_cast<void>(bs.test(pos)), std::out_of_range);
                }
        }
};

struct mem_all
{
        auto operator()(auto const& bs) const noexcept
        {
                BOOST_CHECK_EQUAL(bs.all(), bs.count() == bs.size());           // [bitset.members]/39
        }
};

struct mem_any
{
        auto operator()(auto const& bs) const noexcept
        {
                BOOST_CHECK_EQUAL(bs.any(), bs.count() != 0);                   // [bitset.members]/40
        }
};

struct mem_none
{
        auto operator()(auto const& bs) const noexcept
        {
                BOOST_CHECK_EQUAL(bs.none(), bs.count() == 0);                  // [bitset.members]/41
        }
};

struct op_shift_left
{
        auto operator()(auto const& bs, std::size_t pos) const noexcept
        {
                auto expected = bs; expected <<= pos;
                BOOST_CHECK_EQUAL(bs << pos, expected);                         // [bitset.members]/42
        }
};

struct op_shift_right
{
        auto operator()(auto const& bs, std::size_t pos) const noexcept
        {
                auto expected = bs; expected >>= pos;
                BOOST_CHECK_EQUAL(bs >> pos, expected);                         // [bitset.members]/43
        }
};

struct op_at
{
        auto operator()(auto const& bs, std::size_t pos) const noexcept
        {
                BOOST_CHECK(pos < bs.size());                                   // [bitset.members]/44
                BOOST_CHECK_EQUAL(bs[pos], bs.test(pos));                       // [bitset.members]/45
                BOOST_CHECK_NO_THROW(static_cast<void>(bs[pos]));               // [bitset.members]/46
        }

        // auto operator()(auto& bs, std::size_t pos) const
        // {
        //         BOOST_CHECK(pos < bs.size());                                       // [bitset.members]/47
        //         BOOST_CHECK_EQUAL(at(bs, pos), test(bs, pos));                  // [bitset.members]/48
        //         //BOOST_CHECK_NO_THROW(at(bs, pos));                              // [bitset.members]/49
        // }

        // auto operator()(auto& bs, std::size_t pos, bool val) const
        // {
        //         BOOST_CHECK(pos < fn_size(bs));                                 // [bitset.members]/47
        //         auto src = bs; set(src, pos, val);
        //         at(bs, pos, val);
        //         BOOST_CHECK_EQUAL(bs, src);                                     // [bitset.members]/48
        //         //BOOST_CHECK_NO_THROW(at(bs, pos, val));                         // [bitset.members]/49
        // }
};

// [bitset.hash]/1 stipulates a std::hash<std::bitset<N>> specialization

struct op_bit_and
{
        auto operator()(auto const& a, auto const& b) const noexcept
        {
                auto expected = a; expected &= b;
                BOOST_CHECK_EQUAL((a & b), expected);                           // [bitset.operators]/1
        }
};

struct op_bit_or
{
        auto operator()(auto const& a, auto const& b) const noexcept
        {
                auto expected = a; expected |= b;
                BOOST_CHECK_EQUAL((a | b), expected);                           // [bitset.operators]/2
        }
};

struct op_bit_xor
{
        auto operator()(auto const& a, auto const& b) const noexcept
        {
                auto expected = a; expected ^= b;
                BOOST_CHECK_EQUAL((a ^ b), expected);                           // [bitset.operators]/3
        }
};

struct op_minus
{
        auto operator()(auto const& a, auto const& b) const noexcept
        {
                auto expected = a; expected -= b;
                BOOST_CHECK_EQUAL(a - b, expected);
        }
};

[[nodiscard]] auto fn_is_subset_of(auto const& a, auto const& b) noexcept
{
        if constexpr (requires { a.is_subset_of(b); }) {
                return a.is_subset_of(b);
        } else {
                return xstd::is_subset_of(a, b);
        }
}

struct mem_is_subset_of
{
        auto operator()(auto const& a, auto const& b) const noexcept
        {
                BOOST_CHECK_EQUAL(fn_is_subset_of(a, b), (a & ~b).none());
        }
};

[[nodiscard]] auto fn_is_proper_subset_of(auto const& a, auto const& b) noexcept
{
        if constexpr (requires { a.is_proper_subset_of(b); }) {
                return a.is_proper_subset_of(b);
        } else {
                return xstd::is_proper_subset_of(a, b);
        }
}

struct mem_is_proper_subset_of
{
        auto operator()(auto const& a, auto const& b) const noexcept
        {
                BOOST_CHECK_EQUAL(fn_is_proper_subset_of(a, b), fn_is_subset_of(a, b) && a != b);
        }
};

[[nodiscard]] auto fn_intersects(auto const& a, auto const& b) noexcept
{
        if constexpr (requires { a.intersects(b); }) {
                return a.intersects(b);
        } else {
                return xstd::intersects(a, b);
        }
}

struct mem_intersects
{
        auto operator()(auto const& a, auto const& b) const noexcept
        {
                BOOST_CHECK_EQUAL(fn_intersects(a, b), (a & b).any());
        }
};

struct fn_iostream
{
        auto operator()(auto const& a) const noexcept
        {
                std::stringstream sstr;
                std::remove_cvref_t<decltype(a)> b;
                sstr << a;
                sstr >> b;
                BOOST_CHECK_EQUAL(a, b);                                        // [bitset.operators]/4-8
        }
};

}       // namespace xstd
