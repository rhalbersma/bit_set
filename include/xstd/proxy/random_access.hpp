#ifndef XSTD_PROXY_RANDOM_ACCESS_HPP
#define XSTD_PROXY_RANDOM_ACCESS_HPP

//          Copyright Rein Halbersma 2014-2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>    // equal
#include <cassert>      // assert
#include <compare>      // strong_ordering
#include <concepts>     // constructible_from, convertible_to
#include <cstddef>      // ptrdiff_t, size_t
#include <iterator>     // make_reverse_iterator, random_access_iterator_tag
#include <ranges>       // view_base
#include <type_traits>  // is_class_v, is_convertible_v, is_nothrow_constructible_v

namespace xstd::proxy::random_access {

// Same rationale as xstd::proxy::bidirectional::find (see its comments):
// Bits customizes its iteration either by providing hidden friends
// find_first/find_last/find_at discoverable via ADL (the default below), or
// by giving find<Bits> an explicit specialization for a foreign type that
// cannot provide those via ADL (e.g. std::bitset<N>, whose only associated
// namespace is std, where a program may not add declarations
// [namespace.std]). Each member is individually constrained on the
// underlying ADL call actually being valid, for the same reason bidirectional
// ::find's members are: an unconstrained declaration would make find<Bits>::
// first(c) well-formed for every Bits regardless of whether the body
// actually compiles, breaking bit_range's SFINAE.
template<class Bits>
struct find
{
        [[nodiscard]] static constexpr std::size_t first(Bits const& c) noexcept
                requires requires { { find_first(c) } -> std::convertible_to<std::size_t>; }
        {
                return find_first(c);
        }

        [[nodiscard]] static constexpr std::size_t last(Bits const& c) noexcept
                requires requires { { find_last(c) } -> std::convertible_to<std::size_t>; }
        {
                return find_last(c);
        }

        [[nodiscard]] static constexpr bool at(Bits const& c, std::size_t n) noexcept
                requires requires { { find_at(c, n) } -> std::convertible_to<bool>; }
        {
                return find_at(c, n);
        }
};

template<class Bits>
concept bit_range =
        requires(Bits const& c)
        {
                { find<Bits>::first(c) } -> std::convertible_to<std::size_t>;
                { find<Bits>::last (c) } -> std::convertible_to<std::size_t>;
        } and
        requires(Bits const& c, std::size_t n)
        {
                { find<Bits>::at(c, n) } -> std::convertible_to<bool>;
        }
;

template<bit_range, bool> class iterator;
template<bit_range, bool> class reference;
template<bit_range> struct compare;

template<bit_range Bits, bool IsConst>
class iterator
{
        using ptr_const_t = std::conditional_t<IsConst, Bits const*, Bits*>;

        ptr_const_t m_ptr{};
        std::size_t m_idx{};

        friend Bits;
        friend class reference<Bits, IsConst>;
        friend constexpr auto begin <>(      Bits& c) noexcept -> iterator<Bits, false>;
        friend constexpr auto begin <>(const Bits& c) noexcept -> iterator<Bits, true >;
        friend constexpr auto end   <>(      Bits& c) noexcept -> iterator<Bits, false>;
        friend constexpr auto end   <>(const Bits& c) noexcept -> iterator<Bits, true >;

        [[nodiscard]] constexpr iterator(ptr_const_t ptr, std::size_t idx) noexcept
        :
                m_ptr(ptr),
                m_idx(idx)
        {
                assert(m_ptr != nullptr);
        }

public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type        = bool;
        using difference_type   = std::ptrdiff_t;
        using pointer           = void;
        using reference         = random_access::reference<Bits, IsConst>;

        [[nodiscard]] constexpr iterator() noexcept = default;

        [[nodiscard]] friend constexpr bool operator==(iterator lhs, iterator rhs) noexcept
        {
                assert(lhs.m_ptr == rhs.m_ptr);
                return lhs.m_idx == rhs.m_idx;
        }

