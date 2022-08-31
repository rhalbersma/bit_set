#pragma once

//          Copyright Rein Halbersma 2014-2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <utility>      // declval

namespace xstd {

template<class T>
concept resizeable = requires(T&& t)
{
        t.resize(std::declval<typename T::size_type>());
        t.resize(std::declval<typename T::size_type>(), std::declval<bool>());
};

}       // namespace xstd
