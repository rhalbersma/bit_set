//          Copyright Rein Halbersma 2014-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <aux/bitset/sieve.hpp>         // filter_twins, sift_primes
#include <ext/boost/dynamic_bitset.hpp> // dynamic_bitset
#include <ext/std/bitset.hpp>           // bitset
#include <ext/std/ranges.hpp>           // as_set
#include <xstd/bitset.hpp>              // bitset
#include <xstd/bit_set.hpp>             // bit_set
#include <benchmark/benchmark.h>        // DoNotOptimize, BENCHMARK_TEMPLATE1, BENCHMARK_MAIN

constexpr auto N = 1'000'000;

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
BENCHMARK_TEMPLATE1(bm_sift_primes0, std::bitset<N>);
BENCHMARK_TEMPLATE1(bm_sift_primes0, xstd::bitset<N>);
BENCHMARK_TEMPLATE1(bm_sift_primes0, xstd::bit_set<N>);

BENCHMARK_TEMPLATE1(bm_sift_primes1, boost::dynamic_bitset<>);
BENCHMARK_TEMPLATE1(bm_sift_primes1, std::bitset<N>);
BENCHMARK_TEMPLATE1(bm_sift_primes1, xstd::bitset<N>);
BENCHMARK_TEMPLATE1(bm_sift_primes1, xstd::bit_set<N>);

BENCHMARK_TEMPLATE1(bm_filter_twins, boost::dynamic_bitset<>);
BENCHMARK_TEMPLATE1(bm_filter_twins, std::bitset<N>);
BENCHMARK_TEMPLATE1(bm_filter_twins, xstd::bitset<N>);
BENCHMARK_TEMPLATE1(bm_filter_twins, xstd::bit_set<N>);

BENCHMARK_MAIN();
