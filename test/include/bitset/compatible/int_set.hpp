#pragma once

//          Copyright Rein Halbersma 2014-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/int_set.hpp> // int_set
#include <stdexcept>        // out_of_range
#include <string>

namespace xstd {

template<class T>
using size_t = typename T::size_type;

template<int N, class UIntType>
auto resize(int_set<N, UIntType>& is, int /* num_bits */, bool const value = false) noexcept
{
        if (value) {
                is.fill();
        } else {
                is.clear();
        }
}

template<int N, class UIntType>
auto& fill(int_set<N, UIntType>& is) noexcept
{
        return is.fill();
}

template<int N, class UIntType>
auto& insert(int_set<N, UIntType>& is, size_t<int_set<N, UIntType>> const pos, bool const val = true)
{
        // if (pos >= N) {
        //         std::string s = "int_set<" + std::to_string(N) + ">::set(" + std::to_string(pos) + ")";
        //         throw std::out_of_range(s);
        // }
        if (val) {
                is.insert(pos);
        } else {
                is.erase(pos);
        }
        return is;
}

template<int N, class UIntType>
auto& clear(int_set<N, UIntType>& is) noexcept
{
        return is.clear();
}

template<int N, class UIntType>
auto& erase(int_set<N, UIntType>& is, size_t<int_set<N, UIntType>> const pos)
{
        // if (pos >= N) {
        //         throw std::out_of_range{"int_set::reset"};
        // }
        is.erase(pos);
        return is;
}

template<int N, class UIntType>
auto& complement(int_set<N, UIntType>& is) noexcept
{
        return is.complement();
}

template<int N, class UIntType>
auto& replace(int_set<N, UIntType>& is, size_t<int_set<N, UIntType>> const pos)
{
        // if (pos >= N) {
        //         throw std::out_of_range{"int_set::flip"};
        // }
        return is.replace(pos);
}

template<int N, class UIntType>
constexpr auto max_size(int_set<N, UIntType> const& is) noexcept
{
        return is.max_size();
}

template<int N, class UIntType>
auto contains(int_set<N, UIntType> const& is, size_t<int_set<N, UIntType>> const pos)
{
        // if (pos >= N) {
        //         throw std::out_of_range{"int_set::test"};
        // }
        return is.contains(pos);
}

template<int N, class UIntType>
[[nodiscard]] auto full(int_set<N, UIntType> const& is) noexcept
{
        return is.full();
}

template<int N, class UIntType>
[[nodiscard]] auto not_empty(int_set<N, UIntType> const& is) noexcept
{
        return !is.empty();
}

template<int N, class UIntType>
constexpr auto at(int_set<N, UIntType> const& is, size_t<int_set<N, UIntType>> const pos) // Throws: Nothing.
{
        return is.contains(pos);
}

template<int N, class UIntType>
constexpr auto at(int_set<N, UIntType>& is, size_t<int_set<N, UIntType>> const pos) // Throws: Nothing.
{
        return is.contains(pos);
}

template<int N, class UIntType>
constexpr auto at(int_set<N, UIntType>& is, size_t<int_set<N, UIntType>> const pos, bool const val) // Throws: Nothing.
{
        if (val) {
                is.insert(pos);
        } else {
                is.erase(pos);
        }
        return is;
}

}       // namespace xstd
