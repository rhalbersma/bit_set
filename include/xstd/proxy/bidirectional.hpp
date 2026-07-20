#ifndef XSTD_PROXY_BIDIRECTIONAL_HPP
#define XSTD_PROXY_BIDIRECTIONAL_HPP

//          Copyright Rein Halbersma 2014-2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>    // includes, lexicographical_compare_three_way
#include <cassert>      // assert
#include <compare>      // strong_ordering
#include <concepts>     // constructible_from, convertible_to
#include <cstddef>      // ptrdiff_t, size_t
#include <iterator>     // bidirectional_iterator_tag, make_reverse_iterator
#include <ranges>       // equal, view_base
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
//
// Each member below is individually constrained on the underlying ADL call
// actually being valid, rather than just declared with a fixed return type.
// Without that, `find<Bits>::first(c)` would be a well-formed *expression*
// for any Bits at all (the declaration alone doesn't depend on whether
// find_first(c) in the body compiles - body instantiation is lazy and, on
// failure, a hard error rather than SFINAE), which made bit_range<Bits> a
// false positive for every Bits, including reference<Bits> itself: nothing
// stopped reference<reference<Bits>> from being formed, recursively without
// end. Constraining each member here makes it (and so bit_range) correctly
// SFINAE away when Bits doesn't actually provide the customization.
template<class Bits>
struct find
{
        [[nodiscard]] static constexpr std::size_t first(Bits const& c) noexcept
                requires requires { { find_first(c) } -> std::convertible_to<std::size_t>; }
        {
                return find_first(c);
        }

        [[nodiscard]] static constexpr std::size_t last(Bits const& c) noexcept
                requires requires { { find_last(c) } -> std::convertible_to<std::size_t>; }
        {
                return find_last(c);
        }

        [[nodiscard]] static constexpr std::size_t next(Bits const& c, std::size_t n) noexcept
                requires requires { { find_next(c, n) } -> std::convertible_to<std::size_t>; }
        {
                return find_next(c, n);
        }

        [[nodiscard]] static constexpr std::size_t prev(Bits const& c, std::size_t n) noexcept
                requires requires { { find_prev(c, n) } -> std::convertible_to<std::size_t>; }
        {
                return find_prev(c, n);
        }
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
template<bit_range> struct compare;

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
//
// key_type mirrors bit_set/bitset's own nested key_type: fmt's range
// formatter (fmt/ranges.h) detects "format like a set" purely by checking
// for a nested key_type, so a view<Bits> already formats with {} delimiters
// on its own.
template<bit_range Bits>
class view : public std::ranges::view_base
{
        Bits const* m_ptr;

public:
        using key_type = std::size_t;

        [[nodiscard]] constexpr explicit view(Bits const& c) noexcept : m_ptr(&c) {}

        // begin()/cbegin() (and end()/cend()) coincide: this proxy iteration
        // is inherently read-only (reference<Bits> only converts to
        // std::size_t, there is no assignment-through-iterator here), same
        // as bit_set/bitset's own cbegin()/cend() are plain aliases for
        // begin()/end() rather than a distinct const-iteration path.
        [[nodiscard]] constexpr auto begin() const noexcept { return bidirectional::begin(*m_ptr); }
        [[nodiscard]] constexpr auto end()   const noexcept { return bidirectional::end  (*m_ptr); }
        [[nodiscard]] constexpr auto rbegin() const noexcept { return std::make_reverse_iterator(end());   }
        [[nodiscard]] constexpr auto rend()   const noexcept { return std::make_reverse_iterator(begin()); }

        [[nodiscard]] constexpr auto cbegin()  const noexcept { return begin();  }
        [[nodiscard]] constexpr auto cend()    const noexcept { return end();    }
        [[nodiscard]] constexpr auto crbegin() const noexcept { return rbegin(); }
        [[nodiscard]] constexpr auto crend()   const noexcept { return rend();   }

