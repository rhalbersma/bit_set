//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_BITS_DETAIL_ARRAY_HPP
#define XSTD_BITS_DETAIL_ARRAY_HPP

#include <boost/hash2/hash_append_fwd.hpp>         // hash_append, hash_append_tag
#include <xstd/bits/detail/intrin.hpp>             // countl_zero, countr_zero, popcount
#include <xstd/bits/detail/pred.hpp>               // intersects, is_subset_of, not_equal_to
#include <xstd/ints/concepts/unsigned_integer.hpp> // unsigned_integer
#include <xstd/ints/cstdlib/div.hpp>               // div, div_result
#include <xstd/ints/memory.hpp>                    // align_up
#include <algorithm>                               // all_of, any_of, fill_n, find_if, fold_left, max, shift_left, shift_right
#include <array>                                   // array
#include <cassert>                                 // assert
#include <cstddef>                                 // ptrdiff_t, size_t
#include <functional>                              // plus
#include <iterator>                                // distance, prev, random_access_iterator, sized_sentinel_for
#include <limits>                                  // digits
#include <ranges>                                  // drop, iota, reverse, take, transform, zip
                                                   // (views::drop_last when P22014R2 is accepted)
#include <type_traits>                             // conditional_t, is_const_v, is_nothrow_swappable_v, remove_reference_t
#include <utility>                                 // pair

namespace xstd::detail::bits {

template<std::size_t N, xstd::unsigned_integer Block>
struct array
{
        static constexpr auto bits_per_block = static_cast<std::size_t>(std::numeric_limits<Block>::digits);
        static constexpr auto num_bits       = align_up(N, bits_per_block);
        static constexpr auto num_blocks     = std::ranges::max(num_bits / bits_per_block, 1UZ);

        std::array<Block, num_blocks> m_bits;

        // The block, for xstd::ranges::block_access; padding above N stays zero, which is what makes whole-block comparison mean anything.
        [[nodiscard]] constexpr auto block(std::size_t i) const noexcept
                -> Block
        {
                return m_bits[i];
        }

        [[nodiscard]] friend constexpr auto operator==(array const& x [[maybe_unused]], array const& y [[maybe_unused]]) noexcept
                -> bool
        {
                if constexpr (N == 0) {
                        return true;
                } else {
                        return x.m_bits == y.m_bits;
                }
        }
              
        // No operator<=>: array is pure storage with no opinion on set-of-indices versus sequence-of-bools order; == is unaffected.

        template<class Provider, class Hash, class Flavor>
        friend constexpr void tag_invoke(boost::hash2::hash_append_tag const&, Provider const&, Hash& h, Flavor const& f, array const* v) noexcept
        {
                boost::hash2::hash_append(h, f, v->m_bits);
        }

        [[nodiscard]] constexpr auto find_front() const noexcept
                -> std::size_t
        {
                assert(any());
                if constexpr (num_blocks == 1) {
                        return detail::bits::countr_zero(m_bits[0]);
                } else if constexpr (num_blocks == 2) {
                        return m_bits[0] != zero ? detail::bits::countr_zero(m_bits[0]) : detail::bits::countr_zero(m_bits[1]) + bits_per_block;
                } else if constexpr (num_blocks >= 3) {
                        auto const front = std::ranges::find_if(m_bits, [](auto block) { return block != zero; });
                        assert(front != m_bits.end());
                        return detail::bits::countr_zero(*front) + (bits_per_block * distance(m_bits.begin(), front));
                }
        }

        [[nodiscard]] constexpr auto find_back() const noexcept
                -> std::size_t
        {
                assert(any());
                if constexpr (num_blocks == 1) {
                        return last_bit - detail::bits::countl_zero(m_bits[0]);
                } else if constexpr (num_blocks == 2) {
                        return m_bits[1] != zero ? last_bit - detail::bits::countl_zero(m_bits[1]) : left_bit - detail::bits::countl_zero(m_bits[0]);
                } else if constexpr (num_blocks >= 3) {
                        auto const back = std::ranges::find_if(m_bits | std::views::reverse, [](auto block) { return block != zero; });
                        assert(back != m_bits.rend());
                        return last_bit - detail::bits::countl_zero(*back) - (bits_per_block * distance(m_bits.rbegin(), back));
                }
        }

