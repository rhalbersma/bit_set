#ifndef XSTD_BIT_ARRAY_HPP
#define XSTD_BIT_ARRAY_HPP

//          Copyright Rein Halbersma 2014-2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/bit/array.hpp>   // array
#include <xstd/proxy.hpp>       // const_iterator, const_reference
#include <xstd/utility.hpp>     // aligned_size
#include <cassert>              // assert
#include <compare>              // strong_ordering
#include <concepts>             // constructible_from, unsigned_integral
#include <cstddef>              // ptrdiff_t, size_t
#include <functional>           // less
#include <initializer_list>     // initializer_list
#include <iterator>             // make_reverse_iterator, reverse_iterator, 
                                // input_iterator, sentinel_for
#include <limits>               // digits
#include <ranges>               // begin, empty, end, from_range_t, next, rbegin, rend
                                // input_range
#include <type_traits>          // conditional_t
#include <utility>              // forward, move, pair

namespace xstd {

// class template bit_array
template<std::size_t N, std::unsigned_integral Block>
class bit_array;

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr bool operator== (const bit_array<N, Block>& x, const bit_array<N, Block>& y) noexcept;

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto operator<=>(const bit_array<N, Block>& x, const bit_array<N, Block>& y) noexcept -> std::strong_ordering;

template<std::size_t N, std::unsigned_integral Block = std::size_t>
using bit_array_aligned = bit_array<aligned_size(N, std::numeric_limits<Block>::digits), Block>;

template<std::size_t N, std::unsigned_integral Block = std::size_t>
struct bit_array
{
        bit::array<N, Block> m_bits;

        using value_type             = bool;
        using block_type             = Block;
        using pointer                = void;
        using const_pointer          = pointer;
        using reference              = proxy::bidirectional::const_reference<bit_array>;
        using const_reference        = reference;
        using size_type              = std::size_t;
        using difference_type        = std::ptrdiff_t;
        using iterator               = proxy::bidirectional::const_iterator<bit_array>;
        using const_iterator         = iterator;
        using reverse_iterator       = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        // construct/copy/destroy
        [[nodiscard]] constexpr bit_array() noexcept = default;

        template<std::input_iterator I, std::sentinel_for<I> S>
        [[nodiscard]] constexpr bit_array(I first, S last) noexcept
                requires std::constructible_from<value_type, decltype(*first)>
        {
                insert(first, last);
        }

        template<std::ranges::input_range R>
        [[nodiscard]] constexpr bit_array(std::from_range_t, R&& rg) noexcept
                requires std::constructible_from<value_type, decltype(*std::ranges::begin(rg))>
        {
                insert(std::ranges::begin(rg), std::ranges::end(rg));
        }

        [[nodiscard]] constexpr bit_array(std::initializer_list<value_type> il) noexcept
        {
                insert(il.begin(), il.end());
        }

        constexpr bit_array& operator=(std::initializer_list<value_type> il) noexcept
        {
                m_bits.reset();
                insert(il.begin(), il.end());
                return *this;
        }

        friend constexpr bool operator==  <>(const bit_array&, const bit_array&) noexcept;
        friend constexpr auto operator<=> <>(const bit_array&, const bit_array&) noexcept -> std::strong_ordering;

        // iterators
        [[nodiscard]] constexpr auto begin(this auto&& self) noexcept
                -> std::conditional_t<std::is_const_v<std::remove_reference_t<decltype(self)>>, const_iterator, iterator>
        {
                return bidirectional::begin(self);
        }

