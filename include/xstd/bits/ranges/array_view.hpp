//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_BITS_RANGES_ARRAY_VIEW_HPP
#define XSTD_BITS_RANGES_ARRAY_VIEW_HPP

#include <xstd/bits/detail/intrin.hpp>       // countr_zero
#include <xstd/bits/ranges/bit_extent.hpp>   // bit_extent, static_bit_extent
#include <xstd/bits/ranges/block_access.hpp> // block_access, block_range, first_difference
#include <algorithm>                         // equal, lexicographical_compare_three_way
#include <cassert>                           // assert
#include <compare>                           // strong_ordering
#include <concepts>                          // constructible_from, convertible_to
#include <cstddef>                           // ptrdiff_t, size_t
#include <iterator>                          // make_reverse_iterator, random_access_iterator_tag, reverse_iterator
#include <ranges>                            // view_base
#include <span>                              // dynamic_extent
#include <stdexcept>                         // out_of_range
#include <type_traits>                       // conditional_t, is_class_v, is_const_v, is_convertible_v, is_nothrow_constructible_v, remove_const_t
#include <utility>                           // as_const

// A sequence of bool over bits it does not own: span's shape, one template for both extents, and mutable through.
namespace xstd::ranges {

// Where the bits are; same ADL-with-explicit-specialization design as set_find, and see set_view.hpp for why.
template<class Bits>
struct array_find
{
        [[nodiscard]] static constexpr auto first(Bits const& c) noexcept
                -> std::size_t
                requires requires { { find_first(c) } -> std::convertible_to<std::size_t>; }
        {
                return find_first(c);
        }

        [[nodiscard]] static constexpr auto last(Bits const& c) noexcept
                -> std::size_t
                requires requires { { find_last(c) } -> std::convertible_to<std::size_t>; }
        {
                return find_last(c);
        }

        [[nodiscard]] static constexpr auto at(Bits const& c, std::size_t n) noexcept
                -> bool
                requires requires { { find_at(c, n) } -> std::convertible_to<bool>; }
        {
                return find_at(c, n);
        }
};

// The vocabulary, rewired: only writing needs renaming, a bitset's size() being already a sequence's size().
template<class Bits>
struct array_ops
{
        [[nodiscard]] static constexpr auto size(Bits const& c) noexcept
                -> std::size_t
        {
                if constexpr (static_bit_extent<Bits>) {
                        return bit_extent<Bits>;
                } else {
                        return c.size();
                }
        }

