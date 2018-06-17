#pragma once

//          Copyright Rein Halbersma 2014-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <traits.hpp>           // has_resize_v
#include <array>                // array
#include <cstddef>              // size_t
#include <initializer_list>     // initializer_list
#include <utility>              // declval

namespace xstd {

template<class T, int L>
inline const auto limit_v = tti::has_resize_v<T> ? static_cast<decltype(max_size(std::declval<T>()))>(L) : max_size(T{});

constexpr auto L1 = 128;
constexpr auto L2 =  64;
constexpr auto L3 =  32;

// NOTE: these tests are O(1)

template<class IntSet, class UnaryFunction>
auto empty_set(UnaryFunction fun)
{
        const auto N = limit_v<IntSet, L1>;
        IntSet is0; resize(is0, N);
        assert(empty(is0));
        fun(is0);
}

template<class IntSet, class UnaryFunction>
auto full_set(UnaryFunction fun)
{
        const auto N = limit_v<IntSet, L1>;
        IntSet isN; resize(isN, N, true);
        assert(full(isN));
        fun(isN);
}

// NOTE: these tests are O(N)

template<class IntSet, class UnaryFunction>
auto all_values(UnaryFunction fun)
{
        const auto N = limit_v<IntSet, L1>;
        using SizeType = decltype(N);

        for (auto i = SizeType{0}; i < N; ++i) {
                fun(i);
        }
}

template<class IntSet, class UnaryFunction>
auto all_cardinality_sets(UnaryFunction fun)
{
        const auto N = limit_v<IntSet, L1>;
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
        const auto N = limit_v<IntSet, L1>;
        using SizeType = decltype(N);

        for (auto i = SizeType{0}; i < N; ++i) {
                auto i1 = std::array{ i };
                fun(i1);
        }
}

template<class IntSet, class UnaryFunction>
auto all_singleton_ilists(UnaryFunction fun)
{
        const auto N = limit_v<IntSet, L1>;
        using SizeType = decltype(N);

        for (auto i = SizeType{0}; i < N; ++i) {
                auto i1 = { i };
                fun(i1);
        }
}

template<class IntSet, class UnaryFunction>
auto all_singleton_sets(UnaryFunction fun)
{
        const auto N = limit_v<IntSet, L1>;
        using SizeType = decltype(N);

        for (auto i = SizeType{0}; i < N; ++i) {
                IntSet is1; resize(is1, N); insert(is1, i);
                fun(is1);
        }
}

// NOTE: these tests are O(N^2)

template<class IntSet, class UnaryFunction>
auto all_doubleton_arrays(UnaryFunction fun)
{
        const auto N = limit_v<IntSet, L2>;
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
        const auto N = limit_v<IntSet, L2>;
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
        const auto N = limit_v<IntSet, L2>;
        using SizeType = decltype(N);

        for (auto j = SizeType{1}; j < N; ++j) {
                for (auto i = SizeType{0}; i < j; ++i) {
                        IntSet is2; resize(is2, N); insert(is2, i); insert(is2, j);
                        fun(is2);
                }
        }
}

template<class IntSet, class BinaryFunction>
auto all_singleton_set_pairs(BinaryFunction fun)
{
        const auto N = limit_v<IntSet, L2>;
        using SizeType = decltype(N);

        for (auto i = SizeType{0}; i < N; ++i) {
                IntSet is1_i; resize(is1_i, N); insert(is1_i, i);
                for (auto j = SizeType{0}; j < N; ++j) {
                        IntSet is1_j; resize(is1_j, N); insert(is1_j, j);
                        fun(is1_i, is1_j);
                }
        }
}

// NOTE: this test is O(N^3)

template<class IntSet, class TernaryFunction>
auto all_singleton_set_triples(TernaryFunction fun)
{
        const auto N = limit_v<IntSet, L3>;
        using SizeType = decltype(N);
        
        for (auto i = SizeType{0}; i < N; ++i) {
                IntSet is1_i; resize(is1_i, N); insert(is1_i, i);
                for (auto j = SizeType{0}; j < N; ++j) {
                        IntSet is1_j; resize(is1_j, N); insert(is1_j, j);
                        for (auto k = SizeType{0}; k < N; ++k) {
                                IntSet is1_k; resize(is1_k, N); insert(is1_k, k);
                                fun(is1_i, is1_j, is1_k);
                        }
                }
        }
}

}       // namespace xstd
