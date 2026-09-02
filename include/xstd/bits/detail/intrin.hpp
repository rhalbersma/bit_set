//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_BITS_DETAIL_INTRIN_HPP
#define XSTD_BITS_DETAIL_INTRIN_HPP

#include <xstd/ints/concepts/unsigned_integer.hpp> // unsigned_integer
#include <bit>          // countl_zero, countr_zero, popcount
#include <cstddef>      // size_t

// The seam. These three take every xstd::unsigned_integer, but forward to <bit>,
// which the standard constrains to the standard unsigned integer types alone: an
// integer class -- absl::uint128, boost::uint128, the Microsoft STL's
// _Unsigned128 -- satisfies the constraint here and then fails inside. Even the
// compiler's own 128-bit extension only gets through because libstdc++ admits it
// under GNU extensions; -std=c++23 rejects it.
//
// The constraint is deliberately the wider one anyway, so that an xstd::popcount
// over xstd::unsigned_integer -- which is what these are waiting on -- lands as a
// change of body rather than a change of interface.
namespace xstd::detail::bits {

[[nodiscard]] constexpr std::size_t countl_zero(xstd::unsigned_integer auto block) noexcept
{
        return static_cast<std::size_t>(std::countl_zero(block));
} 

[[nodiscard]] constexpr std::size_t countr_zero(xstd::unsigned_integer auto block) noexcept
{
        return static_cast<std::size_t>(std::countr_zero(block));
}   

[[nodiscard]] constexpr std::size_t popcount(xstd::unsigned_integer auto block) noexcept
{
        return static_cast<std::size_t>(std::popcount(block));
}

}       // namespace xstd::detail::bits

#endif // XSTD_BITS_DETAIL_INTRIN_HPP
