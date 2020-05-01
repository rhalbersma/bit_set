#pragma once

//          Copyright Rein Halbersma 2014-2020.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cstddef>      // ptrdiff_t
#include <set>          // set
#include <type_traits>  // commont_type_t, make_signed_t

namespace xstd {

// provided by <iterator> in C++20
auto ssize(std::set<int> const& is) noexcept
{
        using R = std::common_type_t<std::ptrdiff_t, std::make_signed_t<decltype(is.size())>>;
        return static_cast<R>(is.size());
}

}       // namespace xstd
