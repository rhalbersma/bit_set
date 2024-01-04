#ifndef XSTD_BIT_SET_HPP
#define XSTD_BIT_SET_HPP

//          Copyright Rein Halbersma 2014-2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>            // lexicographical_compare_three_way, shift_left, shift_right
                                // all_of, any_of, fill, find_if, fold_left, max, none_of, swap, swap_ranges
#include <bit>                  // countl_zero, countr_zero, popcount
#include <cassert>              // assert
#include <compare>              // strong_ordering
#include <concepts>             // constructible_from, unsigned_integral
#include <cstddef>              // ptrdiff_t, size_t
#include <functional>           // identity, less
#include <initializer_list>     // initializer_list
#include <iterator>             // bidirectional_iterator_tag, reverse_iterator
                                // input_iterator, sentinel_for
#include <limits>               // digits
#include <ranges>               // begin, distance, empty, end, from_range_t, rbegin, rend, subrange
                                // drop, pairwise_transform, reverse, transform, zip, zip_transform
                                // input_range
#include <tuple>                // tie
#include <type_traits>          // common_type_t, is_class_v, make_signed_t
#include <utility>              // forward, pair, unreachable

namespace xstd {

[[nodiscard]] constexpr auto bit_align(int alignment, int size) noexcept
{
        return ((size - 1 + alignment) / alignment) * alignment;
}

template<int N, std::unsigned_integral>
        requires (N >= 0)
class bit_set;

template<int N, std::unsigned_integral Block = std::size_t>
using bit_set_aligned = bit_set<bit_align(std::numeric_limits<Block>::digits, N), Block>;

template<int N, std::unsigned_integral Block = std::size_t>
        requires (N >= 0)
class bit_set
{
        static constexpr auto block_size = std::numeric_limits<Block>::digits;
        static constexpr auto num_bits = bit_align(block_size, N);
        static constexpr auto num_blocks = std::ranges::max(num_bits / block_size, 1);
        static constexpr auto has_unused_bits = num_bits > N;
        static constexpr auto num_unused_bits = num_bits - N;

        class proxy_iterator;
        class proxy_reference;

        Block m_data[num_blocks]{};     // zero-initialization
public:
        using key_type               = int;
        using key_compare            = std::less<key_type>;
        using value_type             = int;
        using value_compare          = std::less<value_type>;
        using pointer                = proxy_iterator;
        using const_pointer          = proxy_iterator;
        using reference              = proxy_reference;
        using const_reference        = proxy_reference;
        using size_type              = std::size_t;
        using difference_type        = std::ptrdiff_t;
        using iterator               = proxy_iterator;
        using const_iterator         = proxy_iterator;
        using reverse_iterator       = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;
        using block_type             = Block;

        bit_set() = default;            // zero-initialization

        template<std::input_iterator I>
        [[nodiscard]] constexpr bit_set(I first, I last) noexcept
                requires std::constructible_from<value_type, decltype(*first)>
        {
                if constexpr (N == 0) { assert(first == last); }
                do_insert(first, last);
        }

        [[nodiscard]] constexpr bit_set(std::initializer_list<value_type> ilist) noexcept
        {
                if constexpr (N == 0) { assert(std::ranges::empty(ilist)); }
                do_insert(ilist.begin(), ilist.end());
        }

        [[nodiscard]] constexpr bit_set(std::from_range_t, std::ranges::input_range auto&& rg) noexcept
                requires std::constructible_from<value_type, decltype(*std::ranges::begin(rg))>
        {
                if constexpr (N == 0) { assert(std::ranges::empty(rg)); }
                do_insert(std::ranges::begin(rg), std::ranges::end(rg));
        }

        constexpr auto& operator=(std::initializer_list<value_type> ilist) noexcept
        {
                if constexpr (N == 0) { assert(std::ranges::empty(ilist)); }
                clear();
                insert(ilist.begin(), ilist.end());
                return *this;
        }

        bool operator==(bit_set const&) const = default;

        [[nodiscard]] constexpr auto operator<=>(bit_set const& other [[maybe_unused]]) const noexcept
                -> std::strong_ordering
        {
                if constexpr (N == 0) {
                        return std::strong_ordering::equal;
                } else if constexpr (num_blocks == 1) {
                        return other.m_data[0] <=> this->m_data[0];
                } else if constexpr (num_blocks == 2) {
                        constexpr auto tied = [](auto const& bs) {
                                return std::tie(bs.m_data[1], bs.m_data[0]);
                        };
                        return tied(other) <=> tied(*this);
                } else if constexpr (num_blocks >= 3) {
                        return std::lexicographical_compare_three_way(
                                std::ranges::rbegin(other.m_data), std::ranges::rend(other.m_data),
                                std::ranges::rbegin(this->m_data), std::ranges::rend(this->m_data)
                        );
                }
        }

