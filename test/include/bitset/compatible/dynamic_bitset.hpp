#pragma once

//          Copyright Rein Halbersma 2014-2020.
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
auto resize(dynamic_bitset<Block, Allocator>& bs, std::size_t num_bits, bool value = false) noexcept
{
        bs.resize(num_bits, value);
}

template<std::unsigned_integral Block, class Allocator>
auto& left_shift_assign(dynamic_bitset<Block, Allocator>& bs, std::size_t pos) // Throws: Nothing.
{
        return bs <<= pos;
}

template<std::unsigned_integral Block, class Allocator>
auto& right_shift_assign(dynamic_bitset<Block, Allocator>& bs, std::size_t pos) // Throws: Nothing.
{
        return bs >>= pos;
}

template<std::unsigned_integral Block, class Allocator>
auto& set(dynamic_bitset<Block, Allocator>& bs) noexcept
{
        return bs.set();
}

template<std::unsigned_integral Block, class Allocator>
auto& set(dynamic_bitset<Block, Allocator>& bs, std::size_t pos, bool val = true)
{
        if (pos >= bs.size()) {
                throw std::out_of_range{""};
        } else {
                return bs.set(pos, val);
        }
}

template<std::unsigned_integral Block, class Allocator>
auto& reset(dynamic_bitset<Block, Allocator>& bs) noexcept
{
        return bs.reset();
}

template<std::unsigned_integral Block, class Allocator>
auto& reset(dynamic_bitset<Block, Allocator>& bs, std::size_t pos)
{
        if (pos >= bs.size()) {
                throw std::out_of_range{""};
        } else {
                return bs.reset(pos);
        }
}

template<std::unsigned_integral Block, class Allocator>
auto& flip(dynamic_bitset<Block, Allocator>& bs) noexcept
{
        return bs.flip();
}

template<std::unsigned_integral Block, class Allocator>
auto& flip(dynamic_bitset<Block, Allocator>& bs, std::size_t pos)
{
        if (pos >= bs.size()) {
                throw std::out_of_range{""};
        } else {
                return bs.flip(pos);
        }
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] constexpr auto count(dynamic_bitset<Block, Allocator> const& bs) noexcept
{
        return bs.count();
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] constexpr auto fn_size(dynamic_bitset<Block, Allocator> const& bs) noexcept
{
        return bs.size();
}

