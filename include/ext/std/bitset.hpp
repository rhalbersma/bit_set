#ifndef EXT_STD_BITSET_HPP
#define EXT_STD_BITSET_HPP

//          Copyright Rein Halbersma 2014-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <bitset>       // bitset
#include <cassert>      // assert
#include <concepts>     // constructible_from
#include <cstddef>      // ptrdiff_t, size_t
#include <iterator>     // bidirectional_iterator_tag, iter_value_t, reverse_iterator
#include <ranges>       // begin, end, find_if, rbegin, rend
                        // iota, reverse
#include <type_traits>  // is_class_v
#include <utility>      // unreachable

namespace std {

template<std::size_t N>
auto& operator-=(std::bitset<N>& lhs, std::bitset<N> const& rhs) noexcept
{
        return lhs &= ~rhs;
}

template<std::size_t N>
auto operator-(std::bitset<N> const& lhs, std::bitset<N> const& rhs) noexcept
{
        auto nrv = lhs; nrv -= rhs; return nrv;
}

template<std::size_t N>
auto is_subset_of(std::bitset<N> const& lhs, std::bitset<N> const& rhs) noexcept
{
        if constexpr (N == 0) {
                return true;
        } else {
                return (lhs & ~rhs).none();
        }
}

template<std::size_t N>
auto is_proper_subset_of(std::bitset<N> const& lhs, std::bitset<N> const& rhs) noexcept
{
        if constexpr (N == 0) {
                return false;
        } else {
                return is_subset_of(lhs, rhs) && lhs != rhs;
        }
}

template<std::size_t N>
auto intersects(std::bitset<N> const& lhs, std::bitset<N> const& rhs) noexcept
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
        using pimpl_type = std::bitset<N>  const*;
        pimpl_type m_ptr;
        value_type m_val;

public:
        [[nodiscard]] constexpr bitset_iterator()                           noexcept = default;
        [[nodiscard]] constexpr bitset_iterator(pimpl_type p, value_type v) noexcept
        :
                m_ptr(p),
                m_val(v)
        {
                assert(is_valid());
        }

        [[nodiscard]] constexpr auto operator==(bitset_iterator const& other) const noexcept
                -> bool
        {
                assert(is_comparable(other));
                if constexpr (N == 0) {
                        return true;
                } else {
                        return this->m_val == other.m_val;
                }
        }

        [[nodiscard]] constexpr auto operator*() const noexcept
        {
                if constexpr (N == 0) { std::unreachable(); }
                assert(is_dereferenceable());
                return reference(*m_ptr, m_val);
        }

        constexpr auto& operator++() noexcept
        {
                if constexpr (N == 0) { std::unreachable(); }
                assert(is_incrementable());
                m_val = find_next(m_val);
                assert(is_decrementable());
                return *this;
        }

        constexpr auto operator++(int) noexcept
        {
                auto nrv = *this; ++*this; return nrv;
        }

        constexpr auto& operator--() noexcept
        {
                if constexpr (N == 0) { std::unreachable(); }
                assert(is_decrementable());
                m_val = find_prev(m_val);
                assert(is_incrementable());
                return *this;
        }

        constexpr auto operator--(int) noexcept
        {
                auto nrv = *this; --*this; return nrv;
        }

private:
        [[nodiscard]] constexpr auto find_next(value_type n) const noexcept
        {
                if constexpr (requires { m_ptr->_Find_next(n); }) {
                        return m_ptr->_Find_next(n);
                } else {
                        return *std::ranges::find_if(std::views::iota(n + 1, N), [&](auto i) {
                                return (*m_ptr)[i];
                        });
                }
        }

        [[nodiscard]] constexpr auto find_prev(value_type n) const noexcept
        {
                assert(m_ptr->any());
                return *std::ranges::find_if(std::views::iota(value_type(0), n) | std::views::reverse, [&](auto i) {
                        return (*m_ptr)[i];
                });
        }

        [[nodiscard]] constexpr auto is_valid() const noexcept
        {
                return m_ptr != nullptr && m_val <= N;
        }

        [[nodiscard]] constexpr auto is_comparable(bitset_iterator other) const noexcept
        {
                return this->m_ptr == other.m_ptr;
        }

        [[nodiscard]] constexpr auto is_dereferenceable() const noexcept
        {
                return m_ptr != nullptr && m_val < N;
        }