        [[nodiscard]] constexpr auto begin()         noexcept { return       iterator(this, find_first()); }
        [[nodiscard]] constexpr auto begin()   const noexcept { return const_iterator(this, find_first()); }
        [[nodiscard]] constexpr auto end()           noexcept { return       iterator(this, N); }
        [[nodiscard]] constexpr auto end()     const noexcept { return const_iterator(this, N); }

        [[nodiscard]] constexpr auto rbegin()        noexcept { return       reverse_iterator(end()); }
        [[nodiscard]] constexpr auto rbegin()  const noexcept { return const_reverse_iterator(end()); }
        [[nodiscard]] constexpr auto rend()          noexcept { return       reverse_iterator(begin()); }
        [[nodiscard]] constexpr auto rend()    const noexcept { return const_reverse_iterator(begin()); }

        [[nodiscard]] constexpr auto cbegin()  const noexcept { return const_iterator(begin()); }
        [[nodiscard]] constexpr auto cend()    const noexcept { return const_iterator(end());   }
        [[nodiscard]] constexpr auto crbegin() const noexcept { return const_reverse_iterator(rbegin()); }
        [[nodiscard]] constexpr auto crend()   const noexcept { return const_reverse_iterator(rend());   }

        [[nodiscard]] constexpr auto front() const noexcept
        {
                if constexpr (N == 0) { std::unreachable(); }
                assert(!empty());
                return const_reference(*this, find_front());
        }

        [[nodiscard]] constexpr auto back() const noexcept
        {
                if constexpr (N == 0) { std::unreachable(); }
                assert(!empty());
                return const_reference(*this, find_back());
        }

        [[nodiscard]] constexpr auto empty() const noexcept
        {
                if constexpr (N == 0) {
                        return true;
                } else if constexpr (num_blocks == 1) {
                        return !m_data[0];
                } else if constexpr (num_blocks == 2) {
                        return !(m_data[0] || m_data[1]);
                } else if constexpr (num_blocks >= 3) {
                        return std::ranges::none_of(m_data, std::identity());
                }
        }

        [[nodiscard]] constexpr auto full() const noexcept
        {
                if constexpr (has_unused_bits) {
                        if constexpr (num_blocks == 1) {
                                return m_data[0] == used_bits;
                        } else if (num_blocks == 2) {
                                return m_data[0] == used_bits && m_data[1] == ones;
                        } else if (num_blocks >= 3) {
                                return m_data[0] == used_bits && std::ranges::all_of(m_data | std::views::drop(1), [](auto block) { 
                                        return block == ones; 
                                });
                        }
                } else {
                        if constexpr (N == 0) {
                                return true;
                        } else if constexpr (num_blocks == 1) {
                                return m_data[0] == ones;
                        } else if constexpr (num_blocks == 2) {
                                return m_data[0] == ones && m_data[1] == ones;
                        } else if constexpr (num_blocks >= 3) {
                                return std::ranges::all_of(m_data, [](auto block) {
                                        return block == ones;
                                });
                        }
                }
        }

        [[nodiscard]] constexpr auto ssize() const noexcept
        {
                if constexpr (N == 0) {
                        return 0;
                } else if constexpr (num_blocks == 1) {
                        return std::popcount(m_data[0]);
                } else if constexpr (num_blocks == 2) {
                        return std::popcount(m_data[0]) + std::popcount(m_data[1]);
                } else if constexpr (num_blocks >= 3) {
                        return std::ranges::fold_left(
                                m_data | std::views::transform([](auto block) { 
                                        return std::popcount(block); 
                                }), 0, std::plus()
                        );
                }
        }

        [[nodiscard]] constexpr auto size() const noexcept
        {
                return static_cast<size_type>(ssize());
        }

        [[nodiscard]] static constexpr auto max_size() noexcept
        {
                return static_cast<size_type>(N);
        }

        constexpr auto emplace(auto&&... args) noexcept
                requires (sizeof...(args) == 1)
        {
                return insert(value_type(std::forward<decltype(args)>(args)...));
        }

        constexpr auto emplace_hint(const_iterator hint, auto&&... args) noexcept
                requires (sizeof...(args) == 1)
        {
                return insert(hint, value_type(std::forward<decltype(args)>(args)...));
        }

        constexpr auto add(value_type x) noexcept
        {
                auto&& [ block, mask ] = block_mask(x);
                block |= mask;
                assert(contains(x));
        }

private:
        constexpr auto do_insert(value_type x) noexcept
                -> std::pair<iterator, bool>
        {
                auto&& [ block, mask ] = block_mask(x);
                auto const inserted = !(block & mask);
                block |= mask;
                assert(contains(x));
                return { iterator(this, x),  inserted };
        }

public:
        constexpr auto insert(value_type const& x) noexcept
        {
                return do_insert(x);
        }

