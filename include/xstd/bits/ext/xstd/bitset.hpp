//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_BITS_EXT_XSTD_BITSET_HPP
#define XSTD_BITS_EXT_XSTD_BITSET_HPP

#include <xstd/bits/bitset.hpp>                    // bitset
#include <xstd/bits/ranges/set_view.hpp>           // begin, end
#include <xstd/ints/concepts/unsigned_integer.hpp> // unsigned_integer
#include <iterator>                                // make_reverse_iterator
#include <ranges>                                  // begin, end, rbegin, rend

namespace xstd {

template<std::size_t N, xstd::unsigned_integer Block> [[nodiscard]] constexpr auto begin  (      bitset<N, Block>& c) noexcept { return xstd::ranges::set_begin(c); }
template<std::size_t N, xstd::unsigned_integer Block> [[nodiscard]] constexpr auto begin  (const bitset<N, Block>& c) noexcept { return xstd::ranges::set_begin(c); }
template<std::size_t N, xstd::unsigned_integer Block> [[nodiscard]] constexpr auto end    (      bitset<N, Block>& c) noexcept { return xstd::ranges::set_end(c); }
template<std::size_t N, xstd::unsigned_integer Block> [[nodiscard]] constexpr auto end    (const bitset<N, Block>& c) noexcept { return xstd::ranges::set_end(c); }
template<std::size_t N, xstd::unsigned_integer Block> [[nodiscard]] constexpr auto cbegin (const bitset<N, Block>& c) noexcept { return std::ranges::begin(c); }
template<std::size_t N, xstd::unsigned_integer Block> [[nodiscard]] constexpr auto cend   (const bitset<N, Block>& c) noexcept { return std::ranges::end  (c); }
template<std::size_t N, xstd::unsigned_integer Block> [[nodiscard]] constexpr auto rbegin (      bitset<N, Block>& c) noexcept { return std::make_reverse_iterator(std::ranges::end(c)); }
template<std::size_t N, xstd::unsigned_integer Block> [[nodiscard]] constexpr auto rbegin (const bitset<N, Block>& c) noexcept { return std::make_reverse_iterator(std::ranges::end(c)); }
template<std::size_t N, xstd::unsigned_integer Block> [[nodiscard]] constexpr auto rend   (      bitset<N, Block>& c) noexcept { return std::make_reverse_iterator(std::ranges::begin(c)); }
template<std::size_t N, xstd::unsigned_integer Block> [[nodiscard]] constexpr auto rend   (const bitset<N, Block>& c) noexcept { return std::make_reverse_iterator(std::ranges::begin(c)); }
template<std::size_t N, xstd::unsigned_integer Block> [[nodiscard]] constexpr auto crbegin(const bitset<N, Block>& c) noexcept { return std::ranges::rbegin(c); }
template<std::size_t N, xstd::unsigned_integer Block> [[nodiscard]] constexpr auto crend  (const bitset<N, Block>& c) noexcept { return std::ranges::rend  (c); }

}       // namespace xstd

#endif // XSTD_BITS_EXT_XSTD_BITSET_HPP
