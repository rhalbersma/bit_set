#pragma once

//          Copyright Rein Halbersma 2014-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <concepts.hpp>                 // dynamic
#include <boost/test/unit_test.hpp>     // BOOST_CHECK, BOOST_CHECK_EQUAL, BOOST_CHECK_EQUAL_COLLECTIONS, BOOST_CHECK_LE, BOOST_CHECK_NE, BOOST_CHECK_THROW
#include <cstddef>                      // ptrdiff_t, size_t
#include <iterator>                     // distance, inserter
#include <memory>                       // addressof
#include <set>                          // set
#include <sstream>                      // stringstream
#include <stdexcept>                    // out_of_range
#include <type_traits>                  // remove_cvref_t

namespace xstd {

template<class X>
struct constructor
{
        auto operator()() const noexcept
        {
                X a;
                BOOST_CHECK(a.none());                                          // [bitset.cons]/1
        }

        // [bitset.cons]/2-8 describe constructors taking unsigned long long, basic_string and char const*
};

struct op_bit_and_assign
{
        template<class X>
        auto operator()(X& self, const X& rhs) const noexcept
        {
                auto const src = self;
                auto const& dst = self &= rhs;
                for (auto const N = self.size(); auto i : std::views::iota(0uz, N)) {
                        BOOST_CHECK_EQUAL(dst[i], !rhs[i] ? false : src[i]);    // [bitset.members]/1
                }
                BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(self));   // [bitset.members]/2
        }
};

struct op_bit_or_assign
{
        template<class X>
        auto operator()(X& self, const X& rhs) const noexcept
        {
                auto const src = self;
                auto const& dst = self |= rhs;
                for (auto const N = self.size(); auto i : std::views::iota(0uz, N)) {
                        BOOST_CHECK_EQUAL(dst[i], rhs[i] ? true : src[i]);      // [bitset.members]/3
                }
                BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(self));   // [bitset.members]/4
        }
};

struct op_bit_xor_assign
{
        template<class X>
        auto operator()(X& self, const X& rhs) const noexcept
        {
                auto const src = self;
                auto const& dst = self ^= rhs;
                for (auto const N = self.size(); auto i : std::views::iota(0uz, N)) {
                        BOOST_CHECK_EQUAL(dst[i], rhs[i] ? !src[i] : src[i]);   // [bitset.members]/5
                }
                BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(self));   // [bitset.members]/6
        }
};

struct op_minus_assign
{
        template<class X>
        auto operator()(X& self, const X& rhs) const noexcept
        {
                auto const src = self;
                auto const& dst = self -= rhs;
                for (auto const N = self.size(); auto i : std::views::iota(0uz, N)) {
                        BOOST_CHECK_EQUAL(dst[i], rhs[i] ? false : src[i]);
                }
                BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(self));
        }
};

struct op_shift_left_assign
{
        auto operator()(auto& self, std::size_t pos) const noexcept
        {
                auto const src = self;
                auto const& dst = self <<= pos;
                for (auto const N = self.size(); auto I : std::views::iota(0uz, N)) {
                        if (I < pos) {
                                BOOST_CHECK(!dst[I]);                           // [bitset.members]/7.1
                        } else {
                                BOOST_CHECK_EQUAL(dst[I], src[I - pos]);        // [bitset.members]/7.2
                        }
                }
                BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(self));   // [bitset.members]/8
        }
};

struct op_shift_right_assign
{
        auto operator()(auto& self, std::size_t pos) const noexcept
        {
                auto const src = self;
                auto const& dst = self >>= pos;
                for (auto const N = self.size(); auto I : std::views::iota(0uz, N)) {
                        if (pos >= N - I) {
                                BOOST_CHECK(!dst[I]);                           // [bitset.members]/9.1
                        } else {
                                BOOST_CHECK_EQUAL(dst[I], src[I + pos]);        // [bitset.members]/9.2
                        }
                }
                BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(self));   // [bitset.members]/10
        }
};

struct op_shift_left
{
        template<class X>
        auto operator()(const X& self, std::size_t pos) const noexcept
        {
                BOOST_CHECK_EQUAL(self << pos, X(self) <<= pos);                // [bitset.members]/11
        }
};

struct op_shift_right
{
        template<class X>
        auto operator()(const X& self, std::size_t pos) const noexcept
        {
                BOOST_CHECK_EQUAL(self >> pos, X(self) >>= pos);                // [bitset.members]/12
        }
};

