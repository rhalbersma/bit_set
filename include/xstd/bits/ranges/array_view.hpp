//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_BITS_RANGES_ARRAY_VIEW_HPP
#define XSTD_BITS_RANGES_ARRAY_VIEW_HPP

#include <xstd/bits/ranges/bit_extent.hpp> // bit_extent, static_bit_extent
#include <algorithm>                       // equal, lexicographical_compare_three_way
#include <cassert>                         // assert
#include <compare>                         // strong_ordering
#include <concepts>                        // constructible_from, convertible_to
#include <cstddef>                         // ptrdiff_t, size_t
#include <iterator>                        // make_reverse_iterator, random_access_iterator_tag, reverse_iterator
#include <ranges>                          // view_base
#include <span>                            // dynamic_extent
#include <stdexcept>                       // out_of_range
#include <type_traits>                     // conditional_t, is_class_v, is_const_v, is_convertible_v, is_nothrow_constructible_v, remove_const_t
#include <utility>                         // as_const

// A sequence of bool over bits it does not own - std::span's shape, not
// std::array's and not std::vector's.
//
// One template rather than an array_view/vector_view pair, for the reason span is
// one template: a fixed-width and a dynamic bit sequence differ only in whether
// size() is a constant expression, and an Extent parameter says that in one place
// instead of duplicating twenty-five members to vary one of them.
//
// The extent is not the caller's to supply. std::bitset<N> and xstd::bitset<N,
// Block> carry their width in the type and boost::dynamic_bitset<> carries it in
// the object; which of the two a Bits is, is the Bits' business, so it arrives
// through bit_extent<Bits> and the default argument below.
//
// Mutable through, unlike the read-only adaptor this replaces: array_reference
// assigns back into the bits, so std::ranges algorithms that write work on it.
namespace xstd::ranges {

// Where the bits are: the two ends of the sequence and the value at a position.
// Same ADL-with-explicit-specialization design as set_find, for the same reason -
// see xstd/bits/ranges/set_view.hpp, whose comment explains both the [namespace.std]
// problem and why every member is individually constrained.
template<class Bits>
struct array_find
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

// The vocabulary, rewired - the sequence half of what set_ops does for the set
// half. A bitset's size() is already a sequence's size(), so only writing needs
// renaming: set(pos, value) is v[pos] = value, and set()/reset() are fill().
template<class Bits>
struct array_ops
{
        [[nodiscard]] static constexpr std::size_t size(Bits const& c) noexcept
        {
                if constexpr (static_bit_extent<Bits>) {
                        return bit_extent<Bits>;
                } else {
                        return c.size();
                }
        }

        // The write side of array_find::at, and deliberately without an
        // operator[] fallback. A type whose operator[] returns a proxy of ours -
        // xstd::bit_array does - would take `c[n] = value` straight back into
        // array_reference::operator=, which calls this, which calls that: it
        // compiles, and recurses until the stack is gone. So the only ways in are
        // a real set(pos, value) member and the ADL hook, and a type offering
        // neither is a compile error rather than a silent one at run time.
        static constexpr void assign(Bits& c, std::size_t n, bool value) noexcept
                requires requires { c.set(n, value); }
        {
                c.set(n, value);
        }

        static constexpr void assign(Bits& c, std::size_t n, bool value) noexcept
                requires (not requires { c.set(n, value); }) and requires { assign_at(c, n, value); }
        {
                assign_at(c, n, value);
        }

