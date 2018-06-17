#pragma once

//          Copyright Rein Halbersma 2014-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <traits.hpp>           // has_resize_v
#include <array>                // array
#include <cstddef>              // size_t
#include <initializer_list>     // initializer_list

namespace xstd {

template<class T, int L>
constexpr auto limit_v = []{
        if constexpr (tti::has_resize_v<T>) {
                return static_cast<decltype(max_size(std::declval<T>()))>(L);
        } else {
                return max_size(T{});
        }
}();

constexpr auto L1 = 128;
constexpr auto L2 =  64;
constexpr auto L3 =  32;

// NOTE: these tests are O(N)

template<class IntSet, class UnaryFunction>
auto all_values(UnaryFunction fun)
{
        constexpr auto N = limit_v<IntSet, L1>;
        using SizeType = decltype(N);

        for (auto i = SizeType{0}; i < N; ++i) {
                fun(i);
        }
}

template<class IntSet, class UnaryFunction>
auto all_cardinality_sets(UnaryFunction fun)
{
        constexpr auto N = limit_v<IntSet, L1>;
        using SizeType = decltype(N);

        for (auto i = SizeType{0}; i <= N; ++i) {
                IntSet is; resize(is, N);
                for (auto j = SizeType{0}; j < i; ++j) {
                        insert(is, j);
                }
                fun(is);
        }
}

template<class IntSet, class UnaryFunction>
auto all_singleton_arrays(UnaryFunction fun)
{
        constexpr auto N = limit_v<IntSet, L1>;
        using SizeType = decltype(N);

        for (auto i = SizeType{0}; i < N; ++i) {
                auto i1 = std::array{ i };
                fun(i1);
        }
}

template<class IntSet, class UnaryFunction>
auto all_singleton_ilists(UnaryFunction fun)
{
        constexpr auto N = limit_v<IntSet, L1>;
        using SizeType = decltype(N);

        for (auto i = SizeType{0}; i < N; ++i) {
                auto i1 = { i };
                fun(i1);
        }
}

template<class IntSet, class UnaryFunction>
auto all_singleton_sets(UnaryFunction fun)
{
        constexpr auto N = limit_v<IntSet, L1>;
        using SizeType = decltype(N);

        for (auto i = SizeType{0}; i < N; ++i) {
                IntSet tmp; resize(tmp, N);
                auto i1 = insert(tmp, i);
                fun(i1);
        }
}

// NOTE: these tests are O(N^2)

template<class IntSet, class UnaryFunction>
auto all_doubleton_arrays(UnaryFunction fun)
{
        constexpr auto N = limit_v<IntSet, L2>;
        using SizeType = decltype(N);

        for (auto j = SizeType{1}; j < N; ++j) {
                for (auto i = SizeType{0}; i < j; ++i) {
                        auto ij2 = std::array{ i, j };
                        fun(ij2);
                }
        }
}

template<class IntSet, class UnaryFunction>
auto all_doubleton_ilists(UnaryFunction fun)
{
        constexpr auto N = limit_v<IntSet, L2>;
        using SizeType = decltype(N);

        for (auto j = SizeType{1}; j < N; ++j) {
                for (auto i = SizeType{0}; i < j; ++i) {
                        auto ij2 = { i, j };
                        fun(ij2);
                }
        }
}

template<class IntSet, class UnaryFunction>
auto all_doubleton_sets(UnaryFunction fun)
{
        constexpr auto N = limit_v<IntSet, L2>;
        using SizeType = decltype(N);

        for (auto j = SizeType{1}; j < N; ++j) {
                for (auto i = SizeType{0}; i < j; ++i) {
                        IntSet tmp; resize(tmp, N);
                        auto ij2 = insert(insert(tmp, i), j);
                        fun(ij2);
                }
        }
}

template<class IntSet, class BinaryFunction>
auto all_singleton_set_pairs(BinaryFunction fun)
{
        constexpr auto N = limit_v<IntSet, L2>;
        using SizeType = decltype(N);

        for (auto i = SizeType{0}; i < N; ++i) {
                IntSet tmp1; resize(tmp1, N);
                auto i1 = insert(tmp1, i);
                for (auto j = SizeType{0}; j < N; ++j) {
                        IntSet tmp2; resize(tmp2, N);
                        auto j1 = insert(tmp2, j);
                        fun(i1, j1);
                }
        }
}

// NOTE: this test is O(N^3)

template<class IntSet, class TernaryFunction>
auto all_singleton_set_triples(TernaryFunction fun)
{
        constexpr auto N = limit_v<IntSet, L3>;
        using SizeType = decltype(N);

        for (auto i = SizeType{0}; i < N; ++i) {
                IntSet tmp1; resize(tmp1, N);
                auto i1 = insert(tmp1, i);
                for (auto j = SizeType{0}; j < N; ++j) {
                        IntSet tmp2; resize(tmp2, N);
                        auto j1 = insert(tmp2, j);
                        for (auto k = SizeType{0}; k < N; ++k) {
                                IntSet tmp3; resize(tmp3, N);
                                auto k1 = insert(tmp3, k);
                                fun(i1, j1, k1);
                        }
                }
        }
}

}       // namespace xstd
