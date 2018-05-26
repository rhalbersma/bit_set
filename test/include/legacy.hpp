#pragma once

//          Copyright Rein Halbersma 2014-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/int_set.hpp>     // int_set
#include <bitset>               // bitset
#include <cstddef>              // size_t
#include <stdexcept>            // out_of_range

namespace xstd {

template<class T>
using size_type = typename T::size_type;

template<std::size_t N>
auto& set(std::bitset<N>& bs) noexcept
{
        return bs.set();
}

template<int N, class UIntType>
auto& set(int_set<N, UIntType>& is) noexcept
{
        is.fill();
        return is;
}

template<std::size_t N>
auto& set(std::bitset<N>& bs, std::size_t const pos, bool const val = true)
{
        return bs.set(pos, val);
}

template<int N, class UIntType>
auto& set(int_set<N, UIntType>& is, size_type<int_set<N, UIntType>> const pos, bool const val = true)
{
        if (pos >= N) throw std::out_of_range{""};
        return val ? is.insert(pos) : is.erase(pos);
}

template<std::size_t N>
auto& reset(std::bitset<N>& bs) noexcept
{
        return bs.reset();
}

template<int N, class UIntType>
auto& reset(int_set<N, UIntType>& is) noexcept
{
        is.clear();
        return is;
}

template<std::size_t N>
auto& reset(std::bitset<N>& bs, std::size_t const pos)
{
        return bs.reset(pos);
}

template<int N, class UIntType>
auto& reset(int_set<N, UIntType>& is, size_type<int_set<N, UIntType>> const pos)
{
        if (pos >= N) throw std::out_of_range{""};
        return is.erase(pos);
}

template<std::size_t N>
auto& flip(std::bitset<N>& bs) noexcept
{
        return bs.flip();
}

template<int N, class UIntType>
auto& flip(int_set<N, UIntType>& is) noexcept
{
        return is.toggle();
}

template<std::size_t N>
auto& flip(std::bitset<N>& bs, std::size_t const pos)
{
        return bs.flip(pos);
}

template<int N, class UIntType>
auto& flip(int_set<N, UIntType>& is, size_type<int_set<N, UIntType>> const pos)
{
        if (pos >= N) throw std::out_of_range{""};
        return is.toggle(pos);
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

template<int N, class UIntType>
constexpr auto max_size(int_set<N, UIntType> const& is) noexcept
{
        return is.max_size();
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
auto test(std::bitset<N> const& bs, std::size_t const pos)
{
        return bs.test(pos);
}

template<int N, class UIntType>
auto test(int_set<N, UIntType> const& is, size_type<int_set<N, UIntType>> const pos)
{
        if (pos >= N) throw std::out_of_range{""};
        return is.contains(pos);
}

template<std::size_t N>
[[nodiscard]] auto full(std::bitset<N> const& bs) noexcept
{
        return bs.all();
}

template<int N, class UIntType>
[[nodiscard]] auto full(int_set<N, UIntType> const& is) noexcept
{
        return is.full();
}

template<std::size_t N>
[[nodiscard]] auto any(std::bitset<N> const& bs) noexcept
{
        return bs.any();
}

template<int N, class UIntType>
[[nodiscard]] auto any(int_set<N, UIntType> const& is) noexcept
{
        return !is.empty();
}

template<std::size_t N>
[[nodiscard]] auto empty(std::bitset<N> const& bs) noexcept
{
        return bs.none();
}

template<std::size_t N>
[[nodiscard]] auto contains(std::bitset<N> const& bs, std::size_t const pos) // Throws: Nothing.
{
        return bs[pos];
}

template<int N, class UIntType>
[[nodiscard]] constexpr auto contains(int_set<N, UIntType> const& is, size_type<int_set<N, UIntType>> const pos) // Throws: Nothing.
{
        return is.contains(pos);
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
