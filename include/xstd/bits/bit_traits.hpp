//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_BITS_BIT_TRAITS_HPP
#define XSTD_BITS_BIT_TRAITS_HPP

#include <xstd/bits/detail/intrin.hpp>             // countl_zero, countr_zero, popcount
#include <xstd/ints/concepts/unsigned_integer.hpp> // unsigned_integer
#include <cassert>                                 // assert
#include <compare>                                 // strong_ordering
#include <concepts>                                // convertible_to, same_as
#include <cstddef>                                 // size_t
#include <limits>                                  // digits
#include <span>                                    // dynamic_extent
#include <type_traits>                             // remove_cvref_t

// The one door between a bit container and the readings built on it. Everything above this
// header asks bit_traits<Bits> and never the type itself, which is what lets a std::bitset, a
// boost::dynamic_bitset and our own block_sequence wear the same three readings without any of
// them knowing the others exist.
namespace xstd {

// Declared, never defined: adaptation is opt-in, never guessed. A type nobody has adapted is a
// compile error naming an incomplete type rather than a silent fallback onto whatever members
// happened to answer -- which is the failure that per-operation member probing walks into, and
// this file's reason for existing. See bit_storage below for the error that reads better.
template<class Bits>
struct bit_traits;

// Whether a trait offers block access at all. std::bitset on libc++ and boost::dynamic_bitset
// do not, so their scans take the element-wise tier; ours and libstdc++'s std::bitset do.
template<class Traits, class Bits>
concept block_readable =
        requires (Bits const& c, std::size_t i)
        {
                { Traits::num_blocks(c) } -> std::convertible_to<std::size_t>;
                { Traits::block(c, i)   } -> xstd::unsigned_integer;
        }
;

// The fallbacks a specialization calls for whatever it has no native spelling of, so adapting a
// type is never all-or-nothing.
//
// Static members of a class template rather than free functions, and that is load-bearing.
// Since C++20 an unqualified scan_first<Traits>(c) parses its '<' as a template argument list
// and then goes through ADL, explicit arguments included -- so std and boost, the associated
// namespaces of the very types being adapted, would join the overload set. [namespace.std] bars
// users from adding to std but not implementations, and boost is under no such constraint.
// Qualifying every call would close it, but the safety would live in remembering a prefix at
// every site. Member lookup through a qualified-id never consults associated namespaces, so
// there is no unqualified call here to hijack in the first place.
//
// It is also the shape block_access, set_find and sequence_find already had, so the fold keeps
// one idiom rather than introducing free functions as a second.
template<class Traits>
struct bit_scan
{
        // The first set position, or size() if there is none. Total, like every scan here: this
        // is the layer that answers, not the layer with preconditions.
        template<class Bits>
        [[nodiscard]] static constexpr auto first(Bits const& c) noexcept
                -> std::size_t
        {
                return next_inclusive(c, 0UZ);
        }

        // One past the last position, which for every reading is the width: the end iterator's
        // position, not a scan at all. Named alongside the scans because callers ask for it in
        // the same breath.
        template<class Bits>
        [[nodiscard]] static constexpr auto last(Bits const& c) noexcept
                -> std::size_t
        {
                return Traits::size(c);
        }

        // The first set position strictly above n, mirroring block_sequence::find_next_exclusive
        // and boost's find_next. Total in n: a position at or past the width has nothing above it.
        template<class Bits>
        [[nodiscard]] static constexpr auto next(Bits const& c, std::size_t n) noexcept
                -> std::size_t
        {
                auto const size = Traits::size(c);
                return n >= size ? size : next_inclusive(c, n + 1UZ);
        }

        // The last set position strictly below n. Unlike block_sequence::find_prev_exclusive
        // this is total rather than precondition-guarded: that one can demand any() because
        // reverse iteration supplies the guard, and a fallback synthesised for a foreign type
        // has no such caller to lean on.
        template<class Bits>
        [[nodiscard]] static constexpr auto prev(Bits const& c, std::size_t n) noexcept
                -> std::size_t
        {
                auto const size = Traits::size(c);
                if (auto i = n < size ? n : size; i != 0UZ) {
                        do {
                                --i;
                                if (Traits::at(c, i)) {
                                        return i;
                                }
                        } while (i != 0UZ);
                }
                return size;
        }

