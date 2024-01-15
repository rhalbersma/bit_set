#pragma once

//          Copyright Rein Halbersma 2014-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <bitset>       // bitset
#include <cassert>      // assert
#include <concepts>     // constructible_from
#include <cstddef>      // ptrdiff_t, size_t
#include <iterator>     // bidirectional_iterator_tag, iter_value_t
#include <ranges>       // iota, reverse, view_interface
#include <type_traits>  // is_class_v

namespace xstd {
namespace impl {

template<std::size_t N>
[[nodiscard]] constexpr auto find_first(std::bitset<N> const* bs) noexcept
{
        if constexpr (requires { bs->_Find_first(); }) {
                return bs->_Find_first();
        } else {
                return *std::ranges::find_if(std::views::iota(0uz, N), [&](auto i) { 
                        return bs->test(i); 
                });
        }
}

template<std::size_t N>
[[nodiscard]] constexpr auto find_next(std::bitset<N> const* bs, std::size_t n) noexcept
{
        if constexpr (requires { bs->_Find_next(n); }) {
                return bs->_Find_next(n);
        } else {
                return *std::ranges::find_if(std::views::iota(n + 1, N), [&](auto i) { 
                        return bs->test(i); 
                });
        }
}

template<std::size_t N>
[[nodiscard]] constexpr auto find_prev(std::bitset<N> const* bs, std::size_t n) noexcept
{
        assert(bs->any());
        return *std::ranges::find_if(std::views::iota(0, n) | std::views::reverse, [&](auto i) { 
                return bs->test(i); 
        });
}

}       // namespace impl

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
        [[nodiscard]] constexpr bitset_iterator() noexcept = default;

        [[nodiscard]] constexpr bitset_iterator(pimpl_type p, value_type v) noexcept
        :
                m_ptr(p),
                m_val(v)
        {
                assert(m_val <= N);
        }

        [[nodiscard]] constexpr auto operator==(bitset_iterator const& other) const noexcept
                -> bool
        {
                assert(this->m_ptr == other.m_ptr);
                return this->m_val == other.m_val;
        }

        [[nodiscard]] constexpr auto operator*() const noexcept
        {
                assert(m_val < N);
                return reference(*m_ptr, m_val);
        }

        constexpr auto& operator++() noexcept
        {
                assert(m_val < N);
                m_val = impl::find_next(m_ptr, m_val);
                assert(m_val <= N);
                return *this;
        }

        constexpr auto operator++(int) noexcept
        {
                auto nrv = *this; ++*this; return nrv;
        }

        constexpr auto& operator--() noexcept
        {
                assert(m_val <= N);
                m_val = impl::find_prev(m_ptr, m_val);
                assert(m_val < N);
                return *this;
        }

        constexpr auto operator--(int) noexcept
        {
                auto nrv = *this; --*this; return nrv;
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

        [[nodiscard]] explicit constexpr bitset_reference(rimpl_type r, value_type v) noexcept
        :
                m_ref(r),
                m_val(v)
        {
                assert(m_val < N);
        }

        [[nodiscard]] constexpr auto operator==(bitset_reference const& other) const noexcept
                -> bool
        {
                return this->m_val == other.m_val;
        }

        [[nodiscard]] constexpr auto operator&() const noexcept
        {
                return bitset_iterator<N>(&m_ref, m_val);
        }

        [[nodiscard]] explicit(false) constexpr operator value_type() const noexcept
        {
                return m_val;
        }

        template<class T>
        [[nodiscard]] explicit(false) constexpr operator T() const noexcept(noexcept(T(m_val)))
                requires std::is_class_v<T> && std::constructible_from<T, value_type>
        {
                return m_val;
        }
};

template<std::size_t N>
[[nodiscard]] constexpr auto format_as(bitset_reference<N> const& ref) noexcept
        -> std::iter_value_t<bitset_iterator<N>>
{
        return ref;
}

template<std::size_t N>
[[nodiscard]] constexpr auto begin(std::bitset<N>& bs) noexcept
{
        return bitset_iterator<N>(&bs, impl::find_first(bs));
}

template<std::size_t N>
[[nodiscard]] constexpr auto begin(std::bitset<N> const& bs) noexcept
{
        return bitset_iterator<N>(&bs, impl::find_first(bs));
}

template<std::size_t N>
[[nodiscard]] constexpr auto end(std::bitset<N>& bs) noexcept
{
        return bitset_iterator<N>(&bs, N);
}

template<std::size_t N>
[[nodiscard]] constexpr auto end(std::bitset<N> const& bs) noexcept
{
        return bitset_iterator<N>(&bs, N);
}

template<std::size_t N>
[[nodiscard]] constexpr auto cbegin(std::bitset<N> const& bs) noexcept(noexcept(xstd::begin(bs)))
        -> decltype(xstd::begin(bs))
{
        return xstd::begin(bs);
}

template<std::size_t N>
[[nodiscard]] constexpr auto cend(std::bitset<N> const& bs) noexcept(noexcept(xstd::end(bs)))
        -> decltype(xstd::end(bs))
{
        return xstd::end(bs);
}

template<std::size_t N>
class set_view 
: 
        public std::ranges::view_interface<set_view<N>>
{
        std::bitset<N> const& m_data;

public:        
        using key_type = int;

        [[nodiscard]] constexpr set_view(std::bitset<N> const& bs) noexcept
        : 
                m_data(bs) 
        {}

        [[nodiscard]] constexpr auto begin() const noexcept { return xstd::begin(m_data); }
        [[nodiscard]] constexpr auto end()   const noexcept { return xstd::end(m_data); }
};

}       // namespace xstd
