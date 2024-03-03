#ifndef XSTD_BITSET_HPP
#define XSTD_BITSET_HPP

//          Copyright Rein Halbersma 2014-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/bit_set.hpp>     // bit_set
#include <concepts>             // unsigned_integral
#include <cstddef>              // size_t
#include <format>               // format
#include <iosfwd>               // basic_istream, basic_ostream
#include <locale>               // ctype, use_facet
#include <memory>               // allocator
#include <ranges>               // iota
#include <stdexcept>            // invalid_argument, out_of_range
#include <string>               // basic_string, char_traits

namespace xstd {

template<std::size_t N, std::unsigned_integral Block = std::size_t>
class bitset
{
        bit_set<std::size_t, N, Block> m_bits;

public:
        using block_type = Block;

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
        ) noexcept(false)
        {
                if (pos > str.size()) {
                        throw out_of_range("bitset", pos);
                }
                auto const rlen = std::ranges::min(n, str.size() - pos);
                auto const M = std::ranges::min(N, rlen);
                for (auto i : std::views::iota(0uz, M)) {
                        auto const ch = str[pos + M - 1 - i];
                        if (traits::eq(ch, one)) {
                                m_bits.insert(i);
                        } else if (!traits::eq(ch, zero)) {
                                throw std::invalid_argument("");
                        }
                }
        }

        template<class charT>
        [[nodiscard]] constexpr explicit bitset(
                charT const* str,
                std::basic_string<charT>::size_type n = std::basic_string<charT>::npos,
                charT zero = charT('0'),
                charT one = charT('1')
        ) noexcept(false)
        :
                bitset(n == std::basic_string<charT>::npos ? std::basic_string<charT>(str) : std::basic_string<charT>(str, n), 0, n, zero, one)
        {}

        // iterators
        [[nodiscard]] constexpr auto begin()         noexcept { return m_bits.begin();   }
        [[nodiscard]] constexpr auto begin()   const noexcept { return m_bits.begin();   }
        [[nodiscard]] constexpr auto end()           noexcept { return m_bits.end();     }
        [[nodiscard]] constexpr auto end()     const noexcept { return m_bits.end();     }

        [[nodiscard]] constexpr auto rbegin()        noexcept { return m_bits.rbegin();  }
        [[nodiscard]] constexpr auto rbegin()  const noexcept { return m_bits.rbegin();  }
        [[nodiscard]] constexpr auto rend()          noexcept { return m_bits.rend();    }
        [[nodiscard]] constexpr auto rend()    const noexcept { return m_bits.rend();    }

        [[nodiscard]] constexpr auto cbegin()  const noexcept { return m_bits.cbegin();  }
        [[nodiscard]] constexpr auto cend()    const noexcept { return m_bits.cend();    }
        [[nodiscard]] constexpr auto crbegin() const noexcept { return m_bits.crbegin(); }
        [[nodiscard]] constexpr auto crend()   const noexcept { return m_bits.crend();   }

        // bitset operations
        constexpr bitset& operator&=(const bitset& rhs) noexcept { m_bits &= rhs.m_bits; return *this; }
        constexpr bitset& operator|=(const bitset& rhs) noexcept { m_bits |= rhs.m_bits; return *this; }
        constexpr bitset& operator^=(const bitset& rhs) noexcept { m_bits ^= rhs.m_bits; return *this; }
        constexpr bitset& operator-=(const bitset& rhs) noexcept { m_bits -= rhs.m_bits; return *this; }

        constexpr bitset& operator<<=(std::size_t pos) noexcept
        {
                if (pos < N) [[likely]] {
                        m_bits <<= pos;
                } else [[unlikely]] {
                        m_bits.clear();
                }
                return *this;
        }

        constexpr bitset& operator>>=(std::size_t pos) noexcept
        {
                if (pos < N) [[likely]] {
                        m_bits >>= pos;
                } else [[unlikely]] {
                        m_bits.clear();
                }
                return *this;
        }

        [[nodiscard]] constexpr bitset operator<<(std::size_t pos) const noexcept { auto nrv = *this; nrv <<= pos; return nrv; }
        [[nodiscard]] constexpr bitset operator>>(std::size_t pos) const noexcept { auto nrv = *this; nrv >>= pos; return nrv; }

        constexpr bitset& set() noexcept
        {
                m_bits.fill();
                return *this;
        }

        constexpr bitset& set(std::size_t pos, bool val = true) noexcept(false)
        {
                if (pos < N) [[likely]] {
                        if (val) {
                                m_bits.insert(pos);
                        } else {
                                m_bits.erase(pos);
                        }
                        return *this;
                } else [[unlikely]] {
                        throw out_of_range("set", pos);
                }
        }

        constexpr bitset& reset() noexcept
        {
                m_bits.clear();
                return *this;
        }

        constexpr bitset& reset(std::size_t pos) noexcept(false)
        {
                if (pos < N) [[likely]] {
                        m_bits.erase(pos);
                        return *this;
                } else [[unlikely]] {
                        throw out_of_range("reset", pos);
                }
        }

        [[nodiscard]] constexpr bitset operator~() const noexcept
        {
                auto nrv = *this; nrv.flip(); return nrv;
        }

