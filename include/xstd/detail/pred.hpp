#ifndef XSTD_DETAIL_PRED_HPP
#define XSTD_DETAIL_PRED_HPP

//          Copyright Rein Halbersma 2014-2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <concepts>     // unsigned_integral

namespace xstd::detail {

template<std::unsigned_integral Block>
[[nodiscard]] constexpr bool bit_intersects(Block lhs, Block rhs) noexcept
{
        return lhs & rhs;
}   

template<std::unsigned_integral Block>
[[nodiscard]] constexpr bool bit_is_subset_of(Block lhs, Block rhs) noexcept
{
        return not (lhs & ~rhs);
}  

template<std::unsigned_integral Block>
[[nodiscard]] constexpr bool bit_not_equal_to(Block lhs, Block rhs) noexcept
{
        return lhs != rhs;
}

}       // namespace xstd::detail

#endif  // include guard
