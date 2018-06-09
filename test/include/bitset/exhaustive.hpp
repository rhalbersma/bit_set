#pragma once

//          Copyright Rein Halbersma 2014-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <array>                // array
#include <initializer_list>     // initializer_list

namespace xstd {

// NOTE: these tests are O(N)

template<class IntSet, class UnaryFunction>
auto all_values(UnaryFunction fun)
{
        constexpr auto N = max_size(IntSet{});
        using SizeType = decltype(N);

        for (auto i = SizeType{0}; i < N; ++i) {
                fun(i);
        }
}

template<class IntSet, class UnaryFunction>
auto all_cardinality_sets(UnaryFunction fun)
{
        constexpr auto N = max_size(IntSet{});
        using SizeType = decltype(N);

        for (auto i = SizeType{0}; i < N; ++i) {
                auto ii = ~(~IntSet{} << i);
                fun(ii);
        }
        auto iN = ~IntSet{};
        fun(iN);
}

template<class IntSet, class UnaryFunction>
auto all_singleton_arrays(UnaryFunction fun)
{
        constexpr auto N = max_size(IntSet{});
        using SizeType = decltype(N);

        for (auto i = SizeType{0}; i < N; ++i) {
                auto i1 = std::array<SizeType, 1>{{ i }};
                fun(i1);
        }
}

template<class IntSet, class UnaryFunction>
auto all_singleton_ilists(UnaryFunction fun)
{
        constexpr auto N = max_size(IntSet{});
        using SizeType = decltype(N);

        for (auto i = SizeType{0}; i < N; ++i) {
                auto i1 = { i };
                fun(i1);
        }
}

template<class IntSet, class UnaryFunction>
auto all_singleton_sets(UnaryFunction fun)
{
        constexpr auto N = max_size(IntSet{});
        using SizeType = decltype(N);

        for (auto i = SizeType{0}; i < N; ++i) {
                IntSet tmp;
                auto i1 = insert(tmp, i);
                fun(i1);
        }
}

// NOTE: this test is O(N^2)

template<class IntSet, class UnaryFunction>
auto all_doubleton_arrays(UnaryFunction fun)
{
        constexpr auto N = max_size(IntSet{});
        using SizeType = decltype(N);

        for (auto j = SizeType{1}; j < N; ++j) {
                for (auto i = SizeType{0}; i < j; ++i) {
                        auto ij2 = std::array<SizeType, 2>{{ i, j }};
                        fun(ij2);
                }
        }
}

template<class IntSet, class UnaryFunction>
auto all_doubleton_ilists(UnaryFunction fun)
{
        constexpr auto N = max_size(IntSet{});
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
        constexpr auto N = max_size(IntSet{});
        using SizeType = decltype(N);

        for (auto j = SizeType{1}; j < N; ++j) {
                for (auto i = SizeType{0}; i < j; ++i) {
                        IntSet tmp;
                        auto ij2 = insert(insert(tmp, i), j);
                        fun(ij2);
                }
        }
}

template<class IntSet, class BinaryFunction>
auto all_singleton_set_pairs(BinaryFunction fun)
{
        constexpr auto N = max_size(IntSet{});
        using SizeType = decltype(N);

        for (auto i = SizeType{0}; i < N; ++i) {
                IntSet tmp1;
                auto i1 = insert(tmp1, i);
                for (auto j = SizeType{0}; j < N; ++j) {
                        IntSet tmp2;
                        auto j1 = insert(tmp2, j);
                        fun(i1, j1);
                }
        }
}

// NOTE: this test is O(N^3)

template<class IntSet, class TernaryFunction>
auto all_singleton_set_triples(TernaryFunction fun)
{
        constexpr auto N = max_size(IntSet{});
        using SizeType = decltype(N);

        for (auto i = SizeType{0}; i < N; ++i) {
                IntSet tmp1;
                auto i1 = insert(tmp1, i);
                for (auto j = SizeType{0}; j < N; ++j) {
                        IntSet tmp2;
                        auto j1 = insert(tmp2, j);
                        for (auto k = SizeType{0}; k < N; ++k) {
                                IntSet tmp3;
                                auto k1 = insert(tmp3, k);
                                fun(i1, j1, k1);
                        }
                }
        }
}

}       // namespace xstd
