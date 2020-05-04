#pragma once

//          Copyright Rein Halbersma 2014-2020.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/bit_set.hpp>     // bit_set
#include <concepts>             // unsigned_integral
#include <cstddef>              // size_t
#include <stdexcept>            // out_of_range

namespace xstd {

template<class T>
using value_t = typename T::value_type;

template<std::size_t N, std::unsigned_integral Block>
constexpr auto resize(bit_set<N, Block>& bs, std::size_t num_bits [[maybe_unused]], bool value = false) noexcept
{
        if (value) {
                bs.fill();
        } else {
                bs.clear();
        }
}

template<std::size_t N, std::unsigned_integral Block>
constexpr auto& left_shift_assign(bit_set<N, Block>& bs, std::size_t pos) noexcept
{
        if (pos >= N) {
                return bs.clear();
        }
        using value_type = value_t<bit_set<N, Block>>;
        return bs <<= static_cast<value_type>(pos);
}

template<std::size_t N, std::unsigned_integral Block>
constexpr auto& right_shift_assign(bit_set<N, Block>& bs, std::size_t pos) noexcept
{
        if (pos >= N) {
                return bs.clear();
        }
        using value_type = value_t<bit_set<N, Block>>;
        return bs >>= static_cast<value_type>(pos);
}

template<std::size_t N, std::unsigned_integral Block>
constexpr auto& set(bit_set<N, Block>& bs) noexcept
{
        return bs.fill();
}

template<std::size_t N, std::unsigned_integral Block>
constexpr auto& set(bit_set<N, Block>& bs, std::size_t pos, bool val = true)
{
        if (pos >= N) {
                throw std::out_of_range{""};
        }
        using value_type = value_t<bit_set<N, Block>>;
        if (val) {
                bs.insert(static_cast<value_type>(pos));
        } else {
                bs.erase(static_cast<value_type>(pos));
        }
        return bs;
}

template<std::size_t N, std::unsigned_integral Block>
constexpr auto& reset(bit_set<N, Block>& bs) noexcept
{
        return bs.clear();
}

template<std::size_t N, std::unsigned_integral Block>
constexpr auto& reset(bit_set<N, Block>& bs, std::size_t pos)
{
        if (pos >= N) {
                throw std::out_of_range{""};
        }
        using value_type = value_t<bit_set<N, Block>>;
        bs.erase(static_cast<value_type>(pos));
        return bs;
}

template<std::size_t N, std::unsigned_integral Block>
constexpr auto& flip(bit_set<N, Block>& bs) noexcept
{
        return bs.complement();
}

template<std::size_t N, std::unsigned_integral Block>
constexpr auto& flip(bit_set<N, Block>& bs, std::size_t pos)
{
        if (pos >= N) {
                throw std::out_of_range{""};
        }
        using value_type = value_t<bit_set<N, Block>>;
        return bs.replace(static_cast<value_type>(pos));
}

template<std::size_t N, std::unsigned_integral Block>
constexpr auto count(bit_set<N, Block> const& bs) noexcept
{
        return bs.size();
}

template<std::size_t N, std::unsigned_integral Block>
constexpr auto fn_size(bit_set<N, Block> const& bs) noexcept
{
        return bs.max_size();
}

template<std::size_t N, std::unsigned_integral Block>
constexpr auto test(bit_set<N, Block> const& bs, std::size_t pos)
{
        if (pos >= N) {
                throw std::out_of_range{""};
        }
        using value_type = value_t<bit_set<N, Block>>;
        return bs.contains(static_cast<value_type>(pos));
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto all(bit_set<N, Block> const& bs) noexcept
{
        return bs.full();
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto any(bit_set<N, Block> const& bs) noexcept
{
        return !bs.empty();
}

template<std::size_t N, std::unsigned_integral Block>
[[nodiscard]] constexpr auto none(bit_set<N, Block> const& bs) noexcept
{
        return bs.empty();
}

template<std::size_t N, std::unsigned_integral Block>
constexpr auto left_shift(bit_set<N, Block> const& bs, std::size_t pos) noexcept
        -> bit_set<N, Block>
{
        if (pos >= N) {
                return {};
        }
        using value_type = value_t<bit_set<N, Block>>;
        return bs << static_cast<value_type>(pos);
}

template<std::size_t N, std::unsigned_integral Block>
constexpr auto right_shift(bit_set<N, Block> const& bs, std::size_t pos) noexcept
        -> bit_set<N, Block>
{
        if (pos >= N) {
                return {};
        }
        using value_type = value_t<bit_set<N, Block>>;
        return bs >> static_cast<value_type>(pos);
}

template<std::size_t N, std::unsigned_integral Block>
constexpr auto at(bit_set<N, Block> const& bs, std::size_t pos) // Throws: Nothing.
{
        using value_type = value_t<bit_set<N, Block>>;
        return bs.contains(static_cast<value_type>(pos));
}

template<std::size_t N, std::unsigned_integral Block>
constexpr auto at(bit_set<N, Block>& bs, std::size_t pos) // Throws: Nothing.
{
        using value_type = value_t<bit_set<N, Block>>;
        return bs.contains(static_cast<value_type>(pos));
}

template<std::size_t N, std::unsigned_integral Block>
constexpr auto at(bit_set<N, Block>& bs, std::size_t pos, bool val) // Throws: Nothing.
{
        using value_type = value_t<bit_set<N, Block>>;
        if (val) {
                bs.insert(static_cast<value_type>(pos));
        } else {
                bs.erase(static_cast<value_type>(pos));
        }
        return bs;
}

}       // namespace xstd
