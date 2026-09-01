//          Copyright Rein Halbersma 2014-2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <opt/bitset/sieve.hpp>         // filter_twins, sift_primes
#include <xstd/bits/ext/boost/dynamic_bitset.hpp> // dynamic_bitset
#include <xstd/bits/ext/std/bitset.hpp>           // bitset
#include <xstd/bits/ext/xstd/bitset.hpp>          // bitset
#include <xstd/bits/bitset/bitset.hpp>              // bitset
#include <xstd/bits/set/finite_bit_set.hpp>             // finite_bit_set
#include <benchmark/benchmark.h>        // DoNotOptimize, BENCHMARK_TEMPLATE1, BENCHMARK_MAIN

constexpr auto N = 10'000uz;

template<class T>
static void bm_sift_primes0(benchmark::State& state) {
        for (auto _ : state) {
                benchmark::DoNotOptimize(xstd::sift_primes0<T>(N));
        }
}

template<class T>
static void bm_sift_primes1(benchmark::State& state) {
        for (auto _ : state) {
                benchmark::DoNotOptimize(xstd::sift_primes1<T>(N));
        }
}

template<class T>
static void bm_filter_twins(benchmark::State& state) {
        auto const primes = xstd::sift_primes1<T>(N);
        for (auto _ : state) {
                benchmark::DoNotOptimize(xstd::filter_twins(primes));
        }
}

BENCHMARK_TEMPLATE1(bm_sift_primes0, boost::dynamic_bitset<>);
BENCHMARK_TEMPLATE1(bm_sift_primes0,   std::bitset<N>);
BENCHMARK_TEMPLATE1(bm_sift_primes0,  xstd::bitset<N>);
BENCHMARK_TEMPLATE1(bm_sift_primes0,  xstd::finite_bit_set<N>);

BENCHMARK_TEMPLATE1(bm_sift_primes1, boost::dynamic_bitset<>);
BENCHMARK_TEMPLATE1(bm_sift_primes1,   std::bitset<N>);
BENCHMARK_TEMPLATE1(bm_sift_primes1,  xstd::bitset<N>);
BENCHMARK_TEMPLATE1(bm_sift_primes1,  xstd::finite_bit_set<N>);

BENCHMARK_TEMPLATE1(bm_filter_twins, boost::dynamic_bitset<>);
BENCHMARK_TEMPLATE1(bm_filter_twins,   std::bitset<N>);
BENCHMARK_TEMPLATE1(bm_filter_twins,  xstd::bitset<N>);
BENCHMARK_TEMPLATE1(bm_filter_twins,  xstd::finite_bit_set<N>);

BENCHMARK_MAIN();
