//          Copyright Rein Halbersma 2014-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <aux/set/sieve.hpp>            // filter_twins, sift_primes
#include <xstd/bit_set.hpp>             // bit_set
#include <boost/container/flat_set.hpp> // flat_set
#include <set>                          // set
#include <benchmark/benchmark.h>        // DoNotOptimize, BENCHMARK_TEMPLATE1, BENCHMARK_MAIN
#include <boost/preprocessor.hpp>       // BOOST_PP_COMMA

using Key = std::size_t;
constexpr Key N = 1'000'000uz;

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

BENCHMARK_TEMPLATE1(bm_sift_primes0, boost::container::flat_set<Key>);
BENCHMARK_TEMPLATE1(bm_sift_primes0, std::set<Key>);
BENCHMARK_TEMPLATE1(bm_sift_primes0, xstd::bit_set<Key BOOST_PP_COMMA() N>);

BENCHMARK_TEMPLATE1(bm_sift_primes1, boost::container::flat_set<Key>);
BENCHMARK_TEMPLATE1(bm_sift_primes1, std::set<Key>);
BENCHMARK_TEMPLATE1(bm_sift_primes1, xstd::bit_set<Key BOOST_PP_COMMA() N>);

BENCHMARK_TEMPLATE1(bm_filter_twins, std::set<Key>);
BENCHMARK_TEMPLATE1(bm_filter_twins, xstd::bit_set<Key BOOST_PP_COMMA() N>);
BENCHMARK_TEMPLATE1(bm_filter_twins, boost::container::flat_set<Key>);

BENCHMARK_MAIN();
