#pragma once

//          Copyright Rein Halbersma 2014-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <bitset>       // bitset
#include <cstddef>      // size_t

namespace xstd {

template<std::size_t N>
auto resize(std::bitset<N>& bs, std::size_t const num_bits [[maybe_unused]], bool const value = false) noexcept
{
        if (value) {
                bs.set();
        } else {
                bs.reset();
        }
}

template<std::size_t N>
auto& set(std::bitset<N>& bs) noexcept
{
        return bs.set();
}

template<std::size_t N>
auto& set(std::bitset<N>& bs, std::size_t const pos, bool const val = true)
{
        return bs.set(pos, val);
}

template<std::size_t N>
auto& reset(std::bitset<N>& bs) noexcept
{
        return bs.reset();
}

template<std::size_t N>
auto& reset(std::bitset<N>& bs, std::size_t const pos)
{
        return bs.reset(pos);
}

template<std::size_t N>
auto& flip(std::bitset<N>& bs) noexcept
{
        return bs.flip();
}

template<std::size_t N>
auto& flip(std::bitset<N>& bs, std::size_t const pos)
{
        return bs.flip(pos);
}

template<std::size_t N>
constexpr auto count(std::bitset<N> const& bs) noexcept
{
        return bs.count();
}

template<std::size_t N>
constexpr auto fn_size(std::bitset<N> const& bs) noexcept
{
        return bs.size();
}

template<std::size_t N>
auto test(std::bitset<N> const& bs, std::size_t const pos)
{
        return bs.test(pos);
}

template<std::size_t N>
[[nodiscard]] auto all(std::bitset<N> const& bs) noexcept
{
        return bs.all();
}

template<std::size_t N>
[[nodiscard]] auto any(std::bitset<N> const& bs) noexcept
{
        return bs.any();
}

template<std::size_t N>
[[nodiscard]] auto none(std::bitset<N> const& bs) noexcept
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
        return is_subset_of(lhs, rhs) && lhs != rhs;
}

template<std::size_t N>
auto is_proper_superset_of(std::bitset<N> const& lhs, std::bitset<N> const& rhs) noexcept
{
        return is_superset_of(lhs, rhs) && lhs != rhs;
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
