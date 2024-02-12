//          Copyright Rein Halbersma 2014-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <aux/set/sieve.hpp>            // filter_twins, sift_primes
#include <xstd/bit_set.hpp>             // bit_set
#include <boost/container/flat_set.hpp> // flat_set
#include <set>                          // set
#include <benchmark/benchmark.h>        // DoNotOptimize, BENCHMARK_TEMPLATE1, BENCHMARK_MAIN

constexpr auto N = 10'000;

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

using Key = int;

BENCHMARK_TEMPLATE1(bm_sift_primes0, std::set<Key>);
BENCHMARK_TEMPLATE1(bm_sift_primes0, boost::container::flat_set<Key>);
//BENCHMARK_TEMPLATE1(bm_sift_primes0, (xstd::bit_set<Key, N>));

BENCHMARK_TEMPLATE1(bm_sift_primes1, std::set<Key>);
BENCHMARK_TEMPLATE1(bm_sift_primes1, boost::container::flat_set<Key>);
//BENCHMARK_TEMPLATE1(bm_sift_primes1, (xstd::bit_set<Key, N>));

BENCHMARK_TEMPLATE1(bm_filter_twins, std::set<Key>);
BENCHMARK_TEMPLATE1(bm_filter_twins, boost::container::flat_set<Key>);
//BENCHMARK_TEMPLATE1(bm_filter_twins, (xstd::bit_set<Key, N>));

BENCHMARK_MAIN();
