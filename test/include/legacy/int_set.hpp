#pragma once

//          Copyright Rein Halbersma 2014-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/int_set.hpp>     // int_set
#include <stdexcept>            // out_of_range

namespace xstd {

template<class T>
using size_t = typename T::size_type;

template<int N, class UIntType>
auto& set(int_set<N, UIntType>& is) noexcept
{
        is.fill();
        return is;
}

template<int N, class UIntType>
auto& set(int_set<N, UIntType>& is, size_t<int_set<N, UIntType>> const pos, bool const val = true)
{
        if (pos >= N) {
                throw std::out_of_range{"set(int_set<N, UIntType>&, int): index out of range"};
        }
        return val ? is.insert(pos) : is.erase(pos);
}

template<int N, class UIntType>
auto& reset(int_set<N, UIntType>& is) noexcept
{
        is.clear();
        return is;
}

template<int N, class UIntType>
auto& reset(int_set<N, UIntType>& is, size_t<int_set<N, UIntType>> const pos)
{
        if (pos >= N) {
                throw std::out_of_range{"reset(int_set<N, UIntType>&, int): index out of range"};
        }
        return is.erase(pos);
}

template<int N, class UIntType>
auto& flip(int_set<N, UIntType>& is) noexcept
{
        return is.toggle();
}

template<int N, class UIntType>
auto& flip(int_set<N, UIntType>& is, size_t<int_set<N, UIntType>> const pos)
{
        if (pos >= N) {
                throw std::out_of_range{"flip(int_set<N, UIntType>&, int): index out of range"};
        }
        return is.toggle(pos);
}

template<int N, class UIntType>
constexpr auto size(int_set<N, UIntType> const& is) noexcept
{
        return is.max_size();
}

template<int N, class UIntType>
auto test(int_set<N, UIntType> const& is, size_t<int_set<N, UIntType>> const pos)
{
        if (pos >= N) {
                throw std::out_of_range{"test(int_set<N, UIntType>&, int): index out of range"};
        }
        return is.contains(pos);
}

template<int N, class UIntType>
auto all(int_set<N, UIntType> const& is) noexcept
{
        return is.full();
}

template<int N, class UIntType>
auto any(int_set<N, UIntType> const& is) noexcept
{
        return !is.empty();
}

template<int N, class UIntType>
auto none(int_set<N, UIntType> const& is) noexcept
{
        return is.empty();
}

template<int N, class UIntType>
constexpr auto contains(int_set<N, UIntType> const& is, size_t<int_set<N, UIntType>> const pos) // Throws: Nothing.
{
        return is.contains(pos);
}

}       // namespace xstd
