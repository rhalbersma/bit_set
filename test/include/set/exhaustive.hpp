#pragma once

//          Copyright Rein Halbersma 2014-2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>            // max
#include <array>                // array
#include <cassert>              // assert
#include <concepts>             // integral
#include <initializer_list>     // initializer_list
#include <ranges>               // to
                                // cartesian_product, iota
#include <utility>              // declval

#if defined(_MSC_VER)
        // xstd::bit_set<0> gives bogus "unreachable code" warnings
        __pragma(warning(disable: 4702))
#endif

namespace xstd {

inline constexpr auto L1 = 128uz;
inline constexpr auto L2 =  64uz;
inline constexpr auto L3 =  32uz;
inline constexpr auto L4 =  16uz;

template<class X, std::size_t Limit>
inline constexpr auto limit_v = []() {
        if constexpr (requires { X::max_size(); }) {
                return X::max_size();
        } else {
                return Limit;
        }
}();

// NOTE: these tests are O(1)

template<class X>
auto empty_set(auto fun)
{
        X a; assert(a.empty());
        fun(a);
}

template<class X, std::size_t N = limit_v<X, L1>>
auto full_set(auto fun)
{
        auto a = std::views::iota(0uz, N) | std::ranges::to<X>(); assert(a.size() == N);
        fun(a);
}

template<class X>
auto empty_set_pair(auto fun)
{
        X a; assert(a.empty());
        X b; assert(b.empty());
        fun(a, b);
}

// NOTE: these tests are O(N)

template<class X, std::size_t N = limit_v<X, L1>>
auto all_valid(auto fun)
{
        for (auto i : std::views::iota(0uz, N)) {
                fun(i);
        }
}

template<class X, std::size_t N = limit_v<X, L1>>
auto all_cardinality_sets(auto fun)
{
        for (auto i : std::views::iota(0uz, N + 1)) {
                auto a = std::views::iota(0uz, i) | std::ranges::to<X>(); assert(a.size() == i);
                fun(a);
        }
}

template<class X, std::size_t N = limit_v<X, L1>>
auto all_singleton_arrays(auto fun)
{
        for (auto i : std::views::iota(0uz, N)) {
                auto a = std::array{ i }; assert(a.size() == 1);
                fun(a);
        }
}

template<class X, std::size_t N = limit_v<X, L1>>
auto all_singleton_ilists(auto fun)
{
        for (auto i : std::views::iota(0uz, N)) {
                auto a = { i }; assert(a.size() == 1);
                fun(a);
        }
}

template<class X, std::size_t N = limit_v<X, L1>>
auto all_singleton_sets(auto fun)
{
        for (auto i : std::views::iota(0uz, N)) {
                auto a = X({ i }); assert(a.size() == 1);
                fun(a);
        }
}

// NOTE: these tests are O(N^2)

template<class X, std::size_t N = limit_v<X, L2>>
auto all_doubleton_arrays(auto fun)
{
        for (auto j : std::views::iota(1uz, std::ranges::max(N, 1uz))) {
                for (auto i : std::views::iota(0uz, j)) {
                        auto a = std::array{ i, j }; assert(a.size() == 2);
                        fun(a);
                }
        }
}

template<class X, std::size_t N = limit_v<X, L2>>
auto all_doubleton_ilists(auto fun)
{
        for (auto j : std::views::iota(1uz, std::ranges::max(N, 1uz))) {
                for (auto i : std::views::iota(0uz, j)) {
                        auto a = { i, j }; assert(a.size() == 2);
                        fun(a);
                }
        }
}

template<class X, std::size_t N = limit_v<X, L2>>
auto all_doubleton_sets(auto fun)
{
        for (auto j : std::views::iota(1uz, std::ranges::max(N, 1uz))) {
                for (auto i : std::views::iota(0uz, j)) {
                        auto a = X({ i, j }); assert(a.size() == 2);
                        fun(a);
                }
        }
}

template<class X, std::size_t N = limit_v<X, L2>>
auto all_singleton_set_pairs(auto fun)
{
        for (auto [ i, j ] : std::views::cartesian_product(
                std::views::iota(0uz, N),
                std::views::iota(0uz, N)
        )) {
                auto a = X({ i }); assert(a.size() == 1);
                auto b = X({ j }); assert(b.size() == 1);
                fun(a, b);
        }
}

// NOTE: this test is O(N^3)

template<class X, std::size_t N = limit_v<X, L3>>
auto all_singleton_set_triples(auto fun)
{
        for (auto [ i, j, k ] : std::views::cartesian_product(
                std::views::iota(0uz, N),
                std::views::iota(0uz, N),
                std::views::iota(0uz, N)
        )) {
                auto a = X({ i }); assert(a.size() == 1);
                auto b = X({ j }); assert(b.size() == 1);
                auto c = X({ k }); assert(c.size() == 1);
                fun(a, b, c);
        }
}

// NOTE: this test is O(N^4)

template<class X, std::size_t N = limit_v<X, L4>>
auto all_doubleton_set_pairs(auto fun)
{
        for (auto [ j, n ] : std::views::cartesian_product(
                std::views::iota(1uz, std::ranges::max(N, 1uz)),
                std::views::iota(1uz, std::ranges::max(N, 1uz))
        )) {
                for (auto [ i, m ] : std::views::cartesian_product(
                        std::views::iota(0uz, j),
                        std::views::iota(0uz, n)
                )) {
                        auto a = X({ i, j }); assert(a.size() == 2);
                        auto b = X({ m, n }); assert(b.size() == 2);
                        fun(a, b);
                }
        }
}

}       // namespace xstd
