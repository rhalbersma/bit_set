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
using size_t = typename T::size_type;

template<std::size_t N>
auto& fill(std::bitset<N>& bs) noexcept
{
        return bs.set();
}

template<int N, class UIntType>
auto& fill(int_set<N, UIntType>& is) noexcept
{
        return is.fill();
}

template<std::size_t N>
auto& insert(std::bitset<N>& bs, std::size_t const pos, bool const val = true)
{
        return bs.set(pos, val);
}

template<int N, class UIntType>
auto& insert(int_set<N, UIntType>& is, size_t<int_set<N, UIntType>> const pos, bool const val = true)
{
        if (pos >= N) throw std::out_of_range{""};
        return val ? is.insert(pos) : is.erase(pos);
}

template<std::size_t N>
auto& clear(std::bitset<N>& bs) noexcept
{
        return bs.reset();
}

template<int N, class UIntType>
auto& clear(int_set<N, UIntType>& is) noexcept
{
        return is.clear();
}

template<std::size_t N>
auto& erase(std::bitset<N>& bs, std::size_t const pos)
{
        return bs.reset(pos);
}

template<int N, class UIntType>
auto& erase(int_set<N, UIntType>& is, size_t<int_set<N, UIntType>> const pos)
{
        if (pos >= N) throw std::out_of_range{""};
        return is.erase(pos);
}

template<std::size_t N>
auto& complement(std::bitset<N>& bs) noexcept
{
        return bs.flip();
}

template<int N, class UIntType>
auto& complement(int_set<N, UIntType>& is) noexcept
{
        return is.complement();
}

template<std::size_t N>
auto& replace(std::bitset<N>& bs, std::size_t const pos)
{
        return bs.flip(pos);
}

template<int N, class UIntType>
auto& replace(int_set<N, UIntType>& is, size_t<int_set<N, UIntType>> const pos)
{
        if (pos >= N) throw std::out_of_range{""};
        return is.replace(pos);
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
auto contains(std::bitset<N> const& bs, std::size_t const pos)
{
        return bs.test(pos);
}

template<int N, class UIntType>
auto contains(int_set<N, UIntType> const& is, size_t<int_set<N, UIntType>> const pos)
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
[[nodiscard]] auto not_empty(std::bitset<N> const& bs) noexcept
{
        return bs.any();
}

template<int N, class UIntType>
[[nodiscard]] auto not_empty(int_set<N, UIntType> const& is) noexcept
{
        return !is.empty();
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

template<int N, class UIntType>
constexpr auto at(int_set<N, UIntType> const& is, size_t<int_set<N, UIntType>> const pos) // Throws: Nothing.
{
        return is.contains(pos);
}

template<std::size_t N>
auto at(std::bitset<N>& bs, std::size_t const pos) // Throws: Nothing.
{
        return bs[pos];
}

template<int N, class UIntType>
constexpr auto at(int_set<N, UIntType>& is, size_t<int_set<N, UIntType>> const pos) // Throws: Nothing.
{
        return is.contains(pos);
}

template<std::size_t N>
auto at(std::bitset<N>& bs, std::size_t const pos, bool const val) // Throws: Nothing.
{
        return bs[pos] = val;
}

template<int N, class UIntType>
constexpr auto at(int_set<N, UIntType>& is, size_t<int_set<N, UIntType>> const pos, bool const val) // Throws: Nothing.
{
        return val ? is.insert(pos) : is.erase(pos);
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
