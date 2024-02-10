#pragma once

//          Copyright Rein Halbersma 2014-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <boost/dynamic_bitset_fwd.hpp> // dynamic_bitset
#include <concepts>                     // unsigned_integral
#include <cstddef>                      // size_t

namespace xstd {

template<class T>
struct factory
{
        constexpr auto operator()(std::size_t, bool value = false) const noexcept
        {
                T b;
                if (value) {
                        b.set();
                } else {
                        b.reset();
                }
                return b;
        }
};

template<std::unsigned_integral Block, class Allocator>
struct factory<boost::dynamic_bitset<Block, Allocator>>
{
        constexpr auto operator()(std::size_t num_bits, bool value = false) const noexcept
        {
                boost::dynamic_bitset<Block, Allocator> b;
                b.resize(num_bits, value);
                return b;
        }
};

template<class T>
auto make_bitset(std::size_t num_bits, bool value = false)
{
        return factory<T>()(num_bits, value);
}

}       // namespace xstd
