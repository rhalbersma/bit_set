//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_BITS_BITSET_HPP
#define XSTD_BITS_BITSET_HPP

// Bitsets [bitset], Header <bitset> synopsis [bitset.syn]

#include <iosfwd> // basic_istream, basic_ostream
#include <string> // basic_string, char_traits

#include <xstd/ints/concepts/unsigned_integer.hpp> // unsigned_integer
#include <cstddef>                                 // size_t

namespace xstd {

template<std::size_t N, xstd::unsigned_integer Block = std::size_t> class bitset;

template<std::size_t N, xstd::unsigned_integer Block> [[nodiscard]] constexpr bitset<N, Block> operator&(const bitset<N, Block>&, const bitset<N, Block>&) noexcept;
template<std::size_t N, xstd::unsigned_integer Block> [[nodiscard]] constexpr bitset<N, Block> operator|(const bitset<N, Block>&, const bitset<N, Block>&) noexcept;
template<std::size_t N, xstd::unsigned_integer Block> [[nodiscard]] constexpr bitset<N, Block> operator^(const bitset<N, Block>&, const bitset<N, Block>&) noexcept;
template<std::size_t N, xstd::unsigned_integer Block> [[nodiscard]] constexpr bitset<N, Block> operator-(const bitset<N, Block>&, const bitset<N, Block>&) noexcept;

template<class charT, class traits, std::size_t N, xstd::unsigned_integer Block> std::basic_istream<charT, traits>& operator>>(std::basic_istream<charT, traits>& is,       bitset<N, Block>& x);
template<class charT, class traits, std::size_t N, xstd::unsigned_integer Block> std::basic_ostream<charT, traits>& operator<<(std::basic_ostream<charT, traits>& os, const bitset<N, Block>& x);

}       // namespace xstd

#include <boost/hash2/fnv1a.hpp>           // fnv1a_64
#include <boost/hash2/hash_append.hpp>     // hash_append
#include <xstd/bits/detail/array.hpp>      // array
#include <xstd/bits/ranges/array_view.hpp> // array_find
#include <xstd/bits/ranges/set_view.hpp>   // set_find, set_compare
#include <algorithm>                       // find_if, min
#include <cassert>                         // assert
#include <compare>                         // strong_ordering
#include <format>                          // format
#include <functional>                      // hash
#include <ios>                             // ios_base
#include <locale>                          // ctype, use_facet
#include <memory>                          // allocator
#include <ranges>                          // find_if, iota, reverse
#include <source_location>                 // source_location
#include <stdexcept>                       // invalid_argument, out_of_range, overflow_error
#include <string_view>                     // basic_string_view

// Class template bitset [template.bitset], General [template.bitset.general]

namespace xstd {

template<std::size_t N, xstd::unsigned_integer Block>
class bitset
{
        // No iteration and no <=> here by design, because std::bitset has neither: choose set_view or array_view.
        detail::bits::array<N, Block> m_bits{};

        // ADL rather than a specialization, because this type is ours to add hidden friends to.
        [[nodiscard]] friend constexpr std::size_t block_count(const bitset&) noexcept { return detail::bits::array<N, Block>::num_blocks; }
        [[nodiscard]] friend constexpr Block block_at(const bitset& c, std::size_t i) noexcept { return c.m_bits.block(i); }

        template<class Provider, class Hash, class Flavor>
        friend constexpr void tag_invoke(boost::hash2::hash_append_tag const&, Provider const&, Hash& h, Flavor const& f, bitset const* v) noexcept
        {
                boost::hash2::hash_append(h, f, v->m_bits);
        }

public:
        using block_type = Block;

        class reference
        {
        public:
                constexpr reference(const reference& x) noexcept = default;
                constexpr ~reference() = default;
                constexpr reference& operator=(bool x) noexcept;
                constexpr reference& operator=(const reference& x) noexcept = default;
                // A proxy reference assigns through a const proxy, the shape the standard gives vector<bool>::reference.
                constexpr const reference& operator=(bool x) const noexcept;  // NOLINT(misc-unconventional-assign-operator)
                constexpr explicit(false) operator bool() const noexcept;  // NOLINT(misc-explicit-constructor)
                constexpr bool operator~() const noexcept;

                friend constexpr void swap(reference x, reference y) noexcept { bool t = x; x = y; y = t; }
                friend constexpr void swap(reference x,     bool& y) noexcept { bool t = x; x = y; y = t; }
                friend constexpr void swap(    bool& x, reference y) noexcept { bool t = x; x = y; y = t; }

