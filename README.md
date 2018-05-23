[![Language](https://img.shields.io/badge/language-C++-blue.svg)](https://isocpp.org/) 
[![Standard](https://img.shields.io/badge/c%2B%2B-17-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization) 
[![License](https://img.shields.io/badge/license-Boost-blue.svg)](https://opensource.org/licenses/BSL-1.0) 
[![](https://tokei.rs/b1/github/rhalbersma/int_set)](https://github.com/rhalbersma/int_set)

Rebooting the `std::bitset` franchise
-------------------------------------

An `int_set<N>` is a modern reimagining of `std::bitset<N>`, keeping what time has proven to be effective, and throwing out what is not. `int_set`s are array-backed sets of integers that are compact and fast: they do less (i.e. they don't do bounds-checking, and they don't throw exceptions) and offer more (e.g. iterators to seamlessly interact with the rest of the Standard Library). This enables you to do your bit-twiddling with familiar syntax, typically leading to cleaner, more expressive code.

Hello World
-----------

The code below demonstrates how an `int_set` is a drop-in replacement for `std::set<int>` for generating all primes less than a compile time number. With an `int_set`, the usual STL-style iterator code remains valid with the same semantics but with much better performance through higher data parallelism.

For power users, bit-twiddling syntax will make the code even more expressive and performant. Both the `int_set` iterators and its member function `for_each` call platform dependent intrinsics to lookup the first or last 1-bit. Similar intrinsics are called for counting the number of elements in an `int_set`.

[![Try it online](https://img.shields.io/badge/try%20it-online-brightgreen.svg)](https://wandbox.org/permlink/0ZWgJ8JqYTX8upKO)

    #include "xstd/int_set.hpp"
    #include <algorithm>
    #include <iostream>
    #include <experimental/iterator>
    #include <set>

    #define USE_INT_SET 1

    int main()
    {
        constexpr auto N = 100;

    #if USE_INT_SET
        using set_type = xstd::int_set<N>;  // storage: N / 8 bytes on the stack
    #else
        using set_type = std::set<int>;     // storage: 48 bytes on the stack + 32 * N bytes on the heap
    #endif

        // find all primes below N: sieve of Eratosthenes
        set_type primes;
        for (auto i = 2; i < N; ++i) {
            primes.insert(i);
        }
        for (auto p : primes) {
            if (p * p >= N) break;
            for (auto n = p * p; n < N; n += p) {
                primes.erase(n);
            }
        }

        // print solution
        std::copy(primes.begin(), primes.end(), std::experimental::make_ostream_joiner(std::cout, ','));
        std::cout << '\n';

        // find all twin primes below N: STL-style iterator twiddling
        for (auto it = primes.begin(); it != primes.end() && std::next(it) != primes.end(); ) {
            it = std::adjacent_find(it, primes.end(), [](auto p, auto q) {
                return q - p == 2;
            });
            if (it != primes.end()) {
                std::cout << *it << ',' << *std::next(it) << '\n';
                ++it;
            }
        }
        std::cout << '\n';

    #if USE_INT_SET
        // find all twin primes below N: bit-twiddling power-up
        (primes & primes >> 2).for_each([&](auto p) {
            std::cout << p << ',' << (p + 2) << '\n';
        });
        std::cout << '\n';
    #endif
    }

Frequently Asked Questions
--------------------------

**Q**: How can you iterate over individual bits? I thought a byte was the unit of addressing?   
**A**: Using proxy iterators, which hold a pointer and an offset.

**Q**: What happens if you dereference a proxy iterator?   
**A**: You get a proxy reference: `ref == *it`.

**Q**: What happens if you take the address of a proxy reference?   
**A**: You get a proxy iterator: `it == &ref`.

**Q**: How do you get any value out of a proxy reference?   
**A**: They implicitly convert to `int`.

**Q**: How can proxy references work if C++ does not allow overloading of `operator.`?   
**A**: Indeed, proxy references break the equivalence between functions calls like `ref.mem_fn()` and `it->mem_fn()`.

**Q**: How do you work around this?   
**A**: `int` is not a class-type and does not have member functions, so this situation never occurs.

**Q**: So iterating over an `int_set` is really fool-proof?   
**A**: Yes, `int_set` iterators are [easy to use correctly and hard to use incorrectly](http://www.aristeia.com/Papers/IEEE_Software_JulAug_2004_revised.htm).

**Q**: I'm no fool, but a C++ programmer, how do I break things?   
**A**: If you insist, it is possible to use `int_set` iterators incorrectly by relying on too many implicit conversions.

[![Try it online](https://img.shields.io/badge/try%20it-online-brightgreen.svg)](https://wandbox.org/permlink/cF1RGdVqe7M8uXWx)

    #include "xstd/int_set.hpp"
    #include <algorithm>
    #include <iostream>
    #include <experimental/iterator>
    #include <set>

    #define USE_INT_SET 1

    class Int
    {
        int m_value;
    public:
        /* implicit */ Int(int v) noexcept : m_value{v} {}
        /* implicit */ operator auto() const noexcept { return m_value; }
    };

    int main()
    {
    #if USE_INT_SET
        constexpr auto N = 32;
        using set_type = xstd::int_set<N>;
    #else
        using set_type = std::set<int>;
    #endif

        auto primes = set_type { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31 };
        std::set<Int> s;

        // at most one user-defined conversion allowed
        // xstd::int_set::reference -> int -> Int is an error
        // std::set::reference == int -> Int is OK
        std::copy(primes.begin(), primes.end(), std::inserter(s, s.begin()));

        std::copy(s.begin(), s.end(), std::experimental::make_ostream_joiner(std::cout, ','));
    }

Requirements
------------

This single-header library has no other dependencies than the C++ Standard Library and is continuously being tested with the following conforming [C++17](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/n4659.pdf) compilers:

| Platform | Compiler | Versions | Build |
| :------- | :------- | :------- | :---- |
| Linux    | Clang <br> GCC | 6.0, 7-SVN<br> 7.3, 8.1 | [![codecov](https://codecov.io/gh/rhalbersma/int_set/branch/master/graph/badge.svg)](https://codecov.io/gh/rhalbersma/int_set) <br> [![Build Status](https://travis-ci.org/rhalbersma/int_set.svg)](https://travis-ci.org/rhalbersma/int_set) |
| Windows  | Visual Studio  |                    15.7 | [![Build status](https://ci.appveyor.com/api/projects/status/pn0u2i8mcfp4d9un?svg=true)](https://ci.appveyor.com/project/rhalbersma/int-set) |

License
-------

Copyright Rein Halbersma 2014-2018.   
Distributed under the [Boost Software License, Version 1.0](http://www.boost.org/users/license.html).   
(See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
