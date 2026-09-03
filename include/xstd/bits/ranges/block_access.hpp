//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_BITS_RANGES_BLOCK_ACCESS_HPP
#define XSTD_BITS_RANGES_BLOCK_ACCESS_HPP

#include <xstd/ints/concepts/unsigned_integer.hpp> // unsigned_integer
#include <cassert>                                 // assert
#include <concepts>                                // convertible_to
#include <cstddef>                                 // size_t
#include <limits>                                  // numeric_limits
#include <type_traits>                             // remove_const_t
#include <utility>                                 // pair

// Where the bits are stored, so both orderings can go a word at a time: block i holds [i * digits, (i + 1) * digits), padding zero.
namespace xstd::ranges {

template<class Bits>
struct block_access
{
        [[nodiscard]] static constexpr auto num_blocks(Bits const& c) noexcept
                -> std::size_t
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

// A Bits whose blocks can be reached; std::bitset's and boost::dynamic_bitset's cannot, so those keep the element-wise path.
template<class Bits_cv, class Bits = std::remove_const_t<Bits_cv>>
concept block_range =
        requires(Bits const& c, std::size_t i)
        {
                { block_access<Bits>::num_blocks(c) } -> std::convertible_to<std::size_t>;
                { block_access<Bits>::block(c, i)   } -> xstd::unsigned_integer;
        }
;

// The primitive both orderings are built on: the lowest position at which two values differ, as its block and that block's xor.
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

// Whether x holds any position strictly above the given one, which is the whole of what separates the two orderings.
template<block_range Bits>
[[nodiscard]] constexpr auto any_above(Bits const& x, std::size_t index, std::size_t offset) noexcept
        -> bool
{
        using access = block_access<Bits>;
        using block_type = decltype(access::block(x, 0UZ));
        constexpr auto digits = static_cast<std::size_t>(std::numeric_limits<block_type>::digits);

        // The bits of this block above offset, shifted out in two steps because shifting by digits is undefined.
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