        constexpr auto insert(value_type&& x) noexcept
        {
                return do_insert(std::move(x));
        }

private:
        constexpr auto do_insert(const_iterator /* hint */, value_type x) noexcept
        {
                add(x);
                return iterator(this, x);
        }

public:
        constexpr auto insert(const_iterator hint, value_type const& x) noexcept
        {
                return do_insert(hint, x);
        }

        constexpr auto insert(const_iterator hint, value_type&& x) noexcept
        {
                return do_insert(hint, std::move(x));
        }

private:
        template<std::input_iterator I, std::sentinel_for<I> S>
        constexpr auto do_insert(I first, S last) noexcept
                requires std::constructible_from<value_type, decltype(*first)>
        {
                for (auto x : std::ranges::subrange(first, last)) {
                        add(x);
                }
        }

public:
        template<std::input_iterator I>
        constexpr auto insert(I first, I last) noexcept
                requires std::constructible_from<value_type, decltype(*first)>
        {
                if constexpr (N == 0) { assert(first == last); }
                do_insert(first, last);
        }

        constexpr auto insert(std::initializer_list<value_type> ilist) noexcept
        {
                if constexpr (N == 0) { assert(std::ranges::empty(ilist)); }
                do_insert(ilist.begin(), ilist.end());
        }

        constexpr auto insert_range(std::ranges::input_range auto&& rg) noexcept
                requires std::constructible_from<value_type, decltype(*std::ranges::begin(rg))>
        {
                if constexpr (N == 0) { assert(std::ranges::empty(rg)); }
                do_insert(std::ranges::begin(rg), std::ranges::end(rg));
        }

        constexpr auto fill() noexcept
        {
                if constexpr (has_unused_bits) {
                        m_data[0] = used_bits;
                        if constexpr (num_blocks == 2) {
                                m_data[1] = ones;
                        } else if (num_blocks >= 3) {
                                std::ranges::fill(m_data | std::views::drop(1), ones);
                        }
                } else if constexpr (N > 0) {
                        if constexpr (num_blocks == 1) {
                                m_data[0] = ones;
                        } else if constexpr (num_blocks == 2) {
                                m_data[0] = ones;
                                m_data[1] = ones;
                        } else if constexpr (num_blocks >= 3) {
                                std::ranges::fill(m_data, ones);
                        }
                }
                assert(full());
        }

        constexpr auto pop(key_type x) noexcept
        {
                auto&& [ block, mask ] = block_mask(x);
                block &= static_cast<block_type>(~mask);
                assert(!contains(x));
        }

        constexpr auto erase(key_type const& x) noexcept
        {
                auto&& [ block, mask ] = block_mask(x);
                auto const erased = static_cast<size_type>(static_cast<bool>(block & mask));
                block &= static_cast<block_type>(~mask);
                assert(!contains(x));
                return erased;
        }

        constexpr auto erase(const_iterator pos) noexcept
        {
                if constexpr (N == 0) { std::unreachable(); } 
                assert(pos != end());
                pop(*pos++);
                return pos;
        }

        constexpr auto erase(const_iterator first, const_iterator last) noexcept
        {
                if constexpr (N == 0) { assert(first == last); }
                for (auto x : std::ranges::subrange(first, last)) {
                        pop(x);
                }
                return last;
        }

        constexpr auto swap(bit_set& other [[maybe_unused]]) noexcept
        {
                if constexpr (N > 0 && num_blocks == 1) {
                        std::ranges::swap(this->m_data[0], other.m_data[0]);
                } else if constexpr (num_blocks == 2) {
                        std::ranges::swap(this->m_data[0], other.m_data[0]);
                        std::ranges::swap(this->m_data[1], other.m_data[1]);
                } else if constexpr (num_blocks >= 3) {
                        std::ranges::swap_ranges(this->m_data, other.m_data);
                }
        }

        constexpr auto clear() noexcept
        {
                if constexpr (N > 0 && num_blocks == 1) {
                        m_data[0] = zero;
                } else if constexpr (num_blocks == 2) {
                        m_data[0] = zero;
                        m_data[1] = zero;
                } else if constexpr (num_blocks >= 3) {
                        std::ranges::fill(m_data, zero);
                }
                assert(empty());
        }

        constexpr auto complement(value_type x) noexcept
        {
                auto&& [ block, mask ] = block_mask(x);
                block ^= mask;
        }

        [[nodiscard]] constexpr auto find(key_type const& x) noexcept
        {
                return contains(x) ? iterator(this, x) : end();
        }

