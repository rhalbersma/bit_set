#pragma once

//          Copyright Rein Halbersma 2014-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <boost/test/unit_test.hpp>             // BOOST_CHECK, BOOST_CHECK_EQUAL
#include <range/v3/view/set_algorithm.hpp>      // set_difference, set_intersection, set_symmetric_difference, set_union
#include <algorithm>                            // includes
#include <ranges>                               // filter, to, transform
#include <type_traits>                          // remove_cvref_t

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
                        using set_type = std::remove_cvref_t<decltype(a)>;
                        BOOST_CHECK((a | b) == (ranges::views::set_union(a, b) | std::ranges::to<set_type>()));
                }
        }
};

struct set_intersection
{
        auto operator()(auto const& a, auto const& b) const noexcept
        {
                if constexpr (requires { a & b; }) {
                        using set_type = std::remove_cvref_t<decltype(a)>;
                        BOOST_CHECK((a & b) == (ranges::views::set_intersection(a, b) | std::ranges::to<set_type>()));
                }
        }
};

struct set_difference
{
        auto operator()(auto const& a, auto const& b) const noexcept
        {
                if constexpr (requires { a - b; }) {
                        using set_type = std::remove_cvref_t<decltype(a)>;
                        BOOST_CHECK((a - b) == (ranges::views::set_difference(a, b) | std::ranges::to<set_type>()));
                }
        }
};

struct set_symmetric_difference
{
        auto operator()(auto const& a, auto const& b) const noexcept
        {
                if constexpr (requires { a ^ b; }) {
                        using set_type = std::remove_cvref_t<decltype(a)>;
                        BOOST_CHECK((a ^ b) == (ranges::views::set_symmetric_difference(a, b) | std::ranges::to<set_type>()));
                }
        }
};

struct transform_increment_filter
{
        auto operator()(auto const& is, std::size_t n) const
        {
                if constexpr (requires { is << n; }) {
                        using set_type = std::remove_cvref_t<decltype(is)>;
                        constexpr auto N = set_type::max_size();
                        BOOST_CHECK(
                                (is << n) == (is
                                        | std::views::transform([=](auto x) { return x + n; })
                                        | std::views::filter   ([=](auto x) { return x < N; })
                                        | std::ranges::to<set_type>()
                                )
                        );
                }
        }
};

struct transform_decrement_filter
{
        auto operator()(auto const& is, std::size_t n) const
        {
                if constexpr (requires { is >> n; }) {
                        using set_type = std::remove_cvref_t<decltype(is)>;
                        constexpr auto N = set_type::max_size();                        
                        BOOST_CHECK(
                                (is >> n) == (is
                                        | std::views::transform([=](auto x) { return x - n;  })
                                        | std::views::filter   ([=](auto x) { return x < N; })
                                        | std::ranges::to<set_type>()
                                )
                        );
                }
        }
};

}       // namespace xstd::parallel
