#pragma once

//          Copyright Rein Halbersma 2014-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <boost/dynamic_bitset.hpp>     // dynamic_bitset
#include <cstddef>                      // ptrdiff_t, size_t
#include <iterator>                     // forward_iterator_tag
#include <stdexcept>                    // out_of_range

namespace boost {

template<class Block, class Allocator>
auto resize(dynamic_bitset<Block, Allocator>& bs, std::size_t const num_bits, bool const value = false) noexcept
{
        bs.resize(num_bits, value);
}

template<class Block, class Allocator>
auto& set(dynamic_bitset<Block, Allocator>& bs) noexcept
{
        return bs.set();
}

template<class Block, class Allocator>
auto& set(dynamic_bitset<Block, Allocator>& bs, std::size_t const pos, bool const val = true)
{
        if (pos >= bs.size()) {
                throw std::out_of_range{""};
        } else {
                return bs.set(pos, val);
        }
}

template<class Block, class Allocator>
auto& reset(dynamic_bitset<Block, Allocator>& bs) noexcept
{
        return bs.reset();
}

template<class Block, class Allocator>
auto& reset(dynamic_bitset<Block, Allocator>& bs, std::size_t const pos)
{
        if (pos >= bs.size()) {
                throw std::out_of_range{""};
        } else {
                return bs.reset(pos);
        }
}

template<class Block, class Allocator>
auto& flip(dynamic_bitset<Block, Allocator>& bs) noexcept
{
        return bs.flip();
}

template<class Block, class Allocator>
auto& flip(dynamic_bitset<Block, Allocator>& bs, std::size_t const pos)
{
        if (pos >= bs.size()) {
                throw std::out_of_range{""};
        } else {
                return bs.flip(pos);
        }
}

template<class Block, class Allocator>
constexpr auto count(dynamic_bitset<Block, Allocator> const& bs) noexcept
{
        return bs.count();
}

template<class Block, class Allocator>
constexpr auto fn_size(dynamic_bitset<Block, Allocator> const& bs) noexcept
{
        return bs.size();
}

template<class Block, class Allocator>
auto test(dynamic_bitset<Block, Allocator> const& bs, std::size_t const pos)
{
        if (pos >= bs.size()) {
                throw std::out_of_range{""};
        } else {
                return bs.test(pos);
        }
}

template<class Block, class Allocator>
[[nodiscard]] auto all(dynamic_bitset<Block, Allocator> const& bs) noexcept
{
        return bs.all();
}

template<class Block, class Allocator>
[[nodiscard]] auto any(dynamic_bitset<Block, Allocator> const& bs) noexcept
{
        return bs.any();
}

template<class Block, class Allocator>
[[nodiscard]] auto none(dynamic_bitset<Block, Allocator> const& bs) noexcept
{
        return bs.none();
}

template<class Block, class Allocator>
auto at(dynamic_bitset<Block, Allocator> const& bs, std::size_t const pos) // Throws: Nothing.
{
        return bs[pos];
}

template<class Block, class Allocator>
auto at(dynamic_bitset<Block, Allocator>& bs, std::size_t const pos) // Throws: Nothing.
{
        return bs[pos];
}

template<class Block, class Allocator>
auto at(dynamic_bitset<Block, Allocator>& bs, std::size_t const pos, bool const val) // Throws: Nothing.
{
        return bs[pos] = val;
}

template<class Block, class Allocator>
auto is_subset_of(dynamic_bitset<Block, Allocator> const& lhs, dynamic_bitset<Block, Allocator> const& rhs) noexcept
{
        return lhs.is_subset_of(rhs);
}

template<class Block, class Allocator>
auto is_superset_of(dynamic_bitset<Block, Allocator> const& lhs, dynamic_bitset<Block, Allocator> const& rhs) noexcept
{
        return is_subset_of(rhs, lhs);
}

template<class Block, class Allocator>
auto is_proper_subset_of(dynamic_bitset<Block, Allocator> const& lhs, dynamic_bitset<Block, Allocator> const& rhs) noexcept
{
        return lhs.is_proper_subset_of(rhs);
}

template<class Block, class Allocator>
auto is_proper_superset_of(dynamic_bitset<Block, Allocator> const& lhs, dynamic_bitset<Block, Allocator> const& rhs) noexcept
{
        return rhs.is_proper_subset_of(lhs);
}

template<class Block, class Allocator>
auto intersects(dynamic_bitset<Block, Allocator> const& lhs, dynamic_bitset<Block, Allocator> const& rhs) noexcept
{
        return lhs.intersects(rhs);
}

template<class Block, class Allocator>
auto disjoint(dynamic_bitset<Block, Allocator> const& lhs, dynamic_bitset<Block, Allocator> const& rhs) noexcept
{
        return !intersects(lhs, rhs);
}

template<class Block, class Allocator>
class proxy_reference;

template<class Block, class Allocator>
class proxy_iterator;

template<class Block, class Allocator>
class proxy_reference
{
        using value_type = typename dynamic_bitset<Block, Allocator>::size_type;
        dynamic_bitset<Block, Allocator> const& m_bs;
        value_type const m_value;
public:
        ~proxy_reference() = default;
        proxy_reference(proxy_reference const&) = default;
        proxy_reference(proxy_reference&&) = default;
        proxy_reference& operator=(proxy_reference const&) = delete;
        proxy_reference& operator=(proxy_reference&&) = delete;

