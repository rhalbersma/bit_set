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
        bit_set<N, Block> m_impl;

public:
        using block_type = Block;

        [[nodiscard]] constexpr bitset() noexcept = default;

        template<class CharT, class Traits, class Allocator>
        [[nodiscard]] constexpr explicit bitset(
                std::basic_string<CharT, Traits, Allocator> const& str,
                std::basic_string<CharT, Traits, Allocator>::size_type pos = 0,
                std::basic_string<CharT, Traits, Allocator>::size_type n = std::basic_string<CharT, Traits, Allocator>::npos,
                CharT zero = CharT('0'),
                CharT one = CharT('1')
        ) noexcept(false)
        {
                if (pos > str.size()) {
                        throw std::out_of_range("");
                }
                auto const rlen = std::ranges::min(n, str.size() - pos);
                auto const M = std::ranges::min(N, rlen);
                for (auto i : std::views::iota(0uz, M)) {
                        auto const ch = str[pos + M - 1 - i];
                        if (Traits::eq(ch, one)) {
                                m_impl.add(i);
                        } else if (!Traits::eq(ch, zero)) {
                                throw std::invalid_argument("");
                        }
                }
        }

        template<class CharT>
        [[nodiscard]] constexpr explicit bitset(
                CharT const* str,
                std::basic_string<CharT>::size_type n = std::basic_string<CharT>::npos,
                CharT zero = CharT('0'),
                CharT one = CharT('1')
        ) noexcept(false)
        :
                bitset(n == std::basic_string<CharT>::npos ? std::basic_string<CharT>(str) : std::basic_string<CharT>(str, n), 0, n, zero, one)
        {}

        constexpr auto& operator&=(bitset const& other) noexcept
        {
                this->m_impl &= other.m_impl;
                return *this;
        }

        constexpr auto& operator|=(bitset const& other) noexcept
        {
                this->m_impl |= other.m_impl;
                return *this;
        }

        constexpr auto& operator^=(bitset const& other) noexcept
        {
                this->m_impl ^= other.m_impl;
                return *this;
        }

        constexpr auto& operator-=(bitset const& other) noexcept
        {
                this->m_impl -= other.m_impl;
                return *this;
        }

        constexpr auto& operator<<=(std::size_t pos) noexcept
        {
                if (pos < N) {
                        m_impl <<= pos;
                } else {
                        m_impl.clear();
                }
                return *this;
        }

        constexpr auto& operator>>=(std::size_t pos) noexcept
        {
                if (pos < N) {
                        m_impl >>= pos;
                } else {
                        m_impl.clear();
                }
                return *this;
        }

        constexpr auto& set() noexcept
        {
                m_impl.fill();
                return *this;
        }

        constexpr auto& set(std::size_t pos, bool val = true) noexcept(false)
        {
                if (pos >= N) {
                        throw std::out_of_range("");
                }
                if (val) {
                        m_impl.add(pos);
                } else {
                        m_impl.pop(pos);
                }
                return *this;
        }

        constexpr auto& reset() noexcept
        {
                m_impl.clear();
                return *this;
        }

        constexpr auto& reset(std::size_t pos) noexcept(false)
        {
                if (pos >= N) {
                        throw std::out_of_range("");
                }
                m_impl.pop(pos);
                return *this;
        }

        [[nodiscard]] constexpr auto operator~() const noexcept
        {
                auto nrv = *this; nrv.flip(); return nrv;
        }

        constexpr auto& flip() noexcept
        {
                m_impl.complement();
                return *this;
        }

        constexpr auto& flip(std::size_t pos) noexcept(false)
        {
                if (pos >= N) {
                        throw std::out_of_range("");
                }
                m_impl.complement(pos);
                return *this;
        }

        [[nodiscard]] constexpr auto operator[](std::size_t pos) const noexcept
        {
                return m_impl.contains(pos);
        }

        template<
                class CharT = char,
                class Traits = std::char_traits<CharT>,
                class Allocator = std::allocator<CharT>
        >
        [[nodiscard]] constexpr auto to_string(CharT zero = CharT('0'), CharT one = CharT('1')) const noexcept(false)
        {
                auto str = std::basic_string<CharT, Traits, Allocator>(N, zero);
                for (auto i : std::views::iota(0uz, N)) {
                        if (m_impl.contains(N - 1 - i)) {
                                str[i] = one;
                        }
                }
                return str;
        }

        [[nodiscard]] constexpr auto count() const noexcept
        {
                return m_impl.size();
        }

        [[nodiscard]] constexpr auto size() const noexcept
        {
                return m_impl.max_size();
        }

        [[nodiscard]] bool operator==(bitset const&) const = default;

        [[nodiscard]] constexpr auto test(std::size_t pos) const noexcept(false)
        {
                if (pos >= N) {
                        throw std::out_of_range("");
                }
                return m_impl.contains(pos);
        }

        [[nodiscard]] constexpr auto all() const noexcept
        {
                return m_impl.full();
        }

        [[nodiscard]] constexpr auto any() const noexcept
        {
                return !m_impl.empty();
        }

        [[nodiscard]] constexpr auto none() const noexcept
        {
                return m_impl.empty();
        }

        [[nodiscard]] constexpr auto operator<<(std::size_t pos) const noexcept
        {
                auto nrv = *this; nrv <<= pos; return nrv;
        }

        [[nodiscard]] constexpr auto operator>>(std::size_t pos) const noexcept
        {
                auto nrv = *this; nrv >>= pos; return nrv;
        }

        [[nodiscard]] constexpr auto is_subset_of(bitset const& other) const noexcept
        {
                return this->m_impl.is_subset_of(other.m_impl);
        }

        [[nodiscard]] constexpr auto is_proper_subset_of(bitset const& other) const noexcept
        {
                return this->m_impl.is_proper_subset_of(other.m_impl);
        }

        [[nodiscard]] constexpr auto intersects(bitset const& other) const noexcept
        {
                return this->m_impl.intersects(other.m_impl);
        }

        [[nodiscard]] constexpr auto begin()         noexcept { return m_impl.begin(); }
        [[nodiscard]] constexpr auto begin()   const noexcept { return m_impl.begin(); }
        [[nodiscard]] constexpr auto end()           noexcept { return m_impl.end(); }
        [[nodiscard]] constexpr auto end()     const noexcept { return m_impl.end(); }

        [[nodiscard]] constexpr auto rbegin()        noexcept { return m_impl.rbegin(); }
        [[nodiscard]] constexpr auto rbegin()  const noexcept { return m_impl.rbegin(); }
        [[nodiscard]] constexpr auto rend()          noexcept { return m_impl.rend(); }
        [[nodiscard]] constexpr auto rend()    const noexcept { return m_impl.rend(); }

        [[nodiscard]] constexpr auto cbegin()  const noexcept { return m_impl.cbegin(); }
        [[nodiscard]] constexpr auto cend()    const noexcept { return m_impl.cend();   }
        [[nodiscard]] constexpr auto crbegin() const noexcept { return m_impl.crbegin(); }
        [[nodiscard]] constexpr auto crend()   const noexcept { return m_impl.crend();   }
};

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto operator&(bitset<N, Block> const& lhs, bitset<N, Block> const& rhs) noexcept
{
        auto nrv = lhs; nrv &= rhs; return nrv;
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto operator|(bitset<N, Block> const& lhs, bitset<N, Block> const& rhs) noexcept
{
        auto nrv = lhs; nrv |= rhs; return nrv;
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto operator^(bitset<N, Block> const& lhs, bitset<N, Block> const& rhs) noexcept
{
        auto nrv = lhs; nrv ^= rhs; return nrv;
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto operator-(bitset<N, Block> const& lhs, bitset<N, Block> const& rhs) noexcept
{
        auto nrv = lhs; nrv -= rhs; return nrv;
}

template<class CharT, class Traits, std::size_t N, std::unsigned_integral Block>
auto& operator>>(std::basic_istream<CharT, Traits>& is, bitset<N, Block>& x) noexcept(false)
{
        auto str = std::basic_string<CharT, Traits>(N, is.widen('0'));
        CharT ch;
        auto i = 0uz;
        for (/* init-statement before loop */; i < N && !is.eof() && (is.peek() == is.widen('0') || is.peek() == is.widen('1')); ++i) {
                is >> ch;
                if (ch == is.widen('1')) {
                        str[i] = ch;
                }
        }
        x = bitset<N, Block>(str);
        if (N > 0 && i == 0) {
                is.setstate(std::basic_istream<CharT, Traits>::ios_base::failbit);
        }
        return is;
}

template<class CharT, class Traits, std::size_t N, std::unsigned_integral Block>
auto& operator<<(std::basic_ostream<CharT, Traits>& os, bitset<N, Block> const& x) noexcept(false)
{
        return os << x.template to_string<CharT, Traits, std::allocator<CharT>>(
                std::use_facet<std::ctype<CharT>>(os.getloc()).widen('0'),
                std::use_facet<std::ctype<CharT>>(os.getloc()).widen('1')
        );
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto begin(bitset<N, Block>& bs) noexcept
{
        return bs.begin();
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto begin(bitset<N, Block> const& bs) noexcept
{
        return bs.begin();
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto end(bitset<N, Block>& bs) noexcept
{
        return bs.end();
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto end(bitset<N, Block> const& bs) noexcept
{
        return bs.end();
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto rbegin(bitset<N, Block>& bs) noexcept
{
        return bs.rbegin();
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto rbegin(bitset<N, Block> const& bs) noexcept
{
        return bs.rbegin();
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto rend(bitset<N, Block>& bs) noexcept
{
        return bs.rend();
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto rend(bitset<N, Block> const& bs) noexcept
{
        return bs.rend();
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto cbegin(bitset<N, Block> const& bs) noexcept
{
        return bs.cbegin();
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto cend(bitset<N, Block> const& bs) noexcept
{
        return bs.cend();
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto crbegin(bitset<N, Block> const& bs) noexcept
{
        return bs.crbegin();
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto crend(bitset<N, Block> const& bs) noexcept
{
        return bs.crend();
}

}       // namespace xstd

#endif  // include guard
