#pragma once

//          Copyright Rein Halbersma 2014-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <traits.hpp>   // has_resize_v
#include <utility>      // declval

// Visual C++ in Release mode will generate C4702 (unreachable code) errors for int_set<N> for N == 0 if N is defined as const

namespace xstd {

#define XSTD_PP_MSVC_RELEASE_MODE_CONST /* const */

template<class T, int L>
auto XSTD_PP_MSVC_RELEASE_MODE_CONST limit_v = tti::has_resize_v<T> ? static_cast<decltype(fn_size(std::declval<T>()))>(L) : fn_size(T{});

inline constexpr auto L0 = 256;
inline constexpr auto L1 = 128;
inline constexpr auto L2 =  64;
inline constexpr auto L3 =  32;

// NOTE: these tests are O(1)

template<class BitSet, class UnaryFunction>
auto empty_set(UnaryFunction fun)
{
        auto XSTD_PP_MSVC_RELEASE_MODE_CONST N = limit_v<BitSet, L0>;
        BitSet bs0; resize(bs0, N);
        fun(bs0);
}

template<class BitSet, class UnaryFunction>
auto full_set(UnaryFunction fun)
{
        auto XSTD_PP_MSVC_RELEASE_MODE_CONST N = limit_v<BitSet, L0>;
        BitSet bsN; resize(bsN, N, true);
        fun(bsN);
}

// NOTE: these tests are O(N)

template<class BitSet, class UnaryFunction>
auto all_values(UnaryFunction fun)
{
        auto XSTD_PP_MSVC_RELEASE_MODE_CONST N = limit_v<BitSet, L1>;
        using SizeType = decltype(N);

        for (auto i = SizeType{0}; i < N; ++i) {
                fun(i);
        }
}

template<class BitSet, class UnaryFunction>
auto all_cardinality_sets(UnaryFunction fun)
{
        auto XSTD_PP_MSVC_RELEASE_MODE_CONST N = limit_v<BitSet, L1>;
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
auto all_singleton_sets(UnaryFunction fun)
{
        auto XSTD_PP_MSVC_RELEASE_MODE_CONST N = limit_v<BitSet, L1>;
        using SizeType = decltype(N);

        for (auto i = SizeType{0}; i < N; ++i) {
                BitSet bs1; resize(bs1, N); set(bs1, i);
                fun(bs1);
        }
}

// NOTE: these tests are O(N^2)

template<class BitSet, class BinaryFunction>
auto all_singleton_set_pairs(BinaryFunction fun)
{
        auto XSTD_PP_MSVC_RELEASE_MODE_CONST N = limit_v<BitSet, L2>;
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
        auto XSTD_PP_MSVC_RELEASE_MODE_CONST N = limit_v<BitSet, L3>;
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

}       // namespace xstd