        // How many positions are set. The block tier is the whole point: popcount per word
        // rather than a test per bit.
        template<class Bits>
        [[nodiscard]] static constexpr auto count(Bits const& c) noexcept
                -> std::size_t
        {
                if constexpr (block_readable<Traits, Bits>) {
                        auto n = 0UZ;
                        for (auto i = 0UZ, blocks = Traits::num_blocks(c); i < blocks; ++i) {
                                n += detail::bits::popcount(Traits::block(c, i));
                        }
                        return n;
                } else {
                        auto n = 0UZ;
                        for (auto i = 0UZ, size = Traits::size(c); i < size; ++i) {
                                n += Traits::at(c, i) ? 1UZ : 0UZ;
                        }
                        return n;
                }
        }

        // The first set position at or above n: the primitive the two forward scans derive from,
        // exactly as block_sequence's find_next_inclusive is. Both derivations are + 1 and never
        // - 1, which is what lets first() name its own extreme without size_t wrapping at zero.
        template<class Bits>
        [[nodiscard]] static constexpr auto next_inclusive(Bits const& c, std::size_t n) noexcept
                -> std::size_t
        {
                auto const size = Traits::size(c);
                if (n >= size) {
                        return size;
                }
                if constexpr (block_readable<Traits, Bits>) {
                        using block_type = std::remove_cvref_t<decltype(Traits::block(c, 0UZ))>;
                        constexpr auto digits = static_cast<std::size_t>(std::numeric_limits<block_type>::digits);

                        auto const blocks = Traits::num_blocks(c);
                        auto index = n / digits;
                        auto const offset = n % digits;

                        // No offset != 0 guard: >> 0 is the identity, so the boundary case needs
                        // no arm of its own. #88 measured the guard as pure cost.
                        if (auto const block = static_cast<block_type>(Traits::block(c, index) >> offset); block != block_type{}) {
                                return n + detail::bits::countr_zero(block);
                        }
                        for (++index; index < blocks; ++index) {
                                if (auto const block = Traits::block(c, index); block != block_type{}) {
                                        return (digits * index) + detail::bits::countr_zero(block);
                                }
                        }
                } else {
                        for (auto i = n; i < size; ++i) {
                                if (Traits::at(c, i)) {
                                        return i;
                                }
                        }
                }
                return size;
        }

        // The set ordering: the positions in increasing order, compared lexicographically, which
        // is what std::set's <=> means. Deliberately not the magnitude ordering boost::dynamic_bitset
        // gives its own operator<, and that divergence is documented rather than reconciled.
        template<class Bits>
        [[nodiscard]] static constexpr auto lexicographical_three_way(Bits const& x, Bits const& y) noexcept
                -> std::strong_ordering
        {
                auto i = first(x);
                auto j = first(y);
                for (auto const nx = Traits::size(x), ny = Traits::size(y); i != nx and j != ny; i = next(x, i), j = next(y, j)) {
                        if (auto const cmp = i <=> j; cmp != 0) {
                                return cmp;
                        }
                }
                // One ran out: the shorter sequence of positions is the smaller set, and both
                // running out together makes them equal.
                return (i != Traits::size(x)) <=> (j != Traits::size(y));
        }
};

// The floor for wrappability: a width, and an indexed read. Everything else -- blocks, native
// scans, a native ordering -- is an optimization tier a specialization may or may not supply.
//
// Gating the adaptors on this rather than on bit_traits<Bits> being complete is what turns
// "incomplete type" into "constraint not satisfied", which names the actual problem.
template<class Bits>
concept bit_storage =
        requires (Bits const& c, std::size_t n)
        {
                // extent is part of the floor rather than an extra: every adapter states its
                // width policy, dynamic_extent included, which is what lets static_bit_extent
                // below read it without first proving it exists.
                { bit_traits<Bits>::extent   } -> std::convertible_to<std::size_t>;
                { bit_traits<Bits>::size(c)  } -> std::convertible_to<std::size_t>;
                { bit_traits<Bits>::at(c, n) } -> std::convertible_to<bool>;
        }
;

// How many positions a bit sequence has when the type settles it, so a static width reaches the
// readings as a constant expression. Replaces the bit_extent variable template: one door, and
// the extent comes through it like everything else.
template<class Bits>
concept static_bit_extent = bit_storage<Bits> and bit_traits<Bits>::extent != std::dynamic_extent;

} // namespace xstd

#endif // XSTD_BITS_BIT_TRAITS_HPP