        [[nodiscard]] constexpr auto find(key_type const& x) const noexcept
        {
                return contains(x) ? const_iterator(this, x) : cend();
        }

        [[nodiscard]] constexpr auto count(key_type const& x) const noexcept
                -> size_type
        {
                return contains(x);
        }

        [[nodiscard]] constexpr auto contains(key_type const& x) const noexcept
                -> bool
        {
                auto&& [ block, mask ] = block_mask(x);
                return block & mask;
        }

        [[nodiscard]] constexpr auto lower_bound(key_type const& x) noexcept
        {
                return iterator(this, find_next(x));
        }

        [[nodiscard]] constexpr auto lower_bound(key_type const& x) const noexcept
        {
                return const_iterator(this, find_next(x));
        }

        [[nodiscard]] constexpr auto upper_bound(key_type const& x) noexcept
        {
                return iterator(this, find_next(x + 1));
        }

        [[nodiscard]] constexpr auto upper_bound(key_type const& x) const noexcept
        {
                return const_iterator(this, find_next(x + 1));
        }

        [[nodiscard]] constexpr auto equal_range(key_type const& x) noexcept
                -> std::pair<iterator, iterator>
        {
                return { lower_bound(x), upper_bound(x) };
        }

        [[nodiscard]] constexpr auto equal_range(key_type const& x) const noexcept
                -> std::pair<const_iterator, const_iterator>
        {
                return { lower_bound(x), upper_bound(x) };
        }

        constexpr auto complement() noexcept
        {
                if constexpr (N > 0 && num_blocks == 1) {
                        m_data[0] = static_cast<block_type>(~m_data[0]);
                } else if constexpr (num_blocks == 2) {
                        m_data[0] = static_cast<block_type>(~m_data[0]);
                        m_data[1] = static_cast<block_type>(~m_data[1]);
                } else if constexpr (num_blocks >= 3) {
                        for (auto& block : m_data) {
                                block = static_cast<block_type>(~block);
                        }
                }
                clear_unused_bits();
        }

        constexpr auto& operator&=(bit_set const& other [[maybe_unused]]) noexcept
        {
                if constexpr (N > 0 && num_blocks == 1) {
                        this->m_data[0] &= other.m_data[0];
                } else if constexpr (num_blocks == 2) {
                        this->m_data[0] &= other.m_data[0];
                        this->m_data[1] &= other.m_data[1];
                } else if constexpr (num_blocks >= 3) {
                        for (auto&& [lhs, rhs] : std::views::zip(this->m_data, other.m_data)) {
                                lhs &= rhs;
                        }
                }
                return *this;
        }

        constexpr auto& operator|=(bit_set const& other [[maybe_unused]]) noexcept
        {
                if constexpr (N > 0 && num_blocks == 1) {
                        this->m_data[0] |= other.m_data[0];
                } else if constexpr (num_blocks == 2) {
                        this->m_data[0] |= other.m_data[0];
                        this->m_data[1] |= other.m_data[1];
                } else if constexpr (num_blocks >= 3) {
                        for (auto&& [lhs, rhs] : std::views::zip(this->m_data, other.m_data)) {
                                lhs |= rhs;
                        }
                }
                return *this;
        }

        constexpr auto& operator^=(bit_set const& other [[maybe_unused]]) noexcept
        {
                if constexpr (N > 0 && num_blocks == 1) {
                        this->m_data[0] ^= other.m_data[0];
                } else if constexpr (num_blocks == 2) {
                        this->m_data[0] ^= other.m_data[0];
                        this->m_data[1] ^= other.m_data[1];
                } else if constexpr (num_blocks >= 3) {
                        for (auto&& [lhs, rhs] : std::views::zip(this->m_data, other.m_data)) {
                                lhs ^= rhs;
                        }
                }
                return *this;
        }

        constexpr auto& operator-=(bit_set const& other [[maybe_unused]]) noexcept
        {
                if constexpr (N > 0 && num_blocks == 1) {
                        this->m_data[0] &= static_cast<block_type>(~other.m_data[0]);
                } else if constexpr (num_blocks == 2) {
                        this->m_data[0] &= static_cast<block_type>(~other.m_data[0]);
                        this->m_data[1] &= static_cast<block_type>(~other.m_data[1]);
                } else if constexpr (num_blocks >= 3) {
                        for (auto&& [lhs, rhs] : std::views::zip(this->m_data, other.m_data)) {
                                lhs &= static_cast<block_type>(~rhs);
                        }
                }
                return *this;
        }

