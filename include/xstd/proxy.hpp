#ifndef XSTD_PROXY_HPP
#define XSTD_PROXY_HPP

//          Copyright Rein Halbersma 2014-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cassert>      // assert
#include <concepts>     // constructible_from
#include <cstddef>      // ptrdiff_t, size_t
#include <iterator>     // bidirectional_iterator_tag
#include <type_traits>  // is_class_v, is_convertible_v, is_nothrow_constructible_v

namespace xstd::bit::bidirectional {

template<class> class const_iterator;
template<class> class const_reference;

template<class Bits>
class const_iterator
{
        Bits const* m_ptr;
        std::size_t m_idx;

public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type        = std::size_t;
        using difference_type   = std::ptrdiff_t;
        using pointer           = void;
        using reference         = const_reference<Bits>;

        [[nodiscard]] constexpr const_iterator() noexcept = default;

        [[nodiscard]] constexpr const_iterator(Bits const* ptr, std::size_t idx) noexcept
        :
                m_ptr(ptr),
                m_idx(idx)
        {}

        [[nodiscard]] friend constexpr bool operator==(const_iterator lhs, const_iterator rhs) noexcept
        {
                assert(lhs.m_ptr == rhs.m_ptr);
                return lhs.m_idx == rhs.m_idx;
        }

        [[nodiscard]] constexpr reference operator*() const noexcept
        {
                return { *m_ptr, m_idx };
        }

        constexpr const_iterator& operator++() noexcept
        {
                m_idx = find_next(*m_ptr, m_idx);
                return *this;
        }

        constexpr const_iterator operator++(int) noexcept
        {
                auto nrv = *this; ++*this; return nrv;
        }

        constexpr const_iterator& operator--() noexcept
        {
                m_idx = find_prev(*m_ptr, m_idx);
                return *this;
        }

        constexpr const_iterator operator--(int) noexcept
        {
                auto nrv = *this; --*this; return nrv;
        }
};

template<class Bits>
class const_reference
{
        Bits const& m_ref;
        std::size_t m_idx;

public:
        using iterator = const_iterator<Bits>;

        [[nodiscard]] constexpr const_reference(Bits const& ref, std::size_t idx) noexcept
        :
                m_ref(ref),
                m_idx(idx)
        {}

        [[nodiscard]] constexpr iterator operator&() const noexcept
        {
                return { &m_ref, m_idx };
        }

        [[nodiscard]] constexpr explicit(false) operator std::size_t() const noexcept
        {
                return m_idx;
        }

        template<std::constructible_from<std::size_t> T>
        [[nodiscard]] constexpr explicit(not std::is_convertible_v<std::size_t, T>) operator T() const noexcept(std::is_nothrow_constructible_v<T, std::size_t>)
                requires std::is_class_v<T>
        {
                return m_idx;
        }
};

template<class Bits>
[[nodiscard]] constexpr auto format_as(const_reference<Bits> ref) noexcept
        -> std::size_t
{
        return ref;
}

}       // namespace xstd::bit::bidirectional

#endif  // include guard
