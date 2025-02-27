#ifndef XSTD_BIT_SET_HPP
#define XSTD_BIT_SET_HPP

//          Copyright Rein Halbersma 2014-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/base_array.hpp>  // base_array
#include <xstd/proxy.hpp>       // bit_iterator, bit_reference
#include <cassert>              // assert
#include <compare>              // strong_ordering
#include <concepts>             // constructible_from, unsigned_integral
#include <cstddef>              // ptrdiff_t, size_t
#include <functional>           // less
#include <initializer_list>     // initializer_list
#include <iterator>             // make_reverse_iterator, reverse_iterator, 
                                // input_iterator, sentinel_for
#include <limits>               // digits
#include <ranges>               // begin, empty, end, from_range_t, next, rbegin, rend, subrange
                                // input_range
#include <type_traits>          // conditional_t
#include <utility>              // forward, move, pair

namespace xstd {

// class template bit_set
template<std::size_t N, std::unsigned_integral Block>
class bit_set;

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr bool operator== (bit_set<N, Block> const& x, bit_set<N, Block> const& y) noexcept;

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto operator<=>(bit_set<N, Block> const& x, bit_set<N, Block> const& y) noexcept -> std::strong_ordering;

template<std::size_t N, std::unsigned_integral Block = std::size_t>
using bit_set_aligned = bit_set<aligned_size(N, std::numeric_limits<Block>::digits), Block>;

template<std::size_t N, std::unsigned_integral Block = std::size_t>
class bit_set
{
        base_array<N, Block> m_bits;

public:
        using key_type               = std::size_t;
        using key_compare            = std::less<key_type>;
        using value_type             = key_type;
        using value_compare          = key_compare;
        using block_type             = Block;
        using pointer                = void;
        using const_pointer          = pointer;
        using reference              = bit::bidirectional::const_reference<bit_set>;
        using const_reference        = reference;
        using size_type              = std::size_t;
        using difference_type        = std::ptrdiff_t;
        using iterator               = bit::bidirectional::const_iterator<bit_set>;
        using const_iterator         = iterator;
        using reverse_iterator       = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        // construct/copy/destroy
        [[nodiscard]] constexpr bit_set() noexcept = default;

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
                m_bits.reset();
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
                return { &self, self.find_last() };
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
        [[nodiscard]] constexpr bool empty() const noexcept { return m_bits.none(); }
        [[nodiscard]] constexpr bool full()  const noexcept { return m_bits.all();  }

        [[nodiscard]]        constexpr size_type size()     const noexcept { return m_bits.count();           }
        [[nodiscard]] static constexpr size_type max_size()       noexcept { return decltype(m_bits)::size(); }

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
                for (auto x : std::ranges::subrange(first, last)) {
                        m_bits.set(x);
                }
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
                m_bits.set();
        }

        constexpr iterator erase(const_iterator position) noexcept
        {
                assert(position != end());
                m_bits.reset(*position++);
                return position;
        }

        constexpr size_type erase(key_type const& x) noexcept
        {
                return m_bits.erase(x);
        }

        constexpr iterator erase(const_iterator first, const_iterator last) noexcept
        {
                for (auto x : std::ranges::subrange(first, last)) {
                        m_bits.reset(x);
                }
                return last;
        }

        constexpr void swap(bit_set& other) noexcept(noexcept(this->m_bits.swap(other.m_bits)))
        {
                this->m_bits.swap(other.m_bits);
        }

        constexpr void clear() noexcept
        {
                m_bits.reset();
        }

        constexpr void complement(value_type x) noexcept
        {
                m_bits.flip(x);
        }

        constexpr void complement() noexcept
        {
                m_bits.flip();
        }

        constexpr bit_set& operator&=(bit_set const& other) noexcept { this->m_bits &= other.m_bits; return *this; }
        constexpr bit_set& operator|=(bit_set const& other) noexcept { this->m_bits |= other.m_bits; return *this; }
        constexpr bit_set& operator^=(bit_set const& other) noexcept { this->m_bits ^= other.m_bits; return *this; }
        constexpr bit_set& operator-=(bit_set const& other) noexcept { this->m_bits -= other.m_bits; return *this; }

