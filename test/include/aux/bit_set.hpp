#pragma once

//          Copyright Rein Halbersma 2014-2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/bit_set.hpp>     // bit_set
#include <cassert>              // assert
#include <concepts>             // unsigned_integral
#include <cstddef>              // size_t
#include <ios>                  // noboolalpha
#include <iosfwd>               // basic_ostream
#include <stdexcept>            // out_of_range

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
        bit_set<N, Block> m_data;
        
        explicit constexpr bitset(bit_set<N, Block> const& bs) noexcept
        : 
                m_data(bs) 
        {}
public:
        bitset() = default;

        constexpr auto& operator&=(bitset const& other) noexcept
        {
                this->m_data &= other.m_data; 
                return *this;
        }

        constexpr auto& operator|=(bitset const& other) noexcept
        {
                this->m_data |= other.m_data; 
                return *this;
        }

        constexpr auto& operator^=(bitset const& other) noexcept
        {
                this->m_data ^= other.m_data; 
                return *this;
        }        

        constexpr auto& operator-=(bitset const& other) noexcept
        {
                this->m_data -= other.m_data; 
                return *this;
        }

        constexpr auto& operator<<=(std::size_t pos) noexcept
        {
                if (pos >= N) {
                        m_data.clear();
                } else {
                        m_data <<= static_cast<int>(pos);                
                }
                return *this;
        }

        constexpr auto& operator>>=(std::size_t pos) noexcept
        {
                if (pos >= N) {
                        m_data.clear();
                } else {
                        m_data >>= static_cast<int>(pos);                
                }
                return *this;
        }        

        constexpr auto& set() noexcept
        {
                m_data.fill();
                return *this;
        }

        constexpr auto& set(std::size_t pos, bool val = true) noexcept(false)
        {
                if (pos >= N) {
                        throw std::out_of_range{""};
                }
                if (val) {
                        m_data.add(static_cast<int>(pos));
                } else {
                        m_data.pop(static_cast<int>(pos));
                }
                return *this;
        }

        constexpr auto& reset() noexcept
        {
                m_data.clear();
                return *this;
        }

        constexpr auto& reset(std::size_t pos) noexcept(false)
        {
                if (pos >= N) {
                        throw std::out_of_range{""};
                }
                m_data.pop(static_cast<int>(pos));
                return *this;
        }

        [[nodiscard]] constexpr auto operator~() const noexcept
        {
                return bitset(~m_data);
        }

        constexpr auto& flip() noexcept
        {
                m_data.complement();
                return *this;
        }

        constexpr auto& flip(std::size_t pos) noexcept(false)
        {
                if (pos >= N) {
                        throw std::out_of_range{""};
                }
                m_data.replace(static_cast<int>(pos));
                return *this;
        }

        [[nodiscard]] constexpr auto operator[](std::size_t pos) const noexcept
        {
                return m_data.contains(static_cast<int>(pos));
        }

        [[nodiscard]] constexpr auto count() const noexcept
        {
                return m_data.size();
        }

        [[nodiscard]] constexpr auto size() const noexcept
        {
                return m_data.max_size();
        }

        [[nodiscard]] bool operator==(bitset const&) const = default;

        [[nodiscard]] constexpr auto test(std::size_t pos) const noexcept(false)
        {
                if (pos >= N) {
                        throw std::out_of_range{""};
                }
                return m_data.contains(static_cast<int>(pos));
        }

        [[nodiscard]] constexpr auto all() const noexcept
        {
                return m_data.full();
        }

        [[nodiscard]] constexpr auto any() const noexcept
        {
                return !m_data.empty();
        }

        [[nodiscard]] constexpr auto none() const noexcept
        {
                return m_data.empty();
        }

        [[nodiscard]] constexpr auto operator<<(std::size_t pos) const noexcept
        {
                if (pos >= N) {
                        return bitset();
                } else {
                        return bitset(m_data << static_cast<int>(pos));
                }
        }

        [[nodiscard]] constexpr auto operator>>(std::size_t pos) const noexcept
        {
                if (pos >= N) {
                        return bitset();
                } else {
                        return bitset(m_data >> static_cast<int>(pos));
                }
        }

        [[nodiscard]] constexpr auto is_subset_of(bitset const& other) const noexcept
        {
                return this->m_data.is_subset_of(other.m_data);
        }

        [[nodiscard]] constexpr auto is_proper_subset_of(bitset const& other) const noexcept
        {
                return this->m_data.is_proper_subset_of(other.m_data);
        }

        [[nodiscard]] constexpr auto intersects(bitset const& other) const noexcept
        {
                return this->m_data.intersects(other.m_data);
        }

        friend constexpr auto operator&<>(bitset const&, bitset const&) noexcept;
        friend constexpr auto operator|<>(bitset const&, bitset const&) noexcept;
        friend constexpr auto operator^<>(bitset const&, bitset const&) noexcept;
        friend constexpr auto operator-<>(bitset const&, bitset const&) noexcept;
};

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto operator&(bitset<N, Block> const& lhs, bitset<N, Block> const& rhs) noexcept
{
        return bitset<N, Block>(lhs.m_data & rhs.m_data);
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto operator|(bitset<N, Block> const& lhs, bitset<N, Block> const& rhs) noexcept
{
        return bitset<N, Block>(lhs.m_data | rhs.m_data);
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto operator^(bitset<N, Block> const& lhs, bitset<N, Block> const& rhs) noexcept
{
        return bitset<N, Block>(lhs.m_data ^ rhs.m_data);
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto operator-(bitset<N, Block> const& lhs, bitset<N, Block> const& rhs) noexcept
{
        return bitset<N, Block>(lhs.m_data - rhs.m_data);
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
auto& operator<<(std::basic_ostream<CharT, Traits>& ostr, bitset<N, Block> const& bs)
{
        for (auto i = N - 1; i < N; --i) {
                ostr << std::noboolalpha << bs[i];
        }
        return ostr;
}

template<class CharT, class Traits, std::size_t N, std::unsigned_integral Block>
auto& operator>>(std::basic_istream<CharT, Traits>& istr, bitset<N, Block>& bs)
{
        CharT c;
        for (auto i = N - 1; i < N; --i) {
                istr >> c;
                assert(c == CharT('0') || c == CharT('1'));
                bs.set(i, c - CharT('0'));
        }
        return istr;
}

}       // namespace xstd