        [[nodiscard]] constexpr auto find_first() const noexcept
                -> std::size_t
        {
                if constexpr (N > 0 and num_blocks == 1) {
                        if (m_bits[0] != zero) {
                                return detail::bits::countr_zero(m_bits[0]);
                        }
                } else if constexpr (num_blocks == 2) {
                        if (m_bits[0] != zero) {
                                return detail::bits::countr_zero(m_bits[0]);
                        }
                        if (m_bits[1] != zero) {
                                return detail::bits::countr_zero(m_bits[1]) + bits_per_block;
                        }
                } else if constexpr (num_blocks >= 3) {
                        if (auto const first = std::ranges::find_if(m_bits, [](auto block) { return block != zero; }); first != m_bits.end()) {
                                return detail::bits::countr_zero(*first) + (bits_per_block * distance(m_bits.begin(), first));
                        }
                }
                return N;
        }

        [[nodiscard]] constexpr auto find_last() const noexcept
                -> std::size_t
        {
                return N;
        }

        [[nodiscard]] constexpr auto find_next(std::size_t n) const noexcept
                -> std::size_t
        {
                ++n;
                if (n == N) {
                        return N;
                }
                if constexpr (num_blocks == 1) {
                        if (auto const block = static_cast<Block>(m_bits[0] >> n); block != zero) {
                                return n + detail::bits::countr_zero(block);
                        }
                } else if constexpr (num_blocks >= 2) {
                        auto [ index, offset ] = index_offset(n);
                        if (offset != 0) {
                                if (auto const block = static_cast<Block>(m_bits[index] >> offset); block != zero) {
                                        return n + detail::bits::countr_zero(block);
                                }
                                ++index;
                                n += bits_per_block - offset;
                        }
                        auto const rg = m_bits | std::views::drop(index);
                        // next is an iterator. That std::array's is a pointer is
                        // implementation-defined -- [array.overview] requires only that it model
                        // contiguous_iterator -- so readability-qualified-auto's auto const *const
                        // would encode a detail this code never relies on: next is dereferenced and
                        // passed to distance, both pure iterator operations.
                        if (auto const next = std::ranges::find_if(rg, [](auto block) { return block != zero; }); next != rg.end()) {  // NOLINT(readability-qualified-auto)
                                return n + detail::bits::countr_zero(*next) + (bits_per_block * distance(rg.begin(), next));
                        }
                }
                return N;
        }

        [[nodiscard]] constexpr auto find_prev(std::size_t n) const noexcept
                -> std::size_t
        {
                assert(any());
                --n;
                if constexpr (num_blocks == 1) {
                        return n - detail::bits::countl_zero(static_cast<Block>(m_bits[0] << (left_bit - n)));
                } else if constexpr (num_blocks >= 2) {
                        auto [ index, offset ] = index_offset(n);
                        if (auto const reverse_offset = left_bit - offset; reverse_offset != 0) {
                                if (auto const block = static_cast<Block>(m_bits[index] << reverse_offset); block != zero) {
                                        return n - detail::bits::countl_zero(block);
                                }
                                --index;
                                n -= bits_per_block - reverse_offset;
                        }
                        auto const rg = m_bits | std::views::reverse | std::views::drop(last_block - index);
                        auto const prev = std::ranges::find_if(rg, [](auto block) { return block != zero; });
                        assert(prev != rg.end());
                        return n - detail::bits::countl_zero(*prev) - (bits_per_block * distance(rg.begin(), prev));
                }
        }

        constexpr void operator&=(array const& other [[maybe_unused]]) noexcept
        {
                if constexpr (N > 0 and num_blocks == 1) {
                        this->m_bits[0] &= other.m_bits[0];
                } else if constexpr (num_blocks == 2) {
                        this->m_bits[0] &= other.m_bits[0];
                        this->m_bits[1] &= other.m_bits[1];
                } else if constexpr (num_blocks >= 3) {
                        for (auto const i : std::views::iota(0UZ, num_blocks)) {
                                this->m_bits[i] &= other.m_bits[i];
                        }
                }
        }