        constexpr bit_set& operator<<=(std::size_t n) noexcept { m_bits <<= n; return *this; }
        constexpr bit_set& operator>>=(std::size_t n) noexcept { m_bits >>= n; return *this; }

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
                return m_bits.test(x);
        }

        [[nodiscard]] constexpr bool contains(key_type const& x) const noexcept
        {
                return m_bits.test(x);
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

        [[nodiscard]] constexpr bool is_subset_of       (bit_set const& other) const noexcept { return this->m_bits.is_subset_of       (other.m_bits); }
        [[nodiscard]] constexpr bool is_proper_subset_of(bit_set const& other) const noexcept { return this->m_bits.is_proper_subset_of(other.m_bits); }
        [[nodiscard]] constexpr bool intersects         (bit_set const& other) const noexcept { return this->m_bits.intersects         (other.m_bits); }

private:
        constexpr auto do_insert(value_type x) noexcept
                -> std::pair<iterator, bool>
        {
                return { { this, x }, m_bits.insert(x) };
        }

        constexpr auto do_insert(const_iterator, value_type x) noexcept
                -> iterator
        {
                m_bits.set(x);
                return { this, x };
        }

        [[nodiscard]] constexpr size_type find_front() const noexcept { return m_bits.find_front(); }
        [[nodiscard]] constexpr size_type find_back()  const noexcept { return m_bits.find_back();  }

        [[nodiscard]] constexpr size_type find_first() const noexcept { return m_bits.find_first(); }
        [[nodiscard]] constexpr size_type find_last()  const noexcept { return m_bits.find_last();  }

        [[nodiscard]] constexpr size_type find_next(size_type n) const noexcept { return m_bits.find_next(n); }
        [[nodiscard]] constexpr size_type find_prev(size_type n) const noexcept { return m_bits.find_prev(n); }

        [[nodiscard]] friend constexpr size_type find_next(bit_set const& c, size_type n) noexcept { return c.find_next(n); }
        [[nodiscard]] friend constexpr size_type find_prev(bit_set const& c, size_type n) noexcept { return c.find_prev(n); }
};

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr bool operator==(bit_set<N, Block> const& x [[maybe_unused]], bit_set<N, Block> const& y [[maybe_unused]]) noexcept
{
        return x.m_bits == y.m_bits;
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto operator<=>(bit_set<N, Block> const& x [[maybe_unused]], bit_set<N, Block> const& y [[maybe_unused]]) noexcept
        -> std::strong_ordering
{
        return x.m_bits <=> y.m_bits;
}

template<std::size_t N, std::unsigned_integral Block>
constexpr void swap(bit_set<N, Block>& x, bit_set<N, Block>& y) noexcept(noexcept(x.swap(y)))
{
        x.swap(y);
}

// erasure for bit_set
template<std::size_t N, std::unsigned_integral Block, class Predicate>
constexpr auto erase_if(bit_set<N, Block>& c, Predicate pred)
        -> typename bit_set<N, Block>::size_type
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
template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto begin(bit_set<N, Block>& c) noexcept
{
        return c.begin();
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto begin(bit_set<N, Block> const& c) noexcept
{
        return c.begin();
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto end(bit_set<N, Block>& c) noexcept
{
        return c.end();
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto end(bit_set<N, Block> const& c) noexcept
{
        return c.end();
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto cbegin(bit_set<N, Block> const& c) noexcept
{
        return xstd::begin(c);
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto cend(bit_set<N, Block> const& c) noexcept
{
        return xstd::end(c);
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto rbegin(bit_set<N, Block>& c) noexcept
{
        return c.rbegin();
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto rbegin(bit_set<N, Block> const& c) noexcept
{
        return c.rbegin();
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto rend(bit_set<N, Block>& c) noexcept
{
        return c.rend();
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto rend(bit_set<N, Block> const& c) noexcept
{
        return c.rend();
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto crbegin(bit_set<N, Block> const& c) noexcept
{
        return xstd::rbegin(c);
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto crend(bit_set<N, Block> const& c) noexcept
{
        return xstd::rend(c);
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr bit_set<N, Block> operator~(bit_set<N, Block> const& lhs) noexcept
{
        auto nrv = lhs; nrv.complement(); return nrv;
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr bit_set<N, Block> operator&(bit_set<N, Block> const& lhs, bit_set<N, Block> const& rhs) noexcept
{
        auto nrv = lhs; nrv &= rhs; return nrv;
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr bit_set<N, Block> operator|(bit_set<N, Block> const& lhs, bit_set<N, Block> const& rhs) noexcept
{
        auto nrv = lhs; nrv |= rhs; return nrv;
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr bit_set<N, Block> operator^(bit_set<N, Block> const& lhs, bit_set<N, Block> const& rhs) noexcept
{
        auto nrv = lhs; nrv ^= rhs; return nrv;
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr bit_set<N, Block> operator-(bit_set<N, Block> const& lhs, bit_set<N, Block> const& rhs) noexcept
{
        auto nrv = lhs; nrv -= rhs; return nrv;
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr bit_set<N, Block> operator<<(bit_set<N, Block> const& lhs, std::size_t n) noexcept
{
        auto nrv = lhs; nrv <<= n; return nrv;
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr bit_set<N, Block> operator>>(bit_set<N, Block> const& lhs, std::size_t n) noexcept
{
        auto nrv = lhs; nrv >>= n; return nrv;
}

}       // namespace xstd

#endif  // include guard
