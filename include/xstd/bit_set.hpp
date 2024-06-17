#ifndef XSTD_BIT_SET_HPP
#define XSTD_BIT_SET_HPP

//          Copyright Rein Halbersma 2014-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>            // shift_left, shift_right
                                // all_of, any_of, fill_n, find_if, fold_left, for_each, max, none_of
#include <array>                // array
#include <bit>                  // countl_zero, countr_zero, popcount
#include <compare>              // strong_ordering
#include <concepts>             // constructible_from, integral, same_as, unsigned_integral
#include <cstddef>              // ptrdiff_t, size_t
#include <cstring>              // memcmp, memset
#include <functional>           // identity, less
#include <initializer_list>     // initializer_list
#include <iterator>             // bidirectional_iterator_tag, iter_value_t, make_reverse_iterator, reverse_iterator
                                // input_iterator, sentinel_for
#include <limits>               // digits
#include <ranges>               // begin, distance, empty, end, from_range_t, next, rbegin, rend
                                // drop, pairwise_transform, reverse, take, transform, zip, zip_transform
                                // input_range
#include <tuple>                // tie
#include <type_traits>          // common_type_t, conditional_t, is_class_v, make_signed_t, type_identity_t
#include <utility>              // declval, forward, move, pair

namespace xstd {

// class template bit_set
template<std::integral Key, std::size_t N, std::unsigned_integral Block>
class bit_set;

template<std::integral Key, std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr bool operator== (const bit_set<Key, N, Block>& x, const bit_set<Key, N, Block>& y) noexcept;

template<std::integral Key, std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto operator<=>(const bit_set<Key, N, Block>& x, const bit_set<Key, N, Block>& y) noexcept -> std::strong_ordering;

[[nodiscard]] constexpr std::size_t aligned_size(std::size_t size, std::size_t alignment) noexcept
{
        return ((size - 1 + alignment) / alignment) * alignment;
}

template<std::integral Key, std::size_t N, std::unsigned_integral Block = std::size_t>
using bit_set_aligned = bit_set<Key, aligned_size(N, std::numeric_limits<Block>::digits), Block>;

template<std::integral Key, std::size_t N, std::unsigned_integral Block = std::size_t>
class bit_set
{
        class proxy_const_iterator;
        class proxy_const_reference;

public:
        using key_type               = Key;
        using key_compare            = std::less<Key>;
        using value_type             = Key;
        using value_compare          = std::less<Key>;
        using block_type             = Block;
        using pointer                = proxy_const_iterator;
        using const_pointer          = proxy_const_iterator;
        using reference              = proxy_const_reference;
        using const_reference        = proxy_const_reference;
        using size_type              = std::size_t;
        using difference_type        = std::ptrdiff_t;
        using iterator               = proxy_const_iterator;
        using const_iterator         = proxy_const_iterator;
        using reverse_iterator       = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
        static constexpr auto block_size = static_cast<size_type>(std::numeric_limits<Block>::digits);
        static constexpr auto num_bits   = aligned_size(N, block_size);
        static constexpr auto num_blocks = std::ranges::max(num_bits / block_size, 1uz);

        std::array<block_type, num_blocks> m_bits{};            // zero-initialization

public:
        // construct/copy/destroy
        [[nodiscard]] constexpr bit_set() noexcept = default;   // zero-initialization

        template<std::input_iterator I>
        [[nodiscard]] constexpr bit_set(I first, I last) noexcept
                requires std::constructible_from<value_type, decltype(*first)>
        {
                do_insert(first, last);
        }

        template<std::ranges::input_range R>
        [[nodiscard]] constexpr bit_set(std::from_range_t, R&& rg) noexcept
                requires std::constructible_from<value_type, decltype(*std::ranges::begin(rg))>
        {
                do_insert(std::ranges::begin(rg), std::ranges::end(rg));
        }

        [[nodiscard]] constexpr bit_set(const bit_set& x) noexcept = default;
        [[nodiscard]] constexpr bit_set(bit_set&& x) noexcept = default;

        [[nodiscard]] constexpr bit_set(std::initializer_list<value_type> il) noexcept
        {
                do_insert(il.begin(), il.end());
        }

        constexpr ~bit_set() noexcept = default;
        constexpr bit_set& operator=(const bit_set& x) noexcept = default;
        constexpr bit_set& operator=(bit_set&& x) noexcept = default;

        constexpr bit_set& operator=(std::initializer_list<value_type> il) noexcept
        {
                clear();
                do_insert(il.begin(), il.end());
                return *this;
        }

        friend constexpr bool operator==  <>(const bit_set&, const bit_set&) noexcept;
        friend constexpr auto operator<=> <>(const bit_set&, const bit_set&) noexcept -> std::strong_ordering;

        // iterators
        [[nodiscard]] constexpr auto begin(this auto&& self) noexcept
                -> std::conditional_t<
                        std::is_const_v<std::remove_reference_t<decltype(self)>>,
                        const_iterator,
                        iterator
                >
        {
                return { &self, self.find_first() };
        }

