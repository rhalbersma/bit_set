#pragma once

//          Copyright Rein Halbersma 2014-2019.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>            // min
#include <array>                // array
#include <cassert>              // assert
#include <initializer_list>     // initializer_list

#if defined(_MSC_VER)
        // sbit_set<0> gives bogus "unreachable code" warnings
        __pragma(warning(disable: 4702))
#endif

namespace xstd {

template<class T, typename T::value_type L>
inline auto const limit_v = static_cast<int>(std::min(static_cast<std::size_t>(L), static_cast<std::size_t>(T{}.max_size())));

constexpr auto L1 = 256;
constexpr auto L2 = 128;
constexpr auto L3 =  64;
constexpr auto L4 =  32;

// NOTE: these tests are O(1)

template<class IntSet, class UnaryFunction>
auto empty_set(UnaryFunction fun)
{
        IntSet is0;
        assert(is0.empty());
        fun(is0);
}

template<class IntSet, class UnaryFunction>
auto full_set(UnaryFunction fun)
{
        auto const N = limit_v<IntSet, L1>;
        IntSet isN;
        for (auto i = 0; i < N; ++i) {
                isN.insert(i);
        }
        assert(static_cast<int>(isN.size()) == N);
        fun(isN);
}

// NOTE: these tests are O(N)

template<class IntSet, class UnaryFunction>
auto all_values(UnaryFunction fun)
{
        auto const N = limit_v<IntSet, L1>;
        for (auto i = 0; i < N; ++i) {
                fun(i);
        }
}

template<class IntSet, class UnaryFunction>
auto all_cardinality_sets(UnaryFunction fun)
{
        auto const N = limit_v<IntSet, L1>;
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
        auto const N = limit_v<IntSet, L1>;
        for (auto i = 0; i < N; ++i) {
                auto ar1 = std::array{ i };
                fun(ar1);
        }
}

template<class IntSet, class UnaryFunction>
auto all_singleton_ilists(UnaryFunction fun)
{
        auto const N = limit_v<IntSet, L1>;
        for (auto i = 0; i < N; ++i) {
                auto il1 = { i };
                fun(il1);
        }
}

template<class IntSet, class UnaryFunction>
auto all_singleton_sets(UnaryFunction fun)
{
        auto const N = limit_v<IntSet, L1>;
        for (auto i = 0; i < N; ++i) {
                auto is1 = IntSet{ i };
                fun(is1);
        }
}

// NOTE: these tests are O(N^2)

template<class IntSet, class UnaryFunction>
auto all_doubleton_arrays(UnaryFunction fun)
{
        auto const N = limit_v<IntSet, L2>;
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
        auto const N = limit_v<IntSet, L2>;
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
        auto const N = limit_v<IntSet, L2>;
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
        auto const N = limit_v<IntSet, L2>;
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
        auto const N = limit_v<IntSet, L3>;
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

// NOTE: this test is O(N^4)

template<class IntSet, class BinaryFunction>
auto all_doubleton_set_pairs(BinaryFunction fun)
{
        auto const N = limit_v<IntSet, L4>;
        for (auto j = 1; j < N; ++j) {
                for (auto i = 0; i < j; ++i) {
                        auto is2_ij = IntSet{ i, j };
                        for (auto n = 1; n < N; ++n) {
                                for (auto m = 0; m < n; ++m) {
                                        auto is2_mn = IntSet{ m, n };
                                        fun(is2_ij, is2_mn);
                                }
                        }
                }
        }
}

}       // namespace xstd
