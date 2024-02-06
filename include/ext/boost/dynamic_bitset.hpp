#pragma once

//          Copyright Rein Halbersma 2014-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <boost/dynamic_bitset.hpp>     // dynamic_bitset
#include <concepts>                     // constructible_from, unsigned_integral
#include <cstddef>                      // ptrdiff_t
#include <iterator>                     // bidirectional_iterator_tag, iter_value_t, reverse_iterator
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
        using pimpl_type = dynamic_bitset<Block, Allocator>  const*;
        pimpl_type m_ptr;
        value_type m_val;

public:
        [[nodiscard]] constexpr dynamic_bitset_iterator()                           noexcept = default;
        [[nodiscard]] constexpr dynamic_bitset_iterator(pimpl_type p, value_type v) noexcept
        :
                m_ptr(p),
                m_val(v)
        {
                assert(is_valid());
        }

        [[nodiscard]] constexpr auto operator==(dynamic_bitset_iterator const& other) const noexcept
                -> bool
        {
                assert(is_comparable(other));
                return this->m_val == other.m_val;
        }

        [[nodiscard]] constexpr auto operator*() const noexcept
                -> reference
        {
                assert(is_dereferenceable());
                return { *m_ptr, m_val };
        }

        auto& operator++() noexcept
        {
                assert(is_incrementable());
                m_val = m_ptr->find_next(m_val);
                assert(is_decrementable());
                return *this;
        }

        auto operator++(int) noexcept
        {
                auto nrv = *this; ++*this; return nrv;
        }

        auto& operator--() noexcept
        {
                assert(is_decrementable());
                m_val = find_prev(m_val);
                assert(is_incrementable());
                return *this;
        }

        auto operator--(int) noexcept
        {
                auto nrv = *this; --*this; return nrv;
        }

private:
        [[nodiscard]] auto find_prev(value_type n) noexcept
        {
                assert(m_ptr->any());
                return *std::ranges::find_if(std::views::iota(value_type(0), std::ranges::min(n, m_ptr->size())) | std::views::reverse, [&](auto i) {
                        return (*m_ptr)[i];
                });
        }

        [[nodiscard]] constexpr auto is_valid() const noexcept
        {
                return m_ptr != nullptr && (m_val < m_ptr->size() || m_val == m_ptr->npos);
        }

        [[nodiscard]] constexpr auto is_comparable(dynamic_bitset_iterator other) const noexcept
        {
                return this->m_ptr == other.m_ptr;
        }

        [[nodiscard]] constexpr auto is_dereferenceable() const noexcept
        {
                return m_ptr != nullptr && m_val < m_ptr->size();
        }

        [[nodiscard]] constexpr auto is_incrementable() const noexcept
        {
                return m_ptr != nullptr && m_val < m_ptr->size();
        }

        [[nodiscard]] constexpr auto is_decrementable() const noexcept
        {
                return m_ptr != nullptr && (m_val < m_ptr->size() || m_val == m_ptr->npos);
        }
};

template<std::unsigned_integral Block, class Allocator>
class dynamic_bitset_reference
{
        using rimpl_type = dynamic_bitset<Block, Allocator> const&;
        using value_type = std::iter_value_t<dynamic_bitset_iterator<Block, Allocator>>;
        rimpl_type m_ref;
        value_type m_val;
public:
        dynamic_bitset_reference()                                           = delete;
        dynamic_bitset_reference& operator=(dynamic_bitset_reference const&) = delete;
        dynamic_bitset_reference& operator=(dynamic_bitset_reference &&    ) = delete;

                      constexpr ~dynamic_bitset_reference()                                noexcept = default;
        [[nodiscard]] constexpr  dynamic_bitset_reference(dynamic_bitset_reference const&) noexcept = default;
        [[nodiscard]] constexpr  dynamic_bitset_reference(dynamic_bitset_reference &&    ) noexcept = default;

        [[nodiscard]] constexpr dynamic_bitset_reference(rimpl_type r, value_type v) noexcept
        :
                m_ref(r),
                m_val(v)
        {
                assert(is_valid());
        }

        [[nodiscard]] constexpr auto operator==(dynamic_bitset_reference const& other) const noexcept
                -> bool
        {
                return this->m_val == other.m_val;
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
        [[nodiscard]] constexpr auto is_valid() const noexcept
        {
                return m_val < m_ref.size();
        }
};

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] constexpr auto format_as(dynamic_bitset_reference<Block, Allocator> const& ref) noexcept
        -> std::iter_value_t<dynamic_bitset_iterator<Block, Allocator>>
{
        return ref;
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto begin(dynamic_bitset<Block, Allocator>& bs) noexcept
        -> dynamic_bitset_iterator<Block, Allocator>
{
        return { &bs, bs.find_first() };
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto begin(dynamic_bitset<Block, Allocator> const& bs) noexcept
        -> dynamic_bitset_iterator<Block, Allocator>
{
        return { &bs, bs.find_first() };
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto end(dynamic_bitset<Block, Allocator>& bs) noexcept
        -> dynamic_bitset_iterator<Block, Allocator>
{
        return { &bs, bs.npos };
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto end(dynamic_bitset<Block, Allocator> const& bs) noexcept
        -> dynamic_bitset_iterator<Block, Allocator>
{
        return { &bs, bs.npos };
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto rbegin(dynamic_bitset<Block, Allocator>& bs) noexcept
{
        return std::reverse_iterator(std::ranges::end(bs));
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto rbegin(dynamic_bitset<Block, Allocator> const& bs) noexcept
{
        return std::reverse_iterator(std::ranges::end(bs));
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto rend(dynamic_bitset<Block, Allocator>& bs) noexcept
{
        return std::reverse_iterator(std::ranges::begin(bs));
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto rend(dynamic_bitset<Block, Allocator> const& bs) noexcept
{
        return std::reverse_iterator(std::ranges::begin(bs));
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto cbegin(dynamic_bitset<Block, Allocator> const& bs) noexcept
{
        return std::ranges::begin(bs);
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto cend(dynamic_bitset<Block, Allocator> const& bs) noexcept
{
        return std::ranges::end(bs);
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto crbegin(dynamic_bitset<Block, Allocator> const& bs) noexcept
{
        return std::ranges::rbegin(bs);
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto crend(dynamic_bitset<Block, Allocator> const& bs) noexcept
{
        return std::ranges::rend(bs);
}

}       // namespace boost
