//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_BITS_DETAIL_INTRIN_HPP
#define XSTD_BITS_DETAIL_INTRIN_HPP

#include <xstd/ints/concepts/unsigned_integer.hpp> // unsigned_integer
#include <bit>                                     // countl_zero, countr_zero, popcount
#include <cstddef>                                 // size_t

// The seam: constrained on xstd::unsigned_integer but forwarding to <bit>, so an xstd::popcount lands as a change of body, not interface.
namespace xstd::detail::bits {

[[nodiscard]] constexpr auto countl_zero(xstd::unsigned_integer auto block) noexcept
        -> std::size_t
{
        return static_cast<std::size_t>(std::countl_zero(block));
} 

[[nodiscard]] constexpr auto countr_zero(xstd::unsigned_integer auto block) noexcept
        -> std::size_t
{
        return static_cast<std::size_t>(std::countr_zero(block));
}   

[[nodiscard]] constexpr auto popcount(xstd::unsigned_integer auto block) noexcept
        -> std::size_t
{
        return static_cast<std::size_t>(std::popcount(block));
}

}       // namespace xstd::detail::bits

#endif // XSTD_BITS_DETAIL_INTRIN_HPP
