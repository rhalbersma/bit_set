#ifndef XSTD_BITSET_HPP
#define XSTD_BITSET_HPP

//          Copyright Rein Halbersma 2014-2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Bitsets                                                              [bitset]
// Header <bitset> synopsis                                         [bitset.syn]

#include <string>       // basic_string, char_traits
#include <iosfwd>       // basic_istream, basic_ostream

#include <concepts>     // unsigned_integral
#include <cstddef>      // size_t

namespace xstd {

template<std::size_t N, std::unsigned_integral Block = std::size_t> class bitset;

template<std::size_t N, std::unsigned_integral Block> [[nodiscard]] constexpr bitset<N, Block> operator&(const bitset<N, Block>&, const bitset<N, Block>&) noexcept;
template<std::size_t N, std::unsigned_integral Block> [[nodiscard]] constexpr bitset<N, Block> operator|(const bitset<N, Block>&, const bitset<N, Block>&) noexcept;
template<std::size_t N, std::unsigned_integral Block> [[nodiscard]] constexpr bitset<N, Block> operator^(const bitset<N, Block>&, const bitset<N, Block>&) noexcept;
template<std::size_t N, std::unsigned_integral Block> [[nodiscard]] constexpr bitset<N, Block> operator-(const bitset<N, Block>&, const bitset<N, Block>&) noexcept;

template<class charT, class traits, std::size_t N, std::unsigned_integral Block> std::basic_istream<charT, traits>& operator>>(std::basic_istream<charT, traits>& is,       bitset<N, Block>& x);
template<class charT, class traits, std::size_t N, std::unsigned_integral Block> std::basic_ostream<charT, traits>& operator<<(std::basic_ostream<charT, traits>& os, const bitset<N, Block>& x);

}       // namespace xstd

#include <xstd/bit/array.hpp>           // array
#include <xstd/proxy/bidirectional.hpp> // find
#include <boost/hash2/fnv1a.hpp>        // fnv1a_64
#include <boost/hash2/hash_append.hpp>  // hash_append
#include <algorithm>                    // lexicographical_compare_three_way
#include <compare>                      // strong_ordering
#include <format>                       // format
#include <ios>                          // ios_base
#include <locale>                       // ctype, use_facet
#include <memory>                       // allocator
#include <ranges>                       // iota
#include <source_location>              // source_location
#include <string_view>                  // basic_string_view
#include <stdexcept>                    // invalid_argument, out_of_range, overflow_error

// Class template bitset                                       [template.bitset]
// General                                             [template.bitset.general]

namespace xstd {

template<std::size_t N, std::unsigned_integral Block>
class bitset
{
        // No iteration, no <=> here, by design: xstd::bitset mirrors
        // std::bitset's own contract, which has neither (std::bitset has no
        // begin()/end() and no operator<, only ==). Ordering/iteration for
        // an xstd::bitset, like for std::bitset itself (see ext/std/
        // bitset.hpp), is only available by explicitly choosing an
        // interpretation via xstd::proxy::bidirectional::view (set of the
        // indices that are set) or xstd::proxy::random_access::view
        // (fixed-length sequence of bools) - xstd::bitset itself takes no
        // side on which one is "the" ordering.
        bit::array<N, Block> m_bits{};

        friend struct xstd::proxy::bidirectional::find<bitset>;

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
                constexpr const reference& operator=(bool x) const noexcept;
                constexpr explicit(false) operator bool() const noexcept;
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
                typename std::basic_string<charT, traits, Allocator>::size_type pos = 0,
                typename std::basic_string<charT, traits, Allocator>::size_type n = std::basic_string<charT, traits, Allocator>::npos,
                charT zero = charT('0'),
                charT one  = charT('1')
        )
        :
                bitset(std::basic_string_view<charT, traits>(str), pos, n, zero, one)
        {}

        template<class charT, class traits>
        [[nodiscard]] constexpr explicit bitset(
                std::basic_string_view<charT, traits> str,
                typename std::basic_string_view<charT, traits>::size_type pos = 0,
                typename std::basic_string_view<charT, traits>::size_type n = std::basic_string_view<charT, traits>::npos,
                charT zero = charT('0'),
                charT one  = charT('1')
        ) 
        {
                if (pos > str.size()) {
                        throw out_of_range(pos);
                }
                auto const rlen = std::ranges::min(n, str.size() - pos);
                auto const M = std::ranges::min(N, rlen);
                for (auto i : std::views::iota(0uz, M)) {
                        auto const ch = str[pos + M - 1 - i];
                        if (traits::eq(ch, zero)) {
                                continue;
                        } else if (traits::eq(ch, one)) {
                                m_bits.set(i);
                        } else {
                                throw invalid_argument(ch, zero, one);
                        }
                }
        }

