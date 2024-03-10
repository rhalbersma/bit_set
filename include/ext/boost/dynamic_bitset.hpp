#pragma once

//          Copyright Rein Halbersma 2014-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <boost/dynamic_bitset.hpp>     // dynamic_bitset
#include <algorithm>                    // lexicographical_compare_three_way
#include <compare>                      // strong_ordering
#include <concepts>                     // constructible_from, unsigned_integral
#include <cstddef>                      // ptrdiff_t
#include <iterator>                     // bidirectional_iterator_tag, iter_value_t, make_reverse_iterator
#include <ranges>                       // begin, end, find_if, min, rbegin, rend
                                        // iota, reverse
#include <type_traits>                  // is_class_v

namespace boost {

template<std::unsigned_integral Block, class Allocator>
class dynamic_bitset_reference;

template<std::unsigned_integral Block, class Allocator>
class dynamic_bitset_iterator
{
public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type        = dynamic_bitset<Block, Allocator>::size_type;
        using difference_type   = std::ptrdiff_t;
        using pointer           = void;
        using reference         = dynamic_bitset_reference<Block, Allocator>;

private:
        using pimpl_type = const dynamic_bitset<Block, Allocator>*;
        pimpl_type m_ptr;
        value_type m_val;

public:
        [[nodiscard]] constexpr dynamic_bitset_iterator() noexcept = default;

        [[nodiscard]] constexpr dynamic_bitset_iterator(pimpl_type ptr, value_type val) noexcept
        :
                m_ptr(ptr),
                m_val(val)
        {
                [[assume(is_valid())]];
        }

        [[nodiscard]] friend constexpr bool operator==(dynamic_bitset_iterator lhs, dynamic_bitset_iterator rhs) noexcept
        {
                [[assume(is_comparable(lhs, rhs))]];
                return lhs.m_val == rhs.m_val;
        }

        [[nodiscard]] constexpr auto operator*() const noexcept
                -> dynamic_bitset_reference<Block, Allocator>
        {
                [[assume(is_dereferenceable())]];
                return { *m_ptr, m_val };
        }

        dynamic_bitset_iterator& operator++() noexcept
        {
                [[assume(is_incrementable())]];
                m_val = m_ptr->find_next(m_val);
                [[assume(is_decrementable())]];
                return *this;
        }

        dynamic_bitset_iterator operator++(int) noexcept
        {
                auto nrv = *this; ++*this; return nrv;
        }

        dynamic_bitset_iterator& operator--() noexcept
        {
                [[assume(is_decrementable())]];
                m_val = find_prev(m_val);
                [[assume(is_incrementable())]];
                return *this;
        }

        dynamic_bitset_iterator operator--(int) noexcept
        {
                auto nrv = *this; --*this; return nrv;
        }

private:
        [[nodiscard]] value_type find_prev(value_type n) noexcept
        {
                [[assume(m_ptr->any())]];
                return *std::ranges::find_if(std::views::iota(0uz, std::ranges::min(n, m_ptr->size())) | std::views::reverse, [&](auto i) {
                        return (*m_ptr)[i];
                });
        }

        [[nodiscard]] constexpr bool is_valid() const noexcept
        {
                return m_ptr != nullptr && (m_val < m_ptr->size() || m_val == m_ptr->npos);
        }

        [[nodiscard]] friend constexpr bool is_comparable(dynamic_bitset_iterator lhs, dynamic_bitset_iterator rhs) noexcept
        {
                return lhs.m_ptr == rhs.m_ptr;
        }

        [[nodiscard]] constexpr bool is_dereferenceable() const noexcept
        {
                return m_ptr != nullptr && m_val < m_ptr->size();
        }

        [[nodiscard]] constexpr bool is_incrementable() const noexcept
        {
                return m_ptr != nullptr && m_val < m_ptr->size();
        }

        [[nodiscard]] constexpr bool is_decrementable() const noexcept
        {
                return m_ptr != nullptr && 0 < m_val && (m_val < m_ptr->size() || m_val == m_ptr->npos);
        }
};

template<std::unsigned_integral Block, class Allocator>
class dynamic_bitset_reference
{
        using rimpl_type = const dynamic_bitset<Block, Allocator>&;
        using value_type = std::iter_value_t<dynamic_bitset_iterator<Block, Allocator>>;
        rimpl_type m_ref;
        const value_type m_val;

public:
        [[nodiscard]] constexpr dynamic_bitset_reference() noexcept = delete;

