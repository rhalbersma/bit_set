#pragma once

//          Copyright Rein Halbersma 2014-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>            // max
#include <array>                // array
#include <concepts>             // integral
#include <initializer_list>     // initializer_list
#include <ranges>               // to
                                // cartesian_product, iota

#if defined(_MSC_VER)
        // xstd::bit_set<0> gives bogus "unreachable code" warnings
        __pragma(warning(disable: 4702))
#endif

namespace xstd {

inline constexpr auto L1 = 128;
inline constexpr auto L2 =  64;
inline constexpr auto L3 =  32;
inline constexpr auto L4 =  16;

template<class X, auto Limit, std::integral Key = typename X::key_type>
inline constexpr auto limit_v = []() {
        if constexpr (requires { X::max_size(); }) {
                return static_cast<Key>(X::max_size());
        } else {
                return static_cast<Key>(Limit);
        }
}();

// NOTE: these tests are O(1)

template<class X>
auto empty_set(auto fun)
{
        X a; [[assume(a.empty())]];
        fun(a);
}

template<class X, std::integral Key = typename X::key_type, Key N = limit_v<X, L1>>
auto full_set(auto fun)
{
        auto a = std::views::iota(Key(0), N) | std::ranges::to<X>(); [[assume(a.size() == static_cast<std::size_t>(N))]];
        fun(a);
}

template<class X>
auto empty_set_pair(auto fun)
{
        X a; [[assume(a.empty())]];
        X b; [[assume(b.empty())]];
        fun(a, b);
}

// NOTE: these tests are O(N)

template<class X, std::integral Key = typename X::key_type, Key N = limit_v<X, L1>>
auto all_valid(auto fun)
{
        for (auto i : std::views::iota(Key(0), N)) {
                fun(i);
        }
}

template<class X, std::integral Key = typename X::key_type, Key N = limit_v<X, L1>>
auto all_cardinality_sets(auto fun)
{
        for (auto i : std::views::iota(Key(0), N + 1)) {
                auto a = std::views::iota(Key(0), i) | std::ranges::to<X>(); [[assume(a.size() == static_cast<std::size_t>(i))]];
                fun(a);
        }
}

template<class X, std::integral Key = typename X::key_type, Key N = limit_v<X, L1>>
auto all_singleton_arrays(auto fun)
{
        for (auto i : std::views::iota(Key(0), N)) {
                auto a = std::array{ i }; [[assume(a.size() == 1)]];
                fun(a);
        }
}

template<class X, std::integral Key = typename X::key_type, Key N = limit_v<X, L1>>
auto all_singleton_ilists(auto fun)
{
        for (auto i : std::views::iota(Key(0), N)) {
                auto a = { i }; [[assume(a.size() == 1)]];
                fun(a);
        }
}

template<class X, std::integral Key = typename X::key_type, Key N = limit_v<X, L1>>
auto all_singleton_sets(auto fun)
{
        for (auto i : std::views::iota(Key(0), N)) {
                auto a = X({ i }); [[assume(a.size() == 1)]];
                fun(a);
        }
}

// NOTE: these tests are O(N^2)

template<class X, std::integral Key = typename X::key_type, Key N = limit_v<X, L2>>
auto all_doubleton_arrays(auto fun)
{
        for (auto j : std::views::iota(Key(1), std::ranges::max(N, Key(1)))) {
                for (auto i : std::views::iota(Key(0), j)) {
                        auto a = std::array{ i, j }; [[assume(a.size() == 2)]];
                        fun(a);
                }
        }
}

template<class X, std::integral Key = typename X::key_type, Key N = limit_v<X, L2>>
auto all_doubleton_ilists(auto fun)
{
        for (auto j : std::views::iota(Key(1), std::ranges::max(N, Key(1)))) {
                for (auto i : std::views::iota(Key(0), j)) {
                        auto a = { i, j }; [[assume(a.size() == 2)]];
                        fun(a);
                }
        }
}

template<class X, std::integral Key = typename X::key_type, Key N = limit_v<X, L2>>
auto all_doubleton_sets(auto fun)
{
        for (auto j : std::views::iota(Key(1), std::ranges::max(N, Key(1)))) {
                for (auto i : std::views::iota(Key(0), j)) {
                        auto a = X({ i, j }); [[assume(a.size() == 2)]];
                        fun(a);
                }
        }
}

template<class X, std::integral Key = typename X::key_type, Key N = limit_v<X, L2>>
auto all_singleton_set_pairs(auto fun)
{
        for (auto [ i, j ] : std::views::cartesian_product(
                std::views::iota(Key(0), N),
                std::views::iota(Key(0), N)
        )) {
                auto a = X({ i }); [[assume(a.size() == 1)]];
                auto b = X({ j }); [[assume(b.size() == 1)]];
                fun(a, b);
        }
}

// NOTE: this test is O(N^3)

template<class X, std::integral Key = typename X::key_type, Key N = limit_v<X, L3>>
auto all_singleton_set_triples(auto fun)
{
        for (auto [ i, j, k ] : std::views::cartesian_product(
                std::views::iota(Key(0), N),
                std::views::iota(Key(0), N),
                std::views::iota(Key(0), N)
        )) {
                auto a = X({ i }); [[assume(a.size() == 1)]];
                auto b = X({ j }); [[assume(b.size() == 1)]];
                auto c = X({ k }); [[assume(c.size() == 1)]];
                fun(a, b, c);
        }
}

// NOTE: this test is O(N^4)

template<class X, std::integral Key = typename X::key_type, Key N = limit_v<X, L4>>
auto all_doubleton_set_pairs(auto fun)
{
        for (auto [ j, n ] : std::views::cartesian_product(
                std::views::iota(Key(1), std::ranges::max(N, Key(1))),
                std::views::iota(Key(1), std::ranges::max(N, Key(1)))
        )) {
                for (auto [ i, m ] : std::views::cartesian_product(
                        std::views::iota(Key(0), j),
                        std::views::iota(Key(0), n)
                )) {
                        auto a = X({ i, j }); [[assume(a.size() == 2)]];
                        auto b = X({ m, n }); [[assume(b.size() == 2)]];
                        fun(a, b);
                }
        }
}

}       // namespace xstd
