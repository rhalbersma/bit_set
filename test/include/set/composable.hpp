#pragma once

//          Copyright Rein Halbersma 2014-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <boost/test/unit_test.hpp>             // BOOST_CHECK, BOOST_CHECK_EQUAL
#include <range/v3/view/set_algorithm.hpp>      // set_difference, set_intersection, set_symmetric_difference, set_union
#include <algorithm>                            // includes
#include <concepts>                             // integral, signed_integral, unsigned_integral
#include <ranges>                               // to
                                                // filter, transform

namespace xstd::composable {

struct includes
{
        template<class X>
        auto operator()(X const& a, X const& b) const noexcept
        {
                if constexpr (requires { a.is_subset_of(b); }) {
                        BOOST_CHECK_EQUAL(a.is_subset_of(b), std::ranges::includes(a, b));
                }
        }
};

struct set_union
{
        template<class X>
        auto operator()(X const& a, X const& b) const noexcept
        {
                if constexpr (requires { a | b; }) {
                        BOOST_CHECK((a | b) == (ranges::views::set_union(a, b) | std::ranges::to<X>()));
                }
        }
};

struct set_intersection
{
        template<class X>
        auto operator()(X const& a, X const& b) const noexcept
        {
                if constexpr (requires { a & b; }) {
                        BOOST_CHECK((a & b) == (ranges::views::set_intersection(a, b) | std::ranges::to<X>()));
                }
        }
};

struct set_difference
{
        template<class X>
        auto operator()(X const& a, X const& b) const noexcept
        {
                if constexpr (requires { a - b; }) {
                        BOOST_CHECK((a - b) == (ranges::views::set_difference(a, b) | std::ranges::to<X>()));
                }
        }
};

struct set_symmetric_difference
{
        template<class X>
        auto operator()(X const& a, X const& b) const noexcept
        {
                if constexpr (requires { a ^ b; }) {
                        BOOST_CHECK((a ^ b) == (ranges::views::set_symmetric_difference(a, b) | std::ranges::to<X>()));
                }
        }
};


struct increment
{
        template<class X, std::integral Key = typename X::key_type>
        auto operator()(X const& a, Key n) const
        {
                if constexpr (requires { a < n; }) {
                        constexpr auto N = static_cast<Key>(X::max_size());
                        BOOST_CHECK(
                                (a << n) == (a
                                        | std::views::transform([=](auto x) { return x + n;  })
                                        | std::views::filter   ([=](auto x) { return x < N; })
                                        | std::ranges::to<X>()
                                )
                        );
                }
        }
};

struct decrement
{
        template<class X, std::integral Key = typename X::key_type>
        auto operator()(X const& a, Key n) const
        {
                if constexpr (requires { a >> n; }) {
                        if constexpr (std::signed_integral<Key>) {
                                BOOST_CHECK(
                                        (a >> n) == (a
                                                | std::views::transform([=](auto x) { return x - n;  })
                                                | std::views::filter   ([ ](auto x) { return 0 <= x; })
                                                | std::ranges::to<X>()
                                        )
                                );
                        } else if constexpr (std::unsigned_integral<Key>) {
                                constexpr auto N = static_cast<Key>(X::max_size());
                                BOOST_CHECK(
                                        (a >> n) == (a
                                                | std::views::transform([=](auto x) { return x + n; })
                                                | std::views::filter   ([ ](auto x) { return x < N; })
                                                | std::ranges::to<X>()
                                        )
                                );
                        } else {
                                static_assert(false);
                        }
                }
        }
};

}       // namespace xstd::composable