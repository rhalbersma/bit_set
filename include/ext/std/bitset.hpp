#ifndef EXT_STD_BITSET_HPP
#define EXT_STD_BITSET_HPP

//          Copyright Rein Halbersma 2014-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>    // lexicographical_compare_three_way
#include <bitset>       // bitset
#include <compare>      // strong_ordering
#include <concepts>     // constructible_from
#include <cstddef>      // ptrdiff_t, size_t
#include <iterator>     // bidirectional_iterator_tag, iter_value_t, make_reverse_iterator
#include <ranges>       // begin, end, find_if, rbegin, rend
                        // iota, reverse
#include <type_traits>  // is_class_v

namespace std {

template<std::size_t N>
bitset<N>& operator-=(std::bitset<N>& lhs, const std::bitset<N>& rhs) noexcept
{
        return lhs &= ~rhs;
}

template<std::size_t N>
bitset<N> operator-(const std::bitset<N>& lhs, const std::bitset<N>& rhs) noexcept
{
        auto nrv = lhs; nrv -= rhs; return nrv;
}

template<std::size_t N>
bool is_subset_of(const std::bitset<N>& lhs, const std::bitset<N>& rhs) noexcept
{
        if constexpr (N == 0) {
                return true;
        } else {
                return (lhs & ~rhs).none();
        }
}

template<std::size_t N>
bool is_proper_subset_of(const std::bitset<N>& lhs, const std::bitset<N>& rhs) noexcept
{
        if constexpr (N == 0) {
                return false;
        } else {
                return is_subset_of(lhs, rhs) && lhs != rhs;
        }
}

template<std::size_t N>
bool intersects(const std::bitset<N>& lhs, const std::bitset<N>& rhs) noexcept
{
        if constexpr (N == 0) {
                return false;
        } else {
                return (lhs & rhs).any();
        }
}

template<std::size_t N>
class bitset_reference;

template<std::size_t N>
class bitset_iterator
{
public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type        = std::size_t;
        using difference_type   = std::ptrdiff_t;
        using pointer           = void;
        using reference         = bitset_reference<N>;

private:
        using pimpl_type = const std::bitset<N>*;
        pimpl_type m_ptr;
        value_type m_val;

public:
        [[nodiscard]] constexpr bitset_iterator() noexcept = default;

        [[nodiscard]] constexpr bitset_iterator(pimpl_type ptr, value_type val) noexcept
        :
                m_ptr(ptr),
                m_val(val)
        {
                [[assume(is_valid())]];
        }

        [[nodiscard]] friend constexpr bool operator==(bitset_iterator lhs, bitset_iterator rhs) noexcept
        {
                [[assume(is_comparable(lhs, rhs))]];
                if constexpr (N == 0) {
                        return true;
                } else {
                        return lhs.m_val == rhs.m_val;
                }
        }

        [[nodiscard]] constexpr auto operator*() const noexcept
                -> bitset_reference<N>
        {
                [[assume(is_dereferenceable())]];
                return { *m_ptr, m_val };
        }

        constexpr bitset_iterator& operator++() noexcept
        {
                [[assume(is_incrementable())]];
                m_val = find_next(m_val);
                [[assume(is_decrementable())]];
                return *this;
        }

        constexpr bitset_iterator operator++(int) noexcept
        {
                auto nrv = *this; ++*this; return nrv;
        }

        constexpr bitset_iterator& operator--() noexcept
        {
                [[assume(is_decrementable())]];
                m_val = find_prev(m_val);
                [[assume(is_incrementable())]];
                return *this;
        }

        constexpr bitset_iterator operator--(int) noexcept
        {
                auto nrv = *this; --*this; return nrv;
        }

private:
        [[nodiscard]] constexpr value_type find_next(value_type n) const noexcept
        {
                if constexpr (requires { m_ptr->_Find_next(n); }) {
                        return m_ptr->_Find_next(n);
                } else {
                        return *std::ranges::find_if(std::views::iota(n + 1, N), [&](auto i) {
                                return (*m_ptr)[i];
                        });
                }
        }

        [[nodiscard]] constexpr value_type find_prev(value_type n) const noexcept
        {
                [[assume(m_ptr->any())]];
                return *std::ranges::find_if(std::views::iota(0uz, n) | std::views::reverse, [&](auto i) {
                        return (*m_ptr)[i];
                });
        }

        [[nodiscard]] constexpr bool is_valid() const noexcept
        {
                return m_ptr != nullptr && m_val <= N;
        }

        [[nodiscard]] static constexpr bool is_comparable(bitset_iterator lhs, bitset_iterator rhs) noexcept
        {
                return lhs.m_ptr == rhs.m_ptr;
        }

        [[nodiscard]] constexpr bool is_dereferenceable() const noexcept
        {
                return m_ptr != nullptr && m_val < N;
        }