        [[nodiscard]] constexpr auto end(this auto&& self) noexcept
                -> std::conditional_t<std::is_const_v<std::remove_reference_t<decltype(self)>>, const_iterator, iterator>
        {
                return bidirectional::end(self);
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
        [[nodiscard]] constexpr const_reference front() const noexcept { return { *this, m_bits.find_front() }; }
        [[nodiscard]] constexpr const_reference back()  const noexcept { return { *this, m_bits.find_back()  }; }

        constexpr void fill(bool const& u) noexcept
        {
                if (u) {
                        m_bits.set();
                } else {
                        m_bits.reset();
                }
        }

        constexpr void swap(bit_array& other) noexcept(noexcept(this->m_bits.swap(other.m_bits)))
        {
                this->m_bits.swap(other.m_bits);
        }

        constexpr bit_array& operator&=(const bit_array& other) noexcept { this->m_bits &= other.m_bits; return *this; }
        constexpr bit_array& operator|=(const bit_array& other) noexcept { this->m_bits |= other.m_bits; return *this; }
        constexpr bit_array& operator^=(const bit_array& other) noexcept { this->m_bits ^= other.m_bits; return *this; }
        constexpr bit_array& operator-=(const bit_array& other) noexcept { this->m_bits -= other.m_bits; return *this; }

        constexpr bit_array& operator<<=(std::size_t n) noexcept { m_bits <<= n; return *this; }
        constexpr bit_array& operator>>=(std::size_t n) noexcept { m_bits >>= n; return *this; }        
};

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr bool operator==(const bit_array<N, Block>& x, const bit_array<N, Block>& y) noexcept
{
        return x.m_bits == y.m_bits;
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto operator<=>(const bit_array<N, Block>& x, const bit_array<N, Block>& y) noexcept
        -> std::strong_ordering
{
        return x.m_bits <=> y.m_bits;
}

template<std::size_t N, std::unsigned_integral Block>
constexpr void swap(bit_array<N, Block>& x, bit_array<N, Block>& y) noexcept(noexcept(x.swap(y)))
{
        x.swap(y);
}

// range access
template<std::size_t N, std::unsigned_integral Block> [[nodiscard]] constexpr auto begin  (      bit_array<N, Block>& c) noexcept { return c.begin(); }
template<std::size_t N, std::unsigned_integral Block> [[nodiscard]] constexpr auto begin  (const bit_array<N, Block>& c) noexcept { return c.begin(); }
template<std::size_t N, std::unsigned_integral Block> [[nodiscard]] constexpr auto end    (      bit_array<N, Block>& c) noexcept { return c.end(); }
template<std::size_t N, std::unsigned_integral Block> [[nodiscard]] constexpr auto end    (const bit_array<N, Block>& c) noexcept { return c.end(); }
template<std::size_t N, std::unsigned_integral Block> [[nodiscard]] constexpr auto cbegin (const bit_array<N, Block>& c) noexcept { return xstd::begin(c); }
template<std::size_t N, std::unsigned_integral Block> [[nodiscard]] constexpr auto cend   (const bit_array<N, Block>& c) noexcept { return xstd::end(c);   }
template<std::size_t N, std::unsigned_integral Block> [[nodiscard]] constexpr auto rbegin (      bit_array<N, Block>& c) noexcept { return c.rbegin(); }
template<std::size_t N, std::unsigned_integral Block> [[nodiscard]] constexpr auto rbegin (const bit_array<N, Block>& c) noexcept { return c.rbegin(); }
template<std::size_t N, std::unsigned_integral Block> [[nodiscard]] constexpr auto rend   (      bit_array<N, Block>& c) noexcept { return c.rend(); }
template<std::size_t N, std::unsigned_integral Block> [[nodiscard]] constexpr auto rend   (const bit_array<N, Block>& c) noexcept { return c.rend(); }
template<std::size_t N, std::unsigned_integral Block> [[nodiscard]] constexpr auto crbegin(const bit_array<N, Block>& c) noexcept { return xstd::rbegin(c); }
template<std::size_t N, std::unsigned_integral Block> [[nodiscard]] constexpr auto crend  (const bit_array<N, Block>& c) noexcept { return xstd::rend(c);   }

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr bit_array<N, Block> operator~(const bit_array<N, Block>& lhs) noexcept
{
        auto nrv = lhs; nrv.complement(); return nrv;
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr bit_array<N, Block> operator&(const bit_array<N, Block>& lhs, const bit_array<N, Block>& rhs) noexcept
{
        auto nrv = lhs; nrv &= rhs; return nrv;
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr bit_array<N, Block> operator|(const bit_array<N, Block>& lhs, const bit_array<N, Block>& rhs) noexcept
{
        auto nrv = lhs; nrv |= rhs; return nrv;
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr bit_array<N, Block> operator^(const bit_array<N, Block>& lhs, const bit_array<N, Block>& rhs) noexcept
{
        auto nrv = lhs; nrv ^= rhs; return nrv;
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr bit_array<N, Block> operator-(const bit_array<N, Block>& lhs, const bit_array<N, Block>& rhs) noexcept
{
        auto nrv = lhs; nrv -= rhs; return nrv;
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr bit_array<N, Block> operator<<(const bit_array<N, Block>& lhs, std::size_t n) noexcept
{
        auto nrv = lhs; nrv <<= n; return nrv;
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr bit_array<N, Block> operator>>(const bit_array<N, Block>& lhs, std::size_t n) noexcept
{
        auto nrv = lhs; nrv >>= n; return nrv;
}

}       // namespace xstd

#endif  // include guard
