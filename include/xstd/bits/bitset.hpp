//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_BITS_BITSET_HPP
#define XSTD_BITS_BITSET_HPP

// Bitsets                                                              [bitset]
// Header <bitset> synopsis                                         [bitset.syn]

#include <string>       // basic_string, char_traits
#include <iosfwd>       // basic_istream, basic_ostream

#include <xstd/ints/concepts/unsigned_integer.hpp> // unsigned_integer
#include <cstddef>      // size_t

namespace xstd {

template<std::size_t N, xstd::unsigned_integer Block = std::size_t> class bitset;

template<std::size_t N, xstd::unsigned_integer Block> [[nodiscard]] constexpr bitset<N, Block> operator&(const bitset<N, Block>&, const bitset<N, Block>&) noexcept;
template<std::size_t N, xstd::unsigned_integer Block> [[nodiscard]] constexpr bitset<N, Block> operator|(const bitset<N, Block>&, const bitset<N, Block>&) noexcept;
template<std::size_t N, xstd::unsigned_integer Block> [[nodiscard]] constexpr bitset<N, Block> operator^(const bitset<N, Block>&, const bitset<N, Block>&) noexcept;
template<std::size_t N, xstd::unsigned_integer Block> [[nodiscard]] constexpr bitset<N, Block> operator-(const bitset<N, Block>&, const bitset<N, Block>&) noexcept;

template<class charT, class traits, std::size_t N, xstd::unsigned_integer Block> std::basic_istream<charT, traits>& operator>>(std::basic_istream<charT, traits>& is,       bitset<N, Block>& x);
template<class charT, class traits, std::size_t N, xstd::unsigned_integer Block> std::basic_ostream<charT, traits>& operator<<(std::basic_ostream<charT, traits>& os, const bitset<N, Block>& x);

}       // namespace xstd

#include <xstd/bits/detail/array.hpp>           // array
#include <xstd/bits/ranges/array_view.hpp> // array_find
#include <xstd/bits/ranges/set_view.hpp>   // set_find, set_compare
#include <boost/hash2/fnv1a.hpp>        // fnv1a_64
#include <boost/hash2/hash_append.hpp>  // hash_append
#include <algorithm>                    // lexicographical_compare_three_way
#include <cassert>                      // assert
#include <compare>                      // strong_ordering
#include <format>                       // format
#include <functional>                   // hash
#include <ios>                          // ios_base
#include <locale>                       // ctype, use_facet
#include <memory>                       // allocator
#include <ranges>                       // find_if, iota, reverse
#include <source_location>              // source_location
#include <string_view>                  // basic_string_view
#include <stdexcept>                    // invalid_argument, out_of_range, overflow_error

// Class template bitset                                       [template.bitset]
// General                                             [template.bitset.general]

namespace xstd {

template<std::size_t N, xstd::unsigned_integer Block>
class bitset
{
        // No iteration, no <=> here, by design: xstd::bitset mirrors
        // std::bitset's own contract, which has neither (std::bitset has no
        // begin()/end() and no operator<, only ==). Ordering/iteration for
        // an xstd::bitset, like for std::bitset itself (see ext/std/
        // bitset.hpp), is only available by explicitly choosing an
        // interpretation via xstd::set_view (set of the
        // indices that are set) or xstd::array_view
        // (fixed-length sequence of bools) - xstd::bitset itself takes no
        // side on which one is "the" ordering.
        detail::bits::array<N, Block> m_bits{};

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
                // A proxy reference assigns through a const proxy: the proxy is const,
                // the bit it refers to is not. This is the shape the standard gives
                // vector<bool>::reference, so the conventional signature is wrong here.
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

// xstd::bitset provides no find_first/find_last/find_next/find_prev of its
// own (see the comment on m_bits above) - specializing set_find<> here is what
// lets xstd::set_view(x) still work for it, the same
// mechanism ext/std/bitset.hpp uses for the real std::bitset<N>, just
// without that header's [namespace.std] motivation: here it's purely a
// deliberate choice to keep xstd::bitset itself opinion-free about ordering.
//
// This scans through xstd::bitset's own public operator[], the same way
// ext/std/bitset.hpp's set_find<std::bitset<N>> has to (it has no choice - it
// can't reach std::bitset's internals at all) - not through m_bits
// directly, even though this header could grant itself friend access to
// detail::bits::array's already-O(1) find_first/find_last/find_next/find_prev.
// xstd::bitset is meant to be nothing more than std::bitset reimplemented
// in terms of detail::bits::array: it shouldn't get a privileged, faster set_find<> that
// std::bitset itself has no way of also getting.
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

// The other reading, and trivial for the same reason it is trivial in
// ext/std/bitset.hpp: operator[] already answers every position, so unlike the
// set reading there is no bit-scanning to arrange. Without this xstd::bitset
// would be readable as a set and not as a sequence, where std::bitset -- the
// type it reproduces -- is readable as both. The whole claim of this header is
// that reimplementing std::bitset over detail::bits::array gives up nothing, and
// a missing reading is something given up.
template<std::size_t N, xstd::unsigned_integer Block>
struct array_find<xstd::bitset<N, Block>>
{
        [[nodiscard]] static constexpr std::size_t first(xstd::bitset<N, Block> const&) noexcept { return 0UZ; }
        [[nodiscard]] static constexpr std::size_t last (xstd::bitset<N, Block> const&) noexcept { return N;   }
        [[nodiscard]] static constexpr bool         at  (xstd::bitset<N, Block> const& c, std::size_t n) noexcept { return c[n]; }
};

// xstd::bitset has no <=> of its own either (by the same design choice), so
// set_set_compare<Bits>'s default (trust Bits' own <=>) doesn't apply - opt in to
// the safe, iteration-based ordering explicitly, same as std::bitset<N> and
// boost::dynamic_bitset<> do in their own headers.
template<std::size_t N, xstd::unsigned_integer Block>
struct set_compare<xstd::bitset<N, Block>>
{
        [[nodiscard]] static constexpr std::strong_ordering lexicographical_three_way(xstd::bitset<N, Block> const& x, xstd::bitset<N, Block> const& y) noexcept
        {
                auto const xv = set_view(x);
                auto const yv = set_view(y);
                return std::lexicographical_compare_three_way(
                        xv.begin(), xv.end(),
                        yv.begin(), yv.end()
                );
        }
};

}       // namespace xstd::ranges

namespace std {

// NOLINTBEGIN(bugprone-std-namespace-modification)

// bitset hash support                                             [bitset.hash]
//
// No "template<class T> struct hash;" forward declaration here: std::hash's
// primary template is already declared by <functional> (below), and
// redeclaring it ourselves is the same [namespace.std] problem this
// codebase has spent a lot of effort avoiding elsewhere (find_first et al.
// in ext/std/bitset.hpp) - it just happened to go unnoticed here, since
// libstdc++'s declaration of the primary template is compatible enough with
// a redeclaration to not conflict, while libc++'s isn't (confirmed: this
// redeclaration makes "hash" ambiguous under AppleClang's libc++). A full/
// partial specialization of std::hash for a user type is explicitly
// sanctioned by the standard either way - only the primary template's own
// declaration is off-limits to add ourselves.
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
        // One peek per character, held in next. Peeking twice cost a correct
        // extraction its stream state: the first peek past the end sets
        // eofbit, and the second then builds a sentry on a stream that is no
        // longer good, which sets failbit. A short but valid input came back
        // failed, when only an empty one should ([bitset.operators]/6).
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