        // The operator[] fallback this class refuses is the one below, for a type without
        // set(n, value): were its operator[] to return our own proxy, that proxy's assignment
        // would land back here and recurse until the stack is gone. Where set(n, value) does
        // exist the type is a concrete bitset, and its subscript is the unchecked way in --
        // which is the one to take, the position being a precondition asserted just below,
        // where std::bitset::set and xstd::bitset::set check it again and throw out of this
        // noexcept.
        static constexpr void assign(Bits& c, std::size_t n, bool value) noexcept
                requires requires { c.set(n, value); }
        {
                // The assert is on its own line: the coverage job drops assert branches by matching the start of the line.
                assert(n < size(c));
                if constexpr (requires { c[n] = value; }) {
                        c[n] = value;
                } else {
                        c.set(n, value);
                }
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

// The sequence ordering, defined once: the bools in position order, compared lexicographically through value_type.
template<std::ranges::input_range X, std::ranges::input_range Y>
[[nodiscard]] constexpr auto array_three_way(X const& x, Y const& y) noexcept
        -> std::strong_ordering
{
        return std::lexicographical_compare_three_way(
                std::ranges::begin(x), std::ranges::end(x),
                std::ranges::begin(y), std::ranges::end(y),
                [](bool a, bool b) static noexcept -> std::strong_ordering { return static_cast<int>(a) <=> static_cast<int>(b); }
        );
}

// The same ordering a word at a time; at the lowest differing bit the sequence LACKING it is smaller, with no prefix clause.
template<block_range Bits>
[[nodiscard]] constexpr auto array_three_way(Bits const& x, Bits const& y) noexcept
        -> std::strong_ordering
{
        using access = block_access<Bits>;
        using block_type = decltype(access::block(x, 0UZ));

        auto const [index, diff] = first_difference(x, y);
        if (diff == block_type{}) {
                return std::strong_ordering::equal;
        }

        auto const offset = detail::bits::countr_zero(diff);
        auto const x_has  = (static_cast<block_type>(access::block(x, index) >> offset) & block_type{1}) != block_type{};

        return x_has ? std::strong_ordering::greater : std::strong_ordering::less;
}

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

template<class, bool> class array_iterator;
template<class, bool> class array_reference;

// Forward-declared so array_iterator's dependent friend template-ids have a template to name; Clang requires it, GCC does not.
template<array_range Bits> [[nodiscard]] constexpr auto array_begin(      Bits& c) noexcept -> array_iterator<Bits, false>;
template<array_range Bits> [[nodiscard]] constexpr auto array_begin(Bits const& c) noexcept -> array_iterator<Bits, true >;
template<array_range Bits> [[nodiscard]] constexpr auto array_end  (      Bits& c) noexcept -> array_iterator<Bits, false>;
template<array_range Bits> [[nodiscard]] constexpr auto array_end  (Bits const& c) noexcept -> array_iterator<Bits, true >;

template<class Bits, bool IsConst>
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

        [[nodiscard]] friend constexpr auto operator==(array_iterator lhs, array_iterator rhs) noexcept
                -> bool
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

        [[nodiscard]] constexpr auto operator*() const noexcept
                -> reference
        {
                assert(m_ptr != nullptr);
                return { *m_ptr, m_idx };
        }

        constexpr auto operator++() noexcept -> array_iterator& { ++m_idx; return *this; }
        constexpr auto operator--() noexcept -> array_iterator& { --m_idx; return *this; }

        constexpr auto operator++(int) noexcept -> array_iterator { auto nrv = *this; ++*this; return nrv; }
        constexpr auto operator--(int) noexcept -> array_iterator { auto nrv = *this; --*this; return nrv; }

        constexpr auto operator+=(difference_type n) noexcept -> array_iterator& { m_idx = static_cast<std::size_t>(static_cast<difference_type>(m_idx) + n); return *this; }
        constexpr auto operator-=(difference_type n) noexcept -> array_iterator& { m_idx = static_cast<std::size_t>(static_cast<difference_type>(m_idx) - n); return *this; }

        [[nodiscard]] friend constexpr auto operator-(array_iterator lhs, array_iterator rhs) noexcept
                -> difference_type
        {
                return static_cast<difference_type>(lhs.m_idx) - static_cast<difference_type>(rhs.m_idx);
        }

        [[nodiscard]] constexpr auto operator[](difference_type n) const noexcept
                -> reference
        {
                assert(m_ptr != nullptr);
                return { *m_ptr, static_cast<std::size_t>(static_cast<difference_type>(m_idx) + n) };
        }
};

template<array_range Bits, bool IsConst> [[nodiscard]] constexpr auto operator+(array_iterator<Bits, IsConst> lhs, std::ptrdiff_t n) noexcept -> array_iterator<Bits, IsConst> { auto nrv = lhs; nrv += n; return nrv; }
template<array_range Bits, bool IsConst> [[nodiscard]] constexpr auto operator+(std::ptrdiff_t n, array_iterator<Bits, IsConst> rhs) noexcept -> array_iterator<Bits, IsConst> { auto nrv = rhs; nrv += n; return nrv; }
template<array_range Bits, bool IsConst> [[nodiscard]] constexpr auto operator-(array_iterator<Bits, IsConst> lhs, std::ptrdiff_t n) noexcept -> array_iterator<Bits, IsConst> { auto nrv = lhs; nrv -= n; return nrv; }

template<array_range Bits> [[nodiscard]] constexpr auto array_begin(      Bits& c) noexcept -> array_iterator<Bits, false> { return { &c, array_find<Bits>::first(c) }; }
template<array_range Bits> [[nodiscard]] constexpr auto array_begin(Bits const& c) noexcept -> array_iterator<Bits, true > { return { &c, array_find<Bits>::first(c) }; }
template<array_range Bits> [[nodiscard]] constexpr auto array_end  (      Bits& c) noexcept -> array_iterator<Bits, false> { return { &c, array_find<Bits>::last (c) }; }
template<array_range Bits> [[nodiscard]] constexpr auto array_end  (Bits const& c) noexcept -> array_iterator<Bits, true > { return { &c, array_find<Bits>::last (c) }; }

// A proxy bool assigning back into the bits, with std::vector<bool>::reference the precedent, const-qualified assignment included.
template<class Bits, bool IsConst>
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

        [[nodiscard]] constexpr auto operator&() const noexcept
                -> iterator
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

        // const-qualified and returning a const reference, the proxy shape P2321R2 gave std::vector<bool>::reference.
        constexpr auto operator=(bool value) const noexcept  // NOLINT(misc-unconventional-assign-operator)
                -> array_reference const&
                requires (not IsConst) and requires(Bits& c) { array_ops<Bits>::assign(c, 0UZ, true); }
        {
                array_ops<Bits>::assign(m_ref, m_idx, value);
                return *this;
        }