        [[nodiscard]] friend constexpr auto operator<=>(iterator lhs, iterator rhs) noexcept
                -> std::strong_ordering
        {
                assert(lhs.m_ptr == rhs.m_ptr);
                return lhs.m_idx <=> rhs.m_idx;            
        }

        [[nodiscard]] constexpr reference operator*() const noexcept
        {
                assert(m_ptr != nullptr);
                return { *m_ptr, m_idx };
        }

        constexpr iterator& operator++() noexcept { ++m_idx; return *this; }
        constexpr iterator& operator--() noexcept { --m_idx; return *this; }

        constexpr iterator operator++(int) noexcept { auto nrv = *this; ++*this; return nrv; }
        constexpr iterator operator--(int) noexcept { auto nrv = *this; --*this; return nrv; }

        constexpr iterator& operator+=(difference_type n) noexcept { m_idx += n; return *this; }
        constexpr iterator& operator-=(difference_type n) noexcept { m_idx -= n; return *this; }

        [[nodiscard]] friend constexpr difference_type operator-(iterator lhs, iterator rhs) noexcept
        { 
                return static_cast<difference_type>(lhs.m_idx) - static_cast<difference_type>(rhs.m_idx); 
        }

        [[nodiscard]] constexpr reference operator[](difference_type n) const noexcept
        {
                assert(m_ptr != nullptr); 
                return { *m_ptr, m_idx + n }; 
        }
};

template<bit_range Bits, bool IsConst> [[nodiscard]] constexpr iterator<Bits, IsConst> operator+(iterator<Bits, IsConst> lhs, std::ptrdiff_t n           ) noexcept { auto nrv = lhs; nrv += n; return nrv; }
template<bit_range Bits, bool IsConst> [[nodiscard]] constexpr iterator<Bits, IsConst> operator+(std::ptrdiff_t n           , iterator<Bits, IsConst> rhs) noexcept { auto nrv = rhs; nrv += n; return nrv; }
template<bit_range Bits, bool IsConst> [[nodiscard]] constexpr iterator<Bits, IsConst> operator-(iterator<Bits, IsConst> lhs, std::ptrdiff_t n           ) noexcept { auto nrv = lhs; nrv -= n; return nrv; }

template<bit_range Bits> [[nodiscard]] constexpr iterator<Bits, false> begin(      Bits& c) noexcept { return { &c, find<Bits>::first(c) }; }
template<bit_range Bits> [[nodiscard]] constexpr iterator<Bits, true > begin(const Bits& c) noexcept { return { &c, find<Bits>::first(c) }; }
template<bit_range Bits> [[nodiscard]] constexpr iterator<Bits, false> end  (      Bits& c) noexcept { return { &c, find<Bits>::last (c) }; }
template<bit_range Bits> [[nodiscard]] constexpr iterator<Bits, true > end  (const Bits& c) noexcept { return { &c, find<Bits>::last (c) }; }

template<bit_range Bits, bool IsConst>
class reference
{
        using ref_const_t = std::conditional_t<IsConst, Bits const&, Bits&>;

        ref_const_t m_ref;
        std::size_t m_idx;

        friend Bits;
        friend class iterator<Bits, IsConst>;

        [[nodiscard]] constexpr reference(ref_const_t ref, std::size_t idx) noexcept
        :
                m_ref(ref),
                m_idx(idx)
        {}

public:
        using value_type = bool;
        using iterator   = random_access::iterator<Bits, IsConst>;

        [[nodiscard]] constexpr iterator operator&() const noexcept
        {
                return { &m_ref, m_idx };
        }

        [[nodiscard]] constexpr explicit(false) operator value_type() const noexcept
        {
                return find<Bits>::at(m_ref, m_idx);
        }

