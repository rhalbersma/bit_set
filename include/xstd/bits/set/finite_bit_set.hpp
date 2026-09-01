//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_BITS_SET_FINITE_BIT_SET_HPP
#define XSTD_BITS_SET_FINITE_BIT_SET_HPP

// Header <set> synopsis                                   [associative.set.syn]

#include <compare>              // strong_ordering
#include <initializer_list>     // initializer_list

#include <xstd/ints/memory.hpp> // align_up
#include <concepts>             // unsigned_integral
#include <cstddef>              // size_t
#include <limits>               // digits

namespace xstd {

// 23.4.6, class template set
template<std::size_t N, std::unsigned_integral Block = std::size_t> 
class finite_bit_set;

template<std::size_t N, std::unsigned_integral Block> [[nodiscard]] constexpr bool operator== (const finite_bit_set<N, Block>& x, const finite_bit_set<N, Block>& y) noexcept;
template<std::size_t N, std::unsigned_integral Block> [[nodiscard]] constexpr auto operator<=>(const finite_bit_set<N, Block>& x, const finite_bit_set<N, Block>& y) noexcept -> std::strong_ordering;
template<std::size_t N, std::unsigned_integral Block>               constexpr void swap       (      finite_bit_set<N, Block>& x,       finite_bit_set<N, Block>& y) noexcept(noexcept(x.swap(y)));

// 23.4.6.3, erasure for set
template<std::size_t N, std::unsigned_integral Block, class Predicate>
constexpr finite_bit_set<N, Block>::size_type erase_if(finite_bit_set<N, Block>& c, Predicate pred);

namespace aligned {

template<std::size_t N, std::unsigned_integral Block = std::size_t>
using finite_bit_set = xstd::finite_bit_set<xstd::align_up(N, static_cast<std::size_t>(std::numeric_limits<Block>::digits)), Block>;

}       // namespace aligned
}       // namespace xstd

// The synopsis above and the implementation below each list the headers that
// section needs, mirroring the layout of the standard itself. The overlap
// between the two lists is deliberate: neither is complete without it.
// NOLINTBEGIN(readability-duplicate-include): deliberate, see above
#include <xstd/bits/bit/array.hpp>           // array
#include <xstd/bits/ranges.hpp>               // const_iterator, const_reference
#include <boost/hash2/fnv1a.hpp>        // fnv1a_64
#include <boost/hash2/hash_append.hpp>  // hash_append
#include <algorithm>                    // lexicographical_compare_three_way
#include <cassert>                      // assert
#include <compare>                      // strong_ordering
#include <concepts>                     // constructible_from, unsigned_integral
#include <cstddef>                      // ptrdiff_t, size_t
#include <functional>                   // less
#include <initializer_list>             // initializer_list
#include <iterator>                     // make_reverse_iterator, reverse_iterator, 
                                        // input_iterator, sentinel_for
#include <limits>                       // digits
#include <ranges>                       // begin, empty, end, from_range_t, next, rbegin, rend
                                        // input_range
#include <type_traits>                  // conditional_t
#include <utility>                      // forward, move, pair
// NOLINTEND(readability-duplicate-include)

// Class template set                                                      [set]
// Overview                                                       [set.overview]

namespace xstd {

template<std::size_t N, std::unsigned_integral Block>
class finite_bit_set
{
        bit::array<N, Block> m_bits{};

        [[nodiscard]] friend constexpr std::size_t find_first(const finite_bit_set& c)                noexcept { return c.m_bits.find_first(); }
        [[nodiscard]] friend constexpr std::size_t find_last (const finite_bit_set& c)                noexcept { return c.m_bits.find_last();  }
        [[nodiscard]] friend constexpr std::size_t find_next (const finite_bit_set& c, std::size_t n) noexcept { return c.m_bits.find_next(n); }
        [[nodiscard]] friend constexpr std::size_t find_prev (const finite_bit_set& c, std::size_t n) noexcept { return c.m_bits.find_prev(n); }

        template<class Provider, class Hash, class Flavor>
        friend constexpr void tag_invoke(boost::hash2::hash_append_tag const&, Provider const&, Hash& h, Flavor const& f, finite_bit_set const* v) noexcept
        {
                boost::hash2::hash_append(h, f, v->m_bits);
        }

public:
        // types
        using key_type               = std::size_t;
        using key_compare            = std::less<key_type>;
        using value_type             = key_type;
        using value_compare          = key_compare;
        using block_type             = Block;
        using pointer                = void;
        using const_pointer          = pointer;
        using reference              = xstd::ranges::set_reference<finite_bit_set>;
        using const_reference        = reference;
        using size_type              = std::size_t;
        using difference_type        = std::ptrdiff_t;
        using iterator               = xstd::ranges::set_iterator<finite_bit_set>;
        using const_iterator         = iterator;
        using reverse_iterator       = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        // 23.4.6.2, construct/copy/destroy
        [[nodiscard]] constexpr finite_bit_set() noexcept = default;