        static constexpr void fill(Bits& c, bool value) noexcept
        {
                if constexpr (requires { c.fill(value); }) {
                        c.fill(value);
                } else if (value) {
                        c.set();
                } else {
                        c.reset();
                }
        }
};

template<class Bits_cv, class Bits = std::remove_const_t<Bits_cv>>
concept array_range =
        requires(Bits const& c)
        {
                { array_find<Bits>::first(c) } -> std::convertible_to<std::size_t>;
                { array_find<Bits>::last (c) } -> std::convertible_to<std::size_t>;
        } and
        requires(Bits const& c, std::size_t n)
        {
                { array_find<Bits>::at(c, n) } -> std::convertible_to<bool>;
        }
;

template<array_range, bool> class array_iterator;
template<array_range, bool> class array_reference;

// Forward-declared so the dependent friend template-id declarations inside
// array_iterator have a template to refer to - see set_view.hpp for why Clang
// requires this and GCC does not.
template<array_range Bits> [[nodiscard]] constexpr array_iterator<Bits, false> array_begin(      Bits& c) noexcept;
template<array_range Bits> [[nodiscard]] constexpr array_iterator<Bits, true > array_begin(Bits const& c) noexcept;
template<array_range Bits> [[nodiscard]] constexpr array_iterator<Bits, false> array_end  (      Bits& c) noexcept;
template<array_range Bits> [[nodiscard]] constexpr array_iterator<Bits, true > array_end  (Bits const& c) noexcept;

template<array_range Bits, bool IsConst>
class array_iterator
{
        using ptr_const_t = std::conditional_t<IsConst, Bits const*, Bits*>;

        ptr_const_t m_ptr{};
        std::size_t m_idx{};

        friend Bits;
        friend class array_reference<Bits, IsConst>;
        friend constexpr auto array_begin <>(      Bits& c) noexcept -> array_iterator<Bits, false>;
        friend constexpr auto array_begin <>(Bits const& c) noexcept -> array_iterator<Bits, true >;
        friend constexpr auto array_end   <>(      Bits& c) noexcept -> array_iterator<Bits, false>;
        friend constexpr auto array_end   <>(Bits const& c) noexcept -> array_iterator<Bits, true >;

        [[nodiscard]] constexpr array_iterator(ptr_const_t ptr, std::size_t idx) noexcept
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
        using reference         = array_reference<Bits, IsConst>;

        [[nodiscard]] constexpr array_iterator() noexcept = default;

        [[nodiscard]] friend constexpr bool operator==(array_iterator lhs, array_iterator rhs) noexcept
        {
                assert(lhs.m_ptr == rhs.m_ptr);
                return lhs.m_idx == rhs.m_idx;
        }

        [[nodiscard]] friend constexpr auto operator<=>(array_iterator lhs, array_iterator rhs) noexcept
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

        constexpr array_iterator& operator++() noexcept { ++m_idx; return *this; }
        constexpr array_iterator& operator--() noexcept { --m_idx; return *this; }

        constexpr array_iterator operator++(int) noexcept { auto nrv = *this; ++*this; return nrv; }
        constexpr array_iterator operator--(int) noexcept { auto nrv = *this; --*this; return nrv; }

        constexpr array_iterator& operator+=(difference_type n) noexcept { m_idx = static_cast<std::size_t>(static_cast<difference_type>(m_idx) + n); return *this; }
        constexpr array_iterator& operator-=(difference_type n) noexcept { m_idx = static_cast<std::size_t>(static_cast<difference_type>(m_idx) - n); return *this; }

        [[nodiscard]] friend constexpr difference_type operator-(array_iterator lhs, array_iterator rhs) noexcept
        {
                return static_cast<difference_type>(lhs.m_idx) - static_cast<difference_type>(rhs.m_idx);
        }

        [[nodiscard]] constexpr reference operator[](difference_type n) const noexcept
        {
                assert(m_ptr != nullptr);
                return { *m_ptr, static_cast<std::size_t>(static_cast<difference_type>(m_idx) + n) };
        }
};

template<array_range Bits, bool IsConst> [[nodiscard]] constexpr array_iterator<Bits, IsConst> operator+(array_iterator<Bits, IsConst> lhs, std::ptrdiff_t n) noexcept { auto nrv = lhs; nrv += n; return nrv; }
template<array_range Bits, bool IsConst> [[nodiscard]] constexpr array_iterator<Bits, IsConst> operator+(std::ptrdiff_t n, array_iterator<Bits, IsConst> rhs) noexcept { auto nrv = rhs; nrv += n; return nrv; }
template<array_range Bits, bool IsConst> [[nodiscard]] constexpr array_iterator<Bits, IsConst> operator-(array_iterator<Bits, IsConst> lhs, std::ptrdiff_t n) noexcept { auto nrv = lhs; nrv -= n; return nrv; }

template<array_range Bits> [[nodiscard]] constexpr array_iterator<Bits, false> array_begin(      Bits& c) noexcept { return { &c, array_find<Bits>::first(c) }; }
template<array_range Bits> [[nodiscard]] constexpr array_iterator<Bits, true > array_begin(Bits const& c) noexcept { return { &c, array_find<Bits>::first(c) }; }
template<array_range Bits> [[nodiscard]] constexpr array_iterator<Bits, false> array_end  (      Bits& c) noexcept { return { &c, array_find<Bits>::last (c) }; }
template<array_range Bits> [[nodiscard]] constexpr array_iterator<Bits, true > array_end  (Bits const& c) noexcept { return { &c, array_find<Bits>::last (c) }; }

// A proxy bool. The non-const instantiation assigns back into the bits, which is
// what makes this a span rather than a string_view: std::ranges::fill and friends
// work through it. std::vector<bool>::reference is the precedent, down to
// assignment being const-qualified because it writes through the proxy, not to it.
template<array_range Bits, bool IsConst>
class array_reference
{
        using ref_const_t = std::conditional_t<IsConst, Bits const&, Bits&>;

