#ifndef XSTD_PROXY_HPP
#define XSTD_PROXY_HPP

//          Copyright Rein Halbersma 2014-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cassert>      // assert
#include <concepts>     // constructible_from, integral, same_as
#include <cstddef>      // ptrdiff_t, size_t
#include <iterator>     // bidirectional_iterator_tag
#include <type_traits>  // conditional_t, is_class_v
#include <utility>      // declval

namespace xstd {

template<class T>
concept has_value_type = requires 
{ 
        typename T::value_type; 
};

template<class T> 
struct with_value_type
{
        using value_type = T;
};

template<class Bits>
class bidirectional_bit_reference;

template<class Bits>
class bidirectional_bit_iterator
{
        Bits const* m_ptr;
        std::size_t m_idx;

public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type        = std::conditional_t<has_value_type<Bits>, Bits, with_value_type<std::size_t>>::value_type;
        using difference_type   = std::ptrdiff_t;
        using pointer           = void;
        using reference         = bidirectional_bit_reference<Bits>;

        [[nodiscard]] constexpr bidirectional_bit_iterator() noexcept = default;

        [[nodiscard]] constexpr bidirectional_bit_iterator(Bits const* ptr, std::size_t idx) noexcept
        :
                m_ptr(ptr),
                m_idx(idx)
        {}

        [[nodiscard]] constexpr bidirectional_bit_iterator(Bits const* ptr, value_type val) noexcept
                requires (!std::same_as<value_type, std::size_t>)
        :
                bidirectional_bit_iterator(ptr, static_cast<std::size_t>(val))
        {}

        [[nodiscard]] friend constexpr bool operator==(bidirectional_bit_iterator lhs, bidirectional_bit_iterator rhs) noexcept
        {
                assert(lhs.m_ptr == rhs.m_ptr);
                return lhs.m_idx == rhs.m_idx;
        }

        [[nodiscard]] constexpr reference operator*() const noexcept
        {
                return { *m_ptr, m_idx };
        }

        constexpr bidirectional_bit_iterator& operator++() noexcept
        {
                m_idx = find_next(*m_ptr, m_idx);
                return *this;
        }

        constexpr bidirectional_bit_iterator operator++(int) noexcept
        {
                auto nrv = *this; ++*this; return nrv;
        }

        constexpr bidirectional_bit_iterator& operator--() noexcept
        {
                m_idx = find_prev(*m_ptr, m_idx);
                return *this;
        }

        constexpr bidirectional_bit_iterator operator--(int) noexcept
        {
                auto nrv = *this; --*this; return nrv;
        }
};

template<class Bits>
class bidirectional_bit_reference
{
        Bits const& m_ref;
        std::size_t m_idx;

public:
        using value_type = std::conditional_t<has_value_type<Bits>, Bits, with_value_type<std::size_t>>::value_type;
        using iterator   = bidirectional_bit_iterator<Bits>;

        [[nodiscard]] constexpr bidirectional_bit_reference(Bits const& ref, std::size_t idx) noexcept
        :
                m_ref(ref),
                m_idx(idx)
        {}

        [[nodiscard]] friend constexpr bool operator==(bidirectional_bit_reference lhs, bidirectional_bit_reference rhs) noexcept
        {
                return lhs.m_idx == rhs.m_idx;
        }

        [[nodiscard]] constexpr iterator operator&() const noexcept
        {
                return { &m_ref, m_idx };
        }

        [[nodiscard]] constexpr explicit(false) operator value_type() const noexcept
        {
                if constexpr (std::same_as<value_type, std::size_t>) {
                        return m_idx;
                } else {
                        return static_cast<value_type>(m_idx);
                }
        }

        template<class T>
        [[nodiscard]] constexpr explicit(false) operator T() const noexcept(noexcept(T(std::declval<value_type>())))
                requires std::is_class_v<T> && std::constructible_from<T, value_type>
        {
                if constexpr (std::same_as<value_type, std::size_t>) {
                        return m_idx;
                } else {
                        return static_cast<value_type>(m_idx);
                }
        }
};

template<class Bits>
[[nodiscard]] constexpr auto format_as(bidirectional_bit_reference<Bits> ref) noexcept
        -> bidirectional_bit_reference<Bits>::value_type
{
        return ref;
}

}       // namespace xstd

#endif  // include guard