        template<std::constructible_from<value_type> T>
        [[nodiscard]] constexpr explicit(not std::is_convertible_v<value_type, T>) operator T() const noexcept(std::is_nothrow_constructible_v<T, value_type>)
                requires std::is_class_v<T>
        {
                return find<Bits>::at(m_ref, m_idx);
        }
};

template<bit_range Bits, bool IsConst>
[[nodiscard]] constexpr auto format_as(reference<Bits, IsConst> ref) noexcept
        -> reference<Bits, IsConst>::value_type
{
        return ref;
}

// A non-owning adaptor with member begin()/end(), for the same reasons as
// xstd::proxy::bidirectional::view: lets a foreign type's own members (or a
// future addition of them) not interfere, and keeps view assignable (see
// bidirectional::view's comment on why it stores Bits by pointer, not
// reference - the same std::movable/view-concept requirement applies here).
//
// Unlike bidirectional::view, this deliberately has no key_type: it
// represents a fixed-length sequence of bools (index 0 first), not a set,
// so it should format like a plain range ([t, f, t, ...]), which fmt's
// range formatter already does for anything without a nested key_type.
template<bit_range Bits>
class view : public std::ranges::view_base
{
        Bits const* m_ptr;

public:
        [[nodiscard]] constexpr explicit view(Bits const& c) noexcept : m_ptr(&c) {}

        [[nodiscard]] constexpr auto begin() const noexcept { return random_access::begin(*m_ptr); }
        [[nodiscard]] constexpr auto end()   const noexcept { return random_access::end  (*m_ptr); }
        [[nodiscard]] constexpr auto rbegin() const noexcept { return std::make_reverse_iterator(end());   }
        [[nodiscard]] constexpr auto rend()   const noexcept { return std::make_reverse_iterator(begin()); }

        [[nodiscard]] constexpr auto cbegin()  const noexcept { return begin();  }
        [[nodiscard]] constexpr auto cend()    const noexcept { return end();    }
        [[nodiscard]] constexpr auto crbegin() const noexcept { return rbegin(); }
        [[nodiscard]] constexpr auto crend()   const noexcept { return rend();   }

        // Prefer Bits' own == when it has one, else compare elementwise -
        // same reasoning as bidirectional::view::operator==: equality of the
        // underlying bits is unambiguous regardless of interpretation.
        [[nodiscard]] friend constexpr bool operator==(view lhs, view rhs) noexcept
        {
                if constexpr (requires { *lhs.m_ptr == *rhs.m_ptr; }) {
                        return *lhs.m_ptr == *rhs.m_ptr;
                } else {
                        return std::ranges::equal(lhs, rhs);
                }
        }

        // Always goes through compare<Bits> - see its comments for why the
        // default trusts Bits' own <=> and why some Bits need to opt out via
        // a compare<Bits> specialization.
        [[nodiscard]] friend constexpr std::strong_ordering operator<=>(view lhs, view rhs) noexcept
        {
                return compare<Bits>::lexicographical_three_way(*lhs.m_ptr, *rhs.m_ptr);
        }
};

template<bit_range Bits>
view(Bits const&) -> view<Bits>;

// compare<Bits>::lexicographical_three_way defaults to trusting Bits' own
// <=>, meant for xstd's own bit_array (whose <=> already computes the
// fixed-length sequence-of-bool order this namespace's iteration exposes).
// This default is NOT safe for an arbitrary foreign Bits - see
// xstd::proxy::bidirectional::compare's comments for the full reasoning,
// which applies identically here: a foreign type may have no <=> at all
// (std::bitset<N>), or one with different semantics, and must opt out via
// an explicit compare<Bits> specialization instead (see ext/std/bitset.hpp,
// ext/boost/dynamic_bitset.hpp).
template<bit_range Bits>
struct compare
{
        [[nodiscard]] static constexpr std::strong_ordering lexicographical_three_way(Bits const& x, Bits const& y) noexcept
        {
                return x <=> y;
        }
};

}       // namespace xstd::proxy::random_access

#endif  // include guard
