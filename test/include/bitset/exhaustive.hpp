#pragma once

//          Copyright Rein Halbersma 2014-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <traits.hpp>           // has_resize_v
#include <array>                // array
#include <cassert>              // assert
#include <cstddef>              // size_t
#include <initializer_list>     // initializer_list
#include <utility>              // declval

namespace xstd {

template<class T, int L>
inline const auto limit_v = tti::has_resize_v<T> ? static_cast<decltype(fn_size(std::declval<T>()))>(L) : fn_size(T{});

constexpr auto L1 = 128;
constexpr auto L2 =  64;
constexpr auto L3 =  32;
constexpr auto L4 =  16;

// NOTE: these tests are O(1)

template<class BitSet, class UnaryFunction>
auto empty_set(UnaryFunction fun)
{
        const auto N = limit_v<BitSet, L1>;
        BitSet bs0; resize(bs0, N);
        assert(none(bs0));
        fun(bs0);
}

template<class BitSet, class UnaryFunction>
auto full_set(UnaryFunction fun)
{
        const auto N = limit_v<BitSet, L1>;
        BitSet bsN; resize(bsN, N, true);
        assert(all(bsN));
        fun(bsN);
}

// NOTE: these tests are O(N)

template<class BitSet, class UnaryFunction>
auto all_values(UnaryFunction fun)
{
        const auto N = limit_v<BitSet, L1>;
        using SizeType = decltype(N);

        for (auto i = SizeType{0}; i < N; ++i) {
                fun(i);
        }
}

template<class BitSet, class UnaryFunction>
auto all_cardinality_sets(UnaryFunction fun)
{
        const auto N = limit_v<BitSet, L1>;
        using SizeType = decltype(N);

        for (auto i = SizeType{0}; i <= N; ++i) {
                BitSet bs; resize(bs, N);
                for (auto j = SizeType{0}; j < i; ++j) {
                        set(bs, j);
                }
                assert(count(bs) == i);
                fun(bs);
        }
}

template<class BitSet, class UnaryFunction>
auto all_singleton_arrays(UnaryFunction fun)
{
        const auto N = limit_v<BitSet, L1>;
        using SizeType = decltype(N);

        for (auto i = SizeType{0}; i < N; ++i) {
                auto i1 = std::array{ i };
                fun(i1);
        }
}

template<class BitSet, class UnaryFunction>
auto all_singleton_ilists(UnaryFunction fun)
{
        const auto N = limit_v<BitSet, L1>;
        using SizeType = decltype(N);

        for (auto i = SizeType{0}; i < N; ++i) {
                auto i1 = { i };
                fun(i1);
        }
}

template<class BitSet, class UnaryFunction>
auto all_singleton_sets(UnaryFunction fun)
{
        const auto N = limit_v<BitSet, L1>;
        using SizeType = decltype(N);

        for (auto i = SizeType{0}; i < N; ++i) {
                BitSet bs1; resize(bs1, N); set(bs1, i);
                fun(bs1);
        }
}

// NOTE: these tests are O(N^2)

template<class BitSet, class UnaryFunction>
auto all_doubleton_arrays(UnaryFunction fun)
{
        const auto N = limit_v<BitSet, L2>;
        using SizeType = decltype(N);

        for (auto j = SizeType{1}; j < N; ++j) {
                for (auto i = SizeType{0}; i < j; ++i) {
                        auto ij2 = std::array{ i, j };
                        fun(ij2);
                }
        }
}

template<class BitSet, class UnaryFunction>
auto all_doubleton_ilists(UnaryFunction fun)
{
        const auto N = limit_v<BitSet, L2>;
        using SizeType = decltype(N);

        for (auto j = SizeType{1}; j < N; ++j) {
                for (auto i = SizeType{0}; i < j; ++i) {
                        auto ij2 = { i, j };
                        fun(ij2);
                }
        }
}

template<class BitSet, class UnaryFunction>
auto all_doubleton_sets(UnaryFunction fun)
{
        const auto N = limit_v<BitSet, L2>;
        using SizeType = decltype(N);

        for (auto j = SizeType{1}; j < N; ++j) {
                for (auto i = SizeType{0}; i < j; ++i) {
                        BitSet bs2; resize(bs2, N); set(bs2, i); set(bs2, j);
                        fun(bs2);
                }
        }
}

template<class BitSet, class BinaryFunction>
auto all_singleton_set_pairs(BinaryFunction fun)
{
        const auto N = limit_v<BitSet, L2>;
        using SizeType = decltype(N);

        for (auto i = SizeType{0}; i < N; ++i) {
                BitSet bs1_i; resize(bs1_i, N); set(bs1_i, i);
                for (auto j = SizeType{0}; j < N; ++j) {
                        BitSet bs1_j; resize(bs1_j, N); set(bs1_j, j);
                        fun(bs1_i, bs1_j);
                }
        }
}

// NOTE: this test is O(N^3)

template<class BitSet, class TernaryFunction>
auto all_singleton_set_triples(TernaryFunction fun)
{
        const auto N = limit_v<BitSet, L3>;
        using SizeType = decltype(N);

        for (auto i = SizeType{0}; i < N; ++i) {
                BitSet bs1_i; resize(bs1_i, N); set(bs1_i, i);
                for (auto j = SizeType{0}; j < N; ++j) {
                        BitSet bs1_j; resize(bs1_j, N); set(bs1_j, j);
                        for (auto k = SizeType{0}; k < N; ++k) {
                                BitSet bs1_k; resize(bs1_k, N); set(bs1_k, k);
                                fun(bs1_i, bs1_j, bs1_k);
                        }
                }
        }
}

// NOTE: this test is O(N^4)

template<class BitSet, class BinaryFunction>
auto all_doubleton_set_pairs(BinaryFunction fun [[maybe_unused]])
{
        const auto N = limit_v<BitSet, L4>;
        using SizeType = decltype(N);

        for (auto j = SizeType{1}; j < N; ++j) {
                for (auto i = SizeType{0}; i < j; ++i) {
                        BitSet bs2_ij; resize(bs2_ij, N); set(bs2_ij, i); set(bs2_ij, j);
                        for (auto n = SizeType{1}; n < N; ++n) {
                                for (auto m = SizeType{0}; m < n; ++m) {
                                        BitSet bs2_mn; resize(bs2_mn, N); set(bs2_mn, m); set(bs2_mn, n);
                                        //fun(bs2_ij, bs2_mn);
                                }
                        }
                }
        }
}

}       // namespace xstd