        constexpr auto operator=(array_reference const& other) const noexcept  // NOLINT(misc-unconventional-assign-operator)
                -> array_reference const&
                requires (not IsConst) and requires(Bits& c) { array_ops<Bits>::assign(c, 0UZ, true); }
        {
                return *this = static_cast<bool>(other);
        }

        constexpr auto flip() const noexcept  // NOLINT(modernize-use-nodiscard)
                -> array_reference const&
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

// Deliberately no key_type, unlike set_view: fmt should print this as a sequence rather than with set delimiters.
template<array_range Bits_cv, std::size_t Extent = bit_extent<std::remove_const_t<Bits_cv>>>
class array_view : public std::ranges::view_base
{
        static constexpr bool is_const = std::is_const_v<Bits_cv>;

        using bits_type = std::remove_const_t<Bits_cv>;
        using ops       = array_ops<bits_type>;

        Bits_cv* m_ptr;

        // A view is as block-accessible as the thing it views; constrained, so one over an opaque type keeps the element-wise path.
        [[nodiscard]] friend constexpr auto block_count(array_view v) noexcept
                -> std::size_t
                requires block_range<bits_type>
        {
                return block_access<bits_type>::num_blocks(*v.m_ptr);
        }

        [[nodiscard]] friend constexpr auto block_at(array_view v, std::size_t i) noexcept
                requires block_range<bits_type>
        {
                return block_access<bits_type>::block(*v.m_ptr, i);
        }

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

        [[nodiscard]] constexpr auto begin() const noexcept -> iterator { return array_begin(*m_ptr); }
        [[nodiscard]] constexpr auto end()   const noexcept -> iterator { return array_end  (*m_ptr); }

        [[nodiscard]] constexpr auto rbegin() const noexcept -> reverse_iterator { return std::make_reverse_iterator(end());   }
        [[nodiscard]] constexpr auto rend()   const noexcept -> reverse_iterator { return std::make_reverse_iterator(begin()); }

        [[nodiscard]] constexpr auto cbegin()  const noexcept -> const_iterator         { return array_begin(std::as_const(*m_ptr)); }
        [[nodiscard]] constexpr auto cend()    const noexcept -> const_iterator         { return array_end  (std::as_const(*m_ptr)); }
        [[nodiscard]] constexpr auto crbegin() const noexcept -> const_reverse_iterator { return std::make_reverse_iterator(cend());   }
        [[nodiscard]] constexpr auto crend()   const noexcept -> const_reverse_iterator { return std::make_reverse_iterator(cbegin()); }

        // A constant expression wherever the Bits knows its own width, which is why Extent is a parameter at all.
        [[nodiscard]] constexpr auto size() const noexcept
                -> size_type
        {
                if constexpr (extent != std::dynamic_extent) {
                        return extent;
                } else {
                        return ops::size(*m_ptr);
                }
        }

        [[nodiscard]] constexpr auto empty() const noexcept -> bool { return size() == 0UZ; }

        [[nodiscard]] constexpr auto operator[](size_type n) const noexcept
                -> reference
        {
                assert(n < size());
                return *(begin() + static_cast<difference_type>(n));
        }

        [[nodiscard]] constexpr auto at(size_type n) const
                -> reference
        {
                if (n >= size()) {
                        throw std::out_of_range("xstd::array_view::at");
                }
                return (*this)[n];
        }

        [[nodiscard]] constexpr auto front() const noexcept -> reference { return (*this)[0UZ]; }
        [[nodiscard]] constexpr auto back()  const noexcept -> reference { return (*this)[size() - 1UZ]; }

        constexpr void fill(bool value) const noexcept
                requires (not is_const)
        {
                ops::fill(*m_ptr, value);
        }

        [[nodiscard]] friend constexpr auto operator==(array_view lhs, array_view rhs) noexcept
                -> bool
        {
                if constexpr (requires { *lhs.m_ptr == *rhs.m_ptr; }) {
                        return *lhs.m_ptr == *rhs.m_ptr;
                } else {
                        return std::ranges::equal(lhs, rhs);
                }
        }

        // Sequence order, index 0 first, always computed from the sequence: nothing to trust, and no set_compare analogue.
        [[nodiscard]] friend constexpr auto operator<=>(array_view lhs, array_view rhs) noexcept
                -> std::strong_ordering
        {
                return array_three_way(lhs, rhs);
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