        [[nodiscard]] constexpr bool is_incrementable() const noexcept
        {
                return m_ptr != nullptr && m_val < N;
        }

        [[nodiscard]] constexpr bool is_decrementable() const noexcept
        {
                return m_ptr != nullptr && 0 < m_val && m_val <= N;
        }
};

template<std::size_t N>
class bitset_reference
{
        using rimpl_type = const std::bitset<N>&;
        using value_type = std::iter_value_t<bitset_iterator<N>>;
        rimpl_type m_ref;
        value_type m_val;

public:
        [[nodiscard]] constexpr bitset_reference() noexcept = delete;

        [[nodiscard]] constexpr bitset_reference(rimpl_type ref, value_type val) noexcept
        :
                m_ref(ref),
                m_val(val)
        {
                [[assume(is_valid())]];
        }

        [[nodiscard]] constexpr bitset_reference(const bitset_reference&) noexcept = default;
        [[nodiscard]] constexpr bitset_reference(bitset_reference&&) noexcept = default;

        constexpr ~bitset_reference() noexcept = default;
        constexpr bitset_reference& operator=(const bitset_reference&) = delete;
        constexpr bitset_reference& operator=(bitset_reference&&) = delete;

        [[nodiscard]] friend constexpr bool operator==(bitset_reference lhs, bitset_reference rhs) noexcept
        {
                return lhs.m_val == rhs.m_val;
        }

        [[nodiscard]] constexpr auto operator&() const noexcept
                -> bitset_iterator<N>
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
                return m_val < N;
        }
};

template<std::size_t N>
[[nodiscard]] constexpr auto format_as(bitset_reference<N> ref) noexcept
        -> std::iter_value_t<bitset_iterator<N>>
{
        return ref;
}

namespace detail {

template<std::size_t N>
[[nodiscard]] constexpr auto find_first(const std::bitset<N>& c) noexcept
        -> std::iter_value_t<bitset_iterator<N>>
{
        if constexpr (N == 0) {
                return N;
        } else if constexpr (requires { c._Find_first(); }) {
                return c._Find_first();
        } else {
                return *std::ranges::find_if(std::views::iota(0uz, N), [&](auto i) {
                        return c[i];
                });
        }
}

}       // namespace detail

// range access
template<std::size_t N>
[[nodiscard]] constexpr auto begin(std::bitset<N>& c) noexcept
        -> bitset_iterator<N>
{
        return { &c, detail::find_first(c) };
}

template<std::size_t N>
[[nodiscard]] constexpr auto begin(const std::bitset<N>& c) noexcept
        -> bitset_iterator<N>
{
        return { &c, detail::find_first(c) };
}

template<std::size_t N>
[[nodiscard]] constexpr auto end(std::bitset<N>& c) noexcept
        -> bitset_iterator<N>
{
        return { &c, N };
}

template<std::size_t N>
[[nodiscard]] constexpr auto end(const std::bitset<N>& c) noexcept
        -> bitset_iterator<N>
{
        return { &c, N };
}

template<std::size_t N>
[[nodiscard]] constexpr auto cbegin(const std::bitset<N>& c) noexcept
{
        return std::ranges::begin(c);
}

template<std::size_t N>
[[nodiscard]] constexpr auto cend(const std::bitset<N>& c) noexcept
{
        return std::ranges::end(c);
}

template<std::size_t N>
[[nodiscard]] constexpr auto rbegin(std::bitset<N>& c) noexcept
{
        return std::make_reverse_iterator(std::ranges::end(c));
}

template<std::size_t N>
[[nodiscard]] constexpr auto rbegin(const std::bitset<N>& c) noexcept
{
        return std::make_reverse_iterator(std::ranges::end(c));
}

template<std::size_t N>
[[nodiscard]] constexpr auto rend(std::bitset<N>& c) noexcept
{
        return std::make_reverse_iterator(std::ranges::begin(c));
}

template<std::size_t N>
[[nodiscard]] constexpr auto rend(const std::bitset<N>& c) noexcept
{
        return std::make_reverse_iterator(std::ranges::begin(c));
}

template<std::size_t N>
[[nodiscard]] constexpr auto crbegin(const std::bitset<N>& c) noexcept
{
        return std::ranges::rbegin(c);
}

template<std::size_t N>
[[nodiscard]] constexpr auto crend(const std::bitset<N>& c)  noexcept
{
        return std::ranges::rend(c);
}

template<std::size_t N>
[[nodiscard]] auto operator<=>(const std::bitset<N>& x, const std::bitset<N>& y) noexcept
        -> std::strong_ordering
{
        return std::lexicographical_compare_three_way(
                std::ranges::begin(x), std::ranges::end(x),
                std::ranges::begin(y), std::ranges::end(y)
        );
}

}       // namespace std

#endif  // include guard