        constexpr auto& operator<<=(value_type n [[maybe_unused]]) noexcept
        {
                if constexpr (N == 0) { std::unreachable(); } 
                assert(is_valid_reference(n));
                if constexpr (num_blocks == 1) {
                        m_data[0] >>= n;
                } else if constexpr (num_blocks >= 2) {
                        auto const [ n_blocks, R_shift ] = div(n, block_size);
                        if (R_shift == 0) {
                                std::shift_left(std::ranges::begin(m_data), std::ranges::end(m_data), n_blocks);
                        } else {
                                auto const L_shift = block_size - R_shift;
                                for (auto&& [lhs, rhs] : std::views::zip(
                                        m_data,
                                        m_data | std::views::drop(n_blocks) | std::views::pairwise_transform(
                                                [=](auto first, auto second) -> block_type
                                                { 
                                                        return 
                                                                static_cast<block_type>(second << L_shift) | 
                                                                static_cast<block_type>(first  >> R_shift)
                                                        ; 
                                                }
                                        )
                                )) {
                                        lhs = rhs;
                                }                               
                                m_data[last_block - n_blocks] = static_cast<block_type>(m_data[last_block] >> R_shift);
                        }
                        std::ranges::fill(m_data | std::views::drop(num_blocks - n_blocks), zero);
                }
                clear_unused_bits();
                return *this;
        }

        constexpr auto& operator>>=(value_type n [[maybe_unused]]) noexcept
        {
                if constexpr (N == 0) { std::unreachable(); } 
                assert(is_valid_reference(n));
                if constexpr (num_blocks == 1) {
                        m_data[0] <<= n;
                } else if constexpr (num_blocks >= 2) {
                        auto const [ n_blocks, L_shift ] = div(n, block_size);
                        if (L_shift == 0) {
                                std::shift_right(std::ranges::begin(m_data), std::ranges::end(m_data), n_blocks);
                        } else {
                                auto const R_shift = block_size - L_shift;
                                for (auto&& [lhs, rhs] : std::views::zip(
                                        m_data | std::views::reverse,
                                        m_data | std::views::reverse | std::views::drop(n_blocks) | std::views::pairwise_transform(
                                                [=](auto first, auto second) -> block_type
                                                { 
                                                        return 
                                                                static_cast<block_type>(first  << L_shift) | 
                                                                static_cast<block_type>(second >> R_shift)
                                                        ; 
                                                }
                                        )
                                )) {
                                        lhs = rhs;
                                }
                                m_data[n_blocks] = static_cast<block_type>(m_data[0] << L_shift);
                        }
                        std::ranges::fill(m_data | std::views::reverse | std::views::drop(num_blocks - n_blocks), zero);
                }
                return *this;
        }

        [[nodiscard]] constexpr auto is_subset_of(bit_set const& other [[maybe_unused]]) const noexcept
        {
                if constexpr (N == 0) {
                        return true;
                } else if constexpr (num_blocks == 1) {
                        return !(this->m_data[0] & ~other.m_data[0]);
                } else if constexpr (num_blocks == 2) {
                        return !(
                                (this->m_data[0] & ~other.m_data[0]) ||
                                (this->m_data[1] & ~other.m_data[1])
                        );
                } else if constexpr (num_blocks >= 3) {
                        return std::ranges::none_of(
                                std::views::zip_transform(
                                        [](auto lhs, auto rhs) { return lhs & ~rhs; }, 
                                        this->m_data, other.m_data
                                ),
                                std::identity()
                        );
                }
        }

        [[nodiscard]] constexpr auto is_proper_subset_of(bit_set const& other [[maybe_unused]]) const noexcept
        {
                if constexpr (N == 0) {
                        return false;
                } else if constexpr (num_blocks == 1) {
                        return !(
                                (this->m_data[0] & ~other.m_data[0]) ||
                                (this->m_data[0] == other.m_data[0])
                        );
                } else if constexpr (num_blocks == 2) {
                        return !(
                                (this->m_data[0] & ~other.m_data[0]) ||
                                (this->m_data[1] & ~other.m_data[1]) ||
                                (this->m_data[0] == other.m_data[0] && this->m_data[1] == other.m_data[1])
                        );
                } else if constexpr (num_blocks >= 3) {
                        auto i = 0;
                        for (/* init-statement before loop */; i < num_blocks; ++i) {
                                if (this->m_data[i] & ~other.m_data[i]) {
                                        return false;
                                }
                                if (this->m_data[i] != other.m_data[i]) {
                                        break;
                                }
                        }
                        return (i == num_blocks) ? false : std::ranges::none_of(
                                std::views::zip_transform(
                                        [](auto lhs, auto rhs) { return lhs & ~rhs; }, 
                                        this->m_data, other.m_data
                                ) | std::views::drop(i),
                                std::identity()
                        );
                }
        }

