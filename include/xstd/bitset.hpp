#ifndef XSTD_BITSET_HPP
#define XSTD_BITSET_HPP

//          Copyright Rein Halbersma 2014-2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <string>               // basic_string, char_traits, string
#include <iosfwd>               // basic_istream, basic_ostream

#include <xstd/bit/array.hpp>   // array
#include <xstd/proxy.hpp>       // const_iterator, const_reference
#include <concepts>             // unsigned_integral
#include <cstddef>              // size_t
#include <format>               // format
#include <ios>                  // ios_base
#include <locale>               // ctype, use_facet
#include <memory>               // allocator
#include <ranges>               // iota
#include <source_location>      // source_location
#include <stdexcept>            // invalid_argument, out_of_range, overflow_error

namespace xstd {

template<std::size_t N, std::unsigned_integral Block = std::size_t>
class bitset
{
        bit::array<N, Block> m_bits{};

        [[nodiscard]] friend constexpr std::size_t find_first(const bitset& c)                noexcept { return c.m_bits.find_first(); }
        [[nodiscard]] friend constexpr std::size_t find_last (const bitset& c)                noexcept { return c.m_bits.find_last();  }
        [[nodiscard]] friend constexpr std::size_t find_next (const bitset& c, std::size_t n) noexcept { return c.m_bits.find_next(n); }
        [[nodiscard]] friend constexpr std::size_t find_prev (const bitset& c, std::size_t n) noexcept { return c.m_bits.find_prev(n); }

public:
        using block_type             = Block;
        using iterator               = proxy::bidirectional::const_iterator<bitset>;
        using const_iterator         = iterator;
        using reverse_iterator       = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;        

        // constructors
        [[nodiscard]] constexpr bitset() noexcept = default;

        [[nodiscard]] constexpr explicit(false) bitset(unsigned long long val) noexcept = delete;

        template<class charT, class traits, class Allocator>
        [[nodiscard]] constexpr explicit bitset(
                const std::basic_string<charT, traits, Allocator>& str,
                typename std::basic_string<charT, traits, Allocator>::size_type pos = 0,
                typename std::basic_string<charT, traits, Allocator>::size_type n = std::basic_string<charT, traits, Allocator>::npos,
                charT zero = charT('0'),
                charT one  = charT('1')
        )
        {
                if (pos > str.size()) {
                        throw out_of_range(pos);
                }
                auto const rlen = std::ranges::min(n, str.size() - pos);
                auto const M = std::ranges::min(N, rlen);
                for (auto i : std::views::iota(0uz, M)) {
                        auto const ch = str[pos + M - 1 - i];
                        if (traits::eq(ch, zero)) {
                                continue;
                        } else if (traits::eq(ch, one)) {
                                m_bits.set(i);
                        } else {
                                throw invalid_argument(ch, zero, one);
                        }
                }
        }

        template<class charT>
        [[nodiscard]] constexpr explicit bitset(
                const charT* str,
                typename std::basic_string<charT>::size_type n = std::basic_string<charT>::npos,
                charT zero = charT('0'),
                charT one  = charT('1')
        )
        :
                bitset(n == std::basic_string<charT>::npos ? std::basic_string<charT>(str) : std::basic_string<charT>(str, n), 0, n, zero, one)
        {}

        // iterators
        [[nodiscard]] constexpr auto begin(this auto&& self) noexcept
                -> std::conditional_t<std::is_const_v<std::remove_reference_t<decltype(self)>>, const_iterator, iterator>
        {
                return proxy::bidirectional::begin(self);
        }