        constexpr void operator|=(array const& other [[maybe_unused]]) noexcept
        {
                if constexpr (N > 0 and num_blocks == 1) {
                        this->m_bits[0] |= other.m_bits[0];
                } else if constexpr (num_blocks == 2) {
                        this->m_bits[0] |= other.m_bits[0];
                        this->m_bits[1] |= other.m_bits[1];
                } else if constexpr (num_blocks >= 3) {
                        for (auto const i : std::views::iota(0UZ, num_blocks)) {
                                this->m_bits[i] |= other.m_bits[i];
                        }
                }
        }

        constexpr void operator^=(array const& other [[maybe_unused]]) noexcept
        {
                if constexpr (N > 0 and num_blocks == 1) {
                        this->m_bits[0] ^= other.m_bits[0];
                } else if constexpr (num_blocks == 2) {
                        this->m_bits[0] ^= other.m_bits[0];
                        this->m_bits[1] ^= other.m_bits[1];
                } else if constexpr (num_blocks >= 3) {
                        for (auto const i : std::views::iota(0UZ, num_blocks)) {
                                this->m_bits[i] ^= other.m_bits[i];
                        }
                }
        }

        constexpr void operator-=(array const& other [[maybe_unused]]) noexcept
        {
                if constexpr (N > 0 and num_blocks == 1) {
                        this->m_bits[0] &= static_cast<Block>(~other.m_bits[0]);
                } else if constexpr (num_blocks == 2) {
                        this->m_bits[0] &= static_cast<Block>(~other.m_bits[0]);
                        this->m_bits[1] &= static_cast<Block>(~other.m_bits[1]);
                } else if constexpr (num_blocks >= 3) {
                        for (auto const i : std::views::iota(0UZ, num_blocks)) {
                                this->m_bits[i] &= static_cast<Block>(~other.m_bits[i]);
                        }
                }
        }

        constexpr void operator<<=(std::size_t n [[maybe_unused]]) noexcept
        {
                assert(is_valid(n));
                if constexpr (num_blocks == 1) {
                        // m_bits[0] <<= n narrows the promoted int back to a Block implicitly, which -fsanitize=implicit-conversion aborts on once a bit shifts out.
                        m_bits[0] = static_cast<Block>(m_bits[0] << n);
                } else if constexpr (num_blocks >= 2) {
                        auto const [ n_blocks, L_shift ] = xstd::div(n, bits_per_block);
                        if (L_shift == 0) {
                                std::shift_right(m_bits.begin(), m_bits.end(), static_cast<std::ptrdiff_t>(n_blocks));
                        } else {
                                auto const R_shift = bits_per_block - L_shift;
                                for (auto i = last_block; i > n_blocks; --i) {
                                        m_bits[i] = static_cast<Block>(static_cast<Block>(m_bits[i - n_blocks] << L_shift) | static_cast<Block>(m_bits[i - n_blocks - 1] >> R_shift));
                                }
                                m_bits[n_blocks] = static_cast<Block>(m_bits[0] << L_shift);
                        }
                        std::ranges::fill_n(std::ranges::prev(m_bits.rend(), static_cast<std::ptrdiff_t>(n_blocks)), static_cast<std::ptrdiff_t>(n_blocks), zero);
                }
                erase_unused();
        }

        constexpr void operator>>=(std::size_t n [[maybe_unused]]) noexcept
        {
                assert(is_valid(n));
                if constexpr (num_blocks == 1) {
                        // m_bits[0] >>= n narrows the promoted int back to a Block implicitly, which -fsanitize=implicit-conversion instruments.
                        m_bits[0] = static_cast<Block>(m_bits[0] >> n);
                } else if constexpr (num_blocks >= 2) {
                        auto const [ n_blocks, R_shift ] = xstd::div(n, bits_per_block);
                        if (R_shift == 0) {
                                std::shift_left(m_bits.begin(), m_bits.end(), static_cast<std::ptrdiff_t>(n_blocks));
                        } else {
                                auto const L_shift = bits_per_block - R_shift;
                                for (auto i = 0UZ; i + n_blocks < last_block; ++i) {
                                        m_bits[i] = static_cast<Block>(static_cast<Block>(m_bits[i + n_blocks] >> R_shift) | static_cast<Block>(m_bits[i + n_blocks + 1] << L_shift));
                                }
                                m_bits[last_block - n_blocks] = static_cast<Block>(m_bits[last_block] >> R_shift);
                        }
                        std::ranges::fill_n(std::ranges::prev(m_bits.end(), static_cast<std::ptrdiff_t>(n_blocks)), static_cast<std::ptrdiff_t>(n_blocks), zero);
                }
        }
        
