#pragma once

//          Copyright Rein Halbersma 2014-2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <boost/dynamic_bitset.hpp>     // dynamic_bitset
#include <concepts>                     // constructible_from, unsigned_integral
#include <cstddef>                      // ptrdiff_t, size_t
#include <iterator>                     // forward_iterator_tag
#include <stdexcept>                    // out_of_range
#include <type_traits>                  // is_class_v

namespace boost {

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto is_subset_of(dynamic_bitset<Block, Allocator> const& lhs, dynamic_bitset<Block, Allocator> const& rhs) noexcept
{
        return lhs.is_subset_of(rhs);
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto is_proper_subset_of(dynamic_bitset<Block, Allocator> const& lhs, dynamic_bitset<Block, Allocator> const& rhs) noexcept
{
        return lhs.is_proper_subset_of(rhs);
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto intersects(dynamic_bitset<Block, Allocator> const& lhs, dynamic_bitset<Block, Allocator> const& rhs) noexcept
{
        return lhs.intersects(rhs);
}

template<std::unsigned_integral Block, class Allocator>
class proxy_reference;

template<std::unsigned_integral Block, class Allocator>
class proxy_iterator;

template<std::unsigned_integral Block, class Allocator>
class proxy_reference
{
        using rimpl_type = dynamic_bitset<Block, Allocator> const&;
        using value_type = dynamic_bitset<Block, Allocator>::size_type;
        rimpl_type m_ref;
        value_type m_val;
public:
        ~proxy_reference() = default;
        proxy_reference(proxy_reference const&) = default;
        proxy_reference(proxy_reference&&) = default;
        proxy_reference& operator=(proxy_reference const&) = delete;
        proxy_reference& operator=(proxy_reference&&) = delete;

        proxy_reference() = delete;

        [[nodiscard]] explicit constexpr proxy_reference(rimpl_type r, value_type v) noexcept
        :
                m_ref(r),
                m_val(v)
        {
                assert(m_val < m_ref.size());
        }

        [[nodiscard]] constexpr auto operator==(proxy_reference const& other) const noexcept
                -> bool
        {
                return this->m_val == other.m_val;
        }

        [[nodiscard]] constexpr auto operator&() const noexcept
        {
                return proxy_iterator<Block, Allocator>(&m_ref, m_val);
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

template<std::unsigned_integral Block, class Allocator>
class proxy_iterator
{
public:
        using difference_type   = std::ptrdiff_t;
        using value_type        = std::size_t;
        using pointer           = proxy_iterator<Block, Allocator>;
        using reference         = proxy_reference<Block, Allocator>;
        using iterator_category = std::forward_iterator_tag;

private:
        using pimpl_type = dynamic_bitset<Block, Allocator>  const*;
        pimpl_type m_ptr;
        value_type m_val;

public:
        proxy_iterator() = default;

        [[nodiscard]] constexpr proxy_iterator(pimpl_type p, value_type v) noexcept
        :
                m_ptr(p),
                m_val(v)
        {
                assert(m_val < m_ptr->size() || m_val == m_ptr->npos);
        }

        [[nodiscard]] constexpr auto operator==(proxy_iterator const& other) const noexcept
                -> bool
        {
                assert(this->m_ptr == other.m_ptr);
                return this->m_val == other.m_val;
        }

        [[nodiscard]] constexpr auto operator*() const noexcept
        {
                assert(m_val < m_ptr->size());
                return proxy_reference<Block, Allocator>(*m_ptr, m_val);
        }

        auto& operator++() noexcept
        {
                assert(m_val < m_ptr->size());
                m_val = m_ptr->find_next(m_val);
                assert(m_val < m_ptr->size() || m_val == m_ptr->npos);
                return *this;
        }

        auto operator++(int) noexcept
        {
                auto nrv = *this; ++*this; return nrv;
        }
};

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto begin(dynamic_bitset<Block, Allocator>& bs)
{
        return proxy_iterator<Block, Allocator>(&bs, bs.find_first());
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto begin(dynamic_bitset<Block, Allocator> const& bs)
{
        return proxy_iterator<Block, Allocator>(&bs, bs.find_first());
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto end(dynamic_bitset<Block, Allocator>& bs)
{
        return proxy_iterator<Block, Allocator>(&bs, bs.npos);
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto end(dynamic_bitset<Block, Allocator> const& bs)
{
        return proxy_iterator<Block, Allocator>(&bs, bs.npos);
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto cbegin(dynamic_bitset<Block, Allocator> const& bs) noexcept(noexcept(boost::begin(bs)))
        -> decltype(boost::begin(bs))
{
        return boost::begin(bs);
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto cend(dynamic_bitset<Block, Allocator> const& bs) noexcept(noexcept(boost::end(bs)))
        -> decltype(boost::end(bs))
{
        return boost::end(bs);
}

}       // namespace boost
