//          Copyright Rein Halbersma 2014-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/bit_set.hpp>             // bit_set
#include <set/sieve.hpp>                // sift_primes
#include <boost/container/flat_set.hpp> // flat_set
#include <boost/mpl/vector.hpp>         // vector
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE_TEMPLATE
#include <fmt/chrono.h>
#include <fmt/format.h>                 // println
#include <fmt/ranges.h>
#include <chrono>                       // duration_cast, system_clock
#include <locale>                       // locale
#include <set>                          // set

BOOST_AUTO_TEST_SUITE(Sieve)

using namespace xstd;

inline constexpr auto N = 1'000'000;

using set_types = boost::mpl::vector
<       std::set<int>
,       boost::container::flat_set<int>
,       bit_set<N>       
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(Format, T, set_types)
{
        std::locale::global(std::locale("en_US.UTF-8"));
     
        auto t0 = std::chrono::system_clock::now();
        auto const primes = sift_primes<T>(N);
        auto t1 = std::chrono::system_clock::now();
        auto d0 = std::chrono::duration_cast<std::chrono::microseconds>(t1-t0);
        fmt::println("Generated {:>7L} primes in {:>10}", size(primes), d0);
        // BOOST_CHECK_EQUAL(fmt::format("{}", primes), "{2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97}");        
        
        auto t2 = std::chrono::system_clock::now();
        auto const twins = filter_twins(primes);
        auto t3 = std::chrono::system_clock::now();
        auto d1 = std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2);
        fmt::println("Generated {:>7L} twins  in {:>10}", size(twins), d1);
        // BOOST_CHECK_EQUAL(fmt::format("{}", twins), "{3, 5, 11, 17, 29, 41, 59, 71}");
}

BOOST_AUTO_TEST_SUITE_END()
