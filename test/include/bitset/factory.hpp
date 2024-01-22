#pragma once

//          Copyright Rein Halbersma 2014-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <ext/boost/dynamic_bitset.hpp> // dynamic_bitset
#include <ext/std/bitset.hpp>           // bitset
#include <xstd/bitset.hpp>              // bitset
#include <concepts>                     // unsigned_integral
#include <cstddef>                      // size_t

namespace xstd {

template<class T>
struct factory;

template<std::size_t N, std::unsigned_integral Block>
struct factory<bitset<N, Block>>
{
        constexpr auto operator()(std::size_t, bool value = false) const noexcept
        {
                bitset<N, Block> bs;
                if (value) {
                        bs.set();
                } else {
                        bs.reset();
                }
                return bs;
        }
};

template<std::size_t N>
struct factory<std::bitset<N>>
{
        constexpr auto operator()(std::size_t, bool value = false) const noexcept
        {
                std::bitset<N> bs;
                if (value) {
                        bs.set();
                } else {
                        bs.reset();
                }
                return bs;
        }
};

template<std::unsigned_integral Block, class Allocator>
struct factory<boost::dynamic_bitset<Block, Allocator>>
{
        constexpr auto operator()(std::size_t num_bits, bool value = false) const noexcept
        {
                boost::dynamic_bitset<Block, Allocator> bs;
                bs.resize(num_bits, value);
                return bs;
        }
};

template<class T>
auto make_bitset(std::size_t num_bits, bool value = false)
{
        return factory<T>()(num_bits, value);
}

}       // namespace xstd