        constexpr bitset& flip() noexcept
        {
                m_bits.complement();
                return *this;
        }

        constexpr bitset& flip(std::size_t pos) noexcept(false)
        {
                if (pos < N) [[likely]] {
                        m_bits.complement(pos);
                        return *this;
                } else [[unlikely]] {
                        throw out_of_range("flip", pos);
                }
        }

        // element access
        [[nodiscard]] constexpr bool operator[](std::size_t pos) const noexcept
        {
                return m_bits.contains(pos);
        }

        [[nodiscard]] constexpr auto operator[](std::size_t) noexcept = delete;

        [[nodiscard]] constexpr unsigned long      to_ulong()  const noexcept(false) = delete;
        [[nodiscard]] constexpr unsigned long long to_ullong() const noexcept(false) = delete;

        template<
                class charT = char,
                class traits = std::char_traits<charT>,
                class Allocator = std::allocator<charT>
        >
        [[nodiscard]] constexpr std::basic_string<charT, traits, Allocator> to_string(charT zero = charT('0'), charT one = charT('1')) const noexcept(false)
        {
                auto str = std::basic_string<charT, traits, Allocator>(N, zero);
                for (auto i : std::views::iota(0uz, N)) {
                        if (m_bits.contains(N - 1 - i)) {
                                str[i] = one;
                        }
                }
                return str;
        }

        // observers
        [[nodiscard]] constexpr std::size_t count() const noexcept { return m_bits.size();     }
        [[nodiscard]] constexpr std::size_t size()  const noexcept { return m_bits.max_size(); }

        [[nodiscard]] constexpr bool operator==(const bitset& rhs) const noexcept = default;

        [[nodiscard]] constexpr bool test(std::size_t pos) const noexcept(false)
        {
                if (pos < N) [[likely]] {
                        return m_bits.contains(pos);
                } else [[unlikely]] {
                        throw out_of_range("test", pos);
                }
        }

        [[nodiscard]] constexpr bool all()  const noexcept { return  m_bits.full();  }
        [[nodiscard]] constexpr bool any()  const noexcept { return !m_bits.empty(); }
        [[nodiscard]] constexpr bool none() const noexcept { return  m_bits.empty(); }

        [[nodiscard]] constexpr bool is_subset_of       (const bitset& rhs) const noexcept { return m_bits.is_subset_of(rhs.m_bits);        }
        [[nodiscard]] constexpr bool is_proper_subset_of(const bitset& rhs) const noexcept { return m_bits.is_proper_subset_of(rhs.m_bits); }
        [[nodiscard]] constexpr bool intersects         (const bitset& rhs) const noexcept { return m_bits.intersects(rhs.m_bits);          }

private:
        static constexpr auto out_of_range(std::string mem_fn, std::size_t pos) noexcept(false)
        {
                return std::out_of_range(std::format("xstd::bitset<{2}>::{0}({1}): argument out of range [{1} >= {2}]", mem_fn, pos, N));
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
std::basic_istream<charT, traits>& operator>>(std::basic_istream<charT, traits>& is, bitset<N, Block>& x) noexcept(false)
{
        auto str = std::basic_string<charT, traits>(N, is.widen('0'));
        charT ch;
        auto i = 0uz;
        for (/* init-statement before loop */; i < N && !is.eof() && (is.peek() == is.widen('0') || is.peek() == is.widen('1')); ++i) {
                is >> ch;
                if (ch == is.widen('1')) {
                        str[i] = ch;
                }
        }
        x = bitset<N, Block>(str);
        if (N > 0 && i == 0) {
                is.setstate(std::basic_istream<charT, traits>::ios_base::failbit);
        }
        return is;
}

template<class charT, class traits, std::size_t N, std::unsigned_integral Block>
std::basic_ostream<charT, traits>& operator<<(std::basic_ostream<charT, traits>& os, const bitset<N, Block>& x) noexcept(false)
{
        return os << x.template to_string<charT, traits, std::allocator<charT>>(
                std::use_facet<std::ctype<charT>>(os.getloc()).widen('0'),
                std::use_facet<std::ctype<charT>>(os.getloc()).widen('1')
        );
}

// range access
template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto begin(bitset<N, Block>& c) noexcept
{
        return c.begin();
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto begin(const bitset<N, Block>& c) noexcept
{
        return c.begin();
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto end(bitset<N, Block>& c) noexcept
{
        return c.end();
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto end(const bitset<N, Block>& c) noexcept
{
        return c.end();
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto cbegin(const bitset<N, Block>& c) noexcept
{
        return xstd::begin(c);
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto cend(const bitset<N, Block>& c) noexcept
{
        return xstd::end(c);
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto rbegin(bitset<N, Block>& c) noexcept
{
        return c.rbegin();
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto rbegin(const bitset<N, Block>& c) noexcept
{
        return c.rbegin();
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto rend(bitset<N, Block>& c) noexcept
{
        return c.rend();
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto rend(const bitset<N, Block>& c) noexcept
{
        return c.rend();
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto crbegin(const bitset<N, Block>& c) noexcept
{
        return xstd::rbegin(c);
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto crend(const bitset<N, Block>& c) noexcept
{
        return xstd::rend(c);
}

}       // namespace xstd

#endif  // include guard
