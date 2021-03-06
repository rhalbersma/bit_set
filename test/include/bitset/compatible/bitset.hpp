#pragma once

//          Copyright Rein Halbersma 2014-2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <bitset>       // bitset
#include <cstddef>      // size_t

namespace xstd {

template<std::size_t N>
auto resize(std::bitset<N>& bs, std::size_t num_bits [[maybe_unused]], bool value = false) noexcept
{
        if (value) {
                bs.set();
        } else {
                bs.reset();
        }
}

template<std::size_t N>
auto& operator-=(std::bitset<N>& lhs, std::bitset<N> const& rhs) noexcept
{
        return lhs &= ~rhs;
}

template<std::size_t N>
auto& left_shift_assign(std::bitset<N>& bs, std::size_t pos) noexcept
{
        return bs <<= pos;
}

template<std::size_t N>
auto& right_shift_assign(std::bitset<N>& bs, std::size_t pos) noexcept
{
        return bs >>= pos;
}

template<std::size_t N>
auto& set(std::bitset<N>& bs) noexcept
{
        return bs.set();
}

template<std::size_t N>
auto& set(std::bitset<N>& bs, std::size_t pos, bool val = true)
{
        return bs.set(pos, val);
}

template<std::size_t N>
auto& reset(std::bitset<N>& bs) noexcept
{
        return bs.reset();
}

template<std::size_t N>
auto& reset(std::bitset<N>& bs, std::size_t pos)
{
        return bs.reset(pos);
}

template<std::size_t N>
auto& flip(std::bitset<N>& bs) noexcept
{
        return bs.flip();
}

template<std::size_t N>
auto& flip(std::bitset<N>& bs, std::size_t pos)
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
auto test(std::bitset<N> const& bs, std::size_t pos)
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
auto left_shift(std::bitset<N> const& bs, std::size_t pos) noexcept
{
        return bs << pos;
}

template<std::size_t N>
auto right_shift(std::bitset<N> const& bs, std::size_t pos) noexcept
{
        return bs >> pos;
}

template<std::size_t N>
constexpr auto at(std::bitset<N> const& bs, std::size_t pos) // Throws: Nothing.
{
        return bs[pos];
}

template<std::size_t N>
auto at(std::bitset<N>& bs, std::size_t pos) // Throws: Nothing.
{
        return bs[pos];
}

template<std::size_t N>
auto at(std::bitset<N>& bs, std::size_t pos, bool val) // Throws: Nothing.
{
        return bs[pos] = val;
}

template<std::size_t N>
auto operator-(std::bitset<N> const& lhs, std::bitset<N> const& rhs) noexcept
{
        auto nrv = lhs; nrv -= rhs; return nrv;
}

template<std::size_t N>
auto is_subset_of(std::bitset<N> const& lhs, std::bitset<N> const& rhs) noexcept
{
        return (lhs & ~rhs).none();
}

template<std::size_t N>
auto is_superset_of(std::bitset<N> const& lhs, std::bitset<N> const& rhs) noexcept
{
        return (rhs & ~lhs).none();
}

template<std::size_t N>
auto is_proper_subset_of(std::bitset<N> const& lhs, std::bitset<N> const& rhs) noexcept
{
        return (lhs | ~rhs).none();
}

template<std::size_t N>
auto is_proper_superset_of(std::bitset<N> const& lhs, std::bitset<N> const& rhs) noexcept
{
        return (rhs | ~lhs).none();
}

template<std::size_t N>
auto intersects(std::bitset<N> const& lhs, std::bitset<N> const& rhs) noexcept
{
        return (lhs & rhs).any();
}

template<std::size_t N>
auto is_disjoint(std::bitset<N> const& lhs, std::bitset<N> const& rhs) noexcept
{
        return (lhs & rhs).none();
}

}       // namespace xstd
