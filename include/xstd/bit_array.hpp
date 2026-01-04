#ifndef XSTD_BIT_ARRAY_HPP
#define XSTD_BIT_ARRAY_HPP

//          Copyright Rein Halbersma 2014-2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Header <array> synopsis                                           [array.syn]

#include <compare>              // strong_ordering
#include <initializer_list>     // initializer_list

#include <xstd/utility.hpp>     // aligned_size
#include <concepts>             // unsigned_integral
#include <cstddef>              // size_t
#include <limits>               // digits

namespace xstd {

// 23.3.3, class template bit_array
template<std::size_t N, std::unsigned_integral Block> struct bit_array;

template<std::size_t N, std::unsigned_integral Block> [[nodiscard]] constexpr bool operator== (const bit_array<N, Block>& x, const bit_array<N, Block>& y) noexcept;
template<std::size_t N, std::unsigned_integral Block> [[nodiscard]] constexpr auto operator<=>(const bit_array<N, Block>& x, const bit_array<N, Block>& y) noexcept -> std::strong_ordering;

// 23.3.4, specialized algorithms
template<std::size_t N, std::unsigned_integral Block>               constexpr void swap       (      bit_array<N, Block>& x,       bit_array<N, Block>& y) noexcept(noexcept(x.swap(y)));

namespace aligned {

template<std::size_t N, std::unsigned_integral Block = std::size_t>
using bit_array = xstd::bit_array<xstd::aligned_size(N, std::numeric_limits<Block>::digits), Block>;

}       // namespace aligned
}       // namespace xstd

#include <xstd/bit/array.hpp>   // array
#include <xstd/proxy.hpp>       // begin, end, iterator, reference
#include <xstd/utility.hpp>     // aligned_size
#include <cassert>              // assert
#include <compare>              // strong_ordering
#include <concepts>             // unsigned_integral
#include <cstddef>              // ptrdiff_t, size_t
#include <format>               // format
#include <initializer_list>     // initializer_list
#include <iterator>             // make_reverse_iterator, reverse_iterator, 
#include <limits>               // digits
#include <source_location>      // source_location
#include <stdexcept>            // out_of_range
#include <type_traits>          // conditional_t
#include <utility>              // pair

// Class template array                                                  [array]
// Overview                                                     [array.overview]

namespace xstd {

template<std::size_t N, std::unsigned_integral Block = std::size_t>
struct bit_array
{
        bit::array<N, Block> m_bits;

        [[nodiscard]] friend constexpr std::size_t find_first(const bit_array&)                  noexcept { return 0uz;         }
        [[nodiscard]] friend constexpr std::size_t find_last (const bit_array&)                  noexcept { return N;           }
        [[nodiscard]] friend constexpr std::size_t find_at   (const bit_array& c, std::size_t n) noexcept { return c.m_bits[n]; }

        // types
        using value_type             = bool;
        using block_type             = Block;
        using pointer                = void;
        using const_pointer          = pointer;
        using reference              = proxy::random_access::reference<bit_array, false>;
        using const_reference        = proxy::random_access::reference<bit_array, true >;
        using size_type              = std::size_t;
        using difference_type        = std::ptrdiff_t;
        using iterator               = proxy::random_access::iterator<bit_array, false>;
        using const_iterator         = proxy::random_access::iterator<bit_array, true >;
        using reverse_iterator       = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        // no explicit construct/copy/destroy for aggregate type

        constexpr void fill(const value_type& u) noexcept
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

        // iterators
        [[nodiscard]] constexpr auto begin (this auto&& self) noexcept { return proxy::random_access::begin(self); }
        [[nodiscard]] constexpr auto end   (this auto&& self) noexcept { return proxy::random_access::end  (self); }
        [[nodiscard]] constexpr auto rbegin(this auto&& self) noexcept { return std::make_reverse_iterator(self.end()  ); }
        [[nodiscard]] constexpr auto rend  (this auto&& self) noexcept { return std::make_reverse_iterator(self.begin()); }

        [[nodiscard]] constexpr auto cbegin()  const noexcept { return begin();  }
        [[nodiscard]] constexpr auto cend()    const noexcept { return end();    }
        [[nodiscard]] constexpr auto crbegin() const noexcept { return rbegin(); }
        [[nodiscard]] constexpr auto crend()   const noexcept { return rend();   }

        // capacity
        [[nodiscard]] constexpr bool         empty() const noexcept { return N == 0; }
        [[nodiscard]] constexpr size_type     size() const noexcept { return N;      }
        [[nodiscard]] constexpr size_type max_size() const noexcept { return N;      }

        // element access
        [[nodiscard]] constexpr auto operator[](this auto&& self, size_type n) noexcept { assert(n < N); return { self, n };                             }
        [[nodiscard]] constexpr auto at        (this auto&& self, size_type n)          {    if (n < N)  return { self, n }; else throw out_of_range(n); }
        
        [[nodiscard]] constexpr auto front(this auto&& self) noexcept { return { self, 0uz   }; }
        [[nodiscard]] constexpr auto back (this auto&& self) noexcept { return { self, N - 1 }; }

private:
        static constexpr auto out_of_range(std::size_t n, std::source_location const& loc = std::source_location::current())
        {
                return std::out_of_range(
                        std::format(
                                "{}:{}:{}: exception: ‘{}‘: argument ‘n‘ is out of range [{} >= {}]",
                                loc.file_name(), loc.line(), loc.column(), loc.function_name(), n, N
                        )
                );
        }        
};

template<std::size_t N, std::unsigned_integral Block> [[nodiscard]] constexpr bool operator== (const bit_array<N, Block>& x, const bit_array<N, Block>& y) noexcept                         { return x.m_bits ==  y.m_bits; }
template<std::size_t N, std::unsigned_integral Block> [[nodiscard]] constexpr auto operator<=>(const bit_array<N, Block>& x, const bit_array<N, Block>& y) noexcept -> std::strong_ordering { return x.m_bits <=> y.m_bits; }
template<std::size_t N, std::unsigned_integral Block>               constexpr void swap       (      bit_array<N, Block>& x,       bit_array<N, Block>& y) noexcept(noexcept(x.swap(y)))    { x.swap(y);                    }

}       // namespace xstd

#endif  // include guard
