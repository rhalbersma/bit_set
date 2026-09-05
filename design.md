# Design notes

Why the code is shaped the way it is. The headers carry one line each; the reasoning lives here, and a
one-line comment ending in `[design.md#anchor]` points at the section that explains it.

Decisions still in flight are recorded on [#80](https://github.com/rhalbersma/xstd-bits/issues/80); this
file holds what has landed.

## Storage and containers

### block-storage

`block_storage` asks whether a range **is** blocks: a contiguous, sized range of unsigned integers.
`std::array` and `std::vector` both qualify, and so does `std::inplace_vector` — a runtime width over
static capacity, for free.

`xstd::ranges::block_range` is the other side of the same word, and asks whether a bit container will
**hand its blocks over**. Nothing models both, and no scope sees both unqualified.

### the-one-vehicle

`block_sequence<Blocks, N>` is the single storage vehicle. `N` is the width when that is a constant and
`std::dynamic_extent` when the width is carried at run time.

It owns the **unused-tail invariant** — every bit at or above `size()` reads zero — which is what makes
whole-block comparison, popcount and the block-at-a-time scans mean anything at all.

It has no iterators and no proxies. Those are readings, and a reading here would be picking one; it would
also make `std::ranges` see a sequence of blocks rather than of bits.

The hand-unrolled one- and two-block cases are where the static performance lives, so they stay
compile-time branches. The general path they fall through to is written over `m_blocks` as a range, and
therefore serves the dynamic width unchanged.

### padding

`static_used_bits` is the mask of the last block that is not padding. `num_bits` is `align_up(N)`, so
`num_bits - N` lies in `[0, bits_per_block)` and the shift is always in range.

Width zero is the one case that form cannot express — there is nothing to align up, so it reports no
padding where in truth the sole block is all of it — and it gets a selection instead.

**Naming zero rather than computing it matters on MSVC**, which constant-folds both arms of a `?:` and
answers C4293, *shift count too big*, on the arm it discards. `used_bits()` is the same two cases at a
run-time width.

### default-construction

A defaulted default constructor plus an NSDMI, rather than two constructors constrained on the extent:
`std::vector` default-constructs empty, and the at-least-one-block invariant has to hold from the start.

## Scans

### inclusive-is-the-primitive

Inclusive and exclusive name the only thing separating the two forward scans: whether `n` itself is a
candidate. That is a property of the scan rather than of a reading, which is why this is not called
`lower_bound` — that is the set reading's word, and this layer serves the sequence reading equally.
`bit_finite_set::lower_bound` is where the set name belongs, and it maps here one for one.

The inclusive form is the primitive and the exclusive one is derived, because the reverse does not close:

```
exclusive_find_next(n) == inclusive_find_next(n + 1)     for every n
find_first()           == inclusive_find_next(0)
```

Exclusive-first would need `find_first() == exclusive_find_next(-1)`, and `size_t` has no such value —
which is exactly why the container used to branch on `x == 0`. **Both derivations are `+ 1` and never
`− 1`**, so nothing wraps at zero. Boost's `find_next` and libstdc++'s `_M_do_find_next` are both the
exclusive form, correctly: their iteration idiom is `find_first()` then `find_next(i)`, which never needs
the inclusive one.

`n == size()` rather than `n >= size()`: the precondition is `n <= size()`, so `size()` is the only value
the scan cannot start from. `is_valid` does not say this — it is `n < size()` — and `size()` is a
legitimate argument, meaning "no such position".

### offset-guards

Neither forward scan guards on `offset != 0`. `>> 0` is the identity, so the masked test is correct at a
block boundary too, and advancing past that block afterwards is right either way. Neither reference
implementation guards it — boost shifts by `ind` then falls to `m_do_find_from(blk + 1)`, libstdc++ masks
by `~0 << whichbit` then does `__i++` — and the guard skips no work: at offset 0 it merely moves the same
test into the loop's first iteration. [#88](https://github.com/rhalbersma/xstd-bits/issues/88) measured it
as pure cost.

The reverse scan's `reverse_offset != 0` guard is a different question, and is not about the shift:
`left_bit - offset` is in `[0, left_bit]`, so shifting is always in range and by zero is the identity. It
is about which block the range scan must start at — at `index` when the whole block is in range, below it
when the masked block came up empty. Naming the fallback block outright, as the two-block case does, makes
that distinction disappear.

### two-block-case

At two blocks the tail is one named block rather than a range: the general walk costs more than the whole
scan is worth at this width. `index` is a run-time value — `n` is — but the block count is not, so the
second half is a test and not a loop.

**Indexed rather than branched**: `m_blocks[index]`, not an `if` on `index`, so the common path is one
computed load. Only when the starting block comes up empty does it matter which block we started in, and
then only to decide whether block 1 is still ahead. Writing this as a branch instead cost **10 instructions
at `-O3 -march=native`**.

### index-walks

A plain index walk rather than `drop` + `find_if`, and a descending one rather than `reverse` + `drop` +
`find_if`. The iterator forms had to recover the block's position with `distance()`; the index *is* that
position, so it never needed recovering. The reverse form composed two adaptors only to have `distance()`
undo them.

## Contracts

### total-versus-precondition

`block_sequence::exclusive_find_prev` is deliberately **not** total. Where `inclusive_find_next` answers
`size()` for "nothing at or above", this one has a precondition instead, and that is the whole of why it is
three instructions cheaper at every width: it never materializes a not-found value.

It is safe because **reverse iteration supplies the guard the function does not**. `rend()` is
`make_reverse_iterator(begin())`, and `std::reverse_iterator` stops there, so `operator--` is never applied
at `begin()`. A hand-written loop gets no such help: the forward idiom terminates itself against `size()`,
the reverse one runs off the bottom.

Totality is the **container's** contract to keep, not this layer's to absorb —
[#86](https://github.com/rhalbersma/xstd-bits/issues/86) settled the same division one layer down.

### the-wraparound-assert

`exclusive_find_prev` asserts `is_valid(n - 1)`, mirroring `exclusive_find_next`'s `is_valid(n)`: each says
the position actually scanned from is one this container has.

`n - 1` is that position here, and **the wraparound does the work at the bottom** — `0 - 1` is `SIZE_MAX`,
which no width admits — so this states `1 <= n <= size()` without a second predicate. `is_valid` alone
would be wrong: `size()` is a legitimate argument, meaning "from the end".

### the-ceiling-principle

The door's contract is the most efficient form, so `bit_traits<block_sequence<...>>` keeps the contracts
`block_sequence` gives it rather than widening to the total ones the synthesised walks happen to provide.

**A fallback synthesised for a foreign type may be more generous than the contract; it may not be less.**

So `find_next` requires `is_valid(n)`, and `find_prev` requires a set position strictly below `n` — which
`any()` does not establish, a container whose set positions all lie above `n` having none below it.

## The door

### opt-in

`bit_traits` is declared and never defined. Adaptation is opt-in rather than guessed, so a type nobody has
adapted is a compile error naming an incomplete type, rather than a silent fallback onto whatever members
happened to answer. That is the failure per-operation member probing walks into, and the door's reason for
existing.

`bit_storage` gates the adaptors on the floor rather than on `bit_traits<Bits>` being complete, which turns
*incomplete type* into *constraint not satisfied* — the error that names the real problem.

### detection-by-absence

`block_readable` is meaningful **only because nothing supplies a default**: a specialization that does not
spell `block()` genuinely has no block access, so absence is an answer rather than an oversight.

That is also why the walks are free functions and not a base class to inherit from. A base would satisfy
the concept for every type, and the tier choice would collapse silently — and the same applies to the
`requires` probes for `checked_test` and `checked_shift_left`, which read a missing entry as "this backend
has none".

### why-nested

The walks live in `xstd::detail::bits` rather than in `xstd`, and the nesting is load-bearing.

Since C++20 ([temp.names]/3, P0846) an unqualified `scan_first<Traits>(c)` parses its `<` as a template
argument list and then performs ADL **with the explicit template arguments included** — so `std` and
`boost`, the associated namespaces of the very types being adapted, would join the overload set.
`[namespace.std]` bars *users* from adding to `std` but not implementations, and boost is under no such
constraint.

Down here nothing is visible unqualified from `xstd`, so the qualification is enforced by **scoping** rather
than by remembering a prefix at every call site — which is what a class was previously substituting for.

### block-writes

`set_block` is the write side of `block()`, and deliberately not a trait entry: block writes are only ever
needed on storage we control, and the unused tail is `block_sequence`'s to keep.

## Orderings

### two-readings-disagree

"Lexicographic" is underspecified below the reading layer. Both readings are lexicographic; they order over
different sequences, and **they disagree**:

| `{0,1}` against `{1}` | compared as | result |
|---|---|---|
| set reading | ascending positions, `[0,1]` against `[1]` | `{0,1} < {1}` |
| sequence reading | bools from index 0, `[1,1,0…]` against `[0,1,0…]` | `{0,1} > {1}` |

A door serving three readings cannot hold one of their orderings without choosing for its callers, so it
holds neither under that name. `set_three_way` and `sequence_three_way` are each owned by the view whose
reading defines them.

### the-ordering-invariant

Every ordering in the library satisfies

```cpp
std::lexicographical_compare_three_way(a.begin(), a.end(), b.begin(), b.end()) == (a <=> b)
```

which is stated on the **reading**, never on the storage: `block_sequence` has no `begin()`/`end()`, and
`boost::dynamic_bitset` has no public iterators, so it cannot be written against a backend at all.

Two things follow. `basic_bitset` has no `operator<=>` because it does not iterate — the existing "no
iteration and no `<=>` here by design" is a consequence rather than a separate rule. And magnitude ordering
(boost's `operator<`, a top-down block walk) is the lexicographic order of *reverse* iteration, so no
forward-iterating container obeying the invariant can have it; boost escapes only by having no iterators to
be constrained by.

Where a specialization offers nothing faster, the default is that standard algorithm over the reading's own
iterators — so the default cannot disagree with the specification, and only an optimization can.

## Coverage

### per-instantiation-slots

gcovr counts branch slots **per template instantiation and never merges them**. A loop that a one-block
instantiation can never enter is a branch never taken, whatever every other instantiation does.

So a degenerate width does not get an unreachable loop; it gets **different code**, via `if constexpr` on
`static_num_blocks == 1` and `== 2` in `block_sequence`, and on `static_block_count` and `extent == 0` in
the walks. `static_block_count` is derived from `extent` rather than declared for exactly this reason:
nothing new has to be supplied to know it, since `block_readable`'s contract already fixes the layout.

The same gate is why a cursor lives inside the walk it belongs to rather than beside the block index: at one
block the walk is discarded, and a cursor declared outside would never be written — which
`misc-const-correctness` reads, correctly, as a variable that should have been `const`.

### while-not-for

Two descending walks are shaped as a `while` rather than a `for` with a fall-through, deliberately. The
precondition guarantees a set bit at or below, so a loop that could run out would carry an exit branch no
test can take — exactly what the coverage gate catches, and what `find_if` hid by keeping that branch inside
the standard library. Written as a `while`, both branches of the condition are exercised: empty blocks are
skipped, a non-empty one ends it.

### one-function-per-tier

Each tier of each scan is its own function. `readability-function-cognitive-complexity` counts `if constexpr`
like any other branch, and the guards that satisfy the coverage gate put both scans over the threshold when
the tier choice shares the body. The tier was the seam anyway.

## Platform workarounds

### libcxx-views-take

`all_but_last_are_ones` uses an iterator pair rather than `views::take`.

libc++ 18 — Xcode 16.4 on the matrix — writes the return type of `views::take`'s `iota_view` fast path as
`decltype(iota_view(*begin(rng), ...))`, so forming the adaptor's `operator|` over a range of 128-bit blocks
instantiates an `iota_view` over that block type, however unlike an `iota_view` a `std::vector` is. That
trips libc++'s *bigger than integer-like type* `static_assert`, which is a hard error rather than a
substitution failure.

`views::drop`, `views::reverse`, `views::transform` and `views::zip` are all clear. Only `take` carries it,
and only until Xcode 16.4 leaves the matrix.

### gcc-array-bounds

The shift operators assert `n_blocks <= last_block()`, which is implied by the `is_valid(n)` above it. It is
stated again because GCC does not carry the range through `xstd::div`'s aggregate return: without it,
`-Warray-bounds` reports the memmove inside `shift_right` at `-O1` and `-O2` whenever asserts are live.

No CMake build type is that combination — Debug is `-O0`, and the optimized ones carry `NDEBUG` — but
`-O2 -g` is one command away, and the bound is worth saying in any case.

### uint128-printing

Never `BOOST_CHECK_EQUAL` on a block-typed value. It prints its operands on failure, and no standard library
defines `operator<<` for `__int128`, so a `graded_extents` sweep that includes `xstd::uint128` fails to
compile on libc++ where it happens to compile on libstdc++. `BOOST_CHECK` compares without printing.

## Naming

### test-not-subscript

`block_sequence::test` rather than `operator[]`: this reads and cannot be written through. `std::bitset`'s
`operator[]` returns an assignable proxy and this returns `bool`, so the subscript spelling would promise an
assignment that does not compile.

The writable proxy belongs to the containers above, which is also where the checked reading lives —
`std::bitset::test` throws where this asserts, a difference the door states as `unchecked_test` rather than
one this name should try to carry.

### qualifier-prefixes

Contract qualifiers are prefixes, not suffixes — `inclusive_find_next`, `exclusive_find_prev`,
`unchecked_test`, `checked_shift_left` — so that the contract reads before the operation and the cheaper
form cannot be called by mistake.