        template<std::input_iterator I, std::sentinel_for<I> S>
        [[nodiscard]] constexpr finite_bit_set(I first, S last) noexcept
                requires std::constructible_from<value_type, decltype(*first)>
        {
                insert(first, last);
        }

        template<std::ranges::input_range R>
        [[nodiscard]] constexpr finite_bit_set(std::from_range_t, R&& rg) noexcept
                requires std::constructible_from<value_type, decltype(*std::ranges::begin(rg))>
        {
                insert(std::ranges::begin(rg), std::ranges::end(rg));
        }

        [[nodiscard]] constexpr finite_bit_set(std::initializer_list<value_type> il) noexcept
        {
                insert(il.begin(), il.end());
        }

        constexpr finite_bit_set& operator=(std::initializer_list<value_type> il) noexcept
        {
                m_bits.reset();
                insert(il.begin(), il.end());
                return *this;
        }

        friend constexpr bool operator==  <>(const finite_bit_set&, const finite_bit_set&) noexcept;
        friend constexpr auto operator<=> <>(const finite_bit_set&, const finite_bit_set&) noexcept -> std::strong_ordering;

        // iterators
        [[nodiscard]] constexpr auto begin (this auto&& self) noexcept { return xstd::ranges::set_begin(self); }
        [[nodiscard]] constexpr auto end   (this auto&& self) noexcept { return xstd::ranges::set_end  (self); }
        [[nodiscard]] constexpr auto rbegin(this auto&& self) noexcept { return std::make_reverse_iterator(self.end()  ); }
        [[nodiscard]] constexpr auto rend  (this auto&& self) noexcept { return std::make_reverse_iterator(self.begin()); }

        [[nodiscard]] constexpr auto cbegin()  const noexcept { return begin();  }
        [[nodiscard]] constexpr auto cend()    const noexcept { return end();    }
        [[nodiscard]] constexpr auto crbegin() const noexcept { return rbegin(); }
        [[nodiscard]] constexpr auto crend()   const noexcept { return rend();   }

        // capacity
        [[nodiscard]] constexpr bool empty() const noexcept { return m_bits.none(); }
        [[nodiscard]] constexpr bool full()  const noexcept { return m_bits.all();  }

        [[nodiscard]]        constexpr size_type     size() const noexcept { return m_bits.count();           }
        [[nodiscard]] static constexpr size_type max_size()       noexcept { return decltype(m_bits)::size(); }

        // element access
        [[nodiscard]] constexpr reference front() const noexcept { return { *this, m_bits.find_front() }; }
        [[nodiscard]] constexpr reference back()  const noexcept { return { *this, m_bits.find_back()  }; }

        // 23.4.6.4, modifiers
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

        constexpr auto insert(                         const value_type&  x) noexcept -> std::pair<iterator, bool> { return do_insert(          x);            }
        constexpr auto insert(                               value_type&& x) noexcept -> std::pair<iterator, bool> { return do_insert(          std::move(x)); }
        constexpr auto insert(const_iterator position, const value_type&  x) noexcept ->           iterator        { return do_insert(position, x);            }
        constexpr auto insert(const_iterator position,       value_type&& x) noexcept ->           iterator        { return do_insert(position, std::move(x)); }