        [[nodiscard]] constexpr auto is_incrementable() const noexcept
        {
                return m_ptr != nullptr && m_val < N;
        }

        [[nodiscard]] constexpr auto is_decrementable() const noexcept
        {
                return m_ptr != nullptr && 0 < m_val && m_val <= N;
        }
};

template<std::size_t N>
class bitset_reference
{
        using rimpl_type = std::bitset<N> const&;
        using value_type = std::iter_value_t<bitset_iterator<N>>;
        rimpl_type m_ref;
        value_type m_val;
public:
        bitset_reference()                                   = delete;
        bitset_reference& operator=(bitset_reference const&) = delete;
        bitset_reference& operator=(bitset_reference &&    ) = delete;

                      constexpr ~bitset_reference()                        noexcept = default;
        [[nodiscard]] constexpr  bitset_reference(bitset_reference const&) noexcept = default;
        [[nodiscard]] constexpr  bitset_reference(bitset_reference &&    ) noexcept = default;

        [[nodiscard]] constexpr bitset_reference(rimpl_type r, value_type v) noexcept
        :
                m_ref(r),
                m_val(v)
        {
                if constexpr (N == 0) { std::unreachable(); }
                assert(is_valid());
        }

        [[nodiscard]] constexpr auto operator==(bitset_reference const& other) const noexcept
                -> bool
        {
                return this->m_val == other.m_val;
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
        [[nodiscard]] constexpr auto is_valid() const noexcept
        {
                return m_val < N;
        }
};

template<std::size_t N>
[[nodiscard]] constexpr auto format_as(bitset_reference<N> const& ref) noexcept
        -> std::iter_value_t<bitset_iterator<N>>
{
        return ref;
}

namespace detail {

template<std::size_t N>
[[nodiscard]] constexpr auto find_first(std::bitset<N> const* bs) noexcept
{
        if constexpr (N == 0) {
                return N;
        } else if constexpr (requires { bs->_Find_first(); }) {
                return bs->_Find_first();
        } else {
                return *std::ranges::find_if(std::views::iota(std::size_t(0), N), [&](auto i) {
                        return (*bs)[i];
                });
        }
}

}       // namespace detail

template<std::size_t N>
[[nodiscard]] constexpr auto begin(std::bitset<N>& bs) noexcept
        -> bitset_iterator<N>
{
        return { &bs, detail::find_first(&bs) };
}

template<std::size_t N>
[[nodiscard]] constexpr auto begin(std::bitset<N> const& bs) noexcept
        -> bitset_iterator<N>
{
        return { &bs, detail::find_first(&bs) };
}

template<std::size_t N>
[[nodiscard]] constexpr auto end(std::bitset<N>& bs) noexcept
        -> bitset_iterator<N>
{
        return { &bs, N };
}

template<std::size_t N>
[[nodiscard]] constexpr auto end(std::bitset<N> const& bs) noexcept
        -> bitset_iterator<N>
{
        return { &bs, N };
}

template<std::size_t N>
[[nodiscard]] constexpr auto rbegin(std::bitset<N>& bs) noexcept
{
        return std::reverse_iterator(std::ranges::end(bs));
}

template<std::size_t N>
[[nodiscard]] constexpr auto rbegin(std::bitset<N> const& bs) noexcept
{
        return std::reverse_iterator(std::ranges::end(bs));
}

template<std::size_t N>
[[nodiscard]] constexpr auto rend(std::bitset<N>& bs) noexcept
{
        return std::reverse_iterator(std::ranges::begin(bs));
}

template<std::size_t N>
[[nodiscard]] constexpr auto rend(std::bitset<N> const& bs) noexcept
{
        return std::reverse_iterator(std::ranges::begin(bs));
}

template<std::size_t N>
[[nodiscard]] constexpr auto cbegin(std::bitset<N> const& bs) noexcept
{
        return std::ranges::begin(bs);
}

template<std::size_t N>
[[nodiscard]] constexpr auto cend(std::bitset<N> const& bs) noexcept
{
        return std::ranges::end(bs);
}

template<std::size_t N>
[[nodiscard]] constexpr auto crbegin(std::bitset<N> const& bs) noexcept
{
        return std::ranges::rbegin(bs);
}

template<std::size_t N>
[[nodiscard]] constexpr auto crend(std::bitset<N> const& bs)  noexcept
{
        return std::ranges::rend(bs);
}

}       // namespace std

#endif  // include guard
