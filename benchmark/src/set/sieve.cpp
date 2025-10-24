//          Copyright Rein Halbersma 2014-2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <opt/set/sieve.hpp>            // filter_twins, sift_primes
#include <xstd/bit_set.hpp>             // bit_set
#include <flat_set>                     // flat_set
#include <set>                          // set
#include <benchmark/benchmark.h>        // DoNotOptimize, BENCHMARK_TEMPLATE1, BENCHMARK_MAIN

constexpr auto N = 1'000'000uz;

template<class X>
static void bm_sift_primes0(benchmark::State& state) {
        for (auto _ : state) {
                benchmark::DoNotOptimize(xstd::sift_primes0<X>(N));
        }
}

template<class X>
static void bm_sift_primes1(benchmark::State& state) {
        for (auto _ : state) {
                benchmark::DoNotOptimize(xstd::sift_primes1<X>(N));
        }
}

template<class X>
static void bm_filter_twins(benchmark::State& state) {
        auto const primes = xstd::sift_primes1<X>(N);
        for (auto _ : state) {
                benchmark::DoNotOptimize(xstd::filter_twins(primes));
        }
}

BENCHMARK_TEMPLATE1(bm_sift_primes0, std::flat_set<std::size_t>);
BENCHMARK_TEMPLATE1(bm_sift_primes0, std::set<std::size_t>);
BENCHMARK_TEMPLATE1(bm_sift_primes0, xstd::bit_set<N>);

BENCHMARK_TEMPLATE1(bm_sift_primes1, std::flat_set<std::size_t>);
BENCHMARK_TEMPLATE1(bm_sift_primes1, std::set<std::size_t>);
BENCHMARK_TEMPLATE1(bm_sift_primes1, xstd::bit_set<N>);

BENCHMARK_TEMPLATE1(bm_filter_twins, std::set<std::size_t>);
BENCHMARK_TEMPLATE1(bm_filter_twins, xstd::bit_set<N>);
BENCHMARK_TEMPLATE1(bm_filter_twins, std::flat_set<std::size_t>);

BENCHMARK_MAIN();