        [[nodiscard]] constexpr auto intersects(bit_set const& other [[maybe_unused]]) const noexcept
                -> bool
        {
                if constexpr (N == 0) {
                        return false;
                } else if constexpr (num_blocks == 1) {
                        return this->m_data[0] & other.m_data[0];
                } else if constexpr (num_blocks == 2) {
                        return
                                (this->m_data[0] & other.m_data[0]) ||
                                (this->m_data[1] & other.m_data[1])
                        ;
                } else if constexpr (num_blocks >= 3) {
                        return std::ranges::any_of(
                                std::views::zip_transform(
                                        [](auto lhs, auto rhs) { return lhs & rhs; }, 
                                        this->m_data, other.m_data
                                ),
                                std::identity()
                        );
                }
        }

private:
        static constexpr auto zero = static_cast<block_type>( 0);
        static constexpr auto ones = static_cast<block_type>(-1);
        static constexpr auto used_bits = static_cast<block_type>(ones << num_unused_bits);
        static constexpr auto last_block = num_blocks - 1;
        static constexpr auto last_bit = num_bits - 1;
        static constexpr auto left_bit = block_size - 1;
        static constexpr auto left_mask = static_cast<block_type>(static_cast<block_type>(1) << left_bit);

        [[nodiscard]] static constexpr auto is_valid_reference(value_type n) noexcept
        {
                if constexpr (N == 0) {
                        return false;
                } else {
                        return 0 <= n && n < N;
                }
        }

        [[nodiscard]] static constexpr auto is_valid_iterator(value_type n) noexcept
        {
                return 0 <= n && n <= N;
        }

        [[nodiscard]] static constexpr auto is_incrementable(value_type n) noexcept
        {
                return is_valid_reference(n);
        }

        [[nodiscard]] static constexpr auto is_decrementable(value_type n) noexcept
        {
                return is_valid_reference(n - 1);
        }        

        [[nodiscard]] static constexpr auto div(value_type numer, value_type denom) noexcept
                -> std::pair<value_type, value_type>
        {
                return { numer / denom, numer % denom };
        }

        [[nodiscard]] static constexpr auto index_offset(value_type n) noexcept
                -> std::pair<value_type, value_type>
        {
                if constexpr (num_blocks == 1) {
                        return { 0, n };
                } else if constexpr (num_blocks >= 2) {
                        return div(n, block_size);
                }
        }

        [[nodiscard]] constexpr auto block_mask(value_type n) noexcept
                -> std::pair<block_type&, block_type>
        {
                auto const [ index, offset ] = index_offset(n);
                return { m_data[last_block - index], static_cast<block_type>(left_mask >> offset) };
        }

        [[nodiscard]] constexpr auto block_mask(value_type n) const noexcept
                -> std::pair<block_type const&, block_type>
        {
                auto const [ index, offset ] = index_offset(n);
                return { m_data[last_block - index], static_cast<block_type>(left_mask >> offset) };
        }

        constexpr auto clear_unused_bits() noexcept
        {
                if constexpr (has_unused_bits) {
                        m_data[0] &= used_bits;
                }
        }

        [[nodiscard]] constexpr auto find_front() const noexcept
        {
                if constexpr (N == 0) { std::unreachable(); } 
                assert(!empty());
                if constexpr (num_blocks == 1) {
                        return std::countl_zero(m_data[0]);
                } else if constexpr (num_blocks == 2) {
                        return m_data[1] ? std::countl_zero(m_data[1]) : block_size + std::countl_zero(m_data[0]);
                } else if constexpr (num_blocks >= 3) {
                        auto const front = std::ranges::find_if(m_data | std::views::reverse, std::identity());
                        assert(front != std::ranges::rend(m_data));
                        return static_cast<value_type>(std::ranges::distance(std::ranges::rbegin(m_data), front)) * block_size + std::countl_zero(*front);
                }
        }

        [[nodiscard]] constexpr auto find_back() const noexcept
        {
                if constexpr (N == 0) { std::unreachable(); } 
                assert(!empty());
                if constexpr (num_blocks == 1) {
                        return last_bit - std::countr_zero(m_data[0]);
                } else if constexpr (num_blocks == 2) {
                        return m_data[0] ? last_bit - std::countr_zero(m_data[0]) : left_bit - std::countr_zero(m_data[1]);
                } else if constexpr (num_blocks >= 3) {
                        auto const back = std::ranges::find_if(m_data, std::identity());
                        assert(back != std::ranges::end(m_data));
                        return last_bit - static_cast<value_type>(std::ranges::distance(std::ranges::begin(m_data), back)) * block_size - std::countr_zero(*back);
                }
        }