        constexpr void set() noexcept
        {
                if constexpr (has_unused_bits) {
                        std::ranges::fill_n(m_bits.begin(), last_block, ones);
                        m_bits[last_block] = used_bits;
                } else if constexpr (N > 0) {
                        m_bits.fill(ones);
                }
                assert(all());
        }

        constexpr void reset() noexcept
        {
                if constexpr (N > 0) {
                        m_bits.fill(zero);
                }
                assert(none());
        }

        constexpr void flip() noexcept
        {
                if constexpr (N > 0 and num_blocks == 1) {
                        m_bits[0] = static_cast<Block>(~m_bits[0]);
                } else if constexpr (num_blocks == 2) {
                        m_bits[0] = static_cast<Block>(~m_bits[0]);
                        m_bits[1] = static_cast<Block>(~m_bits[1]);
                } else if constexpr (num_blocks >= 3) {
                        for (auto const i : std::views::iota(0UZ, num_blocks)) {
                                m_bits[i] = static_cast<Block>(~m_bits[i]);
                        }
                }
                erase_unused();
        }

        constexpr void swap(array& other [[maybe_unused]]) noexcept(std::is_nothrow_swappable_v<Block>)
        {
                if constexpr (N > 0) {
                        this->m_bits.swap(other.m_bits);
                }
        }
        
        constexpr void set(std::size_t n) noexcept
        {
                assert(is_valid(n));
                auto&& [ block, mask ] = block_mask(n);
                block |= mask;
                assert((*this)[n]);
        }

        [[nodiscard]] constexpr auto insert(std::size_t n) noexcept
                -> bool
        {
                assert(is_valid(n));
                auto&& [ block, mask ] = block_mask(n);
                auto const inserted = not detail::bits::intersects(block, mask);
                block |= mask;
                assert((*this)[n]);
                return inserted;
        }

        constexpr void reset(std::size_t n) noexcept
        {
                assert(is_valid(n));
                auto&& [ block, mask ] = block_mask(n);
                block &= static_cast<Block>(~mask);
                assert(not (*this)[n]);
        }

        [[nodiscard]] constexpr auto erase(std::size_t n) noexcept
                -> bool
        {
                assert(is_valid(n));
                auto&& [ block, mask ] = block_mask(n);
                auto const erased = detail::bits::intersects(block, mask);
                block &= static_cast<Block>(~mask);
                assert(not (*this)[n]);
                return erased;
        }

        constexpr void flip(std::size_t n) noexcept
        {
                assert(is_valid(n));
                auto&& [ block, mask ] = block_mask(n);
                block ^= mask;
        }

        [[nodiscard]] constexpr auto operator[](std::size_t n) const noexcept
                -> bool
        {
                assert(is_valid(n));
                auto&& [ block, mask ] = block_mask(n);
                return detail::bits::intersects(block, mask);
        }

        [[nodiscard]] constexpr auto count() const noexcept
                -> std::size_t
        {
                if constexpr (N == 0) {
                        return 0UZ;
                } else if constexpr (num_blocks == 1) {
                        return detail::bits::popcount(m_bits[0]);
                } else if constexpr (num_blocks == 2) {
                        return detail::bits::popcount(m_bits[0]) + detail::bits::popcount(m_bits[1]);
                } else if constexpr (num_blocks >= 3) {
                        return std::ranges::fold_left(
                                m_bits | std::views::transform([](auto block) { return detail::bits::popcount(block); }), 
                                0UZ, std::plus<>()
                        );
                }
        }

        [[nodiscard]] static constexpr auto size() noexcept
                -> std::size_t
        {
                return N;
        }

