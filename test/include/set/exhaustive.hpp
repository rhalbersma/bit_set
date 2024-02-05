#pragma once

//          Copyright Rein Halbersma 2014-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>            // min
#include <array>                // array
#include <cassert>              // assert
#include <cstddef>              // size_t
#include <initializer_list>     // initializer_list

#if defined(_MSC_VER)
        // xstd::bit_set<0> gives bogus "unreachable code" warnings
        __pragma(warning(disable: 4702))
#endif

namespace xstd {

template<class T, std::size_t L>
inline auto const limit_v = std::ranges::min(L, T().max_size());

inline constexpr auto L1 = 128uz;
inline constexpr auto L2 =  64uz;
inline constexpr auto L3 =  32uz;
inline constexpr auto L4 =  16uz;

// NOTE: these tests are O(1)

template<class T>
auto empty_set(auto fun)
{
        T is0; assert(is0.empty());
        fun(is0);
}

template<class T>
auto full_set(auto fun)
{
        auto const N = limit_v<T, L1>;
        T isN;
        for (auto i = 0uz; i < N; ++i) {
                isN.insert(i);
        }
        assert(isN.size() == N);
        fun(isN);
}

template<class T>
auto empty_set_pair(auto fun)
{
        T is0; assert(is0.empty());
        T is1; assert(is1.empty());
        fun(is0, is1);
}

// NOTE: these tests are O(N)

template<class T>
auto all_valid(auto fun)
{
        auto const N = limit_v<T, L1>;
        for (auto i = 0uz; i < N; ++i) {
                fun(i);
        }
}

template<class T>
auto all_cardinality_sets(auto fun)
{
        auto const N = limit_v<T, L1>;
        for (auto i = 0uz; i <= N; ++i) {
                T is;
                for (auto j = 0uz; j < i; ++j) {
                        is.insert(j);
                }
                fun(is);
        }
}

template<class T>
auto all_singleton_arrays(auto fun)
{
        auto const N = limit_v<T, L1>;
        for (auto i = 0uz; i < N; ++i) {
                auto ar1 = std::array{ i };
                fun(ar1);
        }
}

template<class T>
auto all_singleton_ilists(auto fun)
{
        auto const N = limit_v<T, L1>;
        for (auto i = 0uz; i < N; ++i) {
                auto il1 = { i };
                fun(il1);
        }
}

template<class T>
auto all_singleton_sets(auto fun)
{
        auto const N = limit_v<T, L1>;
        for (auto i = 0uz; i < N; ++i) {
                auto is1 = T({ i });
                fun(is1);
        }
}

// NOTE: these tests are O(N^2)

template<class T>
auto all_doubleton_arrays(auto fun)
{
        auto const N = limit_v<T, L2>;
        for (auto j = 1uz; j < N; ++j) {
                for (auto i = 0uz; i < j; ++i) {
                        auto ar2 = std::array{ i, j };
                        fun(ar2);
                }
        }
}

template<class T>
auto all_doubleton_ilists(auto fun)
{
        auto const N = limit_v<T, L2>;
        for (auto j = 1uz; j < N; ++j) {
                for (auto i = 0uz; i < j; ++i) {
                        auto il2 = { i, j };
                        fun(il2);
                }
        }
}

template<class T>
auto all_doubleton_sets(auto fun)
{
        auto const N = limit_v<T, L2>;
        for (auto j = 1uz; j < N; ++j) {
                for (auto i = 0uz; i < j; ++i) {
                        auto is2 = T({ i, j });
                        fun(is2);
                }
        }
}

template<class T>
auto all_singleton_set_pairs(auto fun)
{
        auto const N = limit_v<T, L2>;
        for (auto i = 0uz; i < N; ++i) {
                auto is1_i = T({ i });
                for (auto j = 0uz; j < N; ++j) {
                        auto is1_j = T({ j });
                        fun(is1_i, is1_j);
                }
        }
}

// NOTE: this test is O(N^3)

template<class T>
auto all_singleton_set_triples(auto fun)
{
        auto const N = limit_v<T, L3>;
        for (auto i = 0uz; i < N; ++i) {
                auto is1_i = T({ i });
                for (auto j = 0uz; j < N; ++j) {
                        auto is1_j = T({ j });
                        for (auto k = 0uz; k < N; ++k) {
                                auto is1_k = T({ k });
                                fun(is1_i, is1_j, is1_k);
                        }
                }
        }
}

// NOTE: this test is O(N^4)

template<class T>
auto all_doubleton_set_pairs(auto fun)
{
        auto const N = limit_v<T, L4>;
        for (auto j = 1uz; j < N; ++j) {
                for (auto i = 0uz; i < j; ++i) {
                        auto is2_ij = T({ i, j });
                        for (auto n = 1uz; n < N; ++n) {
                                for (auto m = 0uz; m < n; ++m) {
                                        auto is2_mn = T({ m, n });
                                        fun(is2_ij, is2_mn);
                                }
                        }
                }
        }
}

}       // namespace xstd
