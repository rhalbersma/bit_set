#ifndef XSTD_BIT_SET_HPP
#define XSTD_BIT_SET_HPP

//          Copyright Rein Halbersma 2014-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/proxy.hpp>       // bidirectional_bit_iterator, bidirectional_bit_reference
#include <algorithm>            // shift_left, shift_right
                                // all_of, any_of, fill_n, find_if, fold_left, for_each, max, none_of
#include <array>                // array
#include <bit>                  // countl_zero, countr_zero, popcount
#include <cassert>              // assert
#include <compare>              // strong_ordering
#include <concepts>             // constructible_from, integral, same_as, unsigned_integral
#include <cstddef>              // ptrdiff_t, size_t
#include <cstring>              // memcmp, memset
#include <functional>           // identity, less
#include <initializer_list>     // initializer_list
#include <iterator>             // bidirectional_iterator_tag, iter_value_t, make_reverse_iterator, reverse_iterator
                                // input_iterator, sentinel_for
#include <limits>               // digits
#include <ranges>               // begin, empty, end, from_range_t, next, rbegin, rend, subrange
                                // drop, pairwise_transform, reverse, take, transform, zip
                                // input_range
#include <type_traits>          // common_type_t, conditional_t, is_class_v, make_signed_t, type_identity_t
#include <utility>              // declval, forward, move, pair

namespace xstd {

// class template bit_set
template<std::integral Key, std::size_t N, std::unsigned_integral Block>
class bit_set;

template<std::integral Key, std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr bool operator== (bit_set<Key, N, Block> const& x, bit_set<Key, N, Block> const& y) noexcept;

template<std::integral Key, std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto operator<=>(bit_set<Key, N, Block> const& x, bit_set<Key, N, Block> const& y) noexcept -> std::strong_ordering;

[[nodiscard]] constexpr std::size_t aligned_size(std::size_t size, std::size_t alignment) noexcept
{
        return ((size - 1 + alignment) / alignment) * alignment;
}

namespace bit {

[[nodiscard]] constexpr std::size_t count(std::unsigned_integral auto block) noexcept
{
        return static_cast<std::size_t>(std::popcount(block));
}

[[nodiscard]] constexpr std::size_t firstl_one(std::unsigned_integral auto block) noexcept
{
        return static_cast<std::size_t>(std::countl_zero(block));
} 

[[nodiscard]] constexpr std::size_t firstr_one(std::unsigned_integral auto block) noexcept
{
        return static_cast<std::size_t>(std::countr_zero(block));
}   

template<std::unsigned_integral Block>
[[nodiscard]] constexpr bool is_subset_of(Block lhs, Block rhs) noexcept
{
        return !(lhs & ~rhs);
}  

template<std::unsigned_integral Block>
[[nodiscard]] constexpr bool intersects(Block lhs, Block rhs) noexcept
{
        return lhs & rhs;
}   

}       // namespace bit

template<std::integral Key, std::size_t N, std::unsigned_integral Block = std::size_t>
using bit_set_aligned = bit_set<Key, aligned_size(N, std::numeric_limits<Block>::digits), Block>;

template<std::integral Key, std::size_t N, std::unsigned_integral Block = std::size_t>
class bit_set
{
        static constexpr auto block_size = static_cast<std::size_t>(std::numeric_limits<Block>::digits);
        static constexpr auto num_bits   = aligned_size(N, block_size);
        static constexpr auto num_blocks = std::ranges::max(num_bits / block_size, std::size_t(1));

        std::array<Block, num_blocks> m_bits{};            // zero-initialization

public:
        using key_type               = Key;
        using key_compare            = std::less<Key>;
        using value_type             = Key;
        using value_compare          = std::less<Key>;
        using block_type             = Block;
        using pointer                = void;
        using const_pointer          = void;
        using reference              = bidirectional_bit_reference<bit_set>;
        using const_reference        = bidirectional_bit_reference<bit_set>;
        using size_type              = std::size_t;
        using difference_type        = std::ptrdiff_t;
        using iterator               = bidirectional_bit_iterator<bit_set>;
        using const_iterator         = bidirectional_bit_iterator<bit_set>;
        using reverse_iterator       = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        // construct/copy/destroy
        [[nodiscard]] constexpr bit_set() noexcept = default;   // zero-initialization

