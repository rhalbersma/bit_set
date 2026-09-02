//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_BITS_RANGES_BLOCK_ACCESS_HPP
#define XSTD_BITS_RANGES_BLOCK_ACCESS_HPP

#include <xstd/bits/detail/intrin.hpp>             // countr_zero
#include <xstd/ints/concepts/unsigned_integer.hpp> // unsigned_integer
#include <cassert>                                 // assert
#include <concepts>                                // convertible_to
#include <cstddef>                                 // size_t
#include <limits>                                  // numeric_limits
#include <type_traits>                             // remove_const_t
#include <utility>                                 // pair

// Where the bits are stored, for the operations that can go faster knowing it.
//
// The two orderings are defined over iteration -- keys in increasing order, or
// bools in position order -- and iteration is what makes them element-at-a-time.
// A type that will say where its blocks are can have both answered a word at a
// time instead, and this is how it says. Same ADL-with-explicit-specialization
// design as set_find and array_find, for the same reason: a foreign type whose
// only associated namespace is std cannot be given ADL hooks [namespace.std],
// but it can be given a specialization.
//
// The layout this assumes, and the one every provider here has: block i holds
// positions [i * digits, (i + 1) * digits), and within a block the least
// significant bit is the lowest position. That is xstd::detail::bits::array's
// layout -- its find_first is countr_zero(block) + bits_per_block * index --
// and it is also boost::dynamic_bitset's and the Microsoft STL's.
//
// It also assumes the padding above the last position is zero. Every provider
// here maintains that: xstd::detail::bits::array's operator== compares whole
// blocks, and a std::bitset that did not would get count() and all() wrong.
namespace xstd::ranges {

template<class Bits>
struct block_access
{
        [[nodiscard]] static constexpr std::size_t num_blocks(Bits const& c) noexcept
                requires requires { { block_count(c) } -> std::convertible_to<std::size_t>; }
        {
                return block_count(c);
        }

        [[nodiscard]] static constexpr auto block(Bits const& c, std::size_t i) noexcept
                requires requires { block_at(c, i); }
        {
                return block_at(c, i);
        }
};

// A Bits whose blocks can be reached. Not every one can: std::bitset's words are
// private on libstdc++ and libc++ and reachable only on the Microsoft STL, and
// boost::dynamic_bitset publishes its blocks only through to_block_range, which
// copies them out -- no use inside a noexcept comparison. Those keep the
// element-wise path, which is why it stays rather than being replaced.
template<class Bits_cv, class Bits = std::remove_const_t<Bits_cv>>
concept block_range =
        requires(Bits const& c, std::size_t i)
        {
                { block_access<Bits>::num_blocks(c) } -> std::convertible_to<std::size_t>;
                { block_access<Bits>::block(c, i)   } -> xstd::unsigned_integer;
        }
;

// The primitive both orderings are built on: the lowest position at which two
// values differ, as the block holding it and the xor of that block. Everything
// either ordering needs follows from those two, and finding them is the only
// part that touches every block.
//
// Returns num_blocks and zero when the two are equal.
template<block_range Bits>
[[nodiscard]] constexpr auto first_difference(Bits const& x, Bits const& y) noexcept
{
        using access = block_access<Bits>;
        using block_type = decltype(access::block(x, 0UZ));

        auto const n = access::num_blocks(x);
        assert(n == access::num_blocks(y));

        for (auto i = 0UZ; i < n; ++i) {
                if (auto const diff = static_cast<block_type>(access::block(x, i) ^ access::block(y, i)); diff != block_type{}) {
                        return std::pair{ i, diff };
                }
        }
        return std::pair{ n, block_type{} };
}

// Whether x holds any position strictly above the one at bit `offset` of block
// `index`. This is the whole of what separates the set ordering from the
// sequence one: a set that has nothing further is a prefix of the other, and a
// prefix is the smaller.
template<block_range Bits>
[[nodiscard]] constexpr bool any_above(Bits const& x, std::size_t index, std::size_t offset) noexcept
{
        using access = block_access<Bits>;
        using block_type = decltype(access::block(x, 0UZ));
        constexpr auto digits = static_cast<std::size_t>(std::numeric_limits<block_type>::digits);

        // The bits of this block above `offset`. Shifting by digits is undefined,
        // so the last position in a block is shifted out in two steps.
        if (offset + 1 < digits and static_cast<block_type>(access::block(x, index) >> (offset + 1)) != block_type{}) {
                return true;
        }
        for (auto i = index + 1; i < access::num_blocks(x); ++i) {
                if (access::block(x, i) != block_type{}) {
                        return true;
                }
        }
        return false;
}

} // namespace xstd::ranges

#endif // XSTD_BITS_RANGES_BLOCK_ACCESS_HPP
