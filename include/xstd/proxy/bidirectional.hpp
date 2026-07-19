#ifndef XSTD_PROXY_BIDIRECTIONAL_HPP
#define XSTD_PROXY_BIDIRECTIONAL_HPP

//          Copyright Rein Halbersma 2014-2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cassert>      // assert
#include <concepts>     // constructible_from, convertible_to
#include <cstddef>      // ptrdiff_t, size_t
#include <iterator>     // bidirectional_iterator_tag
#include <ranges>       // view_base
#include <type_traits>  // is_class_v, is_convertible_v, is_nothrow_constructible_v

namespace xstd::proxy::bidirectional {

// Bits customizes its iteration either by providing hidden friends
// find_first/find_last/find_next/find_prev discoverable via ADL (the
// default below, delegating to them - used by xstd's own types, e.g.
// bit_set/bitset, whose associated namespace is xstd and can legitimately
// hold them), or by giving find<Bits> an explicit specialization for a
// foreign type that cannot provide those via ADL - e.g. std::bitset<N>,
// whose only associated namespace is std, where a program may not add
// declarations [namespace.std]; or boost::dynamic_bitset<>, whose own
// member begin()/end() can shadow a same-named ADL free function. Calls to
// find_first(c) and friends below are dependent (c's type is the template
// parameter Bits), so non-ADL unqualified lookup for them is fixed at this
// point of definition and can never see a later header's free functions -
// only ADL, deferred to each point of instantiation, can. find<Bits>'s
// specializations aren't subject to that: specialization matching considers
// any specialization visible before the point of use, regardless of which
// header declares it, so it works for foreign types where ADL cannot.
template<class Bits>
struct find
{
        [[nodiscard]] static constexpr std::size_t first(Bits const& c) noexcept { return find_first(c); }
        [[nodiscard]] static constexpr std::size_t last (Bits const& c) noexcept { return find_last (c); }
        [[nodiscard]] static constexpr std::size_t next (Bits const& c, std::size_t n) noexcept { return find_next(c, n); }
        [[nodiscard]] static constexpr std::size_t prev (Bits const& c, std::size_t n) noexcept { return find_prev(c, n); }
};

template<class Bits>
concept bit_range =
        requires(Bits const& c)
        {
                { find<Bits>::first(c) } -> std::convertible_to<std::size_t>;
                { find<Bits>::last (c) } -> std::convertible_to<std::size_t>;
        } and
        requires(Bits const& c, std::size_t n)
        {
                { find<Bits>::next(c, n) } -> std::convertible_to<std::size_t>;
                { find<Bits>::prev(c, n) } -> std::convertible_to<std::size_t>;
        }
;

template<bit_range> class iterator;
template<bit_range> class reference;

template<bit_range Bits>
class iterator
{
        Bits const* m_ptr{};
        std::size_t m_idx{};

        friend Bits;
        friend class reference<Bits>;
        friend constexpr auto begin <>(Bits const& c) noexcept -> iterator;
        friend constexpr auto end   <>(Bits const& c) noexcept -> iterator;

        [[nodiscard]] constexpr iterator(Bits const* ptr, std::size_t idx) noexcept
        :
                m_ptr(ptr),
                m_idx(idx)
        {
                assert(m_ptr != nullptr);
        }

public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type        = std::size_t;
        using difference_type   = std::ptrdiff_t;
        using pointer           = void;
        using reference         = bidirectional::reference<Bits>;

        [[nodiscard]] constexpr iterator() noexcept = default;

        [[nodiscard]] friend constexpr bool operator==(iterator lhs, iterator rhs) noexcept
        {
                assert(lhs.m_ptr == rhs.m_ptr);
                return lhs.m_idx == rhs.m_idx;
        }

        [[nodiscard]] constexpr reference operator*() const noexcept
        {
                assert(m_ptr != nullptr);
                return { *m_ptr, m_idx };
        }

        constexpr iterator& operator++() noexcept { assert(m_ptr != nullptr); m_idx = find<Bits>::next(*m_ptr, m_idx); return *this; }
        constexpr iterator& operator--() noexcept { assert(m_ptr != nullptr); m_idx = find<Bits>::prev(*m_ptr, m_idx); return *this; }

        constexpr iterator operator++(int) noexcept { auto nrv = *this; ++*this; return nrv; }
        constexpr iterator operator--(int) noexcept { auto nrv = *this; --*this; return nrv; }
};

template<bit_range Bits> [[nodiscard]] constexpr iterator<Bits> begin(Bits const& c) noexcept { return { &c, find<Bits>::first(c) }; }
template<bit_range Bits> [[nodiscard]] constexpr iterator<Bits> end  (Bits const& c) noexcept { return { &c, find<Bits>::last (c) }; }

template<bit_range Bits>
class reference
{
        Bits const& m_ref;
        std::size_t m_idx;

        friend Bits;
        friend class iterator<Bits>;

        [[nodiscard]] constexpr reference(Bits const& ref, std::size_t idx) noexcept
        :
                m_ref(ref),
                m_idx(idx)
        {}

public:
        using value_type = std::size_t;
        using iterator   = bidirectional::iterator<Bits>;

        [[nodiscard]] constexpr iterator operator&() const noexcept
        {
                return { &m_ref, m_idx };
        }

        [[nodiscard]] constexpr explicit(false) operator value_type() const noexcept
        {
                return m_idx;
        }

        template<std::constructible_from<value_type> T>
        [[nodiscard]] constexpr explicit(not std::is_convertible_v<value_type, T>) operator T() const noexcept(std::is_nothrow_constructible_v<T, value_type>)
                requires std::is_class_v<T>
        {
                return m_idx;
        }
};

template<bit_range Bits>
[[nodiscard]] constexpr auto format_as(reference<Bits> ref) noexcept
        -> reference<Bits>::value_type
{
        return ref;
}

// A non-owning adaptor with member begin()/end(), for Bits types whose own
// member iteration (if any) does not yield this bit_range's set-of-size_t
// semantics - e.g. boost::dynamic_bitset<>, whose own member begin()/end()
// (added upstream after this ADL customization was written) shadow the
// find_first/find_next-based free functions above in ordinary range-for and
// std::ranges algorithms alike, since a range's own members are always
// preferred over ADL. Wrapping in view<Bits> sidesteps that: the adaptor
// itself has no competing members, so its begin()/end() are what get used.
template<bit_range Bits>
class view : public std::ranges::view_base
{
        Bits const* m_ptr;

public:
        [[nodiscard]] constexpr explicit view(Bits const& c) noexcept : m_ptr(&c) {}

        [[nodiscard]] constexpr auto begin() const noexcept { return bidirectional::begin(*m_ptr); }
        [[nodiscard]] constexpr auto end()   const noexcept { return bidirectional::end  (*m_ptr); }
};

template<bit_range Bits>
view(Bits const&) -> view<Bits>;

}       // namespace xstd::proxy::bidirectional

#endif  // include guard
