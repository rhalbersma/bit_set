#pragma once

//          Copyright Rein Halbersma 2014-2019.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/bit_set.hpp>     // bit_set
#include <stdexcept>            // out_of_range

namespace xstd {

template<class T>
using size_t = typename T::size_type;

template<int N, class Block>
auto resize(bit_set<N, Block>& bs, size_t<bit_set<N, Block>> num_bits [[maybe_unused]], bool value = false) noexcept
{
        if (value) {
                bs.fill();
        } else {
                bs.clear();
        }
}

template<int N, class Block>
auto& set(bit_set<N, Block>& bs) noexcept
{
        return bs.fill();
}

template<int N, class Block>
auto& set(bit_set<N, Block>& bs, size_t<bit_set<N, Block>> pos, bool val = true)
{
        if (pos >= N) {
                throw std::out_of_range{""};
        }
        if (val) {
                bs.insert(pos);
        } else {
                bs.erase(pos);
        }
        return bs;
}

template<int N, class Block>
auto& reset(bit_set<N, Block>& bs) noexcept
{
        return bs.clear();
}

template<int N, class Block>
auto& reset(bit_set<N, Block>& bs, size_t<bit_set<N, Block>> pos)
{
        if (pos >= N) {
                throw std::out_of_range{""};
        }
        bs.erase(pos);
        return bs;
}

template<int N, class Block>
auto& flip(bit_set<N, Block>& bs) noexcept
{
        return bs.complement();
}

template<int N, class Block>
auto& flip(bit_set<N, Block>& bs, size_t<bit_set<N, Block>> pos)
{
        if (pos >= N) {
                throw std::out_of_range{""};
        }
        return bs.replace(pos);
}

template<int N, class Block>
constexpr auto count(bit_set<N, Block> const& bs) noexcept
{
        return bs.size();
}

template<int N, class Block>
constexpr auto fn_size(bit_set<N, Block> const& bs) noexcept
{
        return bs.max_size();
}

template<int N, class Block>
auto test(bit_set<N, Block> const& bs, size_t<bit_set<N, Block>> pos)
{
        if (pos >= N) {
                throw std::out_of_range{""};
        }
        return bs.contains(pos);
}

template<int N, class Block>
[[nodiscard]] auto all(bit_set<N, Block> const& bs) noexcept
{
        return bs.full();
}

template<int N, class Block>
[[nodiscard]] auto any(bit_set<N, Block> const& bs) noexcept
{
        return !bs.empty();
}

template<int N, class Block>
[[nodiscard]] auto none(bit_set<N, Block> const& bs) noexcept
{
        return bs.empty();
}

template<int N, class Block>
constexpr auto at(bit_set<N, Block> const& bs, size_t<bit_set<N, Block>> pos) // Throws: Nothing.
{
        return bs.contains(pos);
}

template<int N, class Block>
constexpr auto at(bit_set<N, Block>& bs, size_t<bit_set<N, Block>> pos) // Throws: Nothing.
{
        return bs.contains(pos);
}

template<int N, class Block>
constexpr auto at(bit_set<N, Block>& bs, size_t<bit_set<N, Block>> pos, bool val) // Throws: Nothing.
{
        if (val) {
                bs.insert(pos);
        } else {
                bs.erase(pos);
        }
        return bs;
}

}       // namespace xstd