struct mem_set
{
        auto operator()(auto& self) const noexcept
        {
                auto const& dst = self.set();
                BOOST_CHECK(self.all());                                                // [bitset.members]/13
                BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(self));           // [bitset.members]/14
        }

        auto operator()(auto& self, std::size_t pos, bool val = true) const noexcept
        {
                if (auto const N = self.size(); pos < N) {
                        auto const src = self;
                        auto const& dst = self.set(pos, val);
                        for (auto i : std::views::iota(0uz, N)) {
                                BOOST_CHECK_EQUAL(dst[i], i == pos ? val : src[i]);     // [bitset.members]/15
                        }
                        BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(self));   // [bitset.members]/16
                } else {
                        BOOST_CHECK_THROW(self.set(pos, val), std::out_of_range);       // [bitset.members]/17
                }
        }
};

struct mem_reset
{
        auto operator()(auto& self) const noexcept
        {
                auto const& dst = self.reset();
                BOOST_CHECK(self.none());                                               // [bitset.members]/18
                BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(self));           // [bitset.members]/19
        }

        auto operator()(auto& self, std::size_t pos) const noexcept
        {
                if (auto const N = self.size(); pos < N) {
                        auto const src = self;
                        auto const& dst = self.reset(pos);
                        for (auto i : std::views::iota(0uz, N)) {
                                BOOST_CHECK_EQUAL(dst[i], i == pos ? false : src[i]);   // [bitset.members]/20
                        }
                        BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(self));   // [bitset.members]/21
                } else {
                        BOOST_CHECK_THROW(self.reset(pos), std::out_of_range);          // [bitset.members]/22
                }
        }
};

struct op_bit_not
{
        template<class X>
        auto operator()(const X& self) const noexcept
        {
                auto x = X(self);                                                       // [bitset.members]/23
                BOOST_CHECK_EQUAL(~self, x.flip());                                     // [bitset.members]/24
        }
};

struct mem_flip
{
        auto operator()(auto& self) const noexcept
        {
                auto const src = self;
                auto const& dst = self.flip();
                for (auto const N = self.size(); auto i : std::views::iota(0uz, N)) {
                        BOOST_CHECK_NE(dst[i], src[i]);                                 // [bitset.members]/25
                }
                BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(self));           // [bitset.members]/26
        }

        auto operator()(auto& self, std::size_t pos) const noexcept
        {
                if (auto const N = self.size(); pos < N) {
                        auto const src = self;
                        auto const& dst = self.flip(pos);
                        for (auto i : std::views::iota(0uz, N)) {
                                BOOST_CHECK_EQUAL(dst[i], i == pos ? !src[i] : src[i]); // [bitset.members]/27
                        }
                        BOOST_CHECK_EQUAL(std::addressof(dst), std::addressof(self));   // [bitset.members]/28
                } else {
                        BOOST_CHECK_THROW(self.flip(pos), std::out_of_range);           // [bitset.members]/29
                }
        }
};

struct op_at
{
        auto operator()(const auto& self, std::size_t pos) const noexcept
        {
                auto const N = self.size();
                BOOST_CHECK(pos < N);                                                   // [bitset.members]/30
                BOOST_CHECK_EQUAL(self[pos], self.test(pos));                           // [bitset.members]/31
                BOOST_CHECK_NO_THROW(static_cast<void>(self[pos]));                     // [bitset.members]/32
        }

        // auto operator()(auto& self, std::size_t pos) const
        // {
        //         BOOST_CHECK(pos < bs.size());                                       // [bitset.members]/47
        //         BOOST_CHECK_EQUAL(at(bs, pos), test(bs, pos));                  // [bitset.members]/48
        //         //BOOST_CHECK_NO_THROW(at(bs, pos));                              // [bitset.members]/49
        // }

        // auto operator()(auto& self, std::size_t pos, bool val) const
        // {
        //         BOOST_CHECK(pos < fn_size(bs));                                 // [bitset.members]/47
        //         auto src = bs; set(src, pos, val);
        //         at(bs, pos, val);
        //         BOOST_CHECK_EQUAL(bs, src);                                     // [bitset.members]/48
        //         //BOOST_CHECK_NO_THROW(at(bs, pos, val));                         // [bitset.members]/49
        // }
};


// [bitset.members]/28-33 describe conversion functions to_ulong, to_ullong, to_string

struct mem_count
{
        auto operator()(const auto& self) const noexcept
        {
                auto const N = self.size();
                BOOST_CHECK_EQUAL(
                        self.count(),
                        std::ranges::fold_left(
                                std::views::iota(0uz, N) | std::views::transform([&](auto i) {
                                        return self[i];
                                }), 0uz, std::plus<>()
                        )
                );                                                              // [bitset.members]/43
        }
};

struct mem_size
{
        template<class X>
        auto operator()(const X& self) const noexcept
        {
                if constexpr (!dynamic<X>) {
                        BOOST_CHECK_EQUAL(self.size(), X().size());             // [bitset.members]/44
                }
        }
};

