#pragma once

//          Copyright Rein Halbersma 2014-2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/bit_set.hpp>     // bit_set
#include <cassert>              // assert
#include <concepts>             // unsigned_integral
#include <cstddef>              // size_t
#include <iosfwd>               // basic_istream, basic_ostream
#include <locale>               // ctype, use_facet
#include <memory>               // allocator
#include <stdexcept>            // out_of_range
#include <string>               // basic_string, char_traits

namespace xstd {

template<std::size_t, std::unsigned_integral>
class bitset;

template<std::size_t N, std::unsigned_integral Block>
constexpr auto operator&(bitset<N, Block> const&, bitset<N, Block> const&) noexcept;

template<std::size_t N, std::unsigned_integral Block>
constexpr auto operator|(bitset<N, Block> const&, bitset<N, Block> const&) noexcept;

template<std::size_t N, std::unsigned_integral Block>
constexpr auto operator^(bitset<N, Block> const&, bitset<N, Block> const&) noexcept;

template<std::size_t N, std::unsigned_integral Block>
constexpr auto operator-(bitset<N, Block> const&, bitset<N, Block> const&) noexcept;

template<std::size_t N, std::unsigned_integral Block = std::size_t>
class bitset
{
        bit_set<N, Block> m_impl;

        explicit constexpr bitset(bit_set<N, Block> const& bs) noexcept
        :
                m_impl(bs)
        {}
public:
        bitset() = default;

        template<class CharT, class Traits, class Allocator>
        explicit constexpr bitset(
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
                assert(m_impl.empty());
                auto const rlen = std::min(n, str.size() - pos);
                auto const M = std::min(N, rlen);
                for (auto i = std::size_t(0); i < M; ++i) {
                        auto const ch = str[pos + M - 1 - i];
                        if (Traits::eq(ch, one)) {
                                m_impl.add(static_cast<int>(i));
                        } else if (Traits::eq(ch, zero)) {
                                assert(!m_impl.contains(static_cast<int>(i)));
                        } else {
                                throw std::invalid_argument("");
                        }
                }
        }

        template<class CharT>
        explicit constexpr bitset(
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
                if (pos >= N) {
                        m_impl.clear();
                } else {
                        m_impl <<= static_cast<int>(pos);
                }
                return *this;
        }

        constexpr auto& operator>>=(std::size_t pos) noexcept
        {
                if (pos >= N) {
                        m_impl.clear();
                } else {
                        m_impl >>= static_cast<int>(pos);
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
                        m_impl.add(static_cast<int>(pos));
                } else {
                        m_impl.pop(static_cast<int>(pos));
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
                m_impl.pop(static_cast<int>(pos));
                return *this;
        }

        [[nodiscard]] constexpr auto operator~() const noexcept
        {
                return bitset(~m_impl);
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
                m_impl.complement(static_cast<int>(pos));
                return *this;
        }

        [[nodiscard]] constexpr auto operator[](std::size_t pos) const noexcept
        {
                return m_impl.contains(static_cast<int>(pos));
        }

        template<
                class CharT = char,
                class Traits = std::char_traits<CharT>,
                class Allocator = std::allocator<CharT>
        >
        [[nodiscard]] constexpr auto to_string(CharT zero = CharT('0'), CharT one = CharT('1')) const noexcept(false)
        {
                auto str = std::basic_string<CharT, Traits, Allocator>(N, zero);
                for (auto i = std::size_t(0); i < N; ++i) {
                        if (m_impl.contains(static_cast<int>(N - 1 - i))) {
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
                return m_impl.contains(static_cast<int>(pos));
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
                if (pos >= N) {
                        return bitset();
                } else {
                        return bitset(m_impl << static_cast<int>(pos));
                }
        }

        [[nodiscard]] constexpr auto operator>>(std::size_t pos) const noexcept
        {
                if (pos >= N) {
                        return bitset();
                } else {
                        return bitset(m_impl >> static_cast<int>(pos));
                }
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

        friend constexpr auto operator&<>(bitset const&, bitset const&) noexcept;
        friend constexpr auto operator|<>(bitset const&, bitset const&) noexcept;
        friend constexpr auto operator^<>(bitset const&, bitset const&) noexcept;
        friend constexpr auto operator-<>(bitset const&, bitset const&) noexcept;
};

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto operator&(bitset<N, Block> const& lhs, bitset<N, Block> const& rhs) noexcept
{
        return bitset<N, Block>(lhs.m_impl & rhs.m_impl);
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto operator|(bitset<N, Block> const& lhs, bitset<N, Block> const& rhs) noexcept
{
        return bitset<N, Block>(lhs.m_impl | rhs.m_impl);
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto operator^(bitset<N, Block> const& lhs, bitset<N, Block> const& rhs) noexcept
{
        return bitset<N, Block>(lhs.m_impl ^ rhs.m_impl);
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto operator-(bitset<N, Block> const& lhs, bitset<N, Block> const& rhs) noexcept
{
        return bitset<N, Block>(lhs.m_impl - rhs.m_impl);
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto is_subset_of(bitset<N, Block> const& lhs, bitset<N, Block> const& rhs) noexcept
{
        return lhs.is_subset_of(rhs);
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto is_proper_subset_of(bitset<N, Block> const& lhs, bitset<N, Block> const& rhs) noexcept
{
        return lhs.is_proper_subset_of(rhs);
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto intersects(bitset<N, Block> const& lhs, bitset<N, Block> const& rhs) noexcept
{
        return lhs.intersects(rhs);
}

template<class CharT, class Traits, std::size_t N, std::unsigned_integral Block>
auto& operator>>(std::basic_istream<CharT, Traits>& is, bitset<N, Block>& x) noexcept(false)
{
        auto str = std::basic_string<CharT, Traits>(N, is.widen('0'));
        CharT ch;
        auto i = std::size_t(0);
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

}       // namespace xstd