        [[nodiscard]] constexpr auto end(this auto&& self) noexcept
                -> std::conditional_t<std::is_const_v<std::remove_reference_t<decltype(self)>>, const_iterator, iterator>
        {
                return proxy::bidirectional::end(self);
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

        // bitset operations
                      constexpr bitset& operator&=(const bitset& rhs) noexcept { m_bits &= rhs.m_bits; return *this; }
                      constexpr bitset& operator|=(const bitset& rhs) noexcept { m_bits |= rhs.m_bits; return *this; }
                      constexpr bitset& operator^=(const bitset& rhs) noexcept { m_bits ^= rhs.m_bits; return *this; }
                      constexpr bitset& operator-=(const bitset& rhs) noexcept { m_bits -= rhs.m_bits; return *this; }

                      constexpr bitset& operator<<=(std::size_t pos) noexcept { if (pos < N) { m_bits <<= pos; } else { m_bits.reset(); } return *this; }
                      constexpr bitset& operator>>=(std::size_t pos) noexcept { if (pos < N) { m_bits >>= pos; } else { m_bits.reset(); } return *this; }

        [[nodiscard]] constexpr bitset  operator<<(std::size_t pos) const noexcept { auto nrv = *this; nrv <<= pos; return nrv; }
        [[nodiscard]] constexpr bitset  operator>>(std::size_t pos) const noexcept { auto nrv = *this; nrv >>= pos; return nrv; }

        constexpr bitset& set() noexcept
        {
                m_bits.set();
                return *this;
        }

        constexpr bitset& set(std::size_t pos, bool val = true)
        {
                if (pos < N) {
                        if (val) {
                                m_bits.set(pos);
                        } else {
                                m_bits.reset(pos);
                        }
                        return *this;
                } else {
                        throw out_of_range(pos);
                }
        }

        constexpr bitset& reset() noexcept
        {
                m_bits.reset();
                return *this;
        }

        constexpr bitset& reset(std::size_t pos)
        {
                if (pos < N) {
                        m_bits.reset(pos);
                        return *this;
                } else {
                        throw out_of_range(pos);
                }
        }

        [[nodiscard]] constexpr bitset operator~() const noexcept
        {
                auto nrv = *this; nrv.flip(); return nrv;
        }

        constexpr bitset& flip() noexcept
        {
                m_bits.flip();
                return *this;
        }

        constexpr bitset& flip(std::size_t pos)
        {
                if (pos < N) {
                        m_bits.flip(pos);
                        return *this;
                } else {
                        throw out_of_range(pos);
                }
        }

        // element access
        [[nodiscard]] constexpr bool operator[](std::size_t pos) const noexcept
        {
                return m_bits.test(pos);
        }

        [[nodiscard]] constexpr auto operator[](std::size_t) = delete;

        [[nodiscard]] constexpr unsigned long      to_ulong()  const = delete;
        [[nodiscard]] constexpr unsigned long long to_ullong() const = delete;

        template<
                class charT = char,
                class traits = std::char_traits<charT>,
                class Allocator = std::allocator<charT>
        >
        [[nodiscard]] constexpr std::basic_string<charT, traits, Allocator> to_string(charT zero = charT('0'), charT one = charT('1')) const
        {
                auto str = std::basic_string<charT, traits, Allocator>(N, zero);
                for (auto i : std::views::iota(0uz, N)) {
                        if (m_bits.test(N - 1 - i)) {
                                str[i] = one;
                        }
                }
                return str;
        }

        // observers
        [[nodiscard]] constexpr std::size_t count() const noexcept { return m_bits.count(); }
        [[nodiscard]] constexpr std::size_t size()  const noexcept { return m_bits.size();  }

        [[nodiscard]] constexpr bool operator== (const bitset& rhs) const noexcept                         = default;
        [[nodiscard]] constexpr auto operator<=>(const bitset& rhs) const noexcept -> std::strong_ordering = default;

        [[nodiscard]] constexpr bool test(std::size_t pos) const
        {
                if (pos < N) {
                        return m_bits.test(pos);
                } else {
                        throw out_of_range(pos);
                }
        }

        [[nodiscard]] constexpr bool all()  const noexcept { return m_bits.all();  }
        [[nodiscard]] constexpr bool any()  const noexcept { return m_bits.any();  }
        [[nodiscard]] constexpr bool none() const noexcept { return m_bits.none(); }

        [[nodiscard]] constexpr bool is_subset_of       (const bitset& rhs) const noexcept { return m_bits.is_subset_of       (rhs.m_bits); }
        [[nodiscard]] constexpr bool is_proper_subset_of(const bitset& rhs) const noexcept { return m_bits.is_proper_subset_of(rhs.m_bits); }
        [[nodiscard]] constexpr bool intersects         (const bitset& rhs) const noexcept { return m_bits.intersects         (rhs.m_bits); }

private:
        template<class charT>
        static constexpr auto invalid_argument(
                charT ch, charT zero = charT('0'), charT one = charT('1'),
                std::source_location const& loc = std::source_location::current()
        )
        {
                return std::invalid_argument(
                        std::format(
                                "{}:{}:{}: exception: ‘{}‘: invalid argument ‘ch‘ [{} != {} or {}]",
                                loc.file_name(), loc.line(), loc.column(), loc.function_name(), ch, zero, one
                        )
                );
        }

        static constexpr auto out_of_range(std::size_t pos, std::source_location const& loc = std::source_location::current())
        {
                return std::out_of_range(
                        std::format(
                                "{}:{}:{}: exception: ‘{}‘: argument ‘pos‘ is out of range [{} >= {}]",
                                loc.file_name(), loc.line(), loc.column(), loc.function_name(), pos, N
                        )
                );
        }

        static constexpr auto overflow_error(std::string const& type, std::size_t size, std::source_location const& loc = std::source_location::current())
        {
                return std::overflow_error(
                        std::format(
                                "{}:{}:{}: exception: there are too many bits to be represented in a {} [{} >= {}]",
                                loc.file_name(), loc.line(), loc.column(), loc.function_name(), type, *std::prev(end()), size
                        )
                );
        }
};

// bitset operators
template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr bitset<N, Block> operator&(const bitset<N, Block>& lhs, const bitset<N, Block>& rhs) noexcept
{
        auto nrv = lhs; nrv &= rhs; return nrv;
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr bitset<N, Block> operator|(const bitset<N, Block>& lhs, const bitset<N, Block>& rhs) noexcept
{
        auto nrv = lhs; nrv |= rhs; return nrv;
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr bitset<N, Block> operator^(const bitset<N, Block>& lhs, const bitset<N, Block>& rhs) noexcept
{
        auto nrv = lhs; nrv ^= rhs; return nrv;
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr bitset<N, Block> operator-(const bitset<N, Block>& lhs, const bitset<N, Block>& rhs) noexcept
{
        auto nrv = lhs; nrv -= rhs; return nrv;
}

template<class charT, class traits, std::size_t N, std::unsigned_integral Block>
std::basic_istream<charT, traits>& operator>>(std::basic_istream<charT, traits>& is, bitset<N, Block>& x)
{
        auto str = std::basic_string<charT, traits>(N, is.widen('0'));
        auto state = std::ios_base::goodbit;
        charT ch;
        auto i = 0uz;
        for (
                /* init-statement before loop */; 
                i < N and not is.eof() and (traits::eq_int_type(is.peek(), is.widen('0')) or traits::eq_int_type(is.peek(), is.widen('1'))); 
                ++i
        ) {
                is >> ch;
                if (traits::eq(ch, is.widen('1'))) {
                        str[i] = ch;
                }
        }
        x = bitset<N, Block>(str);
        if constexpr (N > 0) {
                if (i == 0) {
                        state |= std::ios_base::failbit;
                        is.setstate(state);
                }
        }
        return is;
}

template<class charT, class traits, std::size_t N, std::unsigned_integral Block>
std::basic_ostream<charT, traits>& operator<<(std::basic_ostream<charT, traits>& os, const bitset<N, Block>& x)
{
        return os << x.template to_string<charT, traits, std::allocator<charT>>(
                std::use_facet<std::ctype<charT>>(os.getloc()).widen('0'),
                std::use_facet<std::ctype<charT>>(os.getloc()).widen('1')
        );
}

// range access
template<std::size_t N, std::unsigned_integral Block> [[nodiscard]] constexpr auto begin  (      bitset<N, Block>& c) noexcept { return c.begin(); }
template<std::size_t N, std::unsigned_integral Block> [[nodiscard]] constexpr auto begin  (const bitset<N, Block>& c) noexcept { return c.begin(); }
template<std::size_t N, std::unsigned_integral Block> [[nodiscard]] constexpr auto end    (      bitset<N, Block>& c) noexcept { return c.end(); }
template<std::size_t N, std::unsigned_integral Block> [[nodiscard]] constexpr auto end    (const bitset<N, Block>& c) noexcept { return c.end(); }
template<std::size_t N, std::unsigned_integral Block> [[nodiscard]] constexpr auto cbegin (const bitset<N, Block>& c) noexcept { return xstd::begin(c); }
template<std::size_t N, std::unsigned_integral Block> [[nodiscard]] constexpr auto cend   (const bitset<N, Block>& c) noexcept { return xstd::end(c);   }
template<std::size_t N, std::unsigned_integral Block> [[nodiscard]] constexpr auto rbegin (      bitset<N, Block>& c) noexcept { return c.rbegin(); }
template<std::size_t N, std::unsigned_integral Block> [[nodiscard]] constexpr auto rbegin (const bitset<N, Block>& c) noexcept { return c.rbegin(); }
template<std::size_t N, std::unsigned_integral Block> [[nodiscard]] constexpr auto rend   (      bitset<N, Block>& c) noexcept { return c.rend(); }
template<std::size_t N, std::unsigned_integral Block> [[nodiscard]] constexpr auto rend   (const bitset<N, Block>& c) noexcept { return c.rend(); }
template<std::size_t N, std::unsigned_integral Block> [[nodiscard]] constexpr auto crbegin(const bitset<N, Block>& c) noexcept { return xstd::rbegin(c); }
template<std::size_t N, std::unsigned_integral Block> [[nodiscard]] constexpr auto crend  (const bitset<N, Block>& c) noexcept { return xstd::rend(c);   }

}       // namespace xstd

#endif  // include guard
