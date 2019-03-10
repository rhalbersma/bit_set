#pragma once

//          Copyright Rein Halbersma 2014-2019.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/bit_set.hpp>     // basic_bit_set
#include <cstddef>              // size_t
#include <stdexcept>            // out_of_range

namespace xstd {

template<class T>
using value_t = typename T::value_type;

template<std::size_t N, class Block>
auto resize(basic_bit_set<N, Block>& bs, std::size_t num_bits [[maybe_unused]], bool value = false) noexcept
{
        if (value) {
                bs.fill();
        } else {
                bs.clear();
        }
}

template<std::size_t N, class Block>
auto& left_shift_assign(basic_bit_set<N, Block>& bs, std::size_t pos) noexcept
{
        if (pos >= N) {
                return bs.clear();
        }
        using value_type = value_t<basic_bit_set<N, Block>>;
        return bs <<= static_cast<value_type>(pos);
}

template<std::size_t N, class Block>
auto& right_shift_assign(basic_bit_set<N, Block>& bs, std::size_t pos) noexcept
{
        if (pos >= N) {
                return bs.clear();
        }
        using value_type = value_t<basic_bit_set<N, Block>>;
        return bs >>= static_cast<value_type>(pos);
}

template<std::size_t N, class Block>
auto& set(basic_bit_set<N, Block>& bs) noexcept
{
        return bs.fill();
}

template<std::size_t N, class Block>
auto& set(basic_bit_set<N, Block>& bs, std::size_t pos, bool val = true)
{
        if (pos >= N) {
                throw std::out_of_range{""};
        }
        using value_type = value_t<basic_bit_set<N, Block>>;
        if (val) {
                bs.insert(static_cast<value_type>(pos));
        } else {
                bs.erase(static_cast<value_type>(pos));
        }
        return bs;
}

template<std::size_t N, class Block>
auto& reset(basic_bit_set<N, Block>& bs) noexcept
{
        return bs.clear();
}

template<std::size_t N, class Block>
auto& reset(basic_bit_set<N, Block>& bs, std::size_t pos)
{
        if (pos >= N) {
                throw std::out_of_range{""};
        }
        using value_type = value_t<basic_bit_set<N, Block>>;
        bs.erase(static_cast<value_type>(pos));
        return bs;
}

template<std::size_t N, class Block>
auto& flip(basic_bit_set<N, Block>& bs) noexcept
{
        return bs.complement();
}

template<std::size_t N, class Block>
auto& flip(basic_bit_set<N, Block>& bs, std::size_t pos)
{
        if (pos >= N) {
                throw std::out_of_range{""};
        }
        using value_type = value_t<basic_bit_set<N, Block>>;
        return bs.replace(static_cast<value_type>(pos));
}

template<std::size_t N, class Block>
constexpr auto count(basic_bit_set<N, Block> const& bs) noexcept
{
        return bs.size();
}

template<std::size_t N, class Block>
constexpr auto fn_size(basic_bit_set<N, Block> const& bs) noexcept
{
        return bs.max_size();
}

template<std::size_t N, class Block>
auto test(basic_bit_set<N, Block> const& bs, std::size_t pos)
{
        if (pos >= N) {
                throw std::out_of_range{""};
        }
        using value_type = value_t<basic_bit_set<N, Block>>;
        return bs.contains(static_cast<value_type>(pos));
}

template<std::size_t N, class Block>
[[nodiscard]] auto all(basic_bit_set<N, Block> const& bs) noexcept
{
        return bs.full();
}

template<std::size_t N, class Block>
[[nodiscard]] auto any(basic_bit_set<N, Block> const& bs) noexcept
{
        return !bs.empty();
}

template<std::size_t N, class Block>
[[nodiscard]] auto none(basic_bit_set<N, Block> const& bs) noexcept
{
        return bs.empty();
}

template<std::size_t N, class Block>
auto left_shift(basic_bit_set<N, Block> const& bs, std::size_t pos) noexcept
        -> basic_bit_set<N, Block>
{
        if (pos >= N) {
                return {};
        }
        using value_type = value_t<basic_bit_set<N, Block>>;
        return bs << static_cast<value_type>(pos);
}

template<std::size_t N, class Block>
auto right_shift(basic_bit_set<N, Block> const& bs, std::size_t pos) noexcept
        -> basic_bit_set<N, Block>
{
        if (pos >= N) {
                return {};
        }
        using value_type = value_t<basic_bit_set<N, Block>>;
        return bs >> static_cast<value_type>(pos);
}

template<std::size_t N, class Block>
constexpr auto at(basic_bit_set<N, Block> const& bs, std::size_t pos) // Throws: Nothing.
{
        using value_type = value_t<basic_bit_set<N, Block>>;
        return bs.contains(static_cast<value_type>(pos));
}

template<std::size_t N, class Block>
constexpr auto at(basic_bit_set<N, Block>& bs, std::size_t pos) // Throws: Nothing.
{
        using value_type = value_t<basic_bit_set<N, Block>>;
        return bs.contains(static_cast<value_type>(pos));
}

template<std::size_t N, class Block>
constexpr auto at(basic_bit_set<N, Block>& bs, std::size_t pos, bool val) // Throws: Nothing.
{
        using value_type = value_t<basic_bit_set<N, Block>>;
        if (val) {
                bs.insert(static_cast<value_type>(pos));
        } else {
                bs.erase(static_cast<value_type>(pos));
        }
        return bs;
}

}       // namespace xstd
