#pragma once

//          Copyright Rein Halbersma 2014-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <boost/dynamic_bitset.hpp>     // dynamic_bitset
#include <cstddef>                      // size_t
#include <stdexcept>                    // out_of_range

namespace boost {

template<class Block, class Allocator>
auto resize(boost::dynamic_bitset<Block, Allocator>& bs, std::size_t const num_bits, bool const value = false) noexcept
{
        bs.resize(num_bits, value);
}

template<class Block, class Allocator>
auto& fill(boost::dynamic_bitset<Block, Allocator>& bs) noexcept
{
        return bs.set();
}

template<class Block, class Allocator>
auto& insert(boost::dynamic_bitset<Block, Allocator>& bs, std::size_t const pos, bool const val = true)
{
        if (pos >= bs.size()) throw std::out_of_range{""};
        return bs.set(pos, val);
}

template<class Block, class Allocator>
auto& clear(boost::dynamic_bitset<Block, Allocator>& bs) noexcept
{
        return bs.reset();
}

template<class Block, class Allocator>
auto& erase(boost::dynamic_bitset<Block, Allocator>& bs, std::size_t const pos)
{
        if (pos >= bs.size()) throw std::out_of_range{""};
        return bs.reset(pos);
}

template<class Block, class Allocator>
auto& complement(boost::dynamic_bitset<Block, Allocator>& bs) noexcept
{
        return bs.flip();
}

template<class Block, class Allocator>
auto& replace(boost::dynamic_bitset<Block, Allocator>& bs, std::size_t const pos)
{
        if (pos >= bs.size()) throw std::out_of_range{""};
        return bs.flip(pos);
}

template<class Block, class Allocator>
constexpr auto size(boost::dynamic_bitset<Block, Allocator> const& bs) noexcept
{
        return bs.count();
}

template<class Block, class Allocator>
constexpr auto max_size(boost::dynamic_bitset<Block, Allocator> const& bs) noexcept
{
        return bs.size();
}

template<class Block, class Allocator>
auto contains(boost::dynamic_bitset<Block, Allocator> const& bs, std::size_t const pos)
{
        if (pos >= bs.size()) throw std::out_of_range{""};
        return bs.test(pos);
}

template<class Block, class Allocator>
[[nodiscard]] auto full(boost::dynamic_bitset<Block, Allocator> const& bs) noexcept
{
        return bs.all();
}

template<class Block, class Allocator>
[[nodiscard]] auto not_empty(boost::dynamic_bitset<Block, Allocator> const& bs) noexcept
{
        return bs.any();
}

template<class Block, class Allocator>
[[nodiscard]] auto empty(boost::dynamic_bitset<Block, Allocator> const& bs) noexcept
{
        return bs.none();
}

template<class Block, class Allocator>
auto at(boost::dynamic_bitset<Block, Allocator> const& bs, std::size_t const pos) // Throws: Nothing.
{
        return bs[pos];
}

template<class Block, class Allocator>
auto at(boost::dynamic_bitset<Block, Allocator>& bs, std::size_t const pos) // Throws: Nothing.
{
        return bs[pos];
}

template<class Block, class Allocator>
auto at(boost::dynamic_bitset<Block, Allocator>& bs, std::size_t const pos, bool const val) // Throws: Nothing.
{
        return bs[pos] = val;
}

template<class Block, class Allocator>
auto is_subset_of(boost::dynamic_bitset<Block, Allocator> const& lhs, boost::dynamic_bitset<Block, Allocator> const& rhs) noexcept
{
        return lhs.is_subset_of(rhs);
}

template<class Block, class Allocator>
auto is_superset_of(boost::dynamic_bitset<Block, Allocator> const& lhs, boost::dynamic_bitset<Block, Allocator> const& rhs) noexcept
{
        return is_subset_of(rhs, lhs);
}

template<class Block, class Allocator>
auto is_proper_subset_of(boost::dynamic_bitset<Block, Allocator> const& lhs, boost::dynamic_bitset<Block, Allocator> const& rhs) noexcept
{
        return lhs.is_proper_subset_of(rhs);
}

template<class Block, class Allocator>
auto is_proper_superset_of(boost::dynamic_bitset<Block, Allocator> const& lhs, boost::dynamic_bitset<Block, Allocator> const& rhs) noexcept
{
        return rhs.is_proper_subset_of(lhs);
}

template<class Block, class Allocator>
auto intersects(boost::dynamic_bitset<Block, Allocator> const& lhs, boost::dynamic_bitset<Block, Allocator> const& rhs) noexcept
{
        return lhs.intersects(rhs);
}

template<class Block, class Allocator>
auto disjoint(boost::dynamic_bitset<Block, Allocator> const& lhs, boost::dynamic_bitset<Block, Allocator> const& rhs) noexcept
{
        return !intersects(lhs, rhs);
}

}       // namespace boost
