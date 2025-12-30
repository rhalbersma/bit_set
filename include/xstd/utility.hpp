#ifndef XSTD_UTILITY_HPP
#define XSTD_UTILITY_HPP

//          Copyright Rein Halbersma 2014-2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cstddef>  // size_t

namespace xstd {

[[nodiscard]] constexpr std::size_t aligned_size(std::size_t size, std::size_t alignment) noexcept
{
        return ((size - 1 + alignment) / alignment) * alignment;
}

}       // namespace xstd

#endif  // include guard