        // Prefer Bits' own == when it has one (cheaper than iterating through
        // the proxy iterators below), else compare elementwise. Unlike
        // ordering (see is_subset_of and friends below for why <=> is
        // deliberately not provided here), equality of two sets is
        // unambiguous regardless of any bitset's internal bit-layout
        // convention, so this is purely an optimization.
        [[nodiscard]] friend constexpr bool operator==(view lhs, view rhs) noexcept
        {
                if constexpr (requires { *lhs.m_ptr == *rhs.m_ptr; }) {
                        return *lhs.m_ptr == *rhs.m_ptr;
                } else {
                        return std::ranges::equal(lhs, rhs);
                }
        }

        // Unlike == and the predicates above, this deliberately never prefers
        // Bits' own <=> even when it has one: a data-parallel word-at-a-time
        // <=> is only guaranteed to agree with std::set<int>'s
        // lexicographic-of-elements ordering when both operands have equal
        // cardinality in general (bit_set/bitset's own <=> is a case that
        // does get this right for every cardinality - see compare<Bits> -
        // but nothing here can assume that of an arbitrary Bits). Always
        // going through compare<Bits>::lexicographical_three_way keeps this
        // correct for every Bits, at the cost of the word-parallel fast path
        // a trusted native <=> could offer.
        [[nodiscard]] friend constexpr std::strong_ordering operator<=>(view lhs, view rhs) noexcept
        {
                return compare<Bits>::lexicographical_three_way(*lhs.m_ptr, *rhs.m_ptr);
        }

        // Same prefer-native-else-compute-from-iteration pattern as ==
        // above. The fallbacks rely on both views iterating their elements
        // in ascending order (guaranteed by the bit_range contract), so
        // std::ranges::includes and a linear merge scan apply directly.
        [[nodiscard]] constexpr bool is_subset_of(view other) const noexcept
        {
                if constexpr (requires { m_ptr->is_subset_of(*other.m_ptr); }) {
                        return m_ptr->is_subset_of(*other.m_ptr);
                } else {
                        return std::ranges::includes(other, *this);
                }
        }

        [[nodiscard]] constexpr bool is_proper_subset_of(view other) const noexcept
        {
                if constexpr (requires { m_ptr->is_proper_subset_of(*other.m_ptr); }) {
                        return m_ptr->is_proper_subset_of(*other.m_ptr);
                } else {
                        return is_subset_of(other) and *this != other;
                }
        }

        [[nodiscard]] constexpr bool intersects(view other) const noexcept
        {
                if constexpr (requires { m_ptr->intersects(*other.m_ptr); }) {
                        return m_ptr->intersects(*other.m_ptr);
                } else {
                        auto first1 = begin(), last1 = end();
                        auto first2 = other.begin(), last2 = other.end();
                        while (first1 != last1 and first2 != last2) {
                                if (*first1 < *first2) {
                                        ++first1;
                                } else if (*first2 < *first1) {
                                        ++first2;
                                } else {
                                        return true;
                                }
                        }
                        return false;
                }
        }
};

template<bit_range Bits>
view(Bits const&) -> view<Bits>;

// compare<Bits>::lexicographical_three_way always computes std::set<int>-
// equivalent ordering by iterating both Bits' elements via view - it never
// looks at Bits' own <=> (if it has one) at all. That's deliberate, not an
// oversight: a foreign Bits (std::bitset<N>, boost::dynamic_bitset<>) is
// free to add its own <=> in the future with whatever semantics its own
// standard mandates (e.g. sequence-of-bool, matching std::bitset's own
// element/index order rather than std::set<int>'s), and there is no way to
// know from here whether such a <=> would agree with the ordering this
// library promises. Always computing from iteration side-steps that
// question entirely, at the cost of the word-parallel fast path a trusted
// native <=> could offer - xstd's own bit_set/bitset can specialize this
// trait later once (if) their own <=> is confirmed to compute the same
// ordering, without changing anything at the call sites below.
template<bit_range Bits>
struct compare
{
        [[nodiscard]] static constexpr std::strong_ordering lexicographical_three_way(Bits const& x, Bits const& y) noexcept
        {
                auto const xv = view(x);
                auto const yv = view(y);
                return std::lexicographical_compare_three_way(
                        xv.begin(), xv.end(),
                        yv.begin(), yv.end()
                );
        }
};

}       // namespace xstd::proxy::bidirectional

#endif  // include guard