        template<std::input_iterator I, std::sentinel_for<I> S>
        [[nodiscard]] constexpr bit_set(I first, S last) noexcept
                requires std::constructible_from<value_type, decltype(*first)>
        {
                insert(first, last);
        }

        template<std::ranges::input_range R>
        [[nodiscard]] constexpr bit_set(std::from_range_t, R&& rg) noexcept
                requires std::constructible_from<value_type, decltype(*std::ranges::begin(rg))>
        {
                insert(std::ranges::begin(rg), std::ranges::end(rg));
        }

        [[nodiscard]] constexpr bit_set(std::initializer_list<value_type> il) noexcept
        {
                insert(il.begin(), il.end());
        }

        constexpr bit_set& operator=(std::initializer_list<value_type> il) noexcept
        {
                clear();
                insert(il.begin(), il.end());
                return *this;
        }

        friend constexpr bool operator==  <>(bit_set const&, bit_set const&) noexcept;
        friend constexpr auto operator<=> <>(bit_set const&, bit_set const&) noexcept -> std::strong_ordering;

        // iterators
        [[nodiscard]] constexpr auto begin(this auto&& self) noexcept
                -> std::conditional_t<std::is_const_v<std::remove_reference_t<decltype(self)>>, const_iterator, iterator>
        {
                return { &self, self.find_first() };
        }

        [[nodiscard]] constexpr auto end(this auto&& self) noexcept
                -> std::conditional_t<std::is_const_v<std::remove_reference_t<decltype(self)>>, const_iterator, iterator>
        {
                return { &self, N };
        }

        [[nodiscard]] constexpr auto rbegin(this auto&& self) noexcept
                -> std::conditional_t<std::is_const_v<std::remove_reference_t<decltype(self)>>, const_reverse_iterator, reverse_iterator>
        {
                return std::make_reverse_iterator(self.end());
        }

        [[nodiscard]] constexpr auto rend(this auto&& self) noexcept
                -> std::conditional_t<std::is_const_v<std::remove_reference_t<decltype(self)>>, const_reverse_iterator, reverse_iterator>
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
                                return std::ranges::all_of(m_bits | std::views::take(last_block), [](auto block) {
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
                                return std::ranges::all_of(m_bits, [](auto block) {
                                        return block == ones;
                                });
                        }
                }
        }