        [[nodiscard]] constexpr auto find_first() const noexcept
        {
                if constexpr (N > 0 && num_blocks == 1) {
                        if (m_data[0]) {
                                return std::countl_zero(m_data[0]);
                        }
                } else if constexpr (num_blocks == 2) {
                        if (m_data[1]) {
                                return std::countl_zero(m_data[1]);
                        } else if (m_data[0]) {
                                return block_size + std::countl_zero(m_data[0]);
                        }
                } else if constexpr (num_blocks >= 3) {
                        if (
                                auto const first = std::ranges::find_if(m_data | std::views::reverse, std::identity()); 
                                first != std::ranges::rend(m_data)
                        ) {
                                return static_cast<value_type>(std::ranges::distance(std::ranges::rbegin(m_data), first)) * block_size + std::countl_zero(*first);
                        }
                }
                return N;
        }

        [[nodiscard]] constexpr auto find_next(value_type n) const noexcept
        {
                assert(is_valid_iterator(n));
                if (n == N) {
                        return N;
                }
                if constexpr (num_blocks == 1) {
                        if (auto const block = static_cast<block_type>(m_data[0] << n); block) {
                                return n + std::countl_zero(block);
                        }
                } else if constexpr (num_blocks >= 2) {
                        auto [ index, offset ] = index_offset(n);
                        if (offset) {
                                if (auto const block = static_cast<block_type>(m_data[last_block - index] << offset); block) {
                                        return n + std::countl_zero(block);
                                }
                                ++index;
                                n += block_size - offset;
                        }
                        auto const rg = m_data | std::views::reverse | std::views::drop(index);
                        if (
                                auto const next = std::ranges::find_if(rg, std::identity()); 
                                next != std::ranges::end(rg)
                        ) {
                                return n + static_cast<value_type>(std::ranges::distance(std::ranges::begin(rg), next)) * block_size + std::countl_zero(*next);                                
                        }                       
                }
                return N;
        }

        [[nodiscard]] constexpr auto find_prev(value_type n) const noexcept
        {
                if constexpr (N == 0) { std::unreachable(); } 
                assert(is_valid_reference(n));
                assert(!empty());
                if constexpr (num_blocks == 1) {
                        return n - std::countr_zero(static_cast<block_type>(m_data[0] >> (left_bit - n)));
                } else if constexpr (num_blocks >= 2) {
                        auto [ index, offset ] = index_offset(n);
                        if (auto const reverse_offset = left_bit - offset; reverse_offset) {
                                if (auto const block = static_cast<block_type>(m_data[last_block - index] >> reverse_offset); block) {
                                        return n - std::countr_zero(block);
                                }
                                --index;
                                n -= block_size - reverse_offset;
                        }
                        auto const rg = m_data | std::views::drop(last_block - index);
                        auto const prev = std::ranges::find_if(rg, std::identity());
                        assert(prev != std::ranges::end(rg));
                        return n - static_cast<value_type>(std::ranges::distance(std::ranges::begin(rg), prev)) * block_size - std::countr_zero(*prev); 
                }
        }

        class proxy_reference
        {
                using rimpl_type = bit_set const&;
                using value_type = bit_set::value_type;
                rimpl_type m_ref;
                value_type m_val;
        public:
                ~proxy_reference() = default;
                proxy_reference(proxy_reference const&) = default;
                proxy_reference(proxy_reference&&) = default;
                proxy_reference& operator=(proxy_reference const&) = delete;
                proxy_reference& operator=(proxy_reference&&) = delete;

                proxy_reference() = delete;

                [[nodiscard]] explicit constexpr proxy_reference(rimpl_type r, value_type v) noexcept
                :
                        m_ref(r),
                        m_val(v)
                {
                        if constexpr (N == 0) { std::unreachable(); } 
                        assert(is_valid_reference(m_val));
                }

                [[nodiscard]] constexpr auto operator==(proxy_reference const& other) const noexcept
                        -> bool
                {
                        return this->m_val == other.m_val;
                }

                [[nodiscard]] constexpr auto operator&() const noexcept
                {
                        return proxy_iterator(&m_ref, m_val);
                }

                [[nodiscard]] explicit(false) constexpr operator value_type() const noexcept
                {
                        return m_val;
                }

                template<class T>
                [[nodiscard]] explicit(false) constexpr operator T() const noexcept(noexcept(T(m_val)))
                        requires std::is_class_v<T> && std::constructible_from<T, value_type>
                {
                        return m_val;
                }
        };

        class proxy_iterator
        {
        public:
                using iterator_category = std::bidirectional_iterator_tag;
                using difference_type   = bit_set::difference_type;
                using value_type        = bit_set::value_type;
                using pointer           = proxy_iterator;
                using reference         = proxy_reference;

        private:
                using pimpl_type = bit_set const*;
                pimpl_type m_ptr;
                value_type m_val;

        public:
                proxy_iterator() = default;