        [[nodiscard]] constexpr auto end(this auto&& self) noexcept
                -> std::conditional_t<
                        std::is_const_v<std::remove_reference_t<decltype(self)>>,
                        const_iterator,
                        iterator
                >
        {
                return { &self, N };
        }

        [[nodiscard]] constexpr auto rbegin(this auto&& self) noexcept
                -> std::conditional_t<
                        std::is_const_v<std::remove_reference_t<decltype(self)>>,
                        const_reverse_iterator,
                        reverse_iterator
                >
        {
                return std::make_reverse_iterator(self.end());
        }

        [[nodiscard]] constexpr auto rend(this auto&& self) noexcept
                -> std::conditional_t<
                        std::is_const_v<std::remove_reference_t<decltype(self)>>,
                        const_reverse_iterator,
                        reverse_iterator
                >
        {
                return std::make_reverse_iterator(self.begin());
        }

        [[nodiscard]] constexpr const_iterator         cbegin()  const noexcept { return begin();  }
        [[nodiscard]] constexpr const_iterator         cend()    const noexcept { return end();    }
        [[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept { return rbegin(); }
        [[nodiscard]] constexpr const_reverse_iterator crend()   const noexcept { return rend();   }

        // capacity
        [[nodiscard]] constexpr bool empty() const noexcept
        {
                if constexpr (N == 0) {
                        return true;
                } else if constexpr (num_blocks == 1) {
                        return !m_bits[0];
                } else if constexpr (num_blocks == 2) {
                        return !(m_bits[0] || m_bits[1]);
                } else if constexpr (num_blocks >= 3) {
                        return std::ranges::none_of(m_bits, std::identity());
                }
        }

        [[nodiscard]] constexpr bool full() const noexcept
        {
                if constexpr (has_unused_bits) {
                        if constexpr (num_blocks == 1) {
                                return m_bits[0] == used_bits;
                        } else if (num_blocks == 2) {
                                return m_bits[0] == ones && m_bits[1] == used_bits;
                        } else if (num_blocks >= 3) {
                                return std::ranges::all_of(m_bits | std::views::take(last_block), [](auto block) static {
                                        return block == ones;
                                }) && m_bits[last_block] == used_bits;
                        }
                } else {
                        if constexpr (N == 0) {
                                return true;
                        } else if constexpr (num_blocks == 1) {
                                return m_bits[0] == ones;
                        } else if constexpr (num_blocks == 2) {
                                return m_bits[0] == ones && m_bits[1] == ones;
                        } else if constexpr (num_blocks >= 3) {
                                return std::ranges::all_of(m_bits, [](auto block) static {
                                        return block == ones;
                                });
                        }
                }
        }

        [[nodiscard]] constexpr size_type size() const noexcept
        {
                if constexpr (N == 0) {
                        return 0uz;
                } else if constexpr (num_blocks == 1) {
                        return static_cast<size_type>(std::popcount(m_bits[0]));
                } else if constexpr (num_blocks == 2) {
                        return static_cast<size_type>(std::popcount(m_bits[0]) + std::popcount(m_bits[1]));
                } else if constexpr (num_blocks >= 3) {
                        return std::ranges::fold_left(
                                m_bits | std::views::transform([](auto block) static {
                                        return static_cast<size_type>(std::popcount(block));
                                }), 0uz, std::plus<>()
                        );
                }
        }

        [[nodiscard]] static constexpr size_type max_size() noexcept
        {
                return N;
        }

        // element access
        [[nodiscard]] constexpr const_reference front() const noexcept { return { *this, find_front() }; }
        [[nodiscard]] constexpr const_reference back()  const noexcept { return { *this, find_back()  }; }

        // modifiers
        template<class... Args>
        constexpr std::pair<iterator, bool> emplace(Args&&... args) noexcept
                requires (sizeof...(args) == 1)
        {
                return do_insert(value_type(std::forward<Args>(args)...));
        }

        template<class... Args>
        constexpr iterator emplace_hint(const_iterator position, Args&&... args) noexcept
                requires (sizeof...(args) == 1)
        {
                return do_insert(position, value_type(std::forward<Args>(args)...));
        }

        constexpr void add(value_type x) noexcept
        {
                auto&& [ block, mask ] = block_mask(x);
                insert(block, mask);
        }

        constexpr std::pair<iterator, bool> insert(const value_type& x) noexcept
        {
                return do_insert(x);
        }

        constexpr std::pair<iterator, bool> insert(value_type&& x) noexcept
        {
                return do_insert(std::move(x));
        }

        constexpr iterator insert(const_iterator position, const value_type& x) noexcept
        {
                return do_insert(position, x);
        }

        constexpr iterator insert(const_iterator position, value_type&& x) noexcept
        {
                return do_insert(position, std::move(x));
        }

        template<std::input_iterator I>
        constexpr void insert(I first, I last) noexcept
                requires std::constructible_from<value_type, decltype(*first)>
        {
                do_insert(first, last);
        }

        template<std::ranges::input_range R>
        constexpr void insert_range(R&& rg) noexcept
                requires std::constructible_from<value_type, decltype(*std::ranges::begin(rg))>
        {
                do_insert(std::ranges::begin(rg), std::ranges::end(rg));
        }

        constexpr void insert(std::initializer_list<value_type> ilist) noexcept
        {
                do_insert(ilist.begin(), ilist.end());
        }

        constexpr void fill() noexcept
        {
                if constexpr (has_unused_bits) {
                        std::ranges::fill_n(m_bits.begin(), last_block, ones);
                        m_bits[last_block] = used_bits;
                } else if constexpr (N > 0) {
                        m_bits.fill(ones);
                }
                [[assume(full())]];
        }

        constexpr void pop(key_type x) noexcept
        {
                auto&& [ block, mask ] = block_mask(x);
                erase(block, mask);
        }

        constexpr iterator erase(const_iterator position) noexcept
        {
                [[assume(position != end())]];
                pop(*position++);
                return position;
        }

        constexpr size_type erase(const key_type& x) noexcept
        {
                auto&& [ block, mask ] = block_mask(x);
                auto const erased = contains(block, mask);
                erase(block, mask);
                return erased;
        }

        constexpr iterator erase(const_iterator first, const_iterator last) noexcept
        {
                [[assume(N > 0 || first == last)]];
                std::ranges::for_each(first, last, [this](auto x) {
                        pop(x);
                });
                return last;
        }

        constexpr void swap(bit_set& other [[maybe_unused]]) noexcept(std::is_nothrow_swappable_v<block_type>)
        {
                if constexpr (N > 0) {
                        this->m_bits.swap(other.m_bits);
                }
        }

        constexpr void clear() noexcept
        {
                if constexpr (N > 0) {
                        m_bits.fill(zero);
                }
                [[assume(empty())]];
        }

        constexpr void complement(value_type x) noexcept
        {
                auto&& [ block, mask ] = block_mask(x);
                complement(block, mask);
        }

        constexpr void complement() noexcept
        {
                if constexpr (N > 0 && num_blocks == 1) {
                        m_bits[0] = static_cast<block_type>(~m_bits[0]);
                } else if constexpr (num_blocks == 2) {
                        m_bits[0] = static_cast<block_type>(~m_bits[0]);
                        m_bits[1] = static_cast<block_type>(~m_bits[1]);
                } else if constexpr (num_blocks >= 3) {
                        for (auto& block : m_bits) {
                                block = static_cast<block_type>(~block);
                        }
                }
                clear_unused_bits();
        }

        constexpr bit_set& operator&=(const bit_set& other [[maybe_unused]]) noexcept
        {
                if constexpr (N > 0 && num_blocks == 1) {
                        this->m_bits[0] &= other.m_bits[0];
                } else if constexpr (num_blocks == 2) {
                        this->m_bits[0] &= other.m_bits[0];
                        this->m_bits[1] &= other.m_bits[1];
                } else if constexpr (num_blocks >= 3) {
                        for (auto&& [ lhs, rhs ] : std::views::zip(this->m_bits, other.m_bits)) {
                                lhs &= rhs;
                        }
                }
                return *this;
        }

        constexpr bit_set& operator|=(const bit_set& other [[maybe_unused]]) noexcept
        {
                if constexpr (N > 0 && num_blocks == 1) {
                        this->m_bits[0] |= other.m_bits[0];
                } else if constexpr (num_blocks == 2) {
                        this->m_bits[0] |= other.m_bits[0];
                        this->m_bits[1] |= other.m_bits[1];
                } else if constexpr (num_blocks >= 3) {
                        for (auto&& [ lhs, rhs ] : std::views::zip(this->m_bits, other.m_bits)) {
                                lhs |= rhs;
                        }
                }
                return *this;
        }

        constexpr bit_set& operator^=(const bit_set& other [[maybe_unused]]) noexcept
        {
                if constexpr (N > 0 && num_blocks == 1) {
                        this->m_bits[0] ^= other.m_bits[0];
                } else if constexpr (num_blocks == 2) {
                        this->m_bits[0] ^= other.m_bits[0];
                        this->m_bits[1] ^= other.m_bits[1];
                } else if constexpr (num_blocks >= 3) {
                        for (auto&& [ lhs, rhs ] : std::views::zip(this->m_bits, other.m_bits)) {
                                lhs ^= rhs;
                        }
                }
                return *this;
        }

        constexpr bit_set& operator-=(const bit_set& other [[maybe_unused]]) noexcept
        {
                if constexpr (N > 0 && num_blocks == 1) {
                        this->m_bits[0] &= static_cast<block_type>(~other.m_bits[0]);
                } else if constexpr (num_blocks == 2) {
                        this->m_bits[0] &= static_cast<block_type>(~other.m_bits[0]);
                        this->m_bits[1] &= static_cast<block_type>(~other.m_bits[1]);
                } else if constexpr (num_blocks >= 3) {
                        for (auto&& [ lhs, rhs ] : std::views::zip(this->m_bits, other.m_bits)) {
                                lhs &= static_cast<block_type>(~rhs);
                        }
                }
                return *this;
        }

        constexpr bit_set& operator<<=(size_type n [[maybe_unused]]) noexcept
        {
                [[assume(is_valid(n))]];
                if constexpr (num_blocks == 1) {
                        m_bits[0] >>= n;
                } else if constexpr (num_blocks >= 2) {
                        auto const [ n_blocks, R_shift ] = div_mod(n, block_size);
                        if (R_shift == 0) {
                                std::shift_right(m_bits.begin(), m_bits.end(), static_cast<difference_type>(n_blocks));
                        } else {
                                auto const L_shift = block_size - R_shift;
                                for (auto&& [ lhs, rhs ] : std::views::zip(
                                        m_bits | std::views::reverse,
                                        m_bits | std::views::reverse | std::views::drop(n_blocks) | std::views::pairwise_transform(
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
                                m_bits[n_blocks] = static_cast<block_type>(m_bits[0] >> R_shift);
                        }
                        std::ranges::fill_n(
                                std::ranges::next(m_bits.rbegin(), static_cast<difference_type>(num_blocks - n_blocks)),
                                static_cast<difference_type>(n_blocks),
                                zero
                        );
                }
                clear_unused_bits();
                return *this;
        }

        constexpr bit_set& operator>>=(size_type n [[maybe_unused]]) noexcept
        {
                [[assume(is_valid(n))]];
                if constexpr (num_blocks == 1) {
                        m_bits[0] <<= n;
                } else if constexpr (num_blocks >= 2) {
                        auto const [ n_blocks, L_shift ] = div_mod(n, block_size);
                        if (L_shift == 0) {
                                std::shift_left(m_bits.begin(), m_bits.end(), static_cast<difference_type>(n_blocks));
                        } else {
                                auto const R_shift = block_size - L_shift;
                                for (auto&& [ lhs, rhs ] : std::views::zip(
                                        m_bits,
                                        m_bits | std::views::drop(n_blocks) | std::views::pairwise_transform(
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
                                m_bits[last_block - n_blocks] = static_cast<block_type>(m_bits[last_block] << L_shift);
                        }
                        std::ranges::fill_n(
                                std::ranges::next(m_bits.begin(), static_cast<difference_type>(num_blocks - n_blocks)),
                                static_cast<difference_type>(n_blocks),
                                zero
                        );
                }
                return *this;
        }

        // observers
        [[nodiscard]] constexpr key_compare key_comp() const noexcept
        {
                return key_compare();
        }

        [[nodiscard]] constexpr value_compare value_comp() const noexcept
        {
                return value_compare();
        }

        // set operations
        [[nodiscard]] constexpr auto find(this auto&& self, const key_type& x) noexcept
                -> std::conditional_t<
                        std::is_const_v<std::remove_reference_t<decltype(self)>>,
                        const_iterator,
                        iterator
                >
        {
                if (self.contains(x)) {
                        return { &self, x };
                } else {
                        return self.end();
                }
        }

        [[nodiscard]] constexpr size_type count(const key_type& x) const noexcept
        {
                return contains(x);
        }

        [[nodiscard]] constexpr bool contains(const key_type& x) const noexcept
        {
                auto&& [ block, mask ] = block_mask(x);
                return contains(block, mask);
        }

        [[nodiscard]] constexpr auto lower_bound(this auto&& self, const key_type& x) noexcept
                -> std::conditional_t<
                        std::is_const_v<std::remove_reference_t<decltype(self)>>,
                        const_iterator,
                        iterator
                >
        {
                return { &self, self.find_next(x) };
        }

        [[nodiscard]] constexpr auto upper_bound(this auto&& self, const key_type& x) noexcept
                -> std::conditional_t<
                        std::is_const_v<std::remove_reference_t<decltype(self)>>,
                        const_iterator,
                        iterator
                >
        {
                return { &self, self.find_next(x + 1) };
        }

        [[nodiscard]] constexpr auto equal_range(this auto&& self, const key_type& x) noexcept
                -> std::conditional_t<
                        std::is_const_v<std::remove_reference_t<decltype(self)>>,
                        std::pair<const_iterator, const_iterator>,
                        std::pair<iterator, iterator>
                >
        {
                return { self.lower_bound(x), self.upper_bound(x) };
        }

        [[nodiscard]] constexpr bool is_subset_of(const bit_set& other [[maybe_unused]]) const noexcept
        {
                if constexpr (N == 0) {
                        return true;
                } else if constexpr (num_blocks == 1) {
                        return !(this->m_bits[0] & ~other.m_bits[0]);
                } else if constexpr (num_blocks == 2) {
                        return !(
                                (this->m_bits[0] & ~other.m_bits[0]) ||
                                (this->m_bits[1] & ~other.m_bits[1])
                        );
                } else if constexpr (num_blocks >= 3) {
                        return std::ranges::none_of(
                                std::views::zip_transform(
                                        [](auto lhs, auto rhs) static { return lhs & ~rhs; },
                                        this->m_bits, other.m_bits
                                ),
                                std::identity()
                        );
                }
        }

        [[nodiscard]] constexpr bool is_proper_subset_of(const bit_set& other [[maybe_unused]]) const noexcept
        {
                if constexpr (N == 0) {
                        return false;
                } else if constexpr (num_blocks == 1) {
                        return !(
                                (this->m_bits[0] & ~other.m_bits[0]) ||
                                (this->m_bits[0] == other.m_bits[0])
                        );
                } else if constexpr (num_blocks == 2) {
                        return !(
                                (this->m_bits[0] & ~other.m_bits[0]) ||
                                (this->m_bits[1] & ~other.m_bits[1]) ||
                                (this->m_bits[0] == other.m_bits[0] && this->m_bits[1] == other.m_bits[1])
                        );
                } else if constexpr (num_blocks >= 3) {
                        auto i = 0uz;
                        for (/* init-statement before loop */; i < num_blocks; ++i) {
                                if (this->m_bits[i] & ~other.m_bits[i]) {
                                        return false;
                                }
                                if (this->m_bits[i] != other.m_bits[i]) {
                                        break;
                                }
                        }
                        return (i == num_blocks) ? false : std::ranges::none_of(
                                std::views::zip_transform(
                                        [](auto lhs, auto rhs) static { return lhs & ~rhs; },
                                        this->m_bits, other.m_bits
                                ) | std::views::drop(i),
                                std::identity()
                        );
                }
        }

        [[nodiscard]] constexpr bool intersects(const bit_set& other [[maybe_unused]]) const noexcept
        {
                if constexpr (N == 0) {
                        return false;
                } else if constexpr (num_blocks == 1) {
                        return this->m_bits[0] & other.m_bits[0];
                } else if constexpr (num_blocks == 2) {
                        return
                                (this->m_bits[0] & other.m_bits[0]) ||
                                (this->m_bits[1] & other.m_bits[1])
                        ;
                } else if constexpr (num_blocks >= 3) {
                        return std::ranges::any_of(
                                std::views::zip_transform(
                                        [](auto lhs, auto rhs) static { return lhs & rhs; },
                                        this->m_bits, other.m_bits
                                ),
                                std::identity()
                        );
                }
        }

private:
        static constexpr auto zero            = static_cast<block_type>( 0);
        static constexpr auto ones            = static_cast<block_type>(-1);
        static constexpr auto has_unused_bits = num_bits > N;
        static constexpr auto num_unused_bits = num_bits - N;
        static constexpr auto used_bits       = static_cast<block_type>(ones << num_unused_bits);
        static constexpr auto last_block      = num_blocks - 1;
        static constexpr auto last_bit        = num_bits - 1;
        static constexpr auto left_bit        = block_size - 1;
        static constexpr auto left_mask       = static_cast<block_type>(static_cast<block_type>(1) << left_bit);

        [[nodiscard]] static constexpr bool is_valid(size_type n) noexcept
        {
                if constexpr (N == 0) {
                        return false;
                } else {
                        return n < N;
                }
        }

        [[nodiscard]] static constexpr auto div_mod(size_type numer, size_type denom) noexcept
                -> std::pair<size_type, size_type>
        {
                return { numer / denom, numer % denom };
        }

        [[nodiscard]] static constexpr auto index_offset(size_type n) noexcept
                -> std::pair<size_type, size_type>
        {
                if constexpr (num_blocks == 1) {
                        return { 0uz, n };
                } else if constexpr (num_blocks >= 2) {
                        return div_mod(n, block_size);
                }
        }

        [[nodiscard]] constexpr auto block_mask(this auto&& self, size_type n) noexcept
                -> std::conditional_t<
                        std::is_const_v<std::remove_reference_t<decltype(self)>>,
                        std::pair<block_type, block_type>,
                        std::pair<block_type&, block_type>
                >
        {
                [[assume(is_valid(n))]];
                auto const [ index, offset ] = index_offset(n);
                return { self.m_bits[index], static_cast<block_type>(left_mask >> offset) };
        }

        [[nodiscard]] constexpr auto block_mask(this auto&& self, value_type n) noexcept
                requires (!std::same_as<value_type, size_type>)
        {
                return self.block_mask(static_cast<size_type>(n));
        }

        static constexpr void insert(block_type& block, block_type mask) noexcept
        {
                block |= mask;
                [[assume(contains(block, mask))]];
        }

        static constexpr void erase(block_type& block, block_type mask) noexcept
        {
                block &= static_cast<block_type>(~mask);
                [[assume(!contains(block, mask))]];
        }

        static constexpr void complement(block_type& block, block_type mask) noexcept
        {
                block ^= mask;
        }

        [[nodiscard]] static constexpr bool contains(block_type block, block_type mask) noexcept
        {
                return block & mask;
        }

        constexpr auto do_insert(value_type x) noexcept
                -> std::pair<iterator, bool>
        {
                auto&& [ block, mask ] = block_mask(x);
                auto const inserted = !contains(block, mask);
                insert(block, mask);
                return { { this, x }, inserted };
        }

        constexpr auto do_insert(const_iterator, value_type x) noexcept
                -> iterator
        {
                add(x);
                return { this, x };
        }

        template<std::input_iterator I, std::sentinel_for<I> S>
        constexpr void do_insert(I first, S last) noexcept
                requires std::constructible_from<value_type, decltype(*first)>
        {
                [[assume(N > 0 || first == last)]];
                std::ranges::for_each(first, last, [this](auto x) {
                        add(x);
                });
        }

        constexpr void clear_unused_bits() noexcept
        {
                if constexpr (has_unused_bits) {
                        m_bits[last_block] &= used_bits;
                }
        }

        [[nodiscard]] constexpr size_type find_front() const noexcept
        {
                [[assume(!empty())]];
                if constexpr (num_blocks == 1) {
                        return static_cast<size_type>(std::countl_zero(m_bits[0]));
                } else if constexpr (num_blocks == 2) {
                        return m_bits[0] ? static_cast<size_type>(std::countl_zero(m_bits[0])) : block_size + static_cast<size_type>(std::countl_zero(m_bits[1]));
                } else if constexpr (num_blocks >= 3) {
                        auto const front = std::ranges::find_if(m_bits, std::identity());
                        [[assume(front != std::ranges::end(m_bits))]];
                        return static_cast<size_type>(std::ranges::distance(std::ranges::begin(m_bits), front)) * block_size + static_cast<size_type>(std::countl_zero(*front));
                }
        }

        [[nodiscard]] constexpr size_type find_back() const noexcept
        {
                [[assume(!empty())]];
                if constexpr (num_blocks == 1) {
                        return last_bit - static_cast<size_type>(std::countr_zero(m_bits[0]));
                } else if constexpr (num_blocks == 2) {
                        return m_bits[1] ? last_bit - static_cast<size_type>(std::countr_zero(m_bits[1])) : left_bit - static_cast<size_type>(std::countr_zero(m_bits[0]));
                } else if constexpr (num_blocks >= 3) {
                        auto const back = std::ranges::find_if(m_bits | std::views::reverse, std::identity());
                        [[assume(back != std::ranges::rend(m_bits))]];
                        return last_bit - static_cast<size_type>(std::ranges::distance(std::ranges::rbegin(m_bits), back)) * block_size - static_cast<size_type>(std::countr_zero(*back));
                }
        }

        [[nodiscard]] constexpr size_type find_first() const noexcept
        {
                if constexpr (N > 0 && num_blocks == 1) {
                        if (m_bits[0]) {
                                return static_cast<size_type>(std::countl_zero(m_bits[0]));
                        }
                } else if constexpr (num_blocks == 2) {
                        if (m_bits[0]) {
                                return static_cast<size_type>(std::countl_zero(m_bits[0]));
                        } else if (m_bits[1]) {
                                return block_size + static_cast<size_type>(std::countl_zero(m_bits[1]));
                        }
                } else if constexpr (num_blocks >= 3) {
                        if (
                                auto const first = std::ranges::find_if(m_bits, std::identity());
                                first != std::ranges::end(m_bits)
                        ) {
                                return static_cast<size_type>(std::ranges::distance(std::ranges::begin(m_bits), first)) * block_size + static_cast<size_type>(std::countl_zero(*first));
                        }
                }
                return N;
        }

        [[nodiscard]] constexpr size_type find_next(size_type n) const noexcept
        {
                if (n == N) {
                        return N;
                }
                if constexpr (num_blocks == 1) {
                        if (auto const block = static_cast<block_type>(m_bits[0] << n); block) {
                                return n + static_cast<size_type>(std::countl_zero(block));
                        }
                } else if constexpr (num_blocks >= 2) {
                        auto [ index, offset ] = index_offset(n);
                        if (offset) {
                                if (auto const block = static_cast<block_type>(m_bits[index] << offset); block) {
                                        return n + static_cast<size_type>(std::countl_zero(block));
                                }
                                ++index;
                                n += block_size - offset;
                        }
                        auto const rg = m_bits | std::views::drop(index);
                        if (
                                auto const next = std::ranges::find_if(rg, std::identity());
                                next != std::ranges::end(rg)
                        ) {
                                return n + static_cast<size_type>(std::ranges::distance(std::ranges::begin(rg), next)) * block_size + static_cast<size_type>(std::countl_zero(*next));
                        }
                }
                return N;
        }

        [[nodiscard]] constexpr size_type find_next(value_type n) const noexcept
                requires (!std::same_as<value_type, size_type>)
        {
                return find_next(static_cast<size_type>(n));
        }

        [[nodiscard]] constexpr size_type find_prev(size_type n) const noexcept
        {
                [[assume(!empty())]];
                if constexpr (num_blocks == 1) {
                        return n - static_cast<size_type>(std::countr_zero(static_cast<block_type>(m_bits[0] >> (left_bit - n))));
                } else if constexpr (num_blocks >= 2) {
                        auto [ index, offset ] = index_offset(n);
                        if (auto const reverse_offset = left_bit - offset; reverse_offset) {
                                if (auto const block = static_cast<block_type>(m_bits[index] >> reverse_offset); block) {
                                        return n - static_cast<size_type>(std::countr_zero(block));
                                }
                                --index;
                                n -= block_size - reverse_offset;
                        }
                        auto const rg = m_bits | std::views::reverse | std::views::drop(last_block - index);
                        auto const prev = std::ranges::find_if(rg, std::identity());
                        [[assume(prev != std::ranges::end(rg))]];
                        return n - static_cast<size_type>(std::ranges::distance(std::ranges::begin(rg), prev)) * block_size - static_cast<size_type>(std::countr_zero(*prev));
                }
        }

        class proxy_const_iterator
        {
        public:
                using iterator_category = std::bidirectional_iterator_tag;
                using value_type        = bit_set::value_type;
                using difference_type   = bit_set::difference_type;
                using pointer           = void;
                using reference         = bit_set::reference;

        private:
                using pimpl_type = const bit_set*;
                using size_type = bit_set::size_type;
                pimpl_type m_ptr;
                size_type m_val;

        public:
                [[nodiscard]] constexpr proxy_const_iterator() noexcept = default;

                [[nodiscard]] constexpr proxy_const_iterator(pimpl_type ptr, size_type val) noexcept
                :
                        m_ptr(ptr),
                        m_val(val)
                {
                        [[assume(is_valid())]];
                }

                [[nodiscard]] constexpr proxy_const_iterator(pimpl_type ptr, value_type val) noexcept
                        requires (!std::same_as<value_type, size_type>)
                :
                        proxy_const_iterator(ptr, static_cast<size_type>(val))
                {}

                [[nodiscard]] friend constexpr bool operator==(proxy_const_iterator lhs [[maybe_unused]], proxy_const_iterator rhs [[maybe_unused]]) noexcept
                {
                        [[assume(is_comparable(lhs, rhs))]];
                        if constexpr (N == 0) {
                                return true;
                        } else {
                                return lhs.m_val == rhs.m_val;
                        }
                }

                [[nodiscard]] constexpr auto operator*() const noexcept
                        -> proxy_const_reference
                {
                        [[assume(is_dereferenceable())]];
                        return { *m_ptr, m_val };
                }

                constexpr proxy_const_iterator& operator++() noexcept
                {
                        [[assume(is_incrementable())]];
                        m_val = m_ptr->find_next(m_val + 1);
                        [[assume(is_decrementable())]];
                        return *this;
                }

                constexpr proxy_const_iterator operator++(int) noexcept
                {
                        auto nrv = *this; ++*this; return nrv;
                }

                constexpr proxy_const_iterator& operator--() noexcept
                {
                        [[assume(is_decrementable())]];
                        m_val = m_ptr->find_prev(m_val - 1);
                        [[assume(is_incrementable())]];
                        return *this;
                }

                constexpr proxy_const_iterator operator--(int) noexcept
                {
                        auto nrv = *this; --*this; return nrv;
                }

        private:
                [[nodiscard]] constexpr bool is_valid() const noexcept
                {
                        return m_ptr != nullptr && m_val <= N;
                }

                [[nodiscard]] static constexpr bool is_comparable(iterator lhs, iterator rhs) noexcept
                {
                        return lhs.m_ptr == rhs.m_ptr;
                }

                [[nodiscard]] constexpr bool is_dereferenceable() const noexcept
                {
                        return m_ptr != nullptr && bit_set::is_valid(m_val);
                }

                [[nodiscard]] constexpr bool is_incrementable() const noexcept
                {
                        return m_ptr != nullptr && bit_set::is_valid(m_val);
                }

                [[nodiscard]] constexpr bool is_decrementable() const noexcept
                {
                        return m_ptr != nullptr && bit_set::is_valid(m_val - 1);
                }
        };

        class proxy_const_reference
        {
                using rimpl_type = const bit_set&;
                using  size_type = bit_set::size_type;
                using value_type = bit_set::value_type;
                rimpl_type m_ref;
                const size_type m_val;

        public:
                [[nodiscard]] constexpr proxy_const_reference() noexcept = delete;

                [[nodiscard]] constexpr proxy_const_reference(rimpl_type ref, size_type val) noexcept
                :
                        m_ref(ref),
                        m_val(val)
                {
                        [[assume(is_valid())]];
                }

                [[nodiscard]] constexpr proxy_const_reference(const proxy_const_reference&) noexcept = default;
                [[nodiscard]] constexpr proxy_const_reference(proxy_const_reference&&) noexcept = default;

                constexpr ~proxy_const_reference() noexcept = default;
                constexpr proxy_const_reference& operator=(const proxy_const_reference&) noexcept = delete;
                constexpr proxy_const_reference& operator=(proxy_const_reference&&) noexcept = delete;

                [[nodiscard]] friend constexpr bool operator==(proxy_const_reference lhs, proxy_const_reference rhs) noexcept
                {
                        return lhs.m_val == rhs.m_val;
                }

                [[nodiscard]] constexpr auto operator&() const noexcept
                        -> proxy_const_iterator
                {
                        return { &m_ref, m_val };
                }

                [[nodiscard]] constexpr explicit(false) operator value_type() const noexcept
                {
                        if constexpr (std::same_as<size_type, value_type>) {
                                return m_val;
                        } else {
                                return static_cast<value_type>(m_val);
                        }
                }

                template<class T>
                [[nodiscard]] constexpr explicit(false) operator T() const noexcept(noexcept(T(std::declval<value_type>())))
                        requires std::is_class_v<T> && std::constructible_from<T, value_type>
                {
                        if constexpr (std::same_as<size_type, value_type>) {
                                return m_val;
                        } else {
                                return static_cast<value_type>(m_val);
                        }
                }

        private:
                [[nodiscard]] constexpr bool is_valid() const noexcept
                {
                        return bit_set::is_valid(m_val);
                }
        };

        [[nodiscard]] friend constexpr auto format_as(proxy_const_reference ref) noexcept
                -> std::iter_value_t<proxy_const_iterator>
        {
                return ref;
        }
};

template<std::integral Key, std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr bool operator==(const bit_set<Key, N, Block>& x [[maybe_unused]], const bit_set<Key, N, Block>& y [[maybe_unused]]) noexcept
{
        if constexpr (N == 0) {
                return true;
        } else {
                return x.m_bits == y.m_bits;
        }
}

template<std::integral Key, std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto operator<=>(const bit_set<Key, N, Block>& x [[maybe_unused]], const bit_set<Key, N, Block>& y [[maybe_unused]]) noexcept
        -> std::strong_ordering
{
        if constexpr (N == 0) {
                return std::strong_ordering::equal;
        } else {
                return y.m_bits <=> x.m_bits;
        }
}

template<std::integral Key, std::size_t N, std::unsigned_integral Block>
constexpr void swap(bit_set<Key, N, Block>& x, bit_set<Key, N, Block>& y) noexcept(noexcept(x.swap(y)))
{
        x.swap(y);
}

// erasure for bit_set
template<std::integral Key, std::size_t N, std::unsigned_integral Block, class Predicate>
constexpr auto erase_if(bit_set<Key, N, Block>& c, Predicate pred)
        -> typename bit_set<Key, N, Block>::size_type
{
        auto original_size = c.size();
        for (auto i = c.begin(), last = c.end(); i != last;) {
                if (pred(*i)) {
                        i = c.erase(i);
                } else {
                        ++i;
                }
        }
        return original_size - c.size();
}

// range access
template<std::integral Key, std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto begin(bit_set<Key, N, Block>& c) noexcept
{
        return c.begin();
}

template<std::integral Key, std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto begin(const bit_set<Key, N, Block>& c) noexcept
{
        return c.begin();
}

template<std::integral Key, std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto end(bit_set<Key, N, Block>& c) noexcept
{
        return c.end();
}

template<std::integral Key, std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto end(const bit_set<Key, N, Block>& c) noexcept
{
        return c.end();
}

template<std::integral Key, std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto cbegin(const bit_set<Key, N, Block>& c) noexcept
{
        return xstd::begin(c);
}

template<std::integral Key, std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto cend(const bit_set<Key, N, Block>& c) noexcept
{
        return xstd::end(c);
}

template<std::integral Key, std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto rbegin(bit_set<Key, N, Block>& c) noexcept
{
        return c.rbegin();
}

template<std::integral Key, std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto rbegin(const bit_set<Key, N, Block>& c) noexcept
{
        return c.rbegin();
}

template<std::integral Key, std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto rend(bit_set<Key, N, Block>& c) noexcept
{
        return c.rend();
}

template<std::integral Key, std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto rend(const bit_set<Key, N, Block>& c) noexcept
{
        return c.rend();
}

template<std::integral Key, std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto crbegin(const bit_set<Key, N, Block>& c) noexcept
{
        return xstd::rbegin(c);
}

template<std::integral Key, std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto crend(const bit_set<Key, N, Block>& c) noexcept
{
        return xstd::rend(c);
}

template<std::integral Key, std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr bit_set<Key, N, Block> operator~(const bit_set<Key, N, Block>& lhs) noexcept
{
        auto nrv = lhs; nrv.complement(); return nrv;
}

template<std::integral Key, std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr bit_set<Key, N, Block> operator&(const bit_set<Key, N, Block>& lhs, const bit_set<Key, N, Block>& rhs) noexcept
{
        auto nrv = lhs; nrv &= rhs; return nrv;
}

template<std::integral Key, std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr bit_set<Key, N, Block> operator|(const bit_set<Key, N, Block>& lhs, const bit_set<Key, N, Block>& rhs) noexcept
{
        auto nrv = lhs; nrv |= rhs; return nrv;
}

template<std::integral Key, std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr bit_set<Key, N, Block> operator^(const bit_set<Key, N, Block>& lhs, const bit_set<Key, N, Block>& rhs) noexcept
{
        auto nrv = lhs; nrv ^= rhs; return nrv;
}

template<std::integral Key, std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr bit_set<Key, N, Block> operator-(const bit_set<Key, N, Block>& lhs, const bit_set<Key, N, Block>& rhs) noexcept
{
        auto nrv = lhs; nrv -= rhs; return nrv;
}

template<std::integral Key, std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr bit_set<Key, N, Block> operator<<(const bit_set<Key, N, Block>& lhs, std::size_t n) noexcept
{
        auto nrv = lhs; nrv <<= n; return nrv;
}

template<std::integral Key, std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr bit_set<Key, N, Block> operator>>(const bit_set<Key, N, Block>& lhs, std::size_t n) noexcept
{
        auto nrv = lhs; nrv >>= n; return nrv;
}

}       // namespace xstd

#endif  // include guard
