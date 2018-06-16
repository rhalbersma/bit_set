#pragma once

//          Copyright Rein Halbersma 2014-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <bitset>       // bitset
#include <cstddef>      // size_t

namespace xstd {

template<std::size_t N>
auto resize(std::bitset<N>& bs, std::size_t /* num_bits */, bool const value = false) noexcept
{
        if (value) {
                bs.set();
        } else {
                bs.reset();
        }
}

template<std::size_t N>
auto& fill(std::bitset<N>& bs) noexcept
{
        return bs.set();
}

template<std::size_t N>
auto& insert(std::bitset<N>& bs, std::size_t const pos, bool const val = true)
{
        return bs.set(pos, val);
}

template<std::size_t N>
auto& clear(std::bitset<N>& bs) noexcept
{
        return bs.reset();
}

template<std::size_t N>
auto& erase(std::bitset<N>& bs, std::size_t const pos)
{
        return bs.reset(pos);
}

template<std::size_t N>
auto& complement(std::bitset<N>& bs) noexcept
{
        return bs.flip();
}

template<std::size_t N>
auto& replace(std::bitset<N>& bs, std::size_t const pos)
{
        return bs.flip(pos);
}

template<std::size_t N>
constexpr auto size(std::bitset<N> const& bs) noexcept
{
        return bs.count();
}

template<std::size_t N>
constexpr auto max_size(std::bitset<N> const& bs) noexcept
{
        return bs.size();
}

template<std::size_t N>
auto operator<(std::bitset<N> const& lhs, std::bitset<N> const& rhs)
{
        return lhs.to_string() < rhs.to_string();
}

template<std::size_t N>
auto operator>(std::bitset<N> const& lhs, std::bitset<N> const& rhs)
{
        return rhs < lhs;
}

template<std::size_t N>
auto operator>=(std::bitset<N> const& lhs, std::bitset<N> const& rhs)
{
        return !(lhs < rhs);
}

template<std::size_t N>
auto operator<=(std::bitset<N> const& lhs, std::bitset<N> const& rhs)
{
        return !(rhs < lhs);
}

template<std::size_t N>
auto contains(std::bitset<N> const& bs, std::size_t const pos)
{
        return bs.test(pos);
}

template<std::size_t N>
[[nodiscard]] auto full(std::bitset<N> const& bs) noexcept
{
        return bs.all();
}

template<std::size_t N>
[[nodiscard]] auto not_empty(std::bitset<N> const& bs) noexcept
{
        return bs.any();
}

template<std::size_t N>
[[nodiscard]] auto empty(std::bitset<N> const& bs) noexcept
{
        return bs.none();
}

template<std::size_t N>
constexpr auto at(std::bitset<N> const& bs, std::size_t const pos) // Throws: Nothing.
{
        return bs[pos];
}

template<std::size_t N>
auto at(std::bitset<N>& bs, std::size_t const pos) // Throws: Nothing.
{
        return bs[pos];
}

template<std::size_t N>
auto at(std::bitset<N>& bs, std::size_t const pos, bool const val) // Throws: Nothing.
{
        return bs[pos] = val;
}

template<std::size_t N>
auto operator-(std::bitset<N> const& lhs, std::bitset<N> const& rhs) noexcept
{
        return lhs & ~rhs;
}

template<std::size_t N>
auto is_subset_of(std::bitset<N> const& lhs, std::bitset<N> const& rhs) noexcept
{
        return (lhs & ~rhs).none();
}

template<std::size_t N>
auto is_superset_of(std::bitset<N> const& lhs, std::bitset<N> const& rhs) noexcept
{
        return is_subset_of(rhs, lhs);
}

template<std::size_t N>
auto is_proper_subset_of(std::bitset<N> const& lhs, std::bitset<N> const& rhs) noexcept
{
        return is_subset_of(lhs, rhs) && !is_subset_of(rhs, lhs);
}

template<std::size_t N>
auto is_proper_superset_of(std::bitset<N> const& lhs, std::bitset<N> const& rhs) noexcept
{
        return is_superset_of(lhs, rhs) && !is_superset_of(rhs, lhs);
}

template<std::size_t N>
auto intersects(std::bitset<N> const& lhs, std::bitset<N> const& rhs) noexcept
{
        return (lhs & rhs).any();
}

template<std::size_t N>
auto disjoint(std::bitset<N> const& lhs, std::bitset<N> const& rhs) noexcept
{
        return !intersects(lhs, rhs);
}

}       // namespace xstd
