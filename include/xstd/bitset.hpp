#ifndef XSTD_BITSET_HPP
#define XSTD_BITSET_HPP

//          Copyright Rein Halbersma 2014-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/bit_set.hpp>     // bit_set
#include <concepts>             // unsigned_integral
#include <cstddef>              // size_t
#include <iosfwd>               // basic_istream, basic_ostream
#include <locale>               // ctype, use_facet
#include <memory>               // allocator
#include <ranges>               // iota
#include <stdexcept>            // out_of_range
#include <string>               // basic_string, char_traits

namespace xstd {

template<std::size_t N, std::unsigned_integral Block = std::size_t>
class bitset
{
public:
        using block_type = Block;

private:
        bit_set<std::size_t, N, Block> m_impl;

public:
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
                        throw std::out_of_range("");
                }
                auto const rlen = std::ranges::min(n, str.size() - pos);
                auto const M = std::ranges::min(N, rlen);
                for (auto i : std::views::iota(0uz, M)) {
                        auto const ch = str[pos + M - 1 - i];
                        if (traits::eq(ch, one)) {
                                m_impl.insert(i);
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

        [[nodiscard]] constexpr auto begin()         noexcept { return m_impl.begin();   }
        [[nodiscard]] constexpr auto begin()   const noexcept { return m_impl.begin();   }
        [[nodiscard]] constexpr auto end()           noexcept { return m_impl.end();     }
        [[nodiscard]] constexpr auto end()     const noexcept { return m_impl.end();     }

        [[nodiscard]] constexpr auto rbegin()        noexcept { return m_impl.rbegin();  }
        [[nodiscard]] constexpr auto rbegin()  const noexcept { return m_impl.rbegin();  }
        [[nodiscard]] constexpr auto rend()          noexcept { return m_impl.rend();    }
        [[nodiscard]] constexpr auto rend()    const noexcept { return m_impl.rend();    }

        [[nodiscard]] constexpr auto cbegin()  const noexcept { return m_impl.cbegin();  }
        [[nodiscard]] constexpr auto cend()    const noexcept { return m_impl.cend();    }
        [[nodiscard]] constexpr auto crbegin() const noexcept { return m_impl.crbegin(); }
        [[nodiscard]] constexpr auto crend()   const noexcept { return m_impl.crend();   }

        constexpr bitset& operator&=(const bitset& rhs) noexcept
        {
                m_impl &= rhs.m_impl;
                return *this;
        }

        constexpr bitset& operator|=(const bitset& rhs) noexcept
        {
                m_impl |= rhs.m_impl;
                return *this;
        }

        constexpr bitset& operator^=(const bitset& rhs) noexcept
        {
                m_impl ^= rhs.m_impl;
                return *this;
        }

        constexpr bitset& operator-=(const bitset& rhs) noexcept
        {
                m_impl -= rhs.m_impl;
                return *this;
        }

        constexpr bitset& operator<<=(std::size_t pos) noexcept
        {
                if (pos < N) [[likely]] {
                        m_impl <<= pos;
                } else [[unlikely]] {
                        m_impl.clear();
                }
                return *this;
        }

        constexpr bitset& operator>>=(std::size_t pos) noexcept
        {
                if (pos < N) [[likely]] {
                        m_impl >>= pos;
                } else [[unlikely]] {
                        m_impl.clear();
                }
                return *this;
        }

        [[nodiscard]] constexpr bitset operator<<(std::size_t pos) const noexcept
        {
                auto nrv = *this; nrv <<= pos; return nrv;
        }

        [[nodiscard]] constexpr bitset operator>>(std::size_t pos) const noexcept
        {
                auto nrv = *this; nrv >>= pos; return nrv;
        }

        constexpr bitset& set() noexcept
        {
                m_impl.fill();
                return *this;
        }

        constexpr bitset& set(std::size_t pos, bool val = true) noexcept(false)
        {
                if (pos < N) [[likely]] {
                        if (val) {
                                m_impl.insert(pos);
                        } else {
                                m_impl.erase(pos);
                        }
                        return *this;
                } else [[unlikely]] {
                        throw std::out_of_range("");
                }
        }

        constexpr bitset& reset() noexcept
        {
                m_impl.clear();
                return *this;
        }

        constexpr bitset& reset(std::size_t pos) noexcept(false)
        {
                if (pos < N) [[likely]] {
                        m_impl.erase(pos);
                        return *this;
                } else [[unlikely]] {
                        throw std::out_of_range("");
                }
        }

        [[nodiscard]] constexpr bitset operator~() const noexcept
        {
                auto nrv = *this; nrv.flip(); return nrv;
        }

        constexpr bitset& flip() noexcept
        {
                m_impl.complement();
                return *this;
        }

        constexpr bitset& flip(std::size_t pos) noexcept(false)
        {
                if (pos < N) [[likely]] {
                        m_impl.complement(pos);
                        return *this;
                } else [[unlikely]] {
                        throw std::out_of_range("");
                }
        }

        [[nodiscard]] constexpr bool operator[](std::size_t pos) const noexcept
        {
                return m_impl.contains(pos);
        }

        [[nodiscard]] constexpr auto operator[](std::size_t) noexcept = delete;


        [[nodiscard]] constexpr unsigned long      to_ulong()  const noexcept = delete;
        [[nodiscard]] constexpr unsigned long long to_ullong() const noexcept = delete;

        template<
                class charT = char,
                class traits = std::char_traits<charT>,
                class Allocator = std::allocator<charT>
        >
        [[nodiscard]] constexpr std::basic_string<charT, traits, Allocator> to_string(charT zero = charT('0'), charT one = charT('1')) const noexcept(false)
        {
                auto str = std::basic_string<charT, traits, Allocator>(N, zero);
                for (auto i : std::views::iota(0uz, N)) {
                        if (m_impl.contains(N - 1 - i)) {
                                str[i] = one;
                        }
                }
                return str;
        }

        [[nodiscard]] constexpr std::size_t count() const noexcept
        {
                return m_impl.size();
        }

        [[nodiscard]] constexpr std::size_t size() const noexcept
        {
                return m_impl.max_size();
        }

        [[nodiscard]] constexpr bool operator==(const bitset& rhs) const noexcept = default;

        [[nodiscard]] constexpr bool test(std::size_t pos) const noexcept(false)
        {
                if (pos < N) [[likely]] {
                        return m_impl.contains(pos);
                } else [[unlikely]] {
                        throw std::out_of_range("");
                }
        }

        [[nodiscard]] constexpr bool all() const noexcept
        {
                return m_impl.full();
        }

        [[nodiscard]] constexpr bool any() const noexcept
        {
                return !m_impl.empty();
        }

        [[nodiscard]] constexpr bool none() const noexcept
        {
                return m_impl.empty();
        }

        [[nodiscard]] constexpr bool is_subset_of(const bitset& rhs) const noexcept
        {
                return m_impl.is_subset_of(rhs.m_impl);
        }

        [[nodiscard]] constexpr bool is_proper_subset_of(const bitset& rhs) const noexcept
        {
                return m_impl.is_proper_subset_of(rhs.m_impl);
        }

        [[nodiscard]] constexpr bool intersects(const bitset& rhs) const noexcept
        {
                return m_impl.intersects(rhs.m_impl);
        }
};

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto begin(bitset<N, Block>& bs) noexcept
{
        return bs.begin();
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto begin(const bitset<N, Block>& bs) noexcept
{
        return bs.begin();
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto end(bitset<N, Block>& bs) noexcept
{
        return bs.end();
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto end(const bitset<N, Block>& bs) noexcept
{
        return bs.end();
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto rbegin(bitset<N, Block>& bs) noexcept
{
        return bs.rbegin();
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto rbegin(const bitset<N, Block>& bs) noexcept
{
        return bs.rbegin();
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto rend(bitset<N, Block>& bs) noexcept
{
        return bs.rend();
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto rend(const bitset<N, Block>& bs) noexcept
{
        return bs.rend();
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto cbegin(const bitset<N, Block>& bs) noexcept
{
        return bs.cbegin();
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto cend(const bitset<N, Block>& bs) noexcept
{
        return bs.cend();
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto crbegin(const bitset<N, Block>& bs) noexcept
{
        return bs.crbegin();
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto crend(const bitset<N, Block>& bs) noexcept
{
        return bs.crend();
}

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

}       // namespace xstd

#endif  // include guard