                [[nodiscard]] explicit constexpr proxy_iterator(pimpl_type p, value_type v) noexcept
                :
                        m_ptr(p),
                        m_val(v)
                {
                        assert(is_valid_iterator(m_val));
                }

                [[nodiscard]] constexpr auto operator==(proxy_iterator const& other) const noexcept
                        -> bool
                {
                        assert(this->m_ptr == other.m_ptr);
                        if constexpr (N == 0) {
                                return true;
                        } else {
                                return this->m_val == other.m_val;
                        }
                }

                [[nodiscard]] constexpr auto operator*() const noexcept
                {
                        if constexpr (N == 0) { std::unreachable(); } 
                        assert(is_valid_reference(m_val));
                        return proxy_reference(*m_ptr, m_val);
                }

                constexpr auto& operator++() noexcept
                {
                        if constexpr (N == 0) { std::unreachable(); } 
                        assert(is_incrementable(m_val));
                        m_val = m_ptr->find_next(m_val + 1);
                        assert(is_decrementable(m_val));
                        return *this;
                }

                constexpr auto operator++(int) noexcept
                {
                        auto nrv = *this; ++*this; return nrv;
                }

                constexpr auto& operator--() noexcept
                {
                        if constexpr (N == 0) { std::unreachable(); } 
                        assert(is_decrementable(m_val));
                        m_val = m_ptr->find_prev(m_val - 1);
                        assert(is_incrementable(m_val));
                        return *this;
                }

                constexpr auto operator--(int) noexcept
                {
                        auto nrv = *this; --*this; return nrv;
                }
        };
};

template<int N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto operator~(bit_set<N, Block> const& lhs) noexcept
{
        auto nrv = lhs; nrv.complement(); return nrv;
}

template<int N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto operator&(bit_set<N, Block> const& lhs, bit_set<N, Block> const& rhs) noexcept
{
        auto nrv = lhs; nrv &= rhs; return nrv;
}

template<int N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto operator|(bit_set<N, Block> const& lhs, bit_set<N, Block> const& rhs) noexcept
{
        auto nrv = lhs; nrv |= rhs; return nrv;
}

template<int N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto operator^(bit_set<N, Block> const& lhs, bit_set<N, Block> const& rhs) noexcept
{
        auto nrv = lhs; nrv ^= rhs; return nrv;
}

template<int N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto operator-(bit_set<N, Block> const& lhs, bit_set<N, Block> const& rhs) noexcept
{
        auto nrv = lhs; nrv -= rhs; return nrv;
}

template<int N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto operator<<(bit_set<N, Block> const& lhs, int n) noexcept
{
        auto nrv = lhs; nrv <<= n; return nrv;
}

template<int N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto operator>>(bit_set<N, Block> const& lhs, int n) noexcept
{
        auto nrv = lhs; nrv >>= n; return nrv;
}

template<int N, std::unsigned_integral Block>
constexpr auto swap(bit_set<N, Block>& lhs, bit_set<N, Block>& rhs) noexcept
{
        lhs.swap(rhs);
}

template<int N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto begin(bit_set<N, Block>& bs) noexcept
{
        return bs.begin();
}

template<int N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto begin(bit_set<N, Block> const& bs) noexcept
{
        return bs.begin();
}

template<int N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto end(bit_set<N, Block>& bs) noexcept
{
        return bs.end();
}

template<int N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto end(bit_set<N, Block> const& bs) noexcept
{
        return bs.end();
}

template<int N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto rbegin(bit_set<N, Block>& bs) noexcept
{
        return bs.rbegin();
}

template<int N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto rbegin(bit_set<N, Block> const& bs) noexcept
{
        return bs.rbegin();
}

template<int N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto rend(bit_set<N, Block>& bs) noexcept
{
        return bs.rend();
}

template<int N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto rend(bit_set<N, Block> const& bs) noexcept
{
        return bs.rend();
}

template<int N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto cbegin(bit_set<N, Block> const& bs) noexcept
{
        return xstd::begin(bs);
}

template<int N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto cend(bit_set<N, Block> const& bs) noexcept
{
        return xstd::end(bs);
}

template<int N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto crbegin(bit_set<N, Block> const& bs) noexcept
{
        return xstd::rbegin(bs);
}

template<int N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto crend(bit_set<N, Block> const& bs) noexcept
{
        return xstd::rend(bs);
}

template<int N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto size(bit_set<N, Block> const& bs) noexcept
{
        return bs.size();
}

template<int N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto ssize(bit_set<N, Block> const& bs) noexcept
{
        using R = std::common_type_t<std::ptrdiff_t, std::make_signed_t<decltype(bs.size())>>;
        return static_cast<R>(bs.size());
}

template<int N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto empty(bit_set<N, Block> const& bs) noexcept
{
        return bs.empty();
}

}       // namespace xstd

#endif  // include guard
