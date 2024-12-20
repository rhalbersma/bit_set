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
#include <source_location>      // source_location
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
                std::basic_string<charT, traits, Allocator> const& str,
                typename std::basic_string<charT, traits, Allocator>::size_type pos = 0,
                typename std::basic_string<charT, traits, Allocator>::size_type n = std::basic_string<charT, traits, Allocator>::npos,
                charT zero = charT('0'),
                charT one  = charT('1')
        ) noexcept(false)
        {
                if (pos > str.size()) {
                        throw out_of_range(pos);
                }
                auto const rlen = std::ranges::min(n, str.size() - pos);
                auto const M = std::ranges::min(N, rlen);
                for (auto i : std::views::iota(std::size_t(0), M)) {
                        auto const ch = str[pos + M - 1 - i];
                        if (traits::eq(ch, zero)) {
                                continue;
                        } else if (traits::eq(ch, one)) {
                                m_bits.add(i);
                        } else {
                                throw invalid_argument(ch, zero, one);
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
        constexpr bitset& operator&=(bitset const& rhs) noexcept { m_bits &= rhs.m_bits; return *this; }
        constexpr bitset& operator|=(bitset const& rhs) noexcept { m_bits |= rhs.m_bits; return *this; }
        constexpr bitset& operator^=(bitset const& rhs) noexcept { m_bits ^= rhs.m_bits; return *this; }
        constexpr bitset& operator-=(bitset const& rhs) noexcept { m_bits -= rhs.m_bits; return *this; }

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
                        if (val) [[likely]] {
                                m_bits.add(pos);
                        } else [[unlikely]] {
                                m_bits.pop(pos);
                        }
                        return *this;
                } else [[unlikely]] {
                        throw out_of_range(pos);
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
                        m_bits.pop(pos);
                        return *this;
                } else [[unlikely]] {
                        throw out_of_range(pos);
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
                        throw out_of_range(pos);
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
                for (auto i : std::views::iota(std::size_t(0), N)) {
                        if (m_bits.contains(N - 1 - i)) {
                                str[i] = one;
                        }
                }
                return str;
        }

        // observers
        [[nodiscard]] constexpr std::size_t count() const noexcept { return m_bits.size();     }
        [[nodiscard]] constexpr std::size_t size()  const noexcept { return m_bits.max_size(); }

        [[nodiscard]] constexpr bool operator== (bitset const& rhs) const noexcept = default;
        [[nodiscard]] constexpr auto operator<=>(bitset const& rhs) const noexcept -> std::strong_ordering = default;

        [[nodiscard]] constexpr bool test(std::size_t pos) const noexcept(false)
        {
                if (pos < N) [[likely]] {
                        return m_bits.contains(pos);
                } else [[unlikely]] {
                        throw out_of_range(pos);
                }
        }

        [[nodiscard]] constexpr bool all()  const noexcept { return     m_bits.full();  }
        [[nodiscard]] constexpr bool any()  const noexcept { return not m_bits.empty(); }
        [[nodiscard]] constexpr bool none() const noexcept { return     m_bits.empty(); }

        [[nodiscard]] constexpr bool is_subset_of       (bitset const& rhs) const noexcept { return m_bits.is_subset_of       (rhs.m_bits); }
        [[nodiscard]] constexpr bool is_proper_subset_of(bitset const& rhs) const noexcept { return m_bits.is_proper_subset_of(rhs.m_bits); }
        [[nodiscard]] constexpr bool intersects         (bitset const& rhs) const noexcept { return m_bits.intersects         (rhs.m_bits); }

private:
        template<class charT>
        static constexpr auto invalid_argument(
                charT ch, charT zero = charT('0'), charT one = charT('1'),
                std::source_location const& loc = std::source_location::current()
        ) noexcept(false)
        {
                return std::invalid_argument(
                        std::format(
                                "{}:{}:{}: exception: ‘{}‘: invalid argument ‘ch‘ [{} != {} or {}]",
                                loc.file_name(), loc.line(), loc.column(), loc.function_name(), ch, zero, one
                        )
                );
        }

        static constexpr auto out_of_range(std::size_t pos, std::source_location const& loc = std::source_location::current()) noexcept(false)
        {
                return std::out_of_range(
                        std::format(
                                "{}:{}:{}: exception: ‘{}‘: argument ‘pos‘ is out of range [{} >= {}]",
                                loc.file_name(), loc.line(), loc.column(), loc.function_name(), pos, N
                        )
                );
        }
};

// bitset operators
template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr bitset<N, Block> operator&(bitset<N, Block> const& lhs, bitset<N, Block> const& rhs) noexcept
{
        auto nrv = lhs; nrv &= rhs; return nrv;
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr bitset<N, Block> operator|(bitset<N, Block> const& lhs, bitset<N, Block> const& rhs) noexcept
{
        auto nrv = lhs; nrv |= rhs; return nrv;
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr bitset<N, Block> operator^(bitset<N, Block> const& lhs, bitset<N, Block> const& rhs) noexcept
{
        auto nrv = lhs; nrv ^= rhs; return nrv;
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr bitset<N, Block> operator-(bitset<N, Block> const& lhs, bitset<N, Block> const& rhs) noexcept
{
        auto nrv = lhs; nrv -= rhs; return nrv;
}

template<class charT, class traits, std::size_t N, std::unsigned_integral Block>
std::basic_istream<charT, traits>& operator>>(std::basic_istream<charT, traits>& is, bitset<N, Block>& x) noexcept(false)
{
        auto str = std::basic_string<charT, traits>(N, is.widen('0'));
        charT ch;
        auto i = std::size_t(0);
        for (/* init-statement before loop */; i < N and not is.eof() and (is.peek() == is.widen('0') or is.peek() == is.widen('1')); ++i) {
                is >> ch;
                if (ch == is.widen('1')) {
                        str[i] = ch;
                }
        }
        x = bitset<N, Block>(str);
        if (N > 0 and i == 0) {
                is.setstate(std::basic_istream<charT, traits>::ios_base::failbit);
        }
        return is;
}

template<class charT, class traits, std::size_t N, std::unsigned_integral Block>
std::basic_ostream<charT, traits>& operator<<(std::basic_ostream<charT, traits>& os, bitset<N, Block> const& x) noexcept(false)
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
[[nodiscard]] constexpr auto begin(bitset<N, Block> const& c) noexcept
{
        return c.begin();
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto end(bitset<N, Block>& c) noexcept
{
        return c.end();
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto end(bitset<N, Block> const& c) noexcept
{
        return c.end();
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto cbegin(bitset<N, Block> const& c) noexcept
{
        return xstd::begin(c);
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto cend(bitset<N, Block> const& c) noexcept
{
        return xstd::end(c);
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto rbegin(bitset<N, Block>& c) noexcept
{
        return c.rbegin();
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto rbegin(bitset<N, Block> const& c) noexcept
{
        return c.rbegin();
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto rend(bitset<N, Block>& c) noexcept
{
        return c.rend();
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto rend(bitset<N, Block> const& c) noexcept
{
        return c.rend();
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto crbegin(bitset<N, Block> const& c) noexcept
{
        return xstd::rbegin(c);
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto crend(bitset<N, Block> const& c) noexcept
{
        return xstd::rend(c);
}

}       // namespace xstd

#endif  // include guard