                constexpr reference& flip() noexcept;
        };

        // Constructors                                            [bitset.cons]
        [[nodiscard]] constexpr bitset() noexcept = default;

        [[nodiscard]] constexpr explicit(false) bitset(unsigned long long val) noexcept = delete;       // TODO

        template<class charT, class traits, class Allocator>
        [[nodiscard]] constexpr explicit bitset(
                const std::basic_string<charT, traits, Allocator>& str,
                std::basic_string<charT, traits, Allocator>::size_type pos = 0,
                std::basic_string<charT, traits, Allocator>::size_type n = std::basic_string<charT, traits, Allocator>::npos,
                charT zero = charT('0'),
                charT one  = charT('1')
        )
        :
                bitset(std::basic_string_view<charT, traits>(str), pos, n, zero, one)
        {}

        template<class charT, class traits>
        [[nodiscard]] constexpr explicit bitset(
                std::basic_string_view<charT, traits> str,
                std::basic_string_view<charT, traits>::size_type pos = 0,
                std::basic_string_view<charT, traits>::size_type n = std::basic_string_view<charT, traits>::npos,
                charT zero = charT('0'),
                charT one  = charT('1')
        ) 
        {
                if (pos > str.size()) {
                        throw out_of_range(pos);
                }
                auto const rlen = std::ranges::min(n, str.size() - pos);
                auto const M = std::ranges::min(N, rlen);
                for (auto i : std::views::iota(0UZ, M)) {
                        auto const ch = str[pos + M - 1 - i];
                        if (traits::eq(ch, zero)) {
                                continue;
                        }
                        if (traits::eq(ch, one)) {
                                m_bits.set(i);
                        } else {
                                throw invalid_argument(ch, zero, one);
                        }
                }
        }

        template<class charT>
        [[nodiscard]] constexpr explicit bitset(
                const charT* str,
                std::basic_string_view<charT>::size_type n = std::basic_string_view<charT>::npos,
                charT zero = charT('0'),
                charT one  = charT('1')
        )
        :
                bitset(n == std::basic_string_view<charT>::npos ? std::basic_string_view<charT>(str) : std::basic_string_view<charT>(str, n), 0, n, zero, one)
        {}

        // Members                                              [bitset.members]
        constexpr bitset& operator&=(const bitset& rhs) noexcept { m_bits &= rhs.m_bits; return *this; }
        constexpr bitset& operator|=(const bitset& rhs) noexcept { m_bits |= rhs.m_bits; return *this; }
        constexpr bitset& operator^=(const bitset& rhs) noexcept { m_bits ^= rhs.m_bits; return *this; }
        constexpr bitset& operator-=(const bitset& rhs) noexcept { m_bits -= rhs.m_bits; return *this; }

        constexpr bitset& operator<<=(std::size_t pos) noexcept
        {
                if (pos < N) {
                        m_bits <<= pos;
                } else {
                        m_bits.reset();
                }
                return *this;
        }

        constexpr bitset& operator>>=(std::size_t pos) noexcept
        {
                if (pos < N) {
                        m_bits >>= pos;
                } else {
                        m_bits.reset();
                }
                return *this;
        }

        [[nodiscard]] constexpr bitset operator<<(std::size_t pos) const noexcept { auto nrv = *this; nrv <<= pos; return nrv; }
        [[nodiscard]] constexpr bitset operator>>(std::size_t pos) const noexcept { auto nrv = *this; nrv >>= pos; return nrv; }

        [[nodiscard]] constexpr bitset operator~() const noexcept { auto nrv = *this; nrv.flip(); return nrv; }

        constexpr bitset& set  () noexcept { m_bits.set  (); return *this; }
        constexpr bitset& reset() noexcept { m_bits.reset(); return *this; }
        constexpr bitset& flip () noexcept { m_bits.flip (); return *this; }

        constexpr bitset& set(std::size_t pos, bool val = true)
        {
                if (pos < N) {
                        if (val) {
                                m_bits.set(pos);
                        } else {
                                m_bits.reset(pos);
                        }
                        return *this;
                }
                throw out_of_range(pos);
        }

        constexpr bitset& reset(std::size_t pos)
        {
                if (pos < N) {
                        m_bits.reset(pos);
                        return *this;
                }
                throw out_of_range(pos);
        }

        constexpr bitset& flip(std::size_t pos)
        {
                if (pos < N) {
                        m_bits.flip(pos);
                        return *this;
                }
                throw out_of_range(pos);
        }

        [[nodiscard]] constexpr bool operator[](std::size_t pos) const noexcept
        {
                return m_bits[pos];
        }

