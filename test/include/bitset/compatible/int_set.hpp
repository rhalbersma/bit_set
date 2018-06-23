#pragma once

//          Copyright Rein Halbersma 2014-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/int_set.hpp>     // int_set
#include <cassert>              // assert
#include <stdexcept>            // out_of_range

namespace xstd {

template<class T>
using size_t = typename T::size_type;

template<int N, class Block>
auto resize(int_set<N, Block>& is, size_t<int_set<N, Block>> const num_bits [[maybe_unused]], bool const value = false) noexcept
{
        assert(num_bits == N);
        if (value) {
                is.fill();
        } else {
                is.clear();
        }
}

template<int N, class Block>
auto& set(int_set<N, Block>& is) noexcept
{
        return is.fill();
}

template<int N, class Block>
auto& set(int_set<N, Block>& is, size_t<int_set<N, Block>> const pos, bool const val = true)
{
        if (pos >= N) {
                throw std::out_of_range{""};
        }
        if (val) {
                is.insert(pos);
        } else {
                is.erase(pos);
        }
        return is;
}

template<int N, class Block>
auto& reset(int_set<N, Block>& is) noexcept
{
        return is.clear();
}

template<int N, class Block>
auto& reset(int_set<N, Block>& is, size_t<int_set<N, Block>> const pos)
{
        if (pos >= N) {
                throw std::out_of_range{""};
        }
        is.erase(pos);
        return is;
}

template<int N, class Block>
auto& flip(int_set<N, Block>& is) noexcept
{
        return is.complement();
}

template<int N, class Block>
auto& flip(int_set<N, Block>& is, size_t<int_set<N, Block>> const pos)
{
        if (pos >= N) {
                throw std::out_of_range{""};
        }
        return is.replace(pos);
}

template<int N, class Block>
constexpr auto count(int_set<N, Block> const& is) noexcept
{
        return is.size();
}

template<int N, class Block>
constexpr auto fn_size(int_set<N, Block> const& is) noexcept
{
        return is.max_size();
}

template<int N, class Block>
auto test(int_set<N, Block> const& is, size_t<int_set<N, Block>> const pos)
{
        if (pos >= N) {
                throw std::out_of_range{""};
        }
        return is.contains(pos);
}

template<int N, class Block>
[[nodiscard]] auto all(int_set<N, Block> const& is) noexcept
{
        return is.full();
}

template<int N, class Block>
[[nodiscard]] auto any(int_set<N, Block> const& is) noexcept
{
        return !is.empty();
}

template<int N, class Block>
[[nodiscard]] auto none(int_set<N, Block> const& is) noexcept
{
        return is.empty();
}

template<int N, class Block>
constexpr auto at(int_set<N, Block> const& is, size_t<int_set<N, Block>> const pos) // Throws: Nothing.
{
        return is.contains(pos);
}

template<int N, class Block>
constexpr auto at(int_set<N, Block>& is, size_t<int_set<N, Block>> const pos) // Throws: Nothing.
{
        return is.contains(pos);
}

template<int N, class Block>
constexpr auto at(int_set<N, Block>& is, size_t<int_set<N, Block>> const pos, bool const val) // Throws: Nothing.
{
        if (val) {
                is.insert(pos);
        } else {
                is.erase(pos);
        }
        return is;
}

}       // namespace xstd
