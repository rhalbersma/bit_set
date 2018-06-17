#pragma once

//          Copyright Rein Halbersma 2014-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>            // min
#include <array>                // array
#include <cassert>              // assert
#include <initializer_list>     // initializer_list

namespace xstd {

template<class T, typename T::value_type L>
inline const auto limit_v = std::min(L, static_cast<typename T::value_type>(T{}.max_size()));

constexpr auto L1 = 256;
constexpr auto L2 = 128;
constexpr auto L3 =  64;

// NOTE: these tests are O(N)

template<class IntSet, class UnaryFunction>
auto all_values(UnaryFunction fun)
{
        const auto N = limit_v<IntSet, L1>;
        for (auto i = 0; i < N; ++i) {
                fun(i);
        }
}

template<class IntSet, class UnaryFunction>
auto all_cardinality_sets(UnaryFunction fun)
{
        const auto N = limit_v<IntSet, L1>;
        for (auto i = 0; i <= N; ++i) {
                IntSet is;
                for (auto j = 0; j < i; ++j) {
                        is.insert(j);
                }
                fun(is);
        }
}

template<class IntSet, class UnaryFunction>
auto all_singleton_arrays(UnaryFunction fun)
{
        const auto N = limit_v<IntSet, L1>;
        for (auto i = 0; i < N; ++i) {
                auto ar1 = std::array{ i };
                fun(ar1);
        }
}

template<class IntSet, class UnaryFunction>
auto all_singleton_ilists(UnaryFunction fun)
{
        const auto N = limit_v<IntSet, L1>;
        for (auto i = 0; i < N; ++i) {
                auto il1 = { i };
                fun(il1);
        }
}

template<class IntSet, class UnaryFunction>
auto all_singleton_sets(UnaryFunction fun)
{
        const auto N = limit_v<IntSet, L1>;
        for (auto i = 0; i < N; ++i) {
                auto is1 = IntSet{ i };
                fun(is1);
        }
}

// NOTE: these tests are O(N^2)

template<class IntSet, class UnaryFunction>
auto all_doubleton_arrays(UnaryFunction fun)
{
        const auto N = limit_v<IntSet, L2>;
        for (auto j = 1; j < N; ++j) {
                for (auto i = 0; i < j; ++i) {
                        auto ar2 = std::array{ i, j };
                        fun(ar2);
                }
        }
}

template<class IntSet, class UnaryFunction>
auto all_doubleton_ilists(UnaryFunction fun)
{
        const auto N = limit_v<IntSet, L2>;
        for (auto j = 1; j < N; ++j) {
                for (auto i = 0; i < j; ++i) {
                        auto il2 = { i, j };
                        fun(il2);
                }
        }
}

template<class IntSet, class UnaryFunction>
auto all_doubleton_sets(UnaryFunction fun)
{
        const auto N = limit_v<IntSet, L2>;
        for (auto j = 1; j < N; ++j) {
                for (auto i = 0; i < j; ++i) {
                        auto is2 = IntSet{ i, j };
                        fun(is2);
                }
        }
}

template<class IntSet, class BinaryFunction>
auto all_singleton_set_pairs(BinaryFunction fun)
{
        const auto N = limit_v<IntSet, L2>;
        for (auto i = 0; i < N; ++i) {
                auto is1_i = IntSet{ i };
                for (auto j = 0; j < N; ++j) {
                        auto is1_j = IntSet{ j };
                        fun(is1_i, is1_j);
                }
        }
}

// NOTE: this test is O(N^3)

template<class IntSet, class TernaryFunction>
auto all_singleton_set_triples(TernaryFunction fun)
{
        const auto N = limit_v<IntSet, L3>;
        for (auto i = 0; i < N; ++i) {
                auto is1_i = IntSet{ i };
                for (auto j = 0; j < N; ++j) {
                        auto is1_j = IntSet{ j };
                        for (auto k = 0; k < N; ++k) {
                                auto is1_k = IntSet{ k };
                                fun(is1_i, is1_j, is1_k);
                        }
                }
        }
}

}       // namespace xstd