        [[nodiscard]] constexpr auto all() const noexcept
                -> bool
        {
                if constexpr (has_unused_bits) {
                        if constexpr (num_blocks == 1) {
                                return m_bits[0] == used_bits;
                        } else if constexpr (num_blocks == 2) {
                                return m_bits[0] == ones and m_bits[1] == used_bits;
                        } else if constexpr (num_blocks >= 3) {
                                return std::ranges::all_of(m_bits | std::views::take(last_block), [](auto block) {
                                        return block == ones;
                                }) and m_bits[last_block] == used_bits;
                        }
                } else {
                        if constexpr (N == 0) {
                                return true;
                        } else if constexpr (num_blocks == 1) {
                                return m_bits[0] == ones;
                        } else if constexpr (num_blocks == 2) {
                                return m_bits[0] == ones and m_bits[1] == ones;
                        } else if constexpr (num_blocks >= 3) {
                                return std::ranges::all_of(m_bits, [](auto block) { return block == ones; });
                        }
                }
        }

        [[nodiscard]] constexpr auto any() const noexcept
                -> bool
        {
                return not none();
        }

        [[nodiscard]] constexpr auto none() const noexcept
                -> bool
        {
                if constexpr (N == 0) {
                        return true;
                } else if constexpr (num_blocks == 1) {
                        return m_bits[0] == zero;
                } else if constexpr (num_blocks == 2) {
                        return m_bits[0] == zero and m_bits[1] == zero;
                } else if constexpr (num_blocks >= 3) {
                        return std::ranges::all_of(m_bits, [](auto block) { return block == zero; });
                }
        }

        [[nodiscard]] constexpr auto is_subset_of(array const& other [[maybe_unused]]) const noexcept
                -> bool
        {
                if constexpr (N == 0) {
                        return true;
                } else if constexpr (num_blocks == 1) {
                        return detail::bits::is_subset_of(this->m_bits[0], other.m_bits[0]);
                } else if constexpr (num_blocks == 2) {
                        return
                                detail::bits::is_subset_of(this->m_bits[0], other.m_bits[0]) and 
                                detail::bits::is_subset_of(this->m_bits[1], other.m_bits[1])
                        ;
                } else if constexpr (num_blocks >= 3) {
                        return std::ranges::all_of(
                                std::views::zip(this->m_bits, other.m_bits), [](auto&& _) { auto&& [ lhs, rhs] = _;  
                                return detail::bits::is_subset_of(lhs, rhs); 
                        });
                }
        }

        [[nodiscard]] constexpr auto is_proper_subset_of(array const& other [[maybe_unused]]) const noexcept
                -> bool
        {
                if constexpr (N == 0) {
                        return false;
                } else if constexpr (num_blocks == 1) {
                        return 
                                detail::bits::is_subset_of(this->m_bits[0], other.m_bits[0]) and
                                detail::bits::not_equal_to(this->m_bits[0], other.m_bits[0])
                        ;
                } else if constexpr (num_blocks == 2) {                         
                        if (not detail::bits::is_subset_of(this->m_bits[0], other.m_bits[0])) {
                                return false;
                        }
                        if (detail::bits::not_equal_to(this->m_bits[0], other.m_bits[0])) {
                                return detail::bits::is_subset_of(this->m_bits[1], other.m_bits[1]);
                        }
                        return
                                detail::bits::is_subset_of(this->m_bits[1], other.m_bits[1]) and
                                detail::bits::not_equal_to(this->m_bits[1], other.m_bits[1])
                        ;
                } else if constexpr (num_blocks >= 3) {
                        auto i = 0UZ;
                        while(i < num_blocks) {
                                if (not detail::bits::is_subset_of(this->m_bits[i], other.m_bits[i])) {
                                        return false;
                                }
                                if (    detail::bits::not_equal_to(this->m_bits[i], other.m_bits[i])) {
                                        break;
                                }
                                ++i;
                        }
                        return (i == num_blocks) ? false : std::ranges::all_of(
                                std::views::zip(this->m_bits, other.m_bits) | std::views::drop(i), [](auto&& _) { auto&& [ lhs, rhs ] = _; 
                                return detail::bits::is_subset_of(lhs, rhs); 
                        });
                }
        }