        ref_const_t m_ref;
        std::size_t m_idx;

        friend Bits;
        friend class array_iterator<Bits, IsConst>;

        [[nodiscard]] constexpr array_reference(ref_const_t ref, std::size_t idx) noexcept
        :
                m_ref(ref),
                m_idx(idx)
        {}

public:
        using value_type = bool;
        using iterator   = array_iterator<Bits, IsConst>;

        [[nodiscard]] constexpr iterator operator&() const noexcept
        {
                return { &m_ref, m_idx };
        }

        [[nodiscard]] constexpr explicit(false) operator value_type() const noexcept  // NOLINT(misc-explicit-constructor)
        {
                return array_find<Bits>::at(m_ref, m_idx);
        }

        template<std::constructible_from<value_type> T>
        [[nodiscard]] constexpr explicit(not std::is_convertible_v<value_type, T>) operator T() const noexcept(std::is_nothrow_constructible_v<T, value_type>)  // NOLINT(misc-explicit-constructor)
                requires std::is_class_v<T>
        {
                return array_find<Bits>::at(m_ref, m_idx);
        }

        // const-qualified and returning a const reference, which is what a proxy
        // reference is: the assignment writes through the proxy rather than to
        // it, so the proxy itself need not be modifiable. C++23 gives
        // std::vector<bool>::reference exactly this overload, added by P2321R2
        // so that a prvalue proxy coming out of a view's operator* is still
        // assignable. misc-unconventional-assign-operator only knows the
        // ordinary shape - a non-const member returning a mutable reference -
        // and reports the standard's own proxy shape as unconventional.
        constexpr array_reference const& operator=(bool value) const noexcept  // NOLINT(misc-unconventional-assign-operator)
                requires (not IsConst) and requires(Bits& c) { array_ops<Bits>::assign(c, 0UZ, true); }
        {
                array_ops<Bits>::assign(m_ref, m_idx, value);
                return *this;
        }

        constexpr array_reference const& operator=(array_reference const& other) const noexcept  // NOLINT(misc-unconventional-assign-operator)
                requires (not IsConst) and requires(Bits& c) { array_ops<Bits>::assign(c, 0UZ, true); }
        {
                return *this = static_cast<bool>(other);
        }

        constexpr array_reference const& flip() const noexcept
                requires (not IsConst) and requires(Bits& c) { array_ops<Bits>::assign(c, 0UZ, true); }
        {
                return *this = not static_cast<bool>(*this);
        }
};

template<array_range Bits, bool IsConst>
[[nodiscard]] constexpr auto format_as(array_reference<Bits, IsConst> ref) noexcept
        -> array_reference<Bits, IsConst>::value_type
{
        return ref;
}

// Deliberately no key_type, unlike set_view: this is a sequence of bool with
// index 0 first, so fmt's range formatter should print it [t, f, t, ...] rather
// than with the set delimiters a nested key_type would select.
template<array_range Bits_cv, std::size_t Extent = bit_extent<std::remove_const_t<Bits_cv>>>
class array_view : public std::ranges::view_base
{
        static constexpr bool is_const = std::is_const_v<Bits_cv>;