        proxy_reference() = delete;

        constexpr proxy_reference(dynamic_bitset<Block, Allocator> const& bs, value_type const v) noexcept
        :
                m_bs{bs},
                m_value{v}
        {
                assert(0 <= m_value); assert(m_value < bs.size());
        }

        proxy_reference& operator=(value_type const) = delete;

        /* explicit(false) */ constexpr operator value_type() const noexcept
        {
                return m_value;
        }

        template<class T, std::enable_if_t<
                std::is_class_v<T> &&
                std::is_constructible_v<T, value_type>
        >...>
        /* explicit(false) */ constexpr operator T() const noexcept(noexcept(T(m_value)))
        {
                return m_value;
        }

        constexpr auto operator&() const noexcept
                -> proxy_iterator<Block, Allocator>
        {
                return { &m_bs, m_value };
        }
};

template<class Block, class Allocator>
class proxy_iterator
{
public:
        using difference_type   = std::ptrdiff_t;
        using value_type        = std::size_t;
        using pointer           = proxy_iterator<Block, Allocator>;
        using reference         = proxy_reference<Block, Allocator>;
        using iterator_category = std::forward_iterator_tag;

private:
        dynamic_bitset<Block, Allocator>  const* m_bs;
        value_type m_value;

public:
        proxy_iterator() = default;

        constexpr proxy_iterator(dynamic_bitset<Block, Allocator> const* bs, value_type const v) // Throws: Nothing.
        :
                m_bs{bs},
                m_value{v}
        {
                assert(0 <= m_value); assert(m_value < bs->size() || m_value == bs->npos);
        }

        constexpr auto operator*() const // Throws: Nothing.
                -> proxy_reference<Block, Allocator>
        {
                assert(0 <= m_value); assert(m_value < m_bs->size());
                return { *m_bs, m_value };
        }

        auto& operator++() // Throws: Nothing.
        {
                assert(0 <= m_value); assert(m_value < m_bs->size());
                m_value = m_bs->find_next(m_value);
                assert(0 < m_value); assert(m_value < m_bs->size() || m_value == m_bs->npos);
                return *this;
        }

        auto operator++(int) // Throws: Nothing.
        {
                auto nrv = *this; ++*this; return nrv;
        }

        friend constexpr auto operator==(proxy_iterator const& lhs, proxy_iterator const& rhs) noexcept
        {
                assert(lhs.m_bs == rhs.m_bs);
                return lhs.m_value == rhs.m_value;
        }

        friend constexpr auto operator!=(proxy_iterator const& lhs, proxy_iterator const& rhs) noexcept
        {
                return !(lhs == rhs);
        }
};

template<class Block, class Allocator>
auto begin(dynamic_bitset<Block, Allocator>& bs)
        -> proxy_iterator<Block, Allocator>
{
        return { &bs, bs.find_first() };
}

template<class Block, class Allocator>
auto begin(dynamic_bitset<Block, Allocator> const& bs)
        -> proxy_iterator<Block, Allocator>
{
        return { &bs, bs.find_first() };
}

template<class Block, class Allocator>
auto end(dynamic_bitset<Block, Allocator>& bs)
        -> proxy_iterator<Block, Allocator>
{
        return { &bs, bs.npos };
}

template<class Block, class Allocator>
auto end(dynamic_bitset<Block, Allocator> const& bs)
        -> proxy_iterator<Block, Allocator>
{
        return { &bs, bs.npos };
}

template<class Block, class Allocator>
auto cbegin(dynamic_bitset<Block, Allocator> const& bs) noexcept(noexcept(boost::begin(bs)))
        -> decltype(boost::begin(bs))
{
        return boost::begin(bs);
}

template<class Block, class Allocator>
auto cend(dynamic_bitset<Block, Allocator> const& bs) noexcept(noexcept(boost::end(bs)))
        -> decltype(boost::end(bs))
{
        return boost::end(bs);
}

}       // namespace boost