        [[nodiscard]] constexpr reference operator[](std::size_t pos) = delete; // TODO

        [[nodiscard]] constexpr unsigned long      to_ulong()  const = delete;  // TODO
        [[nodiscard]] constexpr unsigned long long to_ullong() const = delete;  // TODO

        template<
                class charT = char,
                class traits = std::char_traits<charT>,
                class Allocator = std::allocator<charT>
        >
        [[nodiscard]] constexpr std::basic_string<charT, traits, Allocator> to_string(charT zero = charT('0'), charT one = charT('1')) const
        {
                auto str = std::basic_string<charT, traits, Allocator>(N, zero);
                for (auto i : std::views::iota(0UZ, N)) {
                        if (m_bits[N - 1 - i]) {
                                str[i] = one;
                        }
                }
                return str;
        }

        // observers
        [[nodiscard]] constexpr std::size_t count() const noexcept { return m_bits.count(); }
        [[nodiscard]] constexpr std::size_t size()  const noexcept { return m_bits.size();  }

        [[nodiscard]] constexpr bool operator==(const bitset& rhs) const noexcept = default;

        [[nodiscard]] constexpr bool test(std::size_t pos) const
        {
                if (pos < N) {
                        return m_bits[pos];
                }
                throw out_of_range(pos);
        }

        [[nodiscard]] constexpr bool all()  const noexcept { return m_bits.all();  }
        [[nodiscard]] constexpr bool any()  const noexcept { return m_bits.any();  }
        [[nodiscard]] constexpr bool none() const noexcept { return m_bits.none(); }

        [[nodiscard]] constexpr bool is_subset_of       (const bitset& rhs) const noexcept { return m_bits.is_subset_of       (rhs.m_bits); }
        [[nodiscard]] constexpr bool is_proper_subset_of(const bitset& rhs) const noexcept { return m_bits.is_proper_subset_of(rhs.m_bits); }
        [[nodiscard]] constexpr bool intersects         (const bitset& rhs) const noexcept { return m_bits.intersects         (rhs.m_bits); }

private:
        template<class charT>
        static constexpr auto invalid_argument(
                charT ch, charT zero = charT('0'), charT one = charT('1'),
                std::source_location const& loc = std::source_location::current()
        )
        {
                return std::invalid_argument(
                        std::format(
                                "{}:{}:{}: exception: ‘{}‘: invalid argument ‘ch‘ [{} != {} or {}]",
                                loc.file_name(), loc.line(), loc.column(), loc.function_name(), ch, zero, one
                        )
                );
        }

        static constexpr auto out_of_range(std::size_t pos, std::source_location const& loc = std::source_location::current())
        {
                return std::out_of_range(
                        std::format(
                                "{}:{}:{}: exception: ‘{}‘: argument ‘pos‘ is out of range [{} >= {}]",
                                loc.file_name(), loc.line(), loc.column(), loc.function_name(), pos, N
                        )
                );
        }
};

}       // namespace xstd

// Specializing set_find is what lets set_view(x) work, and it scans public operator[], claiming no privilege std::bitset lacks.
namespace xstd::ranges {

// As for std::bitset<N>: the width is in the type.
template<std::size_t N, xstd::unsigned_integer Block>
inline constexpr std::size_t bit_extent<xstd::bitset<N, Block>> = N;

template<std::size_t N, xstd::unsigned_integer Block>
struct set_find<xstd::bitset<N, Block>>
{
        [[nodiscard]] static constexpr std::size_t first(xstd::bitset<N, Block> const& c) noexcept
        {
                if constexpr (N == 0) {
                        return N;
                } else {
                        return *std::ranges::find_if(std::views::iota(0UZ, N), [&](auto i) {
                                return c[i];
                        });
                }
        }

        [[nodiscard]] static constexpr std::size_t last(xstd::bitset<N, Block> const&) noexcept
        {
                return N;
        }

        [[nodiscard]] static constexpr std::size_t next(xstd::bitset<N, Block> const& c, std::size_t n) noexcept
        {
                return *std::ranges::find_if(std::views::iota(n + 1, N), [&](auto i) {
                        return c[i];
                });
        }