        template<std::input_iterator I, std::sentinel_for<I> S>
        constexpr void insert(I first, S last) noexcept
                requires std::constructible_from<value_type, decltype(*first)>
        {
                while (first != last) {
                        m_bits.set(*first++);
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

        constexpr size_type erase(const key_type& x) noexcept
        {
                return m_bits.erase(x);
        }

        constexpr iterator erase(const_iterator first, const_iterator last) noexcept
        {
                while (first != last) {
                        m_bits.reset(*first++);
                }
                return last;
        }

        constexpr void swap(finite_bit_set& other) noexcept(noexcept(this->m_bits.swap(other.m_bits)))
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

        constexpr finite_bit_set& operator&=(const finite_bit_set& other) noexcept { this->m_bits &= other.m_bits; return *this; }
        constexpr finite_bit_set& operator|=(const finite_bit_set& other) noexcept { this->m_bits |= other.m_bits; return *this; }
        constexpr finite_bit_set& operator^=(const finite_bit_set& other) noexcept { this->m_bits ^= other.m_bits; return *this; }
        constexpr finite_bit_set& operator-=(const finite_bit_set& other) noexcept { this->m_bits -= other.m_bits; return *this; }

        constexpr finite_bit_set& operator<<=(std::size_t n) noexcept { m_bits <<= n; return *this; }
        constexpr finite_bit_set& operator>>=(std::size_t n) noexcept { m_bits >>= n; return *this; }

        // observers
        [[nodiscard]] constexpr   key_compare   key_comp() const noexcept { return {}; }
        [[nodiscard]] constexpr value_compare value_comp() const noexcept { return {}; }

        // set operations
        [[nodiscard]] constexpr bool contains(const key_type& x) const noexcept              { return m_bits[x]; }
        [[nodiscard]] constexpr auto count   (const key_type& x) const noexcept -> size_type { return m_bits[x]; }

        [[nodiscard]] constexpr auto find(this auto&& self, const key_type& x) noexcept -> iterator
        {
                if (self.contains(x)) {
                        return { &self, x };
                }
                return self.end();
        }
        [[nodiscard]] constexpr auto lower_bound(this auto&& self, const key_type& x) noexcept -> iterator                      { return { &self, (x ? find_next(self, x - 1) : find_first(self)) }; }
        [[nodiscard]] constexpr auto upper_bound(this auto&& self, const key_type& x) noexcept -> iterator                      { return { &self, find_next(self, x) };                              }
        [[nodiscard]] constexpr auto equal_range(this auto&& self, const key_type& x) noexcept -> std::pair<iterator, iterator> { return { self.lower_bound(x), self.upper_bound(x) };               }

        [[nodiscard]] constexpr bool is_subset_of       (const finite_bit_set& other) const noexcept { return this->m_bits.is_subset_of       (other.m_bits); }
        [[nodiscard]] constexpr bool is_proper_subset_of(const finite_bit_set& other) const noexcept { return this->m_bits.is_proper_subset_of(other.m_bits); }
        [[nodiscard]] constexpr bool intersects         (const finite_bit_set& other) const noexcept { return this->m_bits.intersects         (other.m_bits); }

private:
        constexpr auto do_insert(                value_type x) noexcept -> std::pair<iterator, bool> {                return { { this, x }, m_bits.insert(x) }; }
        constexpr auto do_insert(const_iterator, value_type x) noexcept ->           iterator        { m_bits.set(x); return   { this, x };                     }
};

template<std::size_t N, std::unsigned_integral Block> [[nodiscard]] constexpr bool operator== (const finite_bit_set<N, Block>& x, const finite_bit_set<N, Block>& y) noexcept { return x.m_bits == y.m_bits; }

// bit::array is a pure storage vehicle with no <=> of its own (see its
// comments) - finite_bit_set's own ordering is std::set<int>-equivalent: the
// lexicographic order of its own ascending sequence of set-bit indices,
// exactly what std::set<int> would compute for the same elements.
template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto operator<=>(const finite_bit_set<N, Block>& x, const finite_bit_set<N, Block>& y) noexcept
        -> std::strong_ordering
{
        return std::lexicographical_compare_three_way(x.begin(), x.end(), y.begin(), y.end());
}
template<std::size_t N, std::unsigned_integral Block>               constexpr void swap       (      finite_bit_set<N, Block>& x,       finite_bit_set<N, Block>& y) noexcept(noexcept(x.swap(y)))    { x.swap(y);                    }

// 23.4.6.3 Erasure                                                [set.erasure]
template<std::size_t N, std::unsigned_integral Block, class Predicate>
constexpr finite_bit_set<N, Block>::size_type erase_if(finite_bit_set<N, Block>& c, Predicate pred)
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

// bitwise operators
template<std::size_t N, std::unsigned_integral Block> [[nodiscard]] constexpr finite_bit_set<N, Block> operator~(const finite_bit_set<N, Block>& lhs) noexcept { auto nrv = lhs; nrv.complement(); return nrv; }

template<std::size_t N, std::unsigned_integral Block> [[nodiscard]] constexpr finite_bit_set<N, Block> operator&(const finite_bit_set<N, Block>& lhs, const finite_bit_set<N, Block>& rhs) noexcept { auto nrv = lhs; nrv &= rhs; return nrv; }
template<std::size_t N, std::unsigned_integral Block> [[nodiscard]] constexpr finite_bit_set<N, Block> operator|(const finite_bit_set<N, Block>& lhs, const finite_bit_set<N, Block>& rhs) noexcept { auto nrv = lhs; nrv |= rhs; return nrv; }
template<std::size_t N, std::unsigned_integral Block> [[nodiscard]] constexpr finite_bit_set<N, Block> operator^(const finite_bit_set<N, Block>& lhs, const finite_bit_set<N, Block>& rhs) noexcept { auto nrv = lhs; nrv ^= rhs; return nrv; }
template<std::size_t N, std::unsigned_integral Block> [[nodiscard]] constexpr finite_bit_set<N, Block> operator-(const finite_bit_set<N, Block>& lhs, const finite_bit_set<N, Block>& rhs) noexcept { auto nrv = lhs; nrv -= rhs; return nrv; }

template<std::size_t N, std::unsigned_integral Block> [[nodiscard]] constexpr finite_bit_set<N, Block> operator<<(const finite_bit_set<N, Block>& lhs, std::size_t n) noexcept { auto nrv = lhs; nrv <<= n; return nrv; }
template<std::size_t N, std::unsigned_integral Block> [[nodiscard]] constexpr finite_bit_set<N, Block> operator>>(const finite_bit_set<N, Block>& lhs, std::size_t n) noexcept { auto nrv = lhs; nrv >>= n; return nrv; }

}       // namespace xstd

#endif // XSTD_BITS_SET_FINITE_BIT_SET_HPP
