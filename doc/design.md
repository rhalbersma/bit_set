# xstd-bits

## Two vehicles, three Standard sections

The whole library is two storage vehicles carrying the same three interfaces.

|                     | array vehicle              | vector vehicle                |
| ------------------- | -------------------------- | ----------------------------- |
| storage             | `bit::array<N, Block>`     | `bit::vector<Block, Alloc>`   |
| `<array>`/`<vector>`| `bit_array<N, Block>`      | `bit_vector<Block, Alloc>`    |
| `<set>`             | `finite_bit_set<N, Block>` | `bit_set<Block, Alloc>`       |
| `<bitset>`          | `bitset<N, Block>`         | `dynamic_bitset<Block, Alloc>`|

Rows are Standard sections, columns are vehicles. The unqualified name goes to the
dynamic member of each pair, as `std::vector` has it against `std::array`, so the
fixed-size set is `finite_bit_set` and the dynamic one is `bit_set`.

The bottom row is not a third interface. It exists to show that the same two
vehicles reproduce the legacy types at no cost -- `xstd::bitset` against
`std::bitset`, `xstd::dynamic_bitset` against `boost::dynamic_bitset` -- so a thin
interface over the array implementation throws nothing away.

Every container the library provides is one header named after the type it
declares, directly under `xstd/bits`. A directory per Standard section was tried
and dropped: with one entity behind each, `xstd/bits/set/finite_bit_set.hpp` and
`xstd/bits/bitset/bitset.hpp` spent a path component to say what the filename
already said, and the section umbrellas above them re-exported a single header
apiece. The Standard's own sections are still the organizing idea -- they are the
rows of the table above -- they are just not directories.

```
xstd/bits.hpp                 the front door, over every container and the views
xstd/bits/bit_array.hpp       one header per container, named for the type
xstd/bits/bitset.hpp
xstd/bits/finite_bit_set.hpp
xstd/bits/impl/               the vehicles and the block operations, namespace xstd::bit
xstd/bits/ranges/             set_view, array_view, bit_extent
xstd/bits/ext/                the adaptors, asked for by name
```

`impl` rather than `bit` for the vehicles' directory, because flattening made
`bits/bit/array.hpp` and `bits/bit_array.hpp` derive the same include guard, and
the way out of that is not the `XSTD_SUBDIR_BIT_SUBDIR_ARRAY_HPP` spelling this
tree just got rid of. The namespace stays `xstd::bit`: the directory says these
headers are implementation, and that is a separate question from what to call the
bit-level operations, which do not need a namespace layer to hide behind.

## There is no bitset-like concept

A sequence of bits is one of exactly two things: a set of `size_t`, or a sequence
of `bool`. `std::bitset` is neither, or rather it is both at once under a third
vocabulary that names the same operations differently:

| bitset spells | a set calls it | a sequence calls it |
| ------------- | -------------- | ------------------- |
| `count()`     | `size()`       | -                   |
| `size()`      | `max_size()`   | `size()`            |
| `set(pos)`    | `insert(pos)`  | `v[pos] = true`     |
| `reset(pos)`  | `erase(pos)`   | `v[pos] = false`    |
| `reset()`     | `clear()`      | `fill(false)`       |
| `test(pos)`   | `contains(pos)`| `at(pos)`           |
| `none()`      | `empty()`      | -                   |

So the tests carry two contracts, not three-plus-a-bitset-concept: the `[set]`
contract and the sequence contract. A legacy bitset enters either one through a
view.

## Views

A view here is span-shaped, not string_view-shaped: non-owning, but **mutable
through**. Its job is to rewire misnamed members into the nomenclature of the
interface that is struggling to get out, not to freeze them.

`xstd::set_view(b)` presents any bit sequence as a `std::set<size_t>`: the
container typedefs, bidirectional iterators, `empty`/`size`/`max_size`,
`insert`/`erase`/`clear`, `find`/`count`/`contains`/`lower_bound`/`upper_bound`/
`equal_range`, and `==`/`<=>`.

`xstd::array_view<Bits, Extent>(b)` presents the same storage as a sequence of
`bool`, span-shaped with a `dynamic_extent` default: `operator[]`, `at`, `front`,
`back`, `size`, `empty`, `fill`, and `==`/`<=>`. One template rather than an
`array_view`/`vector_view` pair, for the reason `std::span` is one template: the
two differ only in whether `size()` is a constant expression.

Two customization points, both defaulted so that `std::bitset`,
`boost::dynamic_bitset` and our own types all work unspecialized:

- `set_find<Bits>` -- where the elements are: `first`, `last`, `next`, `prev`.
- `set_ops<Bits>` -- how one goes in and comes out: `insert`, `erase`, `clear`.

`set_find` keeps its explicit-specialization design rather than plain ADL, because
`std::bitset`'s only associated namespace is `std`, where a program may not add
declarations, and because `boost::dynamic_bitset`'s own member `begin()`/`end()`
shadow any same-named free function. `set_ops` needs neither: a member call is
always found, so its default just probes for `insert`/`erase`/`clear` and falls
back to `set`/`reset`.

Open: whether `is_subset_of`, `is_proper_subset_of` and `intersects` stay members
of `set_view`. They are not in `[set]`, and lazy set intersection and union may be
the better home for what they do.

## Tests mirror the headers

Every public header is answered by exactly one test source at the mirrored path,
enforced at configure time. A leaf test states what one entity does; an umbrella
test states what holds *across* the entities behind it, which no leaf can see.

Sweep depth is a separate axis. The exhaustive suites run over every set of
cardinality 1 through 4, which is a translation-unit splitting device, not a
statement about a header -- so those sources sit beside the leaves in the section
directory, named for the cost they carry (`constant`, `linear`, `quadratic`,
`cubic`, `quartic`) as their Boost.Test suites already are.

The type lists live in one header per contract rather than being copy-pasted per
source, the way `exact_width_types.hpp` does it in xstd:

- set-like: `std::set<size_t>`, `std::flat_set<size_t>`, `finite_bit_set<N, Block>`,
  `bit_set<Block, Alloc>`, and `set_view` over each legacy bitset.
- sequence-like: `std::array<bool, N>`, `bit_array<N, Block>`, `std::vector<bool>`,
  `bit_vector<Block, Alloc>`, and `array_view` over each legacy bitset.
- `[bitset]` members: `std::bitset<N>`, `xstd::bitset<N, Block>`,
  `boost::dynamic_bitset<>`, `xstd::dynamic_bitset<Block, Alloc>`.

Because the views are complete and mutable, the set primitives split once, by what
they need rather than per type: nothing has to be carved out for a type whose view
lacks a member.

The prime sieve is a benchmark of dynamic containers only, so that it compares
like with like. A fixed-size `finite_bit_set<N>` sifting a universe it was sized
for is not measuring the same thing as a `std::set` growing and shrinking.