struct op_equal_to
{
        template<class X>
        auto operator()(const X& self, const X& rhs) const noexcept
        {
                auto const N = self.size();
                BOOST_CHECK_EQUAL(
                        self == rhs,
                        std::ranges::all_of(std::views::iota(0uz, N), [&](auto i) {
                                return self[i] == rhs[i];
                        })
                );                                                              // [bitset.members]/45
        }
};

struct mem_test
{
        auto operator()(const auto& self, std::size_t pos) const noexcept
        {
                if (auto const N = self.size(); pos < N) {
                        BOOST_CHECK_EQUAL(self.test(pos), self[pos]);                                   // [bitset.members]/46
                } else {
                        BOOST_CHECK_THROW(static_cast<void>(self.test(pos)), std::out_of_range);        // [bitset.members]/47
                }
        }
};

struct mem_all
{
        auto operator()(const auto& self) const noexcept
        {
                BOOST_CHECK_EQUAL(self.all(), self.count() == self.size());     // [bitset.members]/48
        }
};

struct mem_any
{
        auto operator()(const auto& self) const noexcept
        {
                BOOST_CHECK_EQUAL(self.any(), self.count() != 0);               // [bitset.members]/49
        }
};

struct mem_none
{
        auto operator()(const auto& self) const noexcept
        {
                BOOST_CHECK_EQUAL(self.none(), self.count() == 0);              // [bitset.members]/50
        }
};

struct mem_is_subset_of
{
        template<class X>
        [[nodiscard]] static auto op_call(const X& lhs, const X& rhs) noexcept
        {
                if constexpr (requires { lhs.is_subset_of(rhs); }) {
                        return lhs.is_subset_of(rhs);
                } else {
                        return is_subset_of(lhs, rhs);
                }
        }

        template<class X>
        auto operator()(const X& self, const X& rhs) const noexcept
        {
                BOOST_CHECK_EQUAL(op_call(self, rhs), (self & ~rhs).none());
        }
};

struct mem_is_proper_subset_of
{
        template<class X>
        [[nodiscard]] static auto op_call(const X& lhs, const X& rhs) noexcept
        {
                if constexpr (requires { lhs.is_proper_subset_of(rhs); }) {
                        return lhs.is_proper_subset_of(rhs);
                } else {
                        return is_proper_subset_of(lhs, rhs);
                }
        }

        template<class X>
        auto operator()(const X& self, const X& rhs) const noexcept
        {
                BOOST_CHECK_EQUAL(op_call(self, rhs), mem_is_subset_of::op_call(self, rhs) && self != rhs);
        }
};

struct mem_intersects
{
        template<class X>
        [[nodiscard]] static auto op_call(const X& lhs, const X& rhs) noexcept
        {
                if constexpr (requires { lhs.intersects(rhs); }) {
                        return lhs.intersects(rhs);
                } else {
                        return intersects(lhs, rhs);
                }
        }

        template<class X>
        auto operator()(const X& self, const X& rhs) const noexcept
        {
                BOOST_CHECK_EQUAL(op_call(self, rhs), (self & rhs).any());
        }
};

// [bitset.hash]/1 stipulates a std::hash<std::bitset<N>> specialization

struct op_bit_and
{
        template<class X>
        auto operator()(const X& lhs, const X& rhs) const noexcept
        {
                BOOST_CHECK_EQUAL(lhs & rhs, X(lhs) &= rhs);                    // [bitset.operators]/1
        }
};

struct op_bit_or
{
        template<class X>
        auto operator()(const X& lhs, const X& rhs) const noexcept
        {
                BOOST_CHECK_EQUAL(lhs | rhs, X(lhs) |= rhs);                    // [bitset.operators]/2
        }
};

struct op_bit_xor
{
        template<class X>
        auto operator()(const X& lhs, const X& rhs) const noexcept
        {
                BOOST_CHECK_EQUAL(lhs ^ rhs, X(lhs) ^= rhs);                    // [bitset.operators]/3
        }
};

struct op_minus
{
        template<class X>
        auto operator()(const X& lhs, const X& rhs) const noexcept
        {
                auto nrv = lhs;
                BOOST_CHECK_EQUAL(lhs - rhs, nrv -= rhs);
        }
};

struct fn_iostream
{
        template<class X>
        auto operator()(const X& x) const noexcept
        {
                std::stringstream sstr;
                X y;
                sstr << x;
                sstr >> y;
                BOOST_CHECK_EQUAL(x, y);                                        // [bitset.operators]/4-8
        }
};

}       // namespace xstd
