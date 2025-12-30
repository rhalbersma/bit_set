#ifndef XSTD_PROXY_RANDOM_ACCESS_HPP
#define XSTD_PROXY_RANDOM_ACCESS_HPP

//          Copyright Rein Halbersma 2014-2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cassert>      // assert
#include <concepts>     // constructible_from, convertible_to
#include <cstddef>      // ptrdiff_t, size_t
#include <iterator>     // random_access_iterator_tag
#include <type_traits>  // is_class_v, is_convertible_v, is_nothrow_constructible_v

namespace xstd::random_access {

template<class Bits>
concept bit_range = 
        requires(Bits const& c)
        {
                { find_first(c) } -> std::convertible_to<std::size_t>;
                { find_last (c) } -> std::convertible_to<std::size_t>;
        } and
        requires(Bits const& c, std::size_t n)
        {
                { find_next(c, n) } -> std::convertible_to<std::size_t>;
                { find_prev(c, n) } -> std::convertible_to<std::size_t>;
        }
;

template<bit_range, bool> class iterator;
template<bit_range, bool> class reference;

template<bit_range Bits, bool IsConst>
class iterator
{
        using ptr_const_t = std::conditional_t<IsConst, Bits const*, Bits*>;

        ptr_const_t m_ptr{};
        std::size_t m_idx{};

        friend Bits;
        friend class reference<Bits, IsConst>;
        // TODO: figure out the const/non-const overloads of begin/end that are required to be friends

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
        using reference         = xstd::random_access::reference<Bits, IsConst>;

        [[nodiscard]] constexpr iterator() noexcept = default;

        [[nodiscard]] friend constexpr bool operator==(iterator lhs, iterator rhs) noexcept
        {
                assert(lhs.m_ptr == rhs.m_ptr);
                return lhs.m_idx == rhs.m_idx;
        }

        [[nodiscard]] friend constexpr auto operator<=>(iterator lhs, iterator rhs) noexcept
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

// TODO: figure out the const/non-const overloads of begin/end that are required to be friends

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
        using iterator   = xstd::random_access::iterator<Bits, IsConst>;
        using value_type = std::iter_value_t<iterator>;

        [[nodiscard]] constexpr iterator operator&() const noexcept
        {
                return { &m_ref, m_idx };
        }

        [[nodiscard]] constexpr explicit(false) operator value_type() const noexcept
        {
                return m_ref[m_idx];
        }

        template<std::constructible_from<value_type> T>
        [[nodiscard]] constexpr explicit(not std::is_convertible_v<value_type, T>) operator T() const noexcept(std::is_nothrow_constructible_v<T, value_type>)
                requires std::is_class_v<T>
        {
                return m_ref[m_idx];
        }
};

template<bit_range Bits, bool IsConst>
[[nodiscard]] constexpr auto format_as(reference<Bits, IsConst> ref) noexcept
        -> reference<Bits, IsConst>::value_type
{
        return ref;
}

}       // namespace xstd::random_access

#endif  // include guard
