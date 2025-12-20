#ifndef XSTD_SUBDIR_BIT_SUBDIR_ARRAY_HPP
#define XSTD_SUBDIR_BIT_SUBDIR_ARRAY_HPP

//          Copyright Rein Halbersma 2014-2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/bit/intrin.hpp>  // countl_zero, countr_zero, popcount
#include <xstd/bit/pred.hpp>    // intersects, is_subset_of, not_equal_to
#include <xstd/utility.hpp>     // aligned_size
#include <algorithm>            // lexicographical_compare_three_way (ranges::lexicographical_compare when P2022R3 is accepted) 
                                // all_of, any_of, copy, fill_n, find_if, fold_left, max, shift_left, shift_right
#include <array>                // array
#include <cassert>              // assert
#include <compare>              // strong_ordering
#include <concepts>             // unsigned_integral
#include <cstddef>              // ptrdiff_t, size_t
#include <functional>           // plus
#include <limits>               // digits
#include <ranges>               // distance, prev (views::drop_last when P22014R2 is accepted)
                                // drop, pairwise_transform, reverse, take, transform, zip
#include <type_traits>          // is_nothrow_swappable_v
#include <utility>              // pair

namespace xstd::bit {

template<std::size_t N, std::unsigned_integral Block>
struct array
{
        static constexpr auto bits_per_block = static_cast<std::size_t>(std::numeric_limits<Block>::digits);
        static constexpr auto num_bits       = aligned_size(N, bits_per_block);
        static constexpr auto num_blocks     = std::ranges::max(num_bits / bits_per_block, 1uz);

        std::array<Block, num_blocks> m_bits;

        [[nodiscard]] friend constexpr bool operator==(array const& x [[maybe_unused]], array const& y [[maybe_unused]]) noexcept
        {
                if constexpr (N == 0) {
                        return true;
                } else {
                        return x.m_bits == y.m_bits;
                }
        }
              
        [[nodiscard]] friend constexpr auto operator<=>(array const& x [[maybe_unused]], array const& y [[maybe_unused]]) noexcept
                -> std::strong_ordering
        {
                if constexpr (N == 0) {
                        return std::strong_ordering::equal;
                } else {
                        return std::lexicographical_compare_three_way(
                                x.m_bits.begin(), x.m_bits.end(), 
                                y.m_bits.begin(), y.m_bits.end(),
                                [](auto lhs, auto rhs) {
                                        if (auto const ssd = static_cast<Block>(lhs ^ rhs); ssd == zero) {
                                                return std::strong_ordering::equal;
                                        } else {
                                                auto const lsb = static_cast<Block>(unit << bit::countr_zero(ssd)); 
                                                return bit::intersects(lhs, lsb) ? std::strong_ordering::less : std::strong_ordering::greater;
                                        }
                                }
                        );
                }
        }        

        [[nodiscard]] constexpr std::size_t find_front() const noexcept
        {
                assert(any());
                if constexpr (num_blocks == 1) {
                        return bit::countr_zero(m_bits[0]);
                } else if constexpr (num_blocks == 2) {
                        return m_bits[0] != zero ? bit::countr_zero(m_bits[0]) : bit::countr_zero(m_bits[1]) + bits_per_block;
                } else if constexpr (num_blocks >= 3) {
                        auto const front = std::ranges::find_if(m_bits, [](auto block) { return block != zero; });
                        assert(front != m_bits.end());
                        return bit::countr_zero(*front) + bits_per_block * distance(m_bits.begin(), front);
                }
        }

        [[nodiscard]] constexpr std::size_t find_back() const noexcept
        {
                assert(any());
                if constexpr (num_blocks == 1) {
                        return last_bit - bit::countl_zero(m_bits[0]);
                } else if constexpr (num_blocks == 2) {
                        return m_bits[1] != zero ? last_bit - bit::countl_zero(m_bits[1]) : left_bit - bit::countl_zero(m_bits[0]);
                } else if constexpr (num_blocks >= 3) {
                        auto const back = std::ranges::find_if(m_bits | std::views::reverse, [](auto block) { return block != zero; });
                        assert(back != m_bits.rend());
                        return last_bit - bit::countl_zero(*back) - bits_per_block * distance(m_bits.rbegin(), back);
                }
        }