        using bits_type = std::remove_const_t<Bits_cv>;
        using ops       = array_ops<bits_type>;

        Bits_cv* m_ptr;

public:
        using element_type           = bool;
        using value_type             = bool;
        using size_type              = std::size_t;
        using difference_type        = std::ptrdiff_t;
        using const_reference        = array_reference<bits_type, true>;
        using reference              = array_reference<bits_type, is_const>;
        using const_iterator         = array_iterator<bits_type, true>;
        using iterator               = array_iterator<bits_type, is_const>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;
        using reverse_iterator       = std::reverse_iterator<iterator>;

        static constexpr std::size_t extent = Extent;

        [[nodiscard]] constexpr explicit array_view(Bits_cv& c) noexcept : m_ptr(&c) {}

        [[nodiscard]] constexpr iterator begin() const noexcept { return array_begin(*m_ptr); }
        [[nodiscard]] constexpr iterator end()   const noexcept { return array_end  (*m_ptr); }

        [[nodiscard]] constexpr reverse_iterator rbegin() const noexcept { return std::make_reverse_iterator(end());   }
        [[nodiscard]] constexpr reverse_iterator rend()   const noexcept { return std::make_reverse_iterator(begin()); }

        [[nodiscard]] constexpr const_iterator         cbegin()  const noexcept { return array_begin(std::as_const(*m_ptr)); }
        [[nodiscard]] constexpr const_iterator         cend()    const noexcept { return array_end  (std::as_const(*m_ptr)); }
        [[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept { return std::make_reverse_iterator(cend());   }
        [[nodiscard]] constexpr const_reverse_iterator crend()   const noexcept { return std::make_reverse_iterator(cbegin()); }

        // A constant expression wherever the Bits knows its own width, which is
        // the whole reason Extent is a parameter rather than a runtime number.
        [[nodiscard]] constexpr size_type size() const noexcept
        {
                if constexpr (extent != std::dynamic_extent) {
                        return extent;
                } else {
                        return ops::size(*m_ptr);
                }
        }

        [[nodiscard]] constexpr bool empty() const noexcept { return size() == 0UZ; }

        [[nodiscard]] constexpr reference operator[](size_type n) const noexcept
        {
                assert(n < size());
                return *(begin() + static_cast<difference_type>(n));
        }

        [[nodiscard]] constexpr reference at(size_type n) const
        {
                if (n >= size()) {
                        throw std::out_of_range("xstd::array_view::at");
                }
                return (*this)[n];
        }

        [[nodiscard]] constexpr reference front() const noexcept { return (*this)[0UZ]; }
        [[nodiscard]] constexpr reference back()  const noexcept { return (*this)[size() - 1UZ]; }

        constexpr void fill(bool value) const noexcept
                requires (not is_const)
        {
                ops::fill(*m_ptr, value);
        }

        [[nodiscard]] friend constexpr bool operator==(array_view lhs, array_view rhs) noexcept
        {
                if constexpr (requires { *lhs.m_ptr == *rhs.m_ptr; }) {
                        return *lhs.m_ptr == *rhs.m_ptr;
                } else {
                        return std::ranges::equal(lhs, rhs);
                }
        }

        // Sequence order, index 0 first - not the set ordering set_view uses, and
        // not any bitset's own <=>, whose element order is its own business. This
        // one is always computed from the sequence, so there is nothing to trust
        // and no set_compare analogue to opt out of.
        [[nodiscard]] friend constexpr std::strong_ordering operator<=>(array_view lhs, array_view rhs) noexcept
        {
                return std::lexicographical_compare_three_way(
                        lhs.begin(), lhs.end(), rhs.begin(), rhs.end(),
                        [](bool x, bool y) static noexcept { return static_cast<int>(x) <=> static_cast<int>(y); });
        }
};

// Deduces both the constness of the argument and the extent the Bits declares.
template<class Bits>
array_view(Bits&) -> array_view<Bits, bit_extent<std::remove_const_t<Bits>>>;

} // namespace xstd::ranges

namespace xstd {

using ranges::array_range;
using ranges::array_view;

} // namespace xstd

#endif // XSTD_BITS_RANGES_ARRAY_VIEW_HPP
