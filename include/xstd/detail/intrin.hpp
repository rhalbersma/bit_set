#ifndef XSTD_DETAIL_INTRIN_HPP
#define XSTD_DETAIL_INTRIN_HPP

//          Copyright Rein Halbersma 2014-2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <bit>          // countl_zero, countr_zero, popcount
#include <concepts>     // unsigned_integral
#include <cstddef>      // size_t

namespace xstd::detail {

[[nodiscard]] constexpr std::size_t countl_zero(std::unsigned_integral auto block) noexcept
{
        return static_cast<std::size_t>(std::countl_zero(block));
} 

[[nodiscard]] constexpr std::size_t countr_zero(std::unsigned_integral auto block) noexcept
{
        return static_cast<std::size_t>(std::countr_zero(block));
}   

[[nodiscard]] constexpr std::size_t popcount(std::unsigned_integral auto block) noexcept
{
        return static_cast<std::size_t>(std::popcount(block));
}

}       // namespace xstd::detail

#endif  // include guard