        [[nodiscard]] constexpr size_type size() const noexcept
        {
                if constexpr (N == 0) {
                        return std::size_t(0);
                } else if constexpr (num_blocks == 1) {
                        return bit::count(m_bits[0]);
                } else if constexpr (num_blocks == 2) {
                        return bit::count(m_bits[0]) + bit::count(m_bits[1]);
                } else if constexpr (num_blocks >= 3) {
                        return std::ranges::fold_left(
                                m_bits | std::views::transform([](auto block) {
                                        return bit::count(block);
                                }), std::size_t(0), std::plus<>()
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
                block |= mask;
                assert(contains(x));
        }

        constexpr std::pair<iterator, bool> insert(value_type const& x) noexcept
        {
                return do_insert(x);
        }

        constexpr std::pair<iterator, bool> insert(value_type&& x) noexcept
        {
                return do_insert(std::move(x));
        }

        constexpr iterator insert(const_iterator position, value_type const& x) noexcept
        {
                return do_insert(position, x);
        }

        constexpr iterator insert(const_iterator position, value_type&& x) noexcept
        {
                return do_insert(position, std::move(x));
        }

        template<std::input_iterator I, std::sentinel_for<I> S>
        constexpr void insert(I first, S last) noexcept
                requires std::constructible_from<value_type, decltype(*first)>
        {
                std::ranges::for_each(first, last, [this](auto x) {
                        add(x);
                });
        }

        template<std::ranges::input_range R>
        constexpr void insert_range(R&& rg) noexcept
                requires std::constructible_from<value_type, decltype(*std::ranges::begin(rg))>
        {
                insert(std::ranges::begin(rg), std::ranges::end(rg));
        }

        constexpr void insert(std::initializer_list<value_type> ilist) noexcept
        {
                insert(ilist.begin(), ilist.end());
        }

        constexpr void fill() noexcept
        {
                if constexpr (has_unused_bits) {
                        std::ranges::fill_n(m_bits.begin(), last_block, ones);
                        m_bits[last_block] = used_bits;
                } else if constexpr (N > 0) {
                        m_bits.fill(ones);
                }
                assert(full());
        }

        constexpr void pop(key_type x) noexcept
        {
                auto&& [ block, mask ] = block_mask(x);
                block &= static_cast<block_type>(~mask);
                assert(!contains(x));
        }

        constexpr iterator erase(const_iterator position) noexcept
        {
                assert(position != end());
                pop(*position++);
                return position;
        }

        constexpr size_type erase(key_type const& x) noexcept
        {
                auto&& [ block, mask ] = block_mask(x);
                auto const erased = bit::intersects(block, mask);
                block &= static_cast<block_type>(~mask);
                assert(!contains(x));
                return erased;
        }

        constexpr iterator erase(const_iterator first, const_iterator last) noexcept
        {
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
                assert(empty());
        }

        constexpr void complement(value_type x) noexcept
        {
                auto&& [ block, mask ] = block_mask(x);
                block ^= mask;
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
                erase_unused();
        }

        constexpr bit_set& operator&=(bit_set const& other [[maybe_unused]]) noexcept
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

        constexpr bit_set& operator|=(bit_set const& other [[maybe_unused]]) noexcept
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

        constexpr bit_set& operator^=(bit_set const& other [[maybe_unused]]) noexcept
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

        constexpr bit_set& operator-=(bit_set const& other [[maybe_unused]]) noexcept
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
                assert(is_valid(n));
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
                erase_unused();
                return *this;
        }

        constexpr bit_set& operator>>=(size_type n [[maybe_unused]]) noexcept
        {
                assert(is_valid(n));
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
        [[nodiscard]] constexpr auto find(this auto&& self, key_type const& x) noexcept
                -> std::conditional_t<std::is_const_v<std::remove_reference_t<decltype(self)>>, const_iterator, iterator>
        {
                if (self.contains(x)) {
                        return { &self, x };
                } else {
                        return self.end();
                }
        }

        [[nodiscard]] constexpr size_type count(key_type const& x) const noexcept
        {
                return contains(x);
        }

        [[nodiscard]] constexpr bool contains(key_type const& x) const noexcept
        {
                auto&& [ block, mask ] = block_mask(x);
                return bit::intersects(block, mask);
        }

        [[nodiscard]] constexpr auto lower_bound(this auto&& self, key_type const& x) noexcept
                -> std::conditional_t<std::is_const_v<std::remove_reference_t<decltype(self)>>, const_iterator, iterator>
        {
                return { &self, (x ? self.find_next(x - 1) : self.find_first()) };                
        }

        [[nodiscard]] constexpr auto upper_bound(this auto&& self, key_type const& x) noexcept
                -> std::conditional_t<std::is_const_v<std::remove_reference_t<decltype(self)>>, const_iterator, iterator>
        {
                return { &self, self.find_next(x) };                
        }

        [[nodiscard]] constexpr auto equal_range(this auto&& self, key_type const& x) noexcept
                -> std::conditional_t<
                        std::is_const_v<std::remove_reference_t<decltype(self)>>,
                        std::pair<const_iterator, const_iterator>,
                        std::pair<iterator, iterator>
                >
        {
                return { self.lower_bound(x), self.upper_bound(x) };
        }

        [[nodiscard]] constexpr bool is_subset_of(bit_set const& other [[maybe_unused]]) const noexcept
        {
                if constexpr (N == 0) {
                        return true;
                } else if constexpr (num_blocks == 1) {
                        return bit::is_subset_of(this->m_bits[0], other.m_bits[0]);
                } else if constexpr (num_blocks == 2) {
                        return
                                bit::is_subset_of(this->m_bits[0], other.m_bits[0]) && 
                                bit::is_subset_of(this->m_bits[1], other.m_bits[1])
                        ;
                } else if constexpr (num_blocks >= 3) {
                        return std::ranges::all_of(
                                std::views::zip(this->m_bits, other.m_bits), [](auto&& _) { auto&& [ lhs, rhs] = _; 
                                return bit::is_subset_of(lhs, rhs);                                                      
                        });
                }
        }

        [[nodiscard]] constexpr bool is_proper_subset_of(bit_set const& other [[maybe_unused]]) const noexcept
        {
                if constexpr (N == 0) {
                        return false;
                } else if constexpr (num_blocks == 1) {
                        return 
                                bit::is_subset_of(this->m_bits[0], other.m_bits[0]) &&
                                this->m_bits[0] != other.m_bits[0]
                        ;
                } else if constexpr (num_blocks == 2) {                         
                        if (!bit::is_subset_of(this->m_bits[0], other.m_bits[0])) {
                                return false;
                        } else if (this->m_bits[0] != other.m_bits[0]) {
                                return bit::is_subset_of(this->m_bits[1], other.m_bits[1]);
                        } else { 
                                return
                                        bit::is_subset_of(this->m_bits[1], other.m_bits[1]) &&
                                        this->m_bits[1] != other.m_bits[1]
                                ;
                        }                        
                } else if constexpr (num_blocks >= 3) {
                        auto i = std::size_t(0);
                        for (/* init-statement before loop */; i < num_blocks; ++i) {
                                if (!bit::is_subset_of(this->m_bits[i], other.m_bits[i])) {
                                        return false;
                                }
                                if (this->m_bits[i] != other.m_bits[i]) {
                                        break;
                                }
                        }
                        return (i == num_blocks) ? false : std::ranges::all_of(
                                std::views::zip(this->m_bits, other.m_bits) | std::views::drop(i), [](auto&& _) { auto&& [ lhs, rhs ] = _;
                                return bit::is_subset_of(lhs, rhs); 
                        });
                }
        }

        [[nodiscard]] constexpr bool intersects(bit_set const& other [[maybe_unused]]) const noexcept
        {
                if constexpr (N == 0) {
                        return false;
                } else if constexpr (num_blocks == 1) {
                        return bit::intersects(this->m_bits[0], other.m_bits[0]);
                } else if constexpr (num_blocks == 2) {
                        return
                                bit::intersects(this->m_bits[0], other.m_bits[0]) ||
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
        static constexpr auto left_bit        = block_size - 1;
        static constexpr auto zero            = static_cast<block_type>( 0);
        static constexpr auto ones            = static_cast<block_type>(-1);
        static constexpr auto used_bits       = static_cast<block_type>(ones << num_unused_bits);
        static constexpr auto unused_bits     = static_cast<block_type>(~used_bits);
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
                        return { std::size_t(0), n };
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
                assert(is_valid(n));
                auto const [ index, offset ] = index_offset(n);
                return { self.m_bits[index], static_cast<block_type>(left_mask >> offset) };
        }

        [[nodiscard]] constexpr auto block_mask(this auto&& self, value_type n) noexcept
                requires (!std::same_as<value_type, size_type>)
        {
                return self.block_mask(static_cast<size_type>(n));
        }

        constexpr void erase_unused() noexcept
        {
                if constexpr (has_unused_bits) {
                        m_bits[last_block] &= used_bits;
                        assert(!bit::intersects(m_bits[last_block], unused_bits));
                }
        }

        constexpr auto do_insert(value_type x) noexcept
                -> std::pair<iterator, bool>
        {
                auto&& [ block, mask ] = block_mask(x);
                auto const inserted = !bit::intersects(block, mask);
                block |= mask;
                assert(contains(x));
                return { { this, x }, inserted };
        }

        constexpr auto do_insert(const_iterator, value_type x) noexcept
                -> iterator
        {
                add(x);
                return { this, x };
        }

        [[nodiscard]] constexpr size_type find_front() const noexcept
        {
                assert(!empty());
                if constexpr (num_blocks == 1) {
                        return bit::firstl_one(m_bits[0]);
                } else if constexpr (num_blocks == 2) {
                        return m_bits[0] ? bit::firstl_one(m_bits[0]) : block_size + bit::firstl_one(m_bits[1]);
                } else if constexpr (num_blocks >= 3) {
                        auto const front = std::ranges::find_if(m_bits, std::identity());
                        assert(front != std::ranges::end(m_bits));
                        return std::ranges::subrange(std::ranges::begin(m_bits), front).size() * block_size + bit::firstl_one(*front);
                }
        }

        [[nodiscard]] constexpr size_type find_back() const noexcept
        {
                assert(!empty());
                if constexpr (num_blocks == 1) {
                        return last_bit - bit::firstr_one(m_bits[0]);
                } else if constexpr (num_blocks == 2) {
                        return m_bits[1] ? last_bit - bit::firstr_one(m_bits[1]) : left_bit - bit::firstr_one(m_bits[0]);
                } else if constexpr (num_blocks >= 3) {
                        auto const back = std::ranges::find_if(m_bits | std::views::reverse, std::identity());
                        assert(back != std::ranges::rend(m_bits));
                        return last_bit - std::ranges::subrange(std::ranges::rbegin(m_bits), back).size() * block_size - bit::firstr_one(*back);
                }
        }

        [[nodiscard]] constexpr size_type find_first() const noexcept
        {
                if constexpr (N > 0 && num_blocks == 1) {
                        if (m_bits[0]) {
                                return bit::firstl_one(m_bits[0]);
                        }
                } else if constexpr (num_blocks == 2) {
                        if (m_bits[0]) {
                                return bit::firstl_one(m_bits[0]);
                        } else if (m_bits[1]) {
                                return block_size + bit::firstl_one(m_bits[1]);
                        }
                } else if constexpr (num_blocks >= 3) {
                        if (
                                auto const first = std::ranges::find_if(m_bits, std::identity());
                                first != std::ranges::end(m_bits)
                        ) {
                                return std::ranges::subrange(std::ranges::begin(m_bits), first).size() * block_size + bit::firstl_one(*first);
                        }
                }
                return N;
        }

        [[nodiscard]] constexpr size_type find_next(size_type n) const noexcept
        {
                ++n;
                if (n == N) {
                        return N;
                }
                if constexpr (num_blocks == 1) {
                        if (auto const block = static_cast<block_type>(m_bits[0] << n); block) {
                                return n + bit::firstl_one(block);
                        }
                } else if constexpr (num_blocks >= 2) {
                        auto [ index, offset ] = index_offset(n);
                        if (offset) {
                                if (auto const block = static_cast<block_type>(m_bits[index] << offset); block) {
                                        return n + bit::firstl_one(block);
                                }
                                ++index;
                                n += block_size - offset;
                        }
                        auto const rg = m_bits | std::views::drop(index);
                        if (
                                auto const next = std::ranges::find_if(rg, std::identity());
                                next != std::ranges::end(rg)
                        ) {
                                return n + std::ranges::subrange(std::ranges::begin(rg), next).size() * block_size + bit::firstl_one(*next);
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
                assert(!empty());
                --n;
                if constexpr (num_blocks == 1) {
                        return n - bit::firstr_one(static_cast<block_type>(m_bits[0] >> (left_bit - n)));
                } else if constexpr (num_blocks >= 2) {
                        auto [ index, offset ] = index_offset(n);
                        if (auto const reverse_offset = left_bit - offset; reverse_offset) {
                                if (auto const block = static_cast<block_type>(m_bits[index] >> reverse_offset); block) {
                                        return n - bit::firstr_one(block);
                                }
                                --index;
                                n -= block_size - reverse_offset;
                        }
                        auto const rg = m_bits | std::views::reverse | std::views::drop(last_block - index);
                        auto const prev = std::ranges::find_if(rg, std::identity());
                        assert(prev != std::ranges::end(rg));
                        return n - std::ranges::subrange(std::ranges::begin(rg), prev).size() * block_size - bit::firstr_one(*prev);
                }
        }

        [[nodiscard]] friend constexpr size_type find_next(bit_set const& c, size_type n) noexcept { return c.find_next(n); }
        [[nodiscard]] friend constexpr size_type find_prev(bit_set const& c, size_type n) noexcept { return c.find_prev(n); }
};

template<std::integral Key, std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr bool operator==(bit_set<Key, N, Block> const& x [[maybe_unused]], bit_set<Key, N, Block> const& y [[maybe_unused]]) noexcept
{
        if constexpr (N == 0) {
                return true;
        } else {
                return x.m_bits == y.m_bits;
        }
}

template<std::integral Key, std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto operator<=>(bit_set<Key, N, Block> const& x [[maybe_unused]], bit_set<Key, N, Block> const& y [[maybe_unused]]) noexcept
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
        auto const original_size = c.size();
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
[[nodiscard]] constexpr auto begin(bit_set<Key, N, Block> const& c) noexcept
{
        return c.begin();
}

template<std::integral Key, std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto end(bit_set<Key, N, Block>& c) noexcept
{
        return c.end();
}

template<std::integral Key, std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto end(bit_set<Key, N, Block> const& c) noexcept
{
        return c.end();
}

template<std::integral Key, std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto cbegin(bit_set<Key, N, Block> const& c) noexcept
{
        return xstd::begin(c);
}

template<std::integral Key, std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto cend(bit_set<Key, N, Block> const& c) noexcept
{
        return xstd::end(c);
}

template<std::integral Key, std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto rbegin(bit_set<Key, N, Block>& c) noexcept
{
        return c.rbegin();
}

template<std::integral Key, std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto rbegin(bit_set<Key, N, Block> const& c) noexcept
{
        return c.rbegin();
}

template<std::integral Key, std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto rend(bit_set<Key, N, Block>& c) noexcept
{
        return c.rend();
}

template<std::integral Key, std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto rend(bit_set<Key, N, Block> const& c) noexcept
{
        return c.rend();
}

template<std::integral Key, std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto crbegin(bit_set<Key, N, Block> const& c) noexcept
{
        return xstd::rbegin(c);
}

template<std::integral Key, std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto crend(bit_set<Key, N, Block> const& c) noexcept
{
        return xstd::rend(c);
}

template<std::integral Key, std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr bit_set<Key, N, Block> operator~(bit_set<Key, N, Block> const& lhs) noexcept
{
        auto nrv = lhs; nrv.complement(); return nrv;
}

template<std::integral Key, std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr bit_set<Key, N, Block> operator&(bit_set<Key, N, Block> const& lhs, bit_set<Key, N, Block> const& rhs) noexcept
{
        auto nrv = lhs; nrv &= rhs; return nrv;
}

template<std::integral Key, std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr bit_set<Key, N, Block> operator|(bit_set<Key, N, Block> const& lhs, bit_set<Key, N, Block> const& rhs) noexcept
{
        auto nrv = lhs; nrv |= rhs; return nrv;
}

template<std::integral Key, std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr bit_set<Key, N, Block> operator^(bit_set<Key, N, Block> const& lhs, bit_set<Key, N, Block> const& rhs) noexcept
{
        auto nrv = lhs; nrv ^= rhs; return nrv;
}

template<std::integral Key, std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr bit_set<Key, N, Block> operator-(bit_set<Key, N, Block> const& lhs, bit_set<Key, N, Block> const& rhs) noexcept
{
        auto nrv = lhs; nrv -= rhs; return nrv;
}

template<std::integral Key, std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr bit_set<Key, N, Block> operator<<(bit_set<Key, N, Block> const& lhs, std::size_t n) noexcept
{
        auto nrv = lhs; nrv <<= n; return nrv;
}

template<std::integral Key, std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr bit_set<Key, N, Block> operator>>(bit_set<Key, N, Block> const& lhs, std::size_t n) noexcept
{
        auto nrv = lhs; nrv >>= n; return nrv;
}

}       // namespace xstd

#endif  // include guard