        template<class charT>
        [[nodiscard]] constexpr explicit bitset(
                const charT* str,
                typename std::basic_string_view<charT>::size_type n = std::basic_string_view<charT>::npos,
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

        constexpr bitset& operator<<=(std::size_t pos) noexcept { if (pos < N) { m_bits <<= pos; } else { m_bits.reset(); } return *this; }
        constexpr bitset& operator>>=(std::size_t pos) noexcept { if (pos < N) { m_bits >>= pos; } else { m_bits.reset(); } return *this; }

        [[nodiscard]] constexpr bitset operator<<(std::size_t pos) const noexcept { auto nrv = *this; nrv <<= pos; return nrv; }
        [[nodiscard]] constexpr bitset operator>>(std::size_t pos) const noexcept { auto nrv = *this; nrv >>= pos; return nrv; }

        [[nodiscard]] constexpr bitset operator~() const noexcept { auto nrv = *this; nrv.flip(); return nrv; }

        constexpr bitset& set  () noexcept { m_bits.set  (); return *this; }
        constexpr bitset& reset() noexcept { m_bits.reset(); return *this; }
        constexpr bitset& flip () noexcept { m_bits.flip (); return *this; }

        constexpr bitset& set  (std::size_t pos, bool val = true) { if (pos < N) { if (val) m_bits.set  (pos); else m_bits.reset(pos); return *this; } else { throw out_of_range(pos); } }
        constexpr bitset& reset(std::size_t pos)                  { if (pos < N) {          m_bits.reset(pos);                         return *this; } else { throw out_of_range(pos); } }
        constexpr bitset& flip (std::size_t pos)                  { if (pos < N) {          m_bits.flip (pos);                         return *this; } else { throw out_of_range(pos); } }

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
                for (auto i : std::views::iota(0uz, N)) {
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

        [[nodiscard]] constexpr bool test(std::size_t pos) const { if (pos < N) { return m_bits[pos]; } else { throw out_of_range(pos); } }

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
// own (see the comment on m_bits above) - specializing find<> here is what
// lets xstd::proxy::bidirectional::view(x) still work for it, the same
// mechanism ext/std/bitset.hpp uses for the real std::bitset<N>, just
// without that header's [namespace.std] motivation: here it's purely a
// deliberate choice to keep xstd::bitset itself opinion-free about ordering.
namespace xstd::proxy::bidirectional {

template<std::size_t N, std::unsigned_integral Block>
struct find<xstd::bitset<N, Block>>
{
        [[nodiscard]] static constexpr std::size_t first(xstd::bitset<N, Block> const& c) noexcept { return c.m_bits.find_first(); }
        [[nodiscard]] static constexpr std::size_t last (xstd::bitset<N, Block> const& c) noexcept { return c.m_bits.find_last (); }
        [[nodiscard]] static constexpr std::size_t next (xstd::bitset<N, Block> const& c, std::size_t n) noexcept { return c.m_bits.find_next(n); }
        [[nodiscard]] static constexpr std::size_t prev (xstd::bitset<N, Block> const& c, std::size_t n) noexcept { return c.m_bits.find_prev(n); }
};

// xstd::bitset has no <=> of its own either (by the same design choice), so
// compare<Bits>'s default (trust Bits' own <=>) doesn't apply - opt in to
// the safe, iteration-based ordering explicitly, same as std::bitset<N> and
// boost::dynamic_bitset<> do in their own headers.
template<std::size_t N, std::unsigned_integral Block>
struct compare<xstd::bitset<N, Block>>
{
        [[nodiscard]] static constexpr std::strong_ordering lexicographical_three_way(xstd::bitset<N, Block> const& x, xstd::bitset<N, Block> const& y) noexcept
        {
                auto const xv = view(x);
                auto const yv = view(y);
                return std::lexicographical_compare_three_way(
                        xv.begin(), xv.end(),
                        yv.begin(), yv.end()
                );
        }
};

}       // namespace xstd::proxy::bidirectional

namespace std {

// bitset hash support                                             [bitset.hash]
template<class T> 
struct hash;

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

}       // namespace std

namespace xstd {

// bitset operators                                           [bitset.operators]
template<std::size_t N, std::unsigned_integral Block> [[nodiscard]] constexpr bitset<N, Block> operator&(const bitset<N, Block>& lhs, const bitset<N, Block>& rhs) noexcept { auto nrv = lhs; nrv &= rhs; return nrv; }
template<std::size_t N, std::unsigned_integral Block> [[nodiscard]] constexpr bitset<N, Block> operator|(const bitset<N, Block>& lhs, const bitset<N, Block>& rhs) noexcept { auto nrv = lhs; nrv |= rhs; return nrv; }
template<std::size_t N, std::unsigned_integral Block> [[nodiscard]] constexpr bitset<N, Block> operator^(const bitset<N, Block>& lhs, const bitset<N, Block>& rhs) noexcept { auto nrv = lhs; nrv ^= rhs; return nrv; }
template<std::size_t N, std::unsigned_integral Block> [[nodiscard]] constexpr bitset<N, Block> operator-(const bitset<N, Block>& lhs, const bitset<N, Block>& rhs) noexcept { auto nrv = lhs; nrv -= rhs; return nrv; }

template<class charT, class traits, std::size_t N, std::unsigned_integral Block>
std::basic_istream<charT, traits>& operator>>(std::basic_istream<charT, traits>& is, bitset<N, Block>& x)
{
        auto str = std::basic_string<charT, traits>(N, is.widen('0'));
        auto state = std::ios_base::goodbit;
        charT ch;
        auto i = 0uz;
        while (i < N and not is.eof() and (traits::eq_int_type(is.peek(), is.widen('0')) or traits::eq_int_type(is.peek(), is.widen('1')))) {
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

template<class charT, class traits, std::size_t N, std::unsigned_integral Block>
std::basic_ostream<charT, traits>& operator<<(std::basic_ostream<charT, traits>& os, const bitset<N, Block>& x)
{
        return os << x.template to_string<charT, traits, std::allocator<charT>>(
                std::use_facet<std::ctype<charT>>(os.getloc()).widen('0'),
                std::use_facet<std::ctype<charT>>(os.getloc()).widen('1')
        );
}

}       // namespace xstd

#endif  // include guard
