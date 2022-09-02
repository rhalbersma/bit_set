#pragma once

//          Copyright Rein Halbersma 2014-2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <boost/test/unit_test.hpp>     // BOOST_CHECK, BOOST_CHECK_EQUAL
#include <algorithm>                    // includes, set_difference, set_intersection, set_symmetric_difference, set_union
#include <cstddef>                      // size_t
#include <iterator>                     // inserter
#include <ranges>                       // views::filter, views::transform
#include <type_traits>                  // remove_cvref_t

namespace xstd::parallel {

struct includes
{
        auto operator()(auto const& a, auto const& b) const noexcept
        {
                if constexpr (requires { a.is_subset_of(b); }) {
                        BOOST_CHECK_EQUAL(a.is_subset_of(b), std::ranges::includes(a, b));
                }
        }
};

struct set_union
{
        auto operator()(auto const& a, auto const& b) const noexcept
        {
                if constexpr (requires { a | b; }) {
                        auto const lhs = a | b;
                        std::remove_cvref_t<decltype(lhs)> rhs;
                        std::ranges::set_union(a, b, std::inserter(rhs, rhs.end()));
                        BOOST_CHECK(lhs == rhs);
                }
        }
};

struct set_intersection
{
        auto operator()(auto const& a, auto const& b) const noexcept
        {
                if constexpr (requires { a & b; }) {
                        auto const lhs = a & b;
                        std::remove_cvref_t<decltype(lhs)> rhs;
                        std::ranges::set_intersection(a, b, std::inserter(rhs, rhs.end()));
                        BOOST_CHECK(lhs == rhs);
                }
        }
};

struct set_difference
{
        auto operator()(auto const& a, auto const& b) const noexcept
        {
                if constexpr (requires { a - b; }) {
                        auto const lhs = a - b;
                        std::remove_cvref_t<decltype(lhs)> rhs;
                        std::ranges::set_difference(a, b, std::inserter(rhs, rhs.end()));
                        BOOST_CHECK(lhs == rhs);
                }
        }
};

struct set_symmetric_difference
{
        auto operator()(auto const& a, auto const& b) const noexcept
        {
                if constexpr (requires { a ^ b; }) {
                        auto const lhs = a ^ b;
                        std::remove_cvref_t<decltype(lhs)> rhs;
                        std::ranges::set_symmetric_difference(a, b, std::inserter(rhs, rhs.end()));
                        BOOST_CHECK(lhs == rhs);
                }
        }
};

struct transform_increment_filter
{
        auto operator()(auto const& is, int n) const
        {
                if constexpr (requires { is << n; }) {                
                        auto const lhs = is << n;
                        std::remove_cvref_t<decltype(lhs)> rhs;
                        std::ranges::copy(
                                is |
                                std::views::transform([=](auto x) {
                                        return x + n;
                                }) |
                                std::views::filter([&](auto x) {
                                        return x < static_cast<int>(is.max_size());
                                }),
                                std::inserter(rhs, rhs.end())
                        );
                        BOOST_CHECK(lhs == rhs);
                }
        }
};

struct transform_decrement_filter
{
        auto operator()(auto const& is, int n) const
        {
                if constexpr (requires { is >> n; }) {                
                        auto const lhs = is >> n;
                        std::remove_cvref_t<decltype(lhs)> rhs;
                        std::ranges::copy(
                                is |
                                std::views::transform([=](auto x) {
                                        return x - n;
                                }) |
                                std::views::filter([](auto x) {
                                        return 0 <= x;
                                }),
                                std::inserter(rhs, rhs.end())
                        );
                        BOOST_CHECK(lhs == rhs);
                }
        }
};

}       // namespace xstd::parallel
