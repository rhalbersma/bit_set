#pragma once

//          Copyright Rein Halbersma 2014-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>            // max
#include <array>                // array
#include <initializer_list>     // initializer_list
#include <ranges>               // cartesian_product, iota

#if defined(_MSC_VER)
        // xstd::bit_set<0> gives bogus "unreachable code" warnings
        __pragma(warning(disable: 4702))
#endif

namespace xstd {

template<class T, auto Limit>
inline constexpr auto limit_v = []() {
        if constexpr (requires { T::max_size(); }) {
                return T::max_size();
        } else {
                return Limit;
        }
}();

inline constexpr auto L1 = 128uz;
inline constexpr auto L2 =  64uz;
inline constexpr auto L3 =  32uz;
inline constexpr auto L4 =  16uz;

// NOTE: these tests are O(1)

template<class T>
auto empty_set(auto fun)
{
        T is0; [[assume(is0.empty())]];
        fun(is0);
}

template<class T, auto N = limit_v<T, L1>>
auto full_set(auto fun)
{
        auto const v = std::views::iota(0uz, N);
        auto isN = T(v.begin(), v.end()); [[assume(isN.size() == N)]];
        fun(isN);
}

template<class T>
auto empty_set_pair(auto fun)
{
        T is0; [[assume(is0.empty())]];
        T is1; [[assume(is1.empty())]];
        fun(is0, is1);
}

// NOTE: these tests are O(N)

template<class T, auto N = limit_v<T, L1>>
auto all_valid(auto fun)
{
        for (auto i : std::views::iota(0uz, N)) {
                fun(i);
        }
}

template<class T, auto N = limit_v<T, L1>>
auto all_cardinality_sets(auto fun)
{
        for (auto i : std::views::iota(0uz, N + 1)) {
                auto const v = std::views::iota(0uz, i);
                auto is = T(v.begin(), v.end()); [[assume(is.size() == i)]];
                fun(is);
        }
}

template<class T, auto N = limit_v<T, L1>>
auto all_singleton_arrays(auto fun)
{
        for (auto i : std::views::iota(0uz, N)) {
                auto ar1 = std::array{ i }; [[assume(ar1.size() == 1)]];
                fun(ar1);
        }
}

template<class T, auto N = limit_v<T, L1>>
auto all_singleton_ilists(auto fun)
{
        for (auto i : std::views::iota(0uz, N)) {
                auto il1 = { i }; [[assume(il1.size() == 1)]];
                fun(il1);
        }
}

template<class T, auto N = limit_v<T, L1>>
auto all_singleton_sets(auto fun)
{
        for (auto i : std::views::iota(0uz, N)) {
                auto is1 = T({ i }); [[assume(is1.size() == 1)]];
                fun(is1);
        }
}

// NOTE: these tests are O(N^2)

template<class T, auto N = limit_v<T, L2>>
auto all_doubleton_arrays(auto fun)
{
        for (auto j : std::views::iota(1uz, std::ranges::max(N, 1uz))) {
                for (auto i : std::views::iota(0uz, j)) {
                        auto ar2 = std::array{ i, j }; [[assume(ar2.size() == 2)]];
                        fun(ar2);
                }
        }
}

template<class T, auto N = limit_v<T, L2>>
auto all_doubleton_ilists(auto fun)
{
        for (auto j : std::views::iota(1uz, std::ranges::max(N, 1uz))) {
                for (auto i : std::views::iota(0uz, j)) {
                        auto il2 = { i, j }; [[assume(il2.size() == 2)]];
                        fun(il2);
                }
        }
}

template<class T, auto N = limit_v<T, L2>>
auto all_doubleton_sets(auto fun)
{
        for (auto j : std::views::iota(1uz, std::ranges::max(N, 1uz))) {
                for (auto i : std::views::iota(0uz, j)) {
                        auto is2 = T({ i, j }); [[assume(is2.size() == 2)]];
                        fun(is2);
                }
        }
}

template<class T, auto N = limit_v<T, L2>>
auto all_singleton_set_pairs(auto fun)
{
        for (auto [ i, j ] : std::views::cartesian_product(
                std::views::iota(0uz, N),
                std::views::iota(0uz, N)
        )) {
                auto is1_i = T({ i }); [[assume(is1_i.size() == 1)]];
                auto is1_j = T({ j }); [[assume(is1_j.size() == 1)]];
                fun(is1_i, is1_j);
        }
}

// NOTE: this test is O(N^3)

template<class T, auto N = limit_v<T, L3>>
auto all_singleton_set_triples(auto fun)
{
        for (auto [ i, j, k ] : std::views::cartesian_product(
                std::views::iota(0uz, N),
                std::views::iota(0uz, N),
                std::views::iota(0uz, N)
        )) {
                auto is1_i = T({ i }); [[assume(is1_i.size() == 1)]];
                auto is1_j = T({ j }); [[assume(is1_j.size() == 1)]];
                auto is1_k = T({ k }); [[assume(is1_k.size() == 1)]];
                fun(is1_i, is1_j, is1_k);
        }
}

// NOTE: this test is O(N^4)

template<class T, auto N = limit_v<T, L4>>
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
                        auto is2_ij = T({ i, j }); [[assume(is2_ij.size() == 2)]];
                        auto is2_mn = T({ m, n }); [[assume(is2_mn.size() == 2)]];
                        fun(is2_ij, is2_mn);
                }
        }
}

}       // namespace xstd
