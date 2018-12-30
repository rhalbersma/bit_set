[![Language](https://img.shields.io/badge/language-C++-blue.svg)](https://isocpp.org/)
[![Standard](https://img.shields.io/badge/c%2B%2B-17-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization)
[![License](https://img.shields.io/badge/license-Boost-blue.svg)](https://opensource.org/licenses/BSL-1.0)
[![](https://tokei.rs/b1/github/rhalbersma/bit_set)](https://github.com/rhalbersma/bit_set)

> "A `bitset` can be seen as either an array of bits or a set of integers."  
> Chuck Allison, [ISO/WG21/N0075](http://www.open-std.org/Jtc1/sc22/wg21/docs/papers/1991/WG21%201991/X3J16_91-0142%20WG21_N0075.pdf), November 25, 1991

Rebooting the `std::bitset` franchise
=====================================

`xstd::bit_set<N>` is a modern and opiniated reimagining of `std::bitset<N>`, keeping what time has proven to be effective, and throwing out what is not. `xstd::bit_set` is a **fixed-size ordered set of integers** that is compact and fast. It does less than `std::bitset<N>` (e.g. no bounds-checking and no exceptions) yet offers more (e.g. iterators to seamlessly interact with the rest of the Standard Library). This enables **fixed-size bit-twiddling with set-like syntax**, identical to `std::set<int>`, typically leading to cleaner, more expressive code.

Hello World
===========

As the quote from the first C++ Standard Committee paper on what would eventually become `std::bitset<N>` shows, one can take several perspectives on what a bitset actually represents. `std::bitset<N>` mostly leans towards the sequence of bits perspective, emphasising compatibility with unsigned integers and bitstrings. In contrast, this library provides `xstd::bit_set<N>` as an ordered set of integers interface on top of a sequence of bits implementation, combining the best of both worlds. The table below summarizes the landscape of bitset interpretations.

| Ordered set of integers | | Sequence of bits | | This library | |
| :---------------------- |-| :--------------- |-| :----------- |-|
| `std::set<int>` | [![Try it online](https://img.shields.io/badge/try%20it-online-brightgreen.svg)](https://wandbox.org/permlink/D5j68sUHu6XOUL3L) | `std::bitset<N>` | [![Try it online](https://img.shields.io/badge/try%20it-online-brightgreen.svg)](https://wandbox.org/permlink/GjVDJtGl2qDzGsTu) | `xstd::bit_set<N>` | [![Try it online](https://img.shields.io/badge/try%20it-online-brightgreen.svg)](https://wandbox.org/permlink/soZ9TbkG31Kmn1Fv) |
| `boost::flat_set<int>` | [![Try it online](https://img.shields.io/badge/try%20it-online-brightgreen.svg)](https://wandbox.org/permlink/lFMYEzerQpXOEDfH) | `boost::dynamic_bitset<>` | [![Try it online](https://img.shields.io/badge/try%20it-online-brightgreen.svg)](https://wandbox.org/permlink/74IuX2Fw6AO70D4f) || |
| `xstd::bit_set<N>` | [![Try it online](https://img.shields.io/badge/try%20it-online-brightgreen.svg)](https://wandbox.org/permlink/mtoXa0DRLlWZR1wF) |||||

The code below demonstrates how the [Sieve of Eratosthenes](https://en.wikipedia.org/wiki/Sieve_of_Eratosthenes) algorithm to generate all primes below a compile time number `N` can be implemented using set algorithms for different interpretations of a set. The three code examples show how `xstd::bit_set<N>` can be used as a drop-in replacement for `std::set<int>`, including the full iterator manipulation for generating twin primes. In addition, `xstd::bit_set<N>` is also equally expressive and performant as `std::bitset<N>` through its bitwise operators and stack-based allocation.

> **Note**: we use a wrapper `xstd::for_each` around the `find_first()` and `find_next()` member functions of `boost::dynamic_bitset<>`.  
> The GCC compiler provides `_Find_first()` and `_Find_next()` member functions as a similar extension for `std::bitset<N>`.

<table>
<tr>
<th> Ordered set of integers </th>
<th> Sequence of bits </th>
<th> This library </th>
</tr>
<tr>
<td valign="top">

```cpp
#include <algorithm>
#include <iostream>
#include <experimental/iterator>
#include <set>

constexpr auto N = 100;
using set_type = std::set<int>;

int main()
{
    // initialize all numbers from [2, N)
    set_type primes;
    for (auto i = 2; i < N; ++i) {
        primes.insert(i);
    }

    // find all primes below N
    for (auto const& p : primes) {
        auto const p_squared = p * p;
        if (p_squared >= N) break;
        for (auto n = p_squared; n < N; n += p) {
            primes.erase(n);
        }
    }

    // find all twin primes below N
    set_type twins;
    for (auto it = primes.begin();
        it != primes.end()
            && std::next(it) != primes.end()
        ;
    ) {
        it = std::adjacent_find(it, primes.end(),
            [](auto p, auto q) {
                return q - p == 2;
            }
        );
        if (it != primes.end()) {
            twins.insert(*it++);
        }
    }

    // pretty-print solution
    std::cout << '[';
    std::copy(primes.begin(), primes.end(),
        ostream_joiner(std::cout, ',')
    );
    std::cout << "]\n";
    std::cout << '[';
    std::copy(twins.begin(), twins.end(),
        ostream_joiner(std::cout, ',')
    );
    std::cout << "]\n";
}
```
</td>
<td valign="top">

```cpp
#include <xstd/for_each.hpp>
#include <bitset>
#include <iostream>


constexpr auto N = 100;
using set_type = std::bitset<N>;

int main()
{
    // initialize all numbers from [2, N)
    set_type primes;
    primes.set();
    primes.reset(0);
    primes.reset(1);

    // find all primes below N
    xstd::for_each(primes, [&](auto const& p) {
        auto const p_squared = p * p;
        if (p_squared >= N) return;
        for (auto n = p_squared; n < N; n += p) {
            primes.reset(n);
        }
    });

    // find all twin primes below N
    auto const twins = primes & primes >> 2;















    // pretty-print solution
    std::cout << '[';
    xstd::for_each(primes, [&](auto const& p) {
        std::cout << p << ',';
    });
    std::cout << "]\n";
    std::cout << '[';
    xstd::for_each(twins, [&](auto const& t) {
        std::cout << t << ',';
    });
    std::cout << "]\n";
}
```
</td>
<td  valign="top">

```cpp
#include <xstd/bit_set.hpp>
#include <iostream>



constexpr auto N = 100;
using set_type = xstd::bit_set<N>;

int main()
{
    // initialize all numbers from [2, N)
    set_type primes;
    primes.fill();
    primes.erase({0, 1});


    // find all primes below N
    for (auto const& p : primes) {
        auto const p_squared = p * p;
        if (p_squared >= N) break;
        for (auto n = p_squared; n < N; n += p) {
            primes.erase(n);
        }
    }

    // find all twin primes below N
    auto const twins = primes & primes >> 2;















    // pretty-print solution
    std::cout << primes << '\n';




    std::cout << twins << '\n';




}
```
</td>
</tr>
</table>

Documentation
=============

The interface for the class template `xstd::bit_set<N>` consist of three major pieces:
  1. the member functions of [`std::bitset<N>`](http://en.cppreference.com/w/cpp/utility/bitset) renamed to the naming conventions of [`std::set<int>`](http://en.cppreference.com/w/cpp/container/set);
  2. the bitwise operators `&=`, `|=`, `^=`, `<<=`, `>>=`, `~`, `&`, `|`, `^`, `<<`, `>>` from [`std::bitset<N>`](http://en.cppreference.com/w/cpp/utility/bitset);
  3. the set algorithms `is_subset_of`, `is_superset_of`, `is_proper_subset_of`, `is_proper_superset_of`, `intersects`, `disjoint`, many of which can also be found in [`boost::dynamic_bitset<>`](https://www.boost.org/doc/libs/1_67_0/libs/dynamic_bitset/dynamic_bitset.html);

Porting `std::bitset<N>` code to `xstd::bit_set<N>`
---------------------------------------------------

Most `std::bitset<N>` expressions have **a direct translation** to an equivalent `xstd::bit_set<N>` expression. With the exception of the member functions `fill`, `complement`, `replace` and `full`, the renamed `xstd::bit_set<N>` functionality is named after the corresponding `std::set<int>` functionality.

The semantic differences are that `xstd::bit_set<N>`:
  - has a signed integral `size_type`;
  - has a `static` member function `max_size()` returning `N`;
  - does not do bounds-checking for its members `insert`, `erase`, `replace` and `contains`.

Instead of throwing an `out_of_range` exception for argument values outside the range `[0, N)`, this **behavior is undefined**.

| Expression for `std::bitset<N>` | Expression for `xstd::bit_set<N>`            | Semantics for `xstd::bit_set<N>`                    |
| :------------------------------ | :------------------------------------------- | :-------------------------------------------------- |
| `bs.set()`                      | `bs.fill()`                                  | |
| `bs.set(pos)`                   | `bs.insert(pos)`                             | does not do bounds-checking or throw `out_of_range` |
| `bs.set(pos, val)`              | `val ? bs.insert(pos) : bs.erase(pos)`       | does not do bounds-checking or throw `out_of_range` |
| `bs.reset()`                    | `bs.clear()`                                 | |
| `bs.reset(pos)`                 | `bs.erase(pos)`                              | does not do bounds-checking or throw `out_of_range` |
| `bs.flip()`                     | `bs.complement()`                            | |
| `bs.flip(pos)`                  | `bs.replace(pos)`                            | does not do bounds-checking or throw `out_of_range` |
| `bs.count()`                    | `bs.size()`                                  | returns a signed `size_type`                        |
| `bs.size()`                     | `bs.max_size()`                              | returns a signed `size_type`; is a `static` member  |
| `bs.test(pos)`                  | `bs.contains(pos)`                           | does not do bounds-checking or throw `out_of_range` |
| `bs.all()`                      | `bs.full()`                                  | |
| `bs.any()`                      | `!bs.empty()`                                | |
| `bs.none()`                     | `bs.empty()`                                 | |
| `bs[pos]`                       | `bs.contains(pos)`                           | |
| `bs[pos] = val`                 | `val ? bs.insert(pos) : bs.erase(pos)`       | |

Semantic differences for bitwise-shift operators
------------------------------------------------

The bitwise-shift operators in `std::bitset<N>` and `xstd::bit_set<N>` have **identical syntax**. The only semantic difference is that `xstd::bit_set<N>` does not do bounds-checking for its bitwise-shift operators. Instead of throwing an `out_of_range` exception for argument values outside the range `[0, N)`, this **behavior is undefined**.

| Expression    | Semantics for `xstd::bit_set<N>`                    |
| :------------ | :-------------------------------------------------- |
| `bs <<= pos`  | does not do bounds-checking or throw `out_of_range` |
| `bs >>= pos`  | does not do bounds-checking or throw `out_of_range` |
| `bs << pos`   | does not do bounds-checking or throw `out_of_range` |
| `bs >> pos`   | does not do bounds-checking or throw `out_of_range` |

Semantic differences for I/O streaming operators
------------------------------------------------

The semantic differences between `std::bitset<N>` and `xstd::bitset<N>` are most visible in its overloaded I/O streaming `operator<<` and `operator>>`.

<table>
<tr>
<th> Sequence of bits (right to left) </th>
<th> Ordered set of integers (left to right) </th>
</tr>
<tr>
<td  valign="top">

```cpp
#include <bitset>
#include <iostream>

constexpr auto N = 8;
using set_type = std::bitset<N>;

int main()
{
    set_type primes;
    primes.set(2);
    primes.set(3);
    primes.set(5);
    primes.set(7);
    std::cout << primes << '\n';
}
```
</td>

<td  valign="top">

```cpp
#include "xstd/bit_set.hpp"
#include <iostream>

constexpr auto N = 8;
using set_type = xstd::bit_set<N>;

int main()
{
    set_type primes { 2, 3, 5, 7 };




    std::cout << primes << '\n';
}
```
</td>
</tr>
<tr>
<td> <pre> 10101100 </pre> </td>
<td> <pre> [2,3,5,7] </pre> </td>
</tr>
</table>

Functionality from `std::bitset<N>` that is not in `xstd::bit_set<N>`
---------------------------------------------------------------------

  - **Constructors**: No construction from `unsigned long long`, `std::string` or `char const*`.
  - **Conversion**: No conversion to `unsigned long`, `unsigned long long` or `std::string`.
  - **Hashing**: No specialization for `std::hash<>`.

The hashing functionality can be obtained through third-party libraries such as [N3980](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n3980.html) by streaming the `xstd::bit_set<N>` elements and size through an overloaded `hash_append` function.

Functionality from `std::set<int>` that is not in `xstd::bit_set<N>`
--------------------------------------------------------------------

`xstd::bit_set<N>` provides the same functionality as `std::set<int, std::less<int>, Allocator>`, where `Allocator` statically allocates contiguous memory to store `N` integers. This means that `xstd::bit_set<N>` does **not provide**:

  - **Customized key comparison**: `xstd::bit_set<N>` uses `std::less<int>` as its default comparator.
  - **Allocator support**: `xstd::bit_set<N>` is a fixed-size set of non-negative integers and does not dynamically allocate memory. In particular, `xstd::bit_set<N>` does **not provided a `get_allocator()` member function and its constructors do not take an allocator argument.
  - **Splicing**: `xstd::bit_set<N>` is **not a node-based container**, and does not provide the splicing operatorions as defined in [P0083R3](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0083r3.pdf). In particular, `xstd::bit_set<N>` does not provide the nested type `node_type`, the `extract()` or `merge()` member functions, or the `insert()` overloads taking a node handle.

Composable data-parallel set algorithms on sorted ranges
--------------------------------------------------------

Many of the bitwise operators for `xstd::bit_set<N>` are equivalent to set algorithms on sorted ranges from the C++ Standard Library header `<algorithm>`.

| Expression for `xstd::bit_set<N>` | Expression for `std::set<int>` |
| :-------------------------------- | :----------------------------- |
| `is_subset_of(a, b)`              | `includes(begin(a), end(a), begin(b), end(b))` |
| `auto c = a & b;`                 | `set<int> c;` <br> `set_intersection(begin(a), end(a), begin(b), end(b), inserter(c, end(c)));` |
| <code>auto c = a &#124; b;</code> | `set<int> c;` <br> `set_union(begin(a), end(a), begin(b), end(b), inserter(c, end(c)));` |
| `auto c = a ^ b;`                 | `set<int> c;` <br> `set_symmetric_difference(begin(a), end(a), begin(b), end(b), inserter(c, end(c)));` |
| `auto c = a - b;`                 | `set<int> c;` <br> `set_difference(begin(a), end(a), begin(b), end(b), inserter(c, end(c)));` |
| `auto b = a << n;`                | `set<int> tmp, b;` <br> `transform(begin(a), end(a), inserter(tmp, end(tmp)), [=](int x) { return x + n; });` <br> `copy_if(begin(tmp), end(tmp), inserter(b, end(b)), [](int x) { return x < N; });` |
| `auto b = a >> n;`                | `set<int> tmp, b;` <br> `transform(begin(a), end(a), inserter(tmp, end(tmp)), [=](int x) { return x - n; });` <br> `copy_if(begin(tmp), end(tmp), inserter(b, end(b)), [](int x) { return 0 <= x; });` |

The difference with iterator-based algorithms on general sorted ranges is that the bitwise operators from `xstd::bit_set<N>` provide **composable** and **data-parallel** versions of these algorithms. For the upcoming [Ranges TS](http://en.cppreference.com/w/cpp/experimental/ranges), these algorithms can also be formulated in a composable way, but without the data-parallellism that `xstd::bit_set<N>` provides.

Frequently Asked Questions
==========================

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

**Q**: Aren't there too many implicit conversions when assigning a proxy reference to an implicitly `int`-constructible class?  
**A**: No, proxy references also implicity convert to any class type that is implicitly constructible from an `int`.

**Q**: So iterating over an `bit_set` is really fool-proof?  
**A**: Yes, `bit_set` iterators are [easy to use correctly and hard to use incorrectly](http://www.aristeia.com/Papers/IEEE_Software_JulAug_2004_revised.htm).

Requirements
============

This single-header library has no other dependencies than the C++ Standard Library and is continuously being tested with the following conforming [C++17](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/n4659.pdf) compilers:

| Platform | Compiler | Versions | Build |
| :------- | :------- | :------- | :---- |
| Linux    | Clang <br> GCC | 6.0, 7, 8-SVN<br> 7.3, 8.2 | [![codecov](https://codecov.io/gh/rhalbersma/bit_set/branch/master/graph/badge.svg)](https://codecov.io/gh/rhalbersma/bit_set) <br> [![Build Status](https://travis-ci.org/rhalbersma/bit_set.svg)](https://travis-ci.org/rhalbersma/bit_set) |
| Windows  | Visual Studio  |                     15.9.4 | [![Build status](https://ci.appveyor.com/api/projects/status/hcuoesbavuw5v7y8?svg=true)](https://ci.appveyor.com/project/rhalbersma/int-set) |

License
=======

Copyright Rein Halbersma 2014-2018.
Distributed under the [Boost Software License, Version 1.0](http://www.boost.org/users/license.html).
(See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