        [[nodiscard]] constexpr std::size_t find_first() const noexcept
        {
                if constexpr (N > 0 and num_blocks == 1) {
                        if (m_bits[0] != zero) {
                                return bit::countr_zero(m_bits[0]);
                        }
                } else if constexpr (num_blocks == 2) {
                        if (m_bits[0] != zero) {
                                return bit::countr_zero(m_bits[0]);
                        } else if (m_bits[1] != zero) {
                                return bit::countr_zero(m_bits[1]) + bits_per_block;
                        }
                } else if constexpr (num_blocks >= 3) {
                        if (auto const first = std::ranges::find_if(m_bits, [](auto block) { return block != zero; }); first != m_bits.end()) {
                                return bit::countr_zero(*first) + bits_per_block * distance(m_bits.begin(), first);
                        }
                }
                return N;
        }

        [[nodiscard]] constexpr std::size_t find_last() const noexcept
        {
                return N;
        }

        [[nodiscard]] constexpr std::size_t find_next(std::size_t n) const noexcept
        {
                ++n;
                if (n == N) {
                        return N;
                }
                if constexpr (num_blocks == 1) {
                        if (auto const block = static_cast<Block>(m_bits[0] >> n); block != zero) {
                                return n + bit::countr_zero(block);
                        }
                } else if constexpr (num_blocks >= 2) {
                        auto [ index, offset ] = index_offset(n);
                        if (offset != 0) {
                                if (auto const block = static_cast<Block>(m_bits[index] >> offset); block != zero) {
                                        return n + bit::countr_zero(block);
                                }
                                ++index;
                                n += bits_per_block - offset;
                        }
                        auto const rg = m_bits | std::views::drop(index);
                        if (auto const next = std::ranges::find_if(rg, [](auto block) { return block != zero; }); next != rg.end()) {
                                return n + bit::countr_zero(*next) + bits_per_block * distance(rg.begin(), next);
                        }
                }
                return N;
        }

