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

namespace xstd {

template<class Container>
concept has_value_type = requires 
{ 
        typename Container::value_type; 
};

template<class T> 
struct container_of
{
        using value_type = T;
};

template<class Container>
using value_t = typename Container::value_type; 

template<class Container>
class bidirectional_proxy_reference;

template<class Container>
class bidirectional_proxy_iterator
{
        using pimpl_type = Container const*;
        using index_type = std::size_t;
        pimpl_type m_ptr;
        index_type m_idx;

public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type        = value_t<std::conditional_t<has_value_type<Container>, Container, container_of<std::size_t>>>;
        using difference_type   = std::ptrdiff_t;
        using pointer           = void;
        using reference         = bidirectional_proxy_reference<Container>;

        [[nodiscard]] constexpr bidirectional_proxy_iterator() noexcept = default;

        [[nodiscard]] constexpr bidirectional_proxy_iterator(pimpl_type ptr, index_type idx) noexcept
        :
                m_ptr(ptr),
                m_idx(idx)
        {}

        [[nodiscard]] constexpr bidirectional_proxy_iterator(pimpl_type ptr, value_type val) noexcept
                requires (!std::same_as<value_type, index_type>)
        :
                bidirectional_proxy_iterator(ptr, static_cast<index_type>(val))
        {}

        [[nodiscard]] friend constexpr bool operator==(bidirectional_proxy_iterator lhs, bidirectional_proxy_iterator rhs) noexcept
        {
                assert(lhs.m_ptr == rhs.m_ptr);
                return lhs.m_idx == rhs.m_idx;
        }

        [[nodiscard]] constexpr reference operator*() const noexcept
        {
                return { *m_ptr, m_idx };
        }

        constexpr bidirectional_proxy_iterator& operator++() noexcept
        {
                m_idx = find_next(*m_ptr, m_idx);
                return *this;
        }

        constexpr bidirectional_proxy_iterator operator++(int) noexcept
        {
                auto nrv = *this; ++*this; return nrv;
        }

        constexpr bidirectional_proxy_iterator& operator--() noexcept
        {
                m_idx = find_prev(*m_ptr, m_idx);
                return *this;
        }

        constexpr bidirectional_proxy_iterator operator--(int) noexcept
        {
                auto nrv = *this; --*this; return nrv;
        }
};

template<class Container>
class bidirectional_proxy_reference
{
        using rimpl_type = Container const&;
        using index_type = std::size_t;
        rimpl_type m_ref;
        index_type m_idx;

public:
        using iterator   = bidirectional_proxy_iterator<Container>;
        using value_type = value_t<std::conditional_t<has_value_type<Container>, Container, container_of<std::size_t>>>;

        [[nodiscard]] constexpr bidirectional_proxy_reference() noexcept = delete;

        [[nodiscard]] constexpr bidirectional_proxy_reference(rimpl_type ref, index_type idx) noexcept
        :
                m_ref(ref),
                m_idx(idx)
        {}

        [[nodiscard]] constexpr bidirectional_proxy_reference(bidirectional_proxy_reference const&) noexcept = default;
        [[nodiscard]] constexpr bidirectional_proxy_reference(bidirectional_proxy_reference&&) noexcept = default;

        constexpr bidirectional_proxy_reference& operator=(bidirectional_proxy_reference const&) = delete;
        constexpr bidirectional_proxy_reference& operator=(bidirectional_proxy_reference&&) = delete;

        [[nodiscard]] friend constexpr bool operator==(bidirectional_proxy_reference lhs, bidirectional_proxy_reference rhs) noexcept
        {
                return lhs.m_idx == rhs.m_idx;
        }

        [[nodiscard]] constexpr iterator operator&() const noexcept
        {
                return { &m_ref, m_idx };
        }

        [[nodiscard]] constexpr explicit(false) operator value_type() const noexcept
        {
                if constexpr (std::same_as<index_type, value_type>) {
                        return m_idx;
                } else {
                        return static_cast<value_type>(m_idx);
                }
        }

        template<class T>
        [[nodiscard]] constexpr explicit(false) operator T() const noexcept(noexcept(T(std::declval<value_type>())))
                requires std::is_class_v<T> && std::constructible_from<T, value_type>
        {
                if constexpr (std::same_as<index_type, value_type>) {
                        return m_idx;
                } else {
                        return static_cast<value_type>(m_idx);
                }
        }
};

template<class Container>
[[nodiscard]] constexpr auto format_as(bidirectional_proxy_reference<Container> ref) noexcept
        -> value_t<bidirectional_proxy_reference<Container>>
{
        return ref;
}

}       // namespace xstd

#endif  // include guard