template<std::unsigned_integral Block, class Allocator>
auto test(dynamic_bitset<Block, Allocator> const& bs, std::size_t pos)
{
        if (pos >= bs.size()) {
                throw std::out_of_range{""};
        } else {
                return bs.test(pos);
        }
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto all(dynamic_bitset<Block, Allocator> const& bs) noexcept
{
        return bs.all();
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto any(dynamic_bitset<Block, Allocator> const& bs) noexcept
{
        return bs.any();
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto none(dynamic_bitset<Block, Allocator> const& bs) noexcept
{
        return bs.none();
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto left_shift(dynamic_bitset<Block, Allocator> const& bs, std::size_t pos) // Throws: Nothing.
{
        return bs << pos;
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto right_shift(dynamic_bitset<Block, Allocator> const& bs, std::size_t pos) // Throws: Nothing.
{
        return bs >> pos;
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto at(dynamic_bitset<Block, Allocator> const& bs, std::size_t pos) // Throws: Nothing.
{
        return bs[pos];
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto at(dynamic_bitset<Block, Allocator>& bs, std::size_t pos) // Throws: Nothing.
{
        return bs[pos];
}

template<std::unsigned_integral Block, class Allocator>
auto at(dynamic_bitset<Block, Allocator>& bs, std::size_t pos, bool val) // Throws: Nothing.
{
        return bs[pos] = val;
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto is_subset_of(dynamic_bitset<Block, Allocator> const& lhs, dynamic_bitset<Block, Allocator> const& rhs) noexcept
{
        return lhs.is_subset_of(rhs);
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto is_superset_of(dynamic_bitset<Block, Allocator> const& lhs, dynamic_bitset<Block, Allocator> const& rhs) noexcept
{
        return rhs.is_subset_of(lhs);
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto is_proper_subset_of(dynamic_bitset<Block, Allocator> const& lhs, dynamic_bitset<Block, Allocator> const& rhs) noexcept
{
        return lhs.is_proper_subset_of(rhs);
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto is_proper_superset_of(dynamic_bitset<Block, Allocator> const& lhs, dynamic_bitset<Block, Allocator> const& rhs) noexcept
{
        return rhs.is_proper_subset_of(lhs);
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto intersects(dynamic_bitset<Block, Allocator> const& lhs, dynamic_bitset<Block, Allocator> const& rhs) noexcept
{
        return lhs.intersects(rhs);
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto is_disjoint(dynamic_bitset<Block, Allocator> const& lhs, dynamic_bitset<Block, Allocator> const& rhs) noexcept
{
        return !lhs.intersects(rhs);
}

template<std::unsigned_integral Block, class Allocator>
class proxy_reference;

template<std::unsigned_integral Block, class Allocator>
class proxy_iterator;

template<std::unsigned_integral Block, class Allocator>
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

        [[nodiscard]] constexpr proxy_reference(dynamic_bitset<Block, Allocator> const& bs, value_type const& v) noexcept
        :
                m_bs{bs},
                m_value{v}
        {
                assert(0 <= m_value && m_value < bs.size());
        }

        [[nodiscard]] constexpr auto operator&() const noexcept
                -> proxy_iterator<Block, Allocator>
        {
                return { &m_bs, m_value };
        }

        [[nodiscard]] explicit(false) constexpr operator value_type() const noexcept
        {
                return m_value;
        }

        template<class T>
                requires std::is_class_v<T> && std::constructible_from<T, value_type>
        [[nodiscard]] explicit(false) constexpr operator T() const noexcept(noexcept(T(m_value)))
        {
                return m_value;
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
        dynamic_bitset<Block, Allocator>  const* m_bs;
        value_type m_value;

public:
        proxy_iterator() = default;

        [[nodiscard]] constexpr proxy_iterator(dynamic_bitset<Block, Allocator> const* bs, value_type const& v) // Throws: Nothing.
        :
                m_bs{bs},
                m_value{v}
        {
                assert(0 <= m_value && (m_value < bs->size() || m_value == bs->npos));
        }

        [[nodiscard]] constexpr auto operator*() const // Throws: Nothing.
                -> proxy_reference<Block, Allocator>
        {
                assert(0 <= m_value && m_value < m_bs->size());
                return { *m_bs, m_value };
        }

        auto& operator++() // Throws: Nothing.
        {
                assert(0 <= m_value && m_value < m_bs->size());
                m_value = m_bs->find_next(m_value);
                assert(0 < m_value && (m_value < m_bs->size() || m_value == m_bs->npos));
                return *this;
        }

        auto operator++(int) // Throws: Nothing.
        {
                auto nrv = *this; ++*this; return nrv;
        }

        [[nodiscard]] constexpr auto operator==(proxy_iterator const& other) const noexcept
                -> bool
        {
                assert(m_bs == other.m_bs);
                return m_value == other.m_value;
        }
};

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto begin(dynamic_bitset<Block, Allocator>& bs)
        -> proxy_iterator<Block, Allocator>
{
        return { &bs, bs.find_first() };
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto begin(dynamic_bitset<Block, Allocator> const& bs)
        -> proxy_iterator<Block, Allocator>
{
        return { &bs, bs.find_first() };
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto end(dynamic_bitset<Block, Allocator>& bs)
        -> proxy_iterator<Block, Allocator>
{
        return { &bs, bs.npos };
}

template<std::unsigned_integral Block, class Allocator>
[[nodiscard]] auto end(dynamic_bitset<Block, Allocator> const& bs)
        -> proxy_iterator<Block, Allocator>
{
        return { &bs, bs.npos };
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