        [[nodiscard]] constexpr std::size_t find_prev(std::size_t n) const noexcept
        {
                assert(any());
                --n;
                if constexpr (num_blocks == 1) {
                        return n - bit::countl_zero(static_cast<Block>(m_bits[0] << (left_bit - n)));
                } else if constexpr (num_blocks >= 2) {
                        auto [ index, offset ] = index_offset(n);
                        if (auto const reverse_offset = left_bit - offset; reverse_offset != 0) {
                                if (auto const block = static_cast<Block>(m_bits[index] << reverse_offset); block != zero) {
                                        return n - bit::countl_zero(block);
                                }
                                --index;
                                n -= bits_per_block - reverse_offset;
                        }
                        auto const rg = m_bits | std::views::reverse | std::views::drop(last_block - index);
                        auto const prev = std::ranges::find_if(rg, [](auto block) { return block != zero; });
                        assert(prev != rg.end());
                        return n - bit::countl_zero(*prev) - bits_per_block * distance(rg.begin(), prev);
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
                        for (auto i = 0uz; i < num_blocks; ++i) {
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
                        for (auto i = 0uz; i < num_blocks; ++i) {
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
                        for (auto i = 0uz; i < num_blocks; ++i) {
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
                        for (auto i = 0uz; i < num_blocks; ++i) {
                                this->m_bits[i] &= static_cast<Block>(~other.m_bits[i]);
                        }
                }
        }

        constexpr void operator<<=(std::size_t n [[maybe_unused]]) noexcept
        {
                assert(is_valid(n));
                if constexpr (num_blocks == 1) {
                        m_bits[0] <<= n;
                } else if constexpr (num_blocks >= 2) {
                        auto const [ n_blocks, L_shift ] = div_mod(n, bits_per_block);
                        if (L_shift == 0) {
                                std::ranges::shift_right(m_bits, static_cast<std::ptrdiff_t>(n_blocks));
                        } else {
                                auto const R_shift = bits_per_block - L_shift;
                                std::ranges::copy(
                                        m_bits | std::views::reverse | std::views::drop(n_blocks) | std::views::pairwise_transform([=](auto first, auto second) -> Block {
                                                return static_cast<Block>(first << L_shift) | static_cast<Block>(second >> R_shift);
                                        }),
                                        m_bits.rbegin()
                                );
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
                        m_bits[0] >>= n;
                } else if constexpr (num_blocks >= 2) {
                        auto const [ n_blocks, R_shift ] = div_mod(n, bits_per_block);
                        if (R_shift == 0) {
                                std::ranges::shift_left(m_bits, static_cast<std::ptrdiff_t>(n_blocks));
                        } else {
                                auto const L_shift = bits_per_block - R_shift;
                                std::ranges::copy(
                                        m_bits | std::views::drop(n_blocks) | std::views::pairwise_transform([=](auto first, auto second) -> Block {
                                                return static_cast<Block>(first >> R_shift) | static_cast<Block>(second << L_shift);
                                        }),
                                        m_bits.begin()
                                );
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
                        for (auto i = 0uz; i < num_blocks; ++i) {
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
                assert(test(n));
        }

        [[nodiscard]] constexpr bool insert(std::size_t n) noexcept
        {
                assert(is_valid(n));
                auto&& [ block, mask ] = block_mask(n);
                auto const inserted = not bit::intersects(block, mask);
                block |= mask;
                assert(test(n));
                return inserted;
        }

        constexpr void reset(std::size_t n) noexcept
        {
                assert(is_valid(n));
                auto&& [ block, mask ] = block_mask(n);
                block &= static_cast<Block>(~mask);
                assert(not test(n));
        }

        [[nodiscard]] constexpr bool erase(std::size_t n) noexcept
        {
                assert(is_valid(n));
                auto&& [ block, mask ] = block_mask(n);
                auto const erased = bit::intersects(block, mask);
                block &= static_cast<Block>(~mask);
                assert(not test(n));
                return erased;
        }

        constexpr void flip(std::size_t n) noexcept
        {
                assert(is_valid(n));
                auto&& [ block, mask ] = block_mask(n);
                block ^= mask;
        }

        [[nodiscard]] constexpr bool test(std::size_t n) const noexcept
        {
                assert(is_valid(n));
                auto&& [ block, mask ] = block_mask(n);
                return bit::intersects(block, mask);
        }

        [[nodiscard]] constexpr std::size_t count() const noexcept
        {
                if constexpr (N == 0) {
                        return 0uz;
                } else if constexpr (num_blocks == 1) {
                        return bit::popcount(m_bits[0]);
                } else if constexpr (num_blocks == 2) {
                        return bit::popcount(m_bits[0]) + bit::popcount(m_bits[1]);
                } else if constexpr (num_blocks >= 3) {
                        return std::ranges::fold_left(
                                m_bits | std::views::transform([](auto block) { return bit::popcount(block); }), 
                                0uz, std::plus<>()
                        );
                }
        }

        [[nodiscard]] static constexpr std::size_t size() noexcept
        {
                return N;
        }

        [[nodiscard]] constexpr bool all() const noexcept
        {
                if constexpr (has_unused_bits) {
                        if constexpr (num_blocks == 1) {
                                return m_bits[0] == used_bits;
                        } else if (num_blocks == 2) {
                                return m_bits[0] == ones and m_bits[1] == used_bits;
                        } else if (num_blocks >= 3) {
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

        [[nodiscard]] constexpr bool any() const noexcept
        {
                return not none();
        }

        [[nodiscard]] constexpr bool none() const noexcept
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

        [[nodiscard]] constexpr bool is_subset_of(array const& other [[maybe_unused]]) const noexcept
        {
                if constexpr (N == 0) {
                        return true;
                } else if constexpr (num_blocks == 1) {
                        return bit::is_subset_of(this->m_bits[0], other.m_bits[0]);
                } else if constexpr (num_blocks == 2) {
                        return
                                bit::is_subset_of(this->m_bits[0], other.m_bits[0]) and 
                                bit::is_subset_of(this->m_bits[1], other.m_bits[1])
                        ;
                } else if constexpr (num_blocks >= 3) {
                        return std::ranges::all_of(
                                std::views::zip(this->m_bits, other.m_bits), [](auto&& _) { auto&& [ lhs, rhs] = _;  
                                return bit::is_subset_of(lhs, rhs); 
                        });
                }
        }

        [[nodiscard]] constexpr bool is_proper_subset_of(array const& other [[maybe_unused]]) const noexcept
        {
                if constexpr (N == 0) {
                        return false;
                } else if constexpr (num_blocks == 1) {
                        return 
                                bit::is_subset_of(this->m_bits[0], other.m_bits[0]) and
                                bit::not_equal_to(this->m_bits[0], other.m_bits[0])
                        ;
                } else if constexpr (num_blocks == 2) {                         
                        if (not bit::is_subset_of(this->m_bits[0], other.m_bits[0])) {
                                return false;
                        } else if (bit::not_equal_to(this->m_bits[0], other.m_bits[0])) {
                                return bit::is_subset_of(this->m_bits[1], other.m_bits[1]);
                        } else { 
                                return
                                        bit::is_subset_of(this->m_bits[1], other.m_bits[1]) and
                                        bit::not_equal_to(this->m_bits[1], other.m_bits[1])
                                ;
                        }                        
                } else if constexpr (num_blocks >= 3) {
                        auto i = 0uz;
                        while(i < num_blocks) {
                                if (not bit::is_subset_of(this->m_bits[i], other.m_bits[i])) {
                                        return false;
                                }
                                if (    bit::not_equal_to(this->m_bits[i], other.m_bits[i])) {
                                        break;
                                }
                                ++i;
                        }
                        return (i == num_blocks) ? false : std::ranges::all_of(
                                std::views::zip(this->m_bits, other.m_bits) | std::views::drop(i), [](auto&& _) { auto&& [ lhs, rhs ] = _; 
                                return bit::is_subset_of(lhs, rhs); 
                        });
                }
        }

        [[nodiscard]] constexpr bool intersects(array const& other [[maybe_unused]]) const noexcept
        {
                if constexpr (N == 0) {
                        return false;
                } else if constexpr (num_blocks == 1) {
                        return bit::intersects(this->m_bits[0], other.m_bits[0]);
                } else if constexpr (num_blocks == 2) {
                        return
                                bit::intersects(this->m_bits[0], other.m_bits[0]) or
                                bit::intersects(this->m_bits[1], other.m_bits[1])
                        ;
                } else if constexpr (num_blocks >= 3) {
                        return std::ranges::any_of(
                                std::views::zip(this->m_bits, other.m_bits), [](auto&& _) { auto&& [ lhs, rhs ] = _; 
                                return bit::intersects(lhs, rhs); 
                        });
                }
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

        [[nodiscard]] static constexpr bool is_valid(std::size_t n [[maybe_unused]]) noexcept
        {
                if constexpr (N == 0) {
                        return false;
                } else {
                        return n < N;
                }
        }

        [[nodiscard]] static constexpr auto div_mod(std::size_t numer, std::size_t denom) noexcept
                -> std::pair<std::size_t, std::size_t>
        {
                return { numer / denom, numer % denom };
        }

        [[nodiscard]] static constexpr auto index_offset(std::size_t n) noexcept
                -> std::pair<std::size_t, std::size_t>
        {
                if constexpr (num_blocks == 1) {
                        return { 0uz, n };
                } else {
                        return div_mod(n, bits_per_block);
                }
        }

        [[nodiscard]] constexpr auto block_mask(this auto&& self, std::size_t n) noexcept
                -> std::pair<decltype(std::forward<decltype(self)>(self).m_bits[std::declval<std::size_t>()]), Block>
        {
                auto const [ index, offset ] = index_offset(n);
                return { std::forward<decltype(self)>(self).m_bits[index], static_cast<Block>(unit << offset) };
        }

        constexpr void erase_unused() noexcept
        {
                if constexpr (has_unused_bits) {
                        m_bits[last_block] &= used_bits;
                        assert(not bit::intersects(m_bits[last_block], unused_bits));
                }
        }

        template<std::random_access_iterator I, std::sized_sentinel_for<I> S>
        [[nodiscard]] static constexpr auto distance(I first, S last) noexcept
        {
                return static_cast<std::size_t>(std::ranges::distance(first, last));
        }
};

}       // namespace xstd::bit

#endif  // include guard
