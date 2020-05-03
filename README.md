# Rebooting the `std::bitset` franchise

[![Language](https://img.shields.io/badge/language-C++-blue.svg)](https://isocpp.org/)
[![Standard](https://img.shields.io/badge/c%2B%2B-20-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization)
[![License](https://img.shields.io/badge/license-Boost-blue.svg)](https://opensource.org/licenses/BSL-1.0)
[![Lines of Code](https://tokei.rs/b1/github/rhalbersma/bit_set?category=code)](https://github.com/rhalbersma/bit_set)

`xstd::bit_set<N>` is a modern and opinionated reimagining of `std::bitset<N>`, keeping what time has proven to be effective, and throwing out what is not. `xstd::bit_set` is a **fixed-size ordered set of integers** that is compact and fast. It does less work than `std::bitset` (e.g. no bounds-checking and no throwing of `out_of_range` exceptions) yet offers more (e.g. bidirectional iterators over individual bits that seamlessly interact with the rest of the Standard Library). This enables **fixed-size bit-twiddling with set-like syntax** (identical to `std::set<int>`), typically leading to cleaner, more expressive code.

## Design choices for a `bitset` data structure

> "A `bitset` can be seen as either an array of bits or a set of integers. [...] Common usage suggests that dynamic-length `bitsets` are seldom needed."  
> Chuck Allison, [ISO/WG21/N0075](http://www.open-std.org/Jtc1/sc22/wg21/docs/papers/1991/WG21%201991/X3J16_91-0142%20WG21_N0075.pdf), November 25, 1991

The above quote is from the first C++ Standard Committee proposal on what would eventually become `std::bitset<N>`. The quote highlights two design choices to be made for a `bitset` data structure:

1. a sequence of `bool` versus an ordered set of `int`;
2. fixed-size versus variable-size storage.

A `bitset` should also optimize for both space (using contiguous storage) and time (using CPU-intrinsics for data-parallelism) wherever possible.

## The current `bitset` landscape

The C++ Standard Library and Boost provide the following optimized data structures in the landscape spanned by the aforementioned design decisions and optimization directives, as shown in the table below.

|                          | fixed-size         | variable-size                              |
| :--------------------    | :---------         | :------------                              |
| **sequence of `bool`**   | `std::bit_set<N>`  | `std::vector<bool, Allocator>` <br> `boost::dynamic_bit_set<Block, Allocator>` |
| **ordered set of `int`** | `std::bit_set<N>`  | `boost::dynamic_bit_set<Block, Allocator>` (dense) <br> `boost::container::flat_set<int, Compare, Allocator>` (sparse) |

Notes:

1. Both `std::bitset` and `boost::dynamic_bitset` are not clear about the interface they provide. E.g. both offer a hybrid of sequence-like random element access (through `operator[]`) as well as primitives for set-like bidirectional iteration (using non-Standard GCC extensions `_Find_first` and `_Find_next` in the case of `std::bitset`).
2. It [has been known](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2006/n2130.html#96) for over two decades that providing a variable-size sequence of `bool` through specializing `std::vector<bool>` was an unfortunate design choice.
3. For ordered sets, there is a further design choice whether to optimize for **dense** sets or for **sparse** sets. Dense sets require a single bit per **potential** element, whereas sparse sets require a single `int` per **actual** element. For 32-bit integers, if less (more) than 1 in 32 elements (3.125%) are actually present in a set, a dense representation will be less (more) compact than a sparse representation.
4. Only `boost::dynamic_bitset` allows storage configuration through its `Block` template parameter (defaulted to `unsigned long`).

## A reimagined `bitset` landscape

The aforementioned issues with the current `bitset` landscape can be resolved by implementing a single-purpose container for each of the four quadrants in the design space.

|                          | fixed-size                                   | variable-size                             |
| :--------------------    | :---------                                   | :------------                             |
| **sequence of `bool`**   | `xstd::bit_array<N, Block>`                  | `xstd::bit_vector<Block, Allocator>`      |
| **ordered set of `int`** | `xstd::bit_set<N, Block>` (**this library**) | `xstd::dynamic_bit_set<Block, Allocator>` |

Notes:

1. Each data structure is clear about the interface it provides: sequences are random access containers and ordered sets are bidirectional containers.
2. The variable-size sequence of `bool` is named `xstd::bit_vector` and decoupled from the general `std::vector` class template.
3. All containers use a dense (single bit per element) representation. Variable-size sparse sets of `int` can continue to be provided by `boost::container::flat_set` or by a possible future C++ Standard Library addition (as proposed in [p1222r2](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p1222r2.pdf)).
4. All containers allow storage configuration through their `Block` template parameter (defaulted to `std::size_t`).

This library provides one of the four outlined quadrants: `xstd::bit_set<N>` as a **fixed-size ordered set of `int`**. The other three quadrants are not yet implemented.

### Hello World

The code below demonstrates how `xstd::bit_set<N>` implements the [Sieve of Eratosthenes](https://en.wikipedia.org/wiki/Sieve_of_Eratosthenes) algorithm to generate all prime numbers below a compile time number `N`.

[![Try it online](https://img.shields.io/badge/try%20it-online-brightgreen.svg)](https://wandbox.org/permlink/FQCBO9pjTlo1wvxI)

```cpp
#include "xstd/bit_set.hpp"
#include <iostream>

constexpr auto N = 100;
using set_type = xstd::bit_set<N>;

int main()
{
    // initialize all numbers from [2, N)
    set_type primes;
    primes.fill();                              // renamed set() member from std::bitset<N>
    primes.erase(0);
    primes.erase(1);

    // find all primes below N
    for (auto const& p : primes) {              // range-for finds begin() / end() iterators
        auto const p_squared = p * p;
        if (p_squared >= N) break;
        for (auto n = p_squared; n < N; n += p) {
            primes.erase(n);
        }
    }

    // find all twin primes below N
    auto const twins = primes & primes >> 2;    // bitwise operators from std::bitset<N>

    // pretty-print solution (similar to Ranges TS)
    std::cout << primes << '\n';
    std::cout << twins  << '\n';
}
```

which has as output:
<pre>
[2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97]
[3,5,11,17,29,41,59,71]
</pre>

### Sequence of bits

How would the Sieve of Eratosthenes code look when using a sequence of bits? The links in the table below provide the full code examples for `std::bitset<N>` and `boost::dynamic_bitset<>`.

| Library                   | Try it online |
| :------                   | :------------ |
| `std::bitset<N>`          | [![Try it online](https://img.shields.io/badge/try%20it-online-brightgreen.svg)](https://wandbox.org/permlink/pCZBezV8DSbTdpYz) |
| `boost::dynamic_bitset<>` | [![Try it online](https://img.shields.io/badge/try%20it-online-brightgreen.svg)](https://wandbox.org/permlink/LpQn81IIVm3qbOYN) |

The essential difference (apart from differently member functions) is the lack of proxy iterators. The GCC Standard Library `libstdc++` provides member functions `_Find_first` and `_Find_next` for `std::bitset<N>` as **non-standard extensions**. For `boost::dynamic_bitset<>`, similarly named member functions `find_first` and `find_next` exist. For `boost::dynammic_bitset<>`, these can be retro-fitted into forward proxy iterators `begin` and `end`, but for `std::bitset<N>` the required user-defined specializations of `begin` and `end` inside `namespace std` entail **undefined behavior**, preventing range-for support for `std::bitset<N>`. The best one can do is a manual loop like below (or wrapped in a `for_each` non-member function)

```cpp
// find all primes below N
for (auto p = primes._Find_first(); p < N; p = primes._Find_next(p)) {
    // ...
}
```

The output of these `bitset` implemenations gives a bitstring, to be read from right-to-left:
<pre>
0010000000100000100010000010100010000010100000100000100010100010000010100000100010100010100010101100
0000000000000000000000000000100000000000100000000000000000100000000000100000000000100000100000101000
</pre>

Printing the actual bit indices requires a manual loop using the `_Find_first` and `_Find_next` extensions mentioned above.

### Ordered set of integers

How would the Sieve of Eratosthenes code look when using an ordered set of integers? The links in the table below provide the full code examples for `std::set<int>` and `boost::container::flat_set<int>`. By design, `xstd::bit_set<N>` is an almost **drop-in replacement** for either of these `set` implementations.

| Library                           | Try it online |
| :------                           | :------------ |
| `std::set<int>`                   | [![Try it online](https://img.shields.io/badge/try%20it-online-brightgreen.svg)](https://wandbox.org/permlink/Wqk9tsDYfSmb2RFN) |
| `boost::container::flat_set<int>` | [![Try it online](https://img.shields.io/badge/try%20it-online-brightgreen.svg)](https://wandbox.org/permlink/BSAu0BWw3St5TCdm) |
| `xstd::bit_set<N>`                | [![Try it online](https://img.shields.io/badge/try%20it-online-brightgreen.svg)](https://wandbox.org/permlink/54aG3PgL22P1tDpY) |

The essential difference is the lack of bitwise operators `&` and `>>` to efficiently find twin primes. Instead, one has to iterate over the ordered set of primes using `std::adjacent_find` and write these one-by-one into a new `set`. Printing the solution is also a little more involved, requiring the use of the still experimental `ostream_joiner` iterator.

```cpp
// find all twin primes below N
set_type twins;
for (auto it = primes.begin(); it != primes.end() && std::next(it) != primes.end();) {
    it = std::adjacent_find(it, primes.end(), [](auto p, auto q) {
        return q - p == 2;
    });
    if (it != primes.end()) {
        twins.insert(*it++);
    }
}

// print solution
using namespace std::experimental;
std::copy(primes.begin(), primes.end(), ostream_joiner(std::cout, ',')); std::cout << '\n';
std::copy(twins .begin(), twins .end(), ostream_joiner(std::cout, ',')); std::cout << '\n';
```

which has as output:
<pre>
2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97
3,5,11,17,29,41,59,71
</pre>

## Documentation

The interface for the class template `xstd::bit_set<N>` is the coherent union of the following five pieces:

1. An almost **drop-in** implementation of the full interface of `std::set<int>`.
2. An almost complete **translation** of the [`std::bitset<N>`](http://en.cppreference.com/w/cpp/utility/bitset) member functions into [`std::set<int>`](http://en.cppreference.com/w/cpp/container/set) equivalents.
3. The bitwise operators from [`std::bitset<N>`](http://en.cppreference.com/w/cpp/utility/bitset) and [`boost::dynamic_bitset`](https://www.boost.org/doc/libs/1_69_0/libs/dynamic_bitset/dynamic_bitset.html) reimagined as composable and data-parallel **set algorithms**.
4. The **set predicate** member functions from [`boost::dynamic_bitset`](https://www.boost.org/doc/libs/1_69_0/libs/dynamic_bitset/dynamic_bitset.html) as non-member functions.
5. Overloaded I/O streaming operators.

### 1 An almost drop-in replacement for `std::set<int>`

`xstd::bit_set<N>` is a fixed-size ordered set of integers, providing conceptually the same functionality as `std::set<int, std::less<int>, Allocator>`, where `Allocator` statically allocates memory to store `N` integers. In particular, `xstd::bit_set<N>` does **not provide**:

- **Customized key comparison**: `xstd::bit_set` uses `std::less<int>` as its default comparator (accessible through its nested types `key_compare` and `value_compare`). In particular, the `xstd::bit_set` constructors do not take a comparator argument.
- **Allocator support**: `xstd::bit_set` is a fixed-size set of non-negative integers and does not dynamically allocate memory. In particular, `xstd::bit_set` does **not provide** a `get_allocator()` member function and its constructors do not take an allocator argument.
- **Splicing**: `xstd::bit_set` is **not a node-based container**, and does not provide the splicing operations as defined in [p0083r3](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0083r3.pdf). In particular, `xstd::bit_set` does **not provide** the nested types `node_type` and `insert_return_type`, the `extract()` or `merge()` member functions, or the `insert()` overloads taking a node handle.

Minor **semantic differences** between common functionality in `xstd::bit_set<N>` and `std::set<int>` are:

- the `xstd::bit_set` member fucntion `max_size` is a `static` member function (because `N` is a constant expression).
- the `xstd::bit_set` member function `clear` returns `*this` instead of `void` as for `std::set`, to allow better chaining of member functions (consisent with `std::bitset`).
- the `xstd::bit_set` iterators are **proxy iterators**, and taking their address yields **proxy references**. The difference is almost undetectable. See the FAQ at the end of this document.

With these caveats in mind, all fixed-size, defaulted comparing, non-allocating, non-splicing `std::set<int>` code in the wild should continue work out-of-the-box with `xstd::bit_set<N>`.

### 2 An almost complete translation of `std::bitset<N>`

Almost all `std::bitset<N>` expressions have **a direct translation** (i.e. achievable through search-and-replace) to an equivalent `std::set<int>` expression, that is fully supported by `xstd::bit_set<N>`.

| `std::bitset<N>`   | `xstd::bit_set<N>`                     | Notes                                           |
| :---------------   | :-----------------                     | :----                                           |
| `bs.set()`         | `bs.fill()`                            | not a member of `std::set<int>`                 |
| `bs.set(pos)`      | `bs.insert(pos)`                       | no bounds-checking or `out_of_range` exceptions |
| `bs.set(pos, val)` | `val ? bs.insert(pos) : bs.erase(pos)` | no bounds-checking or `out_of_range` exceptions |
| `bs.reset()`       | `bs.clear()`                           | returns `*this`, not `void` as `std::set<int>`  |
| `bs.reset(pos)`    | `bs.erase(pos)`                        | no bounds-checking or `out_of_range` exceptions |
| `bs.flip()`        | `bs.complement()`                      | not a member of `std::set<int>`                 |
| `bs.flip(pos)`     | `bs.replace(pos)`                      | no bounds-checking or `out_of_range` exceptions <br> not a member of `std::set<int>` |
| `bs.count()`       | `bs.size()`                            | |
| `bs.size()`        | `bs.max_size()`                        | is a `static` member                            |
| `bs.test(pos)`     | `bs.contains(pos)` (C++20)             | no bounds-checking or `out_of_range` exceptions |
| `bs.all()`         | `bs.full()`                            | not a member of `std::set<int>`                 |
| `bs.any()`         | `!bs.empty()`                          | |
| `bs.none()`        | `bs.empty()`                           | |
| `bs[pos]`          | `bs.contains(pos)` (C++20)             | |
| `bs[pos] = val`    | `val ? bs.insert(pos) : bs.erase(pos)` | |

The semantic differences between `xstd::bit_set<N>` and `std::bitset<N>` are:

- `xstd::bit_set` has a `static` member function `max_size()`;
- `xstd::bit_set` does not do bounds-checking for its members `insert`, `erase`, `replace` and `contains`. Instead of throwing an `out_of_range` exception for argument values outside the range `[0, N)`, this **behavior is undefined**. This gives `xstd::bit_set<N>` a small performance benefit over `std::bitset<N>`.

Functionality from `std::bitset<N>` that is not in `xstd::bit_set<N>`:

- **Constructors**: No construction from `unsigned long long`, `std::string` or `char const*`.
- **Conversion operators**: No conversion to `unsigned long`, `unsigned long long` or `std::string`.
- **Hashing**: No specialization for `std::hash<>`.

The hashing functionality can be obtained through third-party libraries such as [N3980](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n3980.html) by streaming the `xstd::bit_set<N>` elements and size through an overloaded `hash_append` function.

Note that the `xstd::bit_set` members `fill`, `complement`, `replace` and `full` do not exist for `std::set`. In other words: `std::set<int>` is not a drop-in replacment for `xstd::bit_set<N>` code that uses these member functions.

### 3 The bitwise operators from `std::bitset` and `boost::dynamic_bitset` reimagined as set algorithms

The bitwise operators (`&=`, `|=`, `^=`, `-=`, `~`, `&`, `|`, `^`, `-`) from `std::bitset` and `boost::dynamic_bitset` are present in `xstd::bit_set` with **identical syntax** and **identical semantics**. Note that the bitwise difference operators (`-=` and `-`) are not present in `std::bitset`.

The bitwise-shift operators (`<<=`, `>>=`, `<<`, `>>`) from `std::bitset` and `boost::dynamic_bitset` are present in `xstd::bit_set` with **identical syntax**, but with the **semantic difference** that `xstd::bit_set<N>` does not support bit-shifting for lengths `>= N`. Instead of calling `clear()` for argument values outside the range `[0, N)`, this **behavior is undefined**. Note that these semantics for `xstd::bit_set<N>` are identical to bit-shifting on native unsigned integers. This gives `xstd::bit_set<N>` a small performance benefit over `std::bitset<N>`.

With the exception of `operator~`, the non-member bitwise operators can be reimagined as **composable** and **data-parallel** versions of the set algorithms on sorted ranges from the C++ Standard Library header `<algorithm>`. For the upcoming [Ranges TS](http://en.cppreference.com/w/cpp/experimental/ranges), these algorithms can also be formulated in a composable way, but without the data-parallelism that `xstd::bit_set<N>` provides.

| `xstd::bit_set<N>`                | `std::set<int>` |
| :-----------------                | :-------------- |
| `auto c = a & b;`                 | `set<int> c;` <br> `set_intersection(begin(a), end(a), begin(b), end(b), inserter(c, end(c)));` |
| <code>auto c = a &#124; b;</code> | `set<int> c;` <br> `set_union(begin(a), end(a), begin(b), end(b), inserter(c, end(c)));` |
| `auto c = a ^ b;`                 | `set<int> c;` <br> `set_symmetric_difference(begin(a), end(a), begin(b), end(b), inserter(c, end(c)));` |
| `auto c = a - b;`                 | `set<int> c;` <br> `set_difference(begin(a), end(a), begin(b), end(b), inserter(c, end(c)));` |
| `auto b = a << n;`                | `set<int> tmp, b;` <br> `transform(begin(a), end(a), inserter(tmp, end(tmp)), [=](int x) { return x + n; });` <br> `copy_if(begin(tmp), end(tmp), inserter(b, end(b)), [](int x) { return x < N; });` |
| `auto b = a >> n;`                | `set<int> tmp, b;` <br> `transform(begin(a), end(a), inserter(tmp, end(tmp)), [=](int x) { return x - n; });` <br> `copy_if(begin(tmp), end(tmp), inserter(b, end(b)), [](int x) { return 0 <= x; });` |

### 4 Set predicates from `boost::dynamic_bitset`

### 5 Overloaded I/O streaming operators

The semantic differences between `std::bitset` and `xstd::bit_set` are most visible in its overloaded I/O streaming `operator<<` and `operator>>`. The former reads and writes a bitstring right-to-left, whereas the latter reads and writes an ordered set of integers left-to-right, formatted as a comma-separated sequence between brackets. Note that `std::set<int>` lacks overloaded I/O streaming operators.

## Frequently Asked Questions

### Iterators

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

**Q**: So iterating over an `xstd::bit_set` is really fool-proof?  
**A**: Yes, `xstd::bit_set` iterators are [easy to use correctly and hard to use incorrectly](http://www.aristeia.com/Papers/IEEE_Software_JulAug_2004_revised.htm).

### Bit-ordering

**Q**: How is `xstd::bit_set` implemented?  
**A**: It uses a stack-allocated array of unsigned integers as bit storage.

**Q**: What is the bit-ordering inside this array?  
**A**: The lowest (highest) set-value correspond to the most (least) significant bits of the highest (lowest) array word.

**Q**: Why was this bit-ordering chosen?  
**A**: To efficiently satisfy the requirement `a < b == std::lexicographical_compare(begin(a), end(a), begin(b), end(b))`.

**Q**: What do you mean with "efficient" lexicographical comparison?  
**A**: `xstd::bit_set` implements `L < R` as a data-parallel lexicographical comparison on the array words `wL` and `wR`.

**Q**: How is this connected to the bit-ordering within words?  
**A**: The array-word comparison is `wL > wR`, which starts with comparing the most significant bits of the two words.

### Storage type

**Q**: What storage type does `xstd::bit_set` use?  
**A**: By default, `xstd::bit_set` uses an array of `std::size_t` integers.

**Q**: Can I customize the storage type?  
**A**: Yes, the full class template signature is `template<std::size_t N, std::unsigned_integral Block = std::size_t> xstd::bit_set`.

**Q**: What other storage types can be used as template argument for `Block`?  
**A**: Any unsigned integral type, including (for GCC and Clang) the non-Standard `__uint128_t`.

**Q**: Does the `xstd::bit_set` implementation optimize for the case of a small number of words of storage?  
**A**: Yes, there are three special cases for 0, 1 and 2 words of storage, as well as the general case of 3 or more words.

## Requirements

This single-header library has no other dependencies than the C++ Standard Library and is continuously being tested with the following conforming [C++20](http://www.open-std.org/jtc1/sc22/wg21/prot/14882fdis/n4860.pdf) compilers:

| Platform | Compiler | Versions | Build |
| :------- | :------- | :------- | :---- |
| Linux    | Clang <br> GCC | 10, 11-SVN<br> 10, 11-SVN | [![codecov](https://codecov.io/gh/rhalbersma/bit_set/branch/master/graph/badge.svg)](https://codecov.io/gh/rhalbersma/bit_set) <br> [![Build Status](https://travis-ci.org/rhalbersma/bit_set.svg)](https://travis-ci.org/rhalbersma/bit_set) |
| Windows  | Visual Studio  | 2019                      | [![Build status](https://ci.appveyor.com/api/projects/status/hcuoesbavuw5v7y8?svg=true)](https://ci.appveyor.com/project/rhalbersma/bit-set) |

Note that this library makes liberal use of C++20 features, in particular Concepts, `constexpr` algorithms and the `<=>` operator for comparisons. Not all compilers and Standard Libraries support these yet. Libc++ does not yet support most of the mentioned C++20 features, so the Clang compiler is only tested with libstdc++. Visual Studio 2019 supports these features as of version 16.7, but on AppVeyor only version 16.5 is available so far.

## License

Copyright Rein Halbersma 2014-2020.  
Distributed under the [Boost Software License, Version 1.0](http://www.boost.org/users/license.html).  
(See accompanying file LICENSE_1_0.txt or copy at [http://www.boost.org/LICENSE_1_0.txt](http://www.boost.org/LICENSE_1_0.txt))