        [[nodiscard]] static std::size_t prev(xstd::bitset<N, Block> const& c, std::size_t n) noexcept
        {
                assert(c.any());
                return *std::ranges::find_if(std::views::iota(0UZ, n) | std::views::reverse, [&](auto i) {
                        return c[i];
                });
        }
};

// The other reading, trivial because operator[] answers every position; without it we would be a set and not a sequence.
template<std::size_t N, xstd::unsigned_integer Block>
struct array_find<xstd::bitset<N, Block>>
{
        [[nodiscard]] static constexpr std::size_t first(xstd::bitset<N, Block> const&) noexcept { return 0UZ; }
        [[nodiscard]] static constexpr std::size_t last (xstd::bitset<N, Block> const&) noexcept { return N;   }
        [[nodiscard]] static constexpr bool         at  (xstd::bitset<N, Block> const& c, std::size_t n) noexcept { return c[n]; }
};

// No <=> of its own, so opt in to the iteration-based ordering explicitly, as std::bitset and dynamic_bitset do.
template<std::size_t N, xstd::unsigned_integer Block>
struct set_compare<xstd::bitset<N, Block>>
{
        [[nodiscard]] static constexpr std::strong_ordering lexicographical_three_way(xstd::bitset<N, Block> const& x, xstd::bitset<N, Block> const& y) noexcept
        {
                return set_three_way(set_view(x), set_view(y));
        }
};

}       // namespace xstd::ranges

namespace std {

// NOLINTBEGIN(bugprone-std-namespace-modification)

// bitset hash support [bitset.hash]; no redeclaration of std::hash's primary template, which [namespace.std] forbids.
template<size_t N, unsigned_integral Block>
struct hash<xstd::bitset<N, Block>>
{
        [[nodiscard]] constexpr std::size_t operator()(xstd::bitset<N, Block> const& v) const noexcept
        {
                boost::hash2::fnv1a_64 h;
                boost::hash2::hash_append(h, {}, v);
                return boost::hash2::get_integral_result<std::size_t>(h);
        }
};

// NOLINTEND(bugprone-std-namespace-modification)

}       // namespace std

namespace xstd {

// bitset operators                                           [bitset.operators]
template<std::size_t N, xstd::unsigned_integer Block> [[nodiscard]] constexpr bitset<N, Block> operator&(const bitset<N, Block>& lhs, const bitset<N, Block>& rhs) noexcept { auto nrv = lhs; nrv &= rhs; return nrv; }
template<std::size_t N, xstd::unsigned_integer Block> [[nodiscard]] constexpr bitset<N, Block> operator|(const bitset<N, Block>& lhs, const bitset<N, Block>& rhs) noexcept { auto nrv = lhs; nrv |= rhs; return nrv; }
template<std::size_t N, xstd::unsigned_integer Block> [[nodiscard]] constexpr bitset<N, Block> operator^(const bitset<N, Block>& lhs, const bitset<N, Block>& rhs) noexcept { auto nrv = lhs; nrv ^= rhs; return nrv; }
template<std::size_t N, xstd::unsigned_integer Block> [[nodiscard]] constexpr bitset<N, Block> operator-(const bitset<N, Block>& lhs, const bitset<N, Block>& rhs) noexcept { auto nrv = lhs; nrv -= rhs; return nrv; }

template<class charT, class traits, std::size_t N, xstd::unsigned_integer Block>
std::basic_istream<charT, traits>& operator>>(std::basic_istream<charT, traits>& is, bitset<N, Block>& x)
{
        auto str = std::basic_string<charT, traits>(N, is.widen('0'));
        auto state = std::ios_base::goodbit;
        charT ch;
        auto i = 0UZ;
        // One peek per character: peeking twice sets eofbit then failbit, failing a short but valid extraction ([bitset.operators]/6).
        while (i < N) {
                auto const next = is.peek();
                if (not traits::eq_int_type(next, is.widen('0')) and not traits::eq_int_type(next, is.widen('1'))) {
                        break;
                }
                is >> ch;
                if (traits::eq(ch, is.widen('1'))) {
                        str[i] = ch;
                }
                ++i;
        }
        x = bitset<N, Block>(str);
        if constexpr (N > 0) {
                if (i == 0) {
                        state |= std::ios_base::failbit;
                        is.setstate(state);
                }
        }
        return is;
}

template<class charT, class traits, std::size_t N, xstd::unsigned_integer Block>
std::basic_ostream<charT, traits>& operator<<(std::basic_ostream<charT, traits>& os, const bitset<N, Block>& x)
{
        return os << x.template to_string<charT, traits, std::allocator<charT>>(
                std::use_facet<std::ctype<charT>>(os.getloc()).widen('0'),
                std::use_facet<std::ctype<charT>>(os.getloc()).widen('1')
        );
}

}       // namespace xstd

#endif // XSTD_BITS_BITSET_HPP