        [[nodiscard]] constexpr dynamic_bitset_reference(rimpl_type ref, value_type val) noexcept
        :
                m_ref(ref),
                m_val(val)
        {
                [[assume(is_valid())]];
        }

        [[nodiscard]] constexpr dynamic_bitset_reference(const dynamic_bitset_reference&) noexcept = default;
        [[nodiscard]] constexpr dynamic_bitset_reference(dynamic_bitset_reference&&) noexcept = default;

        constexpr ~dynamic_bitset_reference() noexcept = default;
        constexpr dynamic_bitset_reference& operator=(const dynamic_bitset_reference&) = delete;
        constexpr dynamic_bitset_reference& operator=(dynamic_bitset_reference&&) = delete;

        [[nodiscard]] friend constexpr bool operator==(dynamic_bitset_reference lhs, dynamic_bitset_reference rhs) noexcept
        {
                return lhs.m_val == rhs.m_val;
        }

        [[nodiscard]] constexpr auto operator&() const noexcept
                -> dynamic_bitset_iterator<Block, Allocator>
        {
                return { &m_ref, m_val };
        }

        [[nodiscard]] constexpr explicit(false) operator value_type() const noexcept
        {
                return m_val;
        }

        template<class T>
        [[nodiscard]] constexpr explicit(false) operator T() const noexcept(noexcept(T(m_val)))
                requires std::is_class_v<T> && std::constructible_from<T, value_type>
        {
                return m_val;
        }

private:
        [[nodiscard]] constexpr bool is_valid() const noexcept
        {
                return m_val < m_ref.size();
        }
};

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] constexpr auto format_as(dynamic_bitset_reference<Block, Allocator> ref) noexcept
        -> std::iter_value_t<dynamic_bitset_iterator<Block, Allocator>>
{
        return ref;
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto begin(dynamic_bitset<Block, Allocator>& c) noexcept
        -> dynamic_bitset_iterator<Block, Allocator>
{
        return { &c, c.find_first() };
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto begin(const dynamic_bitset<Block, Allocator>& c) noexcept
        -> dynamic_bitset_iterator<Block, Allocator>
{
        return { &c, c.find_first() };
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto end(dynamic_bitset<Block, Allocator>& c) noexcept
        -> dynamic_bitset_iterator<Block, Allocator>
{
        return { &c, c.npos };
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto end(const dynamic_bitset<Block, Allocator>& c) noexcept
        -> dynamic_bitset_iterator<Block, Allocator>
{
        return { &c, c.npos };
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto cbegin(const dynamic_bitset<Block, Allocator>& c) noexcept
{
        return std::ranges::begin(c);
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto cend(const dynamic_bitset<Block, Allocator>& c) noexcept
{
        return std::ranges::end(c);
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto rbegin(dynamic_bitset<Block, Allocator>& c) noexcept
{
        return std::make_reverse_iterator(std::ranges::end(c));
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto rbegin(const dynamic_bitset<Block, Allocator>& c) noexcept
{
        return std::make_reverse_iterator(std::ranges::end(c));
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto rend(dynamic_bitset<Block, Allocator>& c) noexcept
{
        return std::make_reverse_iterator(std::ranges::begin(c));
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto rend(const dynamic_bitset<Block, Allocator>& c) noexcept
{
        return std::make_reverse_iterator(std::ranges::begin(c));
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto crbegin(const dynamic_bitset<Block, Allocator>& bs) noexcept
{
        return std::ranges::rbegin(bs);
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto crend(const dynamic_bitset<Block, Allocator>& c) noexcept
{
        return std::ranges::rend(c);
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto operator<=>(const dynamic_bitset<Block, Allocator>& x, const dynamic_bitset<Block, Allocator>& y) noexcept
        -> std::strong_ordering
{
        return std::lexicographical_compare_three_way(
                std::ranges::begin(x), std::ranges::end(x),
                std::ranges::begin(y), std::ranges::end(y)
        );
}

}       // namespace boost
