#pragma once

//          Copyright Rein Halbersma 2014-2020.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cstddef>      // ptrdiff_t
#include <type_traits>  // commont_type_t, make_signed_t

namespace xstd {

template<class C>
constexpr auto ssize(C const& c) noexcept
{
        using R = std::common_type_t<std::ptrdiff_t, std::make_signed_t<decltype(c.size())>>;
        return static_cast<R>(c.size());
}

}       // namespace xstd