        [[nodiscard]] constexpr auto intersects(array const& other [[maybe_unused]]) const noexcept
                -> bool
        {
                if constexpr (N == 0) {
                        return false;
                } else if constexpr (num_blocks == 1) {
                        return detail::bits::intersects(this->m_bits[0], other.m_bits[0]);
                } else if constexpr (num_blocks == 2) {
                        return
                                detail::bits::intersects(this->m_bits[0], other.m_bits[0]) or
                                detail::bits::intersects(this->m_bits[1], other.m_bits[1])
                        ;
                } else if constexpr (num_blocks >= 3) {
                        return std::ranges::any_of(
                                std::views::zip(this->m_bits, other.m_bits), [](auto&& _) { auto&& [ lhs, rhs ] = _; 
                                return detail::bits::intersects(lhs, rhs); 
                        });
                }
        }

        template<class Hash, class Flavor>
        constexpr void hash_append(Hash& h, Flavor const& f) noexcept
        {
                boost::hash2::hash_append(h, f, m_bits);
        }

private:
        static constexpr auto num_unused_bits = num_bits - N;
        static constexpr auto has_unused_bits = num_bits > N;
        static constexpr auto last_block      = num_blocks - 1;
        static constexpr auto last_bit        = num_bits - 1;
        static constexpr auto left_bit        = bits_per_block - 1;
        static constexpr auto unit            = static_cast<Block>( 1);
        static constexpr auto zero            = static_cast<Block>( 0);
        static constexpr auto ones            = static_cast<Block>(-1);
        static constexpr auto used_bits       = static_cast<Block>(ones >> num_unused_bits);
        static constexpr auto unused_bits     = static_cast<Block>(~used_bits);

        [[nodiscard]] static constexpr auto is_valid(std::size_t n [[maybe_unused]]) noexcept
                -> bool
        {
                if constexpr (N == 0) {
                        // Unreachable: only an assert calls is_valid, and a bit_array<0> has no member that reaches one. Not removable either - MSVC's /W4 rejects a bare n < N as always false (C4296).
                        return false;                   // GCOVR_EXCL_LINE
                } else {
                        return n < N;
                }
        }

        [[nodiscard]] static constexpr auto index_offset(std::size_t n) noexcept
                -> xstd::div_result<std::size_t>
        {
                if constexpr (num_blocks == 1) {
                        return { .quotient = 0UZ, .remainder = n };
                } else {
                        return xstd::div(n, bits_per_block);
                }
        }

        // cl rejects a member of the explicit object parameter in a trailing return type (C2228); bit_array.hpp's result_t is the same idiom.
        template<class Self>
        using block_reference_t = std::conditional_t<
                std::is_const_v<std::remove_reference_t<Self>>, Block const&, Block&>;

        [[nodiscard]] constexpr auto block_mask(this auto&& self, std::size_t n) noexcept
                -> std::pair<block_reference_t<decltype(self)>, Block>
        {
                auto const [ index, offset ] = index_offset(n);
                return { std::forward<decltype(self)>(self).m_bits[index], static_cast<Block>(unit << offset) };
        }

        constexpr void erase_unused() noexcept
        {
                if constexpr (has_unused_bits) {
                        m_bits[last_block] &= used_bits;
                        assert(not detail::bits::intersects(m_bits[last_block], unused_bits));
                }
        }

        // Iterators are taken by value, as std::ranges::distance itself takes them.
        //
        // performance-unnecessary-value-param flags both parameters, and only for the
        // reverse_iterator instantiations. libstdc++ hand-writes that class's copy
        // constructor -- a pre-"= default" idiom; the copy assignment beside it is
        // defaulted -- so it is not trivially copyable, where libc++'s is and the check
        // stays silent. The verdict is a property of the standard library rather than of
        // this signature.
        //
        // Nor is the cost it names paid here. Not trivially copyable means non-trivial
        // for the purposes of calls under the Itanium ABI, so an out-of-line callee takes
        // a pointer to a caller-built temporary where a trivially copyable type of the
        // same eight bytes arrives in a register. This is a static constexpr one-liner:
        // at -O2 it inlines and both iterators fold away to a pointer subtraction.
        template<std::random_access_iterator I, std::sized_sentinel_for<I> S>
        [[nodiscard]] static constexpr auto distance(I first, S last) noexcept  // NOLINT(performance-unnecessary-value-param)
        {
                return static_cast<std::size_t>(std::ranges::distance(first, last));
        }
};

}       // namespace xstd::detail::bits

#endif // XSTD_BITS_DETAIL_ARRAY_HPP
