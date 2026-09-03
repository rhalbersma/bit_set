//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_BITS_RANGES_SET_VIEW_HPP
#define XSTD_BITS_RANGES_SET_VIEW_HPP

#include <xstd/bits/detail/intrin.hpp>       // countr_zero
#include <xstd/bits/ranges/bit_extent.hpp>   // bit_extent, static_bit_extent
#include <xstd/bits/ranges/block_access.hpp> // block_access, block_range, first_difference, any_above
#include <algorithm>                         // includes
#include <cassert>                           // assert
#include <compare>                           // strong_ordering
#include <concepts>                          // constructible_from, convertible_to
#include <cstddef>                           // ptrdiff_t, size_t
#include <functional>                        // less
#include <initializer_list>                  // initializer_list
#include <iterator>                          // bidirectional_iterator_tag, make_reverse_iterator, reverse_iterator
#include <ranges>                            // distance, equal, view_base
#include <type_traits>                       // is_class_v, is_const_v, is_convertible_v, is_nothrow_constructible_v, remove_const_t
#include <utility>                           // pair

// A std::set<std::size_t> over bits it does not own: span's hook direction, non-owning, and mutable through.
namespace xstd::ranges {

// Where the elements are: ADL hidden friends by default, an explicit specialization for foreign types, each member constrained.
template<class Bits>
struct set_find
{
        [[nodiscard]] static constexpr auto first(Bits const& c) noexcept
                -> std::size_t
                requires requires { { find_first(c) } -> std::convertible_to<std::size_t>; }
        {
                return find_first(c);
        }

        [[nodiscard]] static constexpr auto last(Bits const& c) noexcept
                -> std::size_t
                requires requires { { find_last(c) } -> std::convertible_to<std::size_t>; }
        {
                return find_last(c);
        }

        [[nodiscard]] static constexpr auto next(Bits const& c, std::size_t n) noexcept
                -> std::size_t
                requires requires { { find_next(c, n) } -> std::convertible_to<std::size_t>; }
        {
                return find_next(c, n);
        }

        [[nodiscard]] static constexpr auto prev(Bits const& c, std::size_t n) noexcept
                -> std::size_t
                requires requires { { find_prev(c, n) } -> std::convertible_to<std::size_t>; }
        {
                return find_prev(c, n);
        }
};

// Which vocabulary a Bits speaks, decided once by no-arg count(): probing member by member picks clear() and empties the container.
template<class Bits>
concept bitset_vocabulary = requires(Bits const& c)
{
        { c.count() } -> std::convertible_to<std::size_t>;
};

// The vocabulary, rewired; this needs no ADL, a member call being found whoever declared it.
template<class Bits>
struct set_ops
{
        // A bitset's count() is a set's size().
        [[nodiscard]] static constexpr auto size(Bits const& c) noexcept
                -> std::size_t
        {
                if constexpr (bitset_vocabulary<Bits>) {
                        return c.count();
                } else {
                        return c.size();
                }
        }

        // and a bitset's size() is a set's max_size(), constant where the type declares its width.
        [[nodiscard]] static constexpr auto max_size(Bits const& c) noexcept
                -> std::size_t
        {
                if constexpr (static_bit_extent<Bits>) {
                        return bit_extent<Bits>;
                } else if constexpr (bitset_vocabulary<Bits>) {
                        return c.size();
                } else {
                        return c.max_size();
                }
        }

        // Two models meet here. A static extent is a fixed-capacity set that cannot grow, so -- as with
        // std::array -- a position outside it is a precondition violation and not a question worth an
        // answer. A dynamic extent can still grow, so it is a std::set: any key may be asked about, and one
        // past the current size is simply absent, which is what lets set_view::find(x) return end().
        //
        // Either way the position is in range by the time the bitset is read, so the read goes through
        // operator[] and not test(): the latter is the checked accessor whose throw would escape this
        // noexcept, and bugprone-exception-escape is right to say so.
        [[nodiscard]] static constexpr auto contains(Bits const& c, std::size_t n) noexcept
                -> bool
        {
                if constexpr (bitset_vocabulary<Bits>) {
                        if constexpr (static_bit_extent<Bits>) {
                                // The assert is on its own line: the coverage job drops assert branches by matching the start of the line.
                                assert(n < max_size(c));
                        } else if (n >= max_size(c)) {
                                return false;
                        }
                        return c[n];
                } else {
                        return c.contains(n);
                }
        }

        static constexpr void insert(Bits& c, std::size_t n) noexcept
        {
                // The assert is on its own line: the coverage job drops assert branches by matching the start of the line.
                assert(n < max_size(c));
                if constexpr (bitset_vocabulary<Bits>) {
                        c.set(n);
                } else {
                        c.insert(n);
                }
        }

        static constexpr void erase(Bits& c, std::size_t n) noexcept
        {
                if constexpr (bitset_vocabulary<Bits>) {
                        c.reset(n);
                } else {
                        c.erase(n);
                }
        }

        // reset(), never clear(): see bitset_vocabulary above for what dynamic_bitset::clear() would have done.
        static constexpr void clear(Bits& c) noexcept
        {
                if constexpr (bitset_vocabulary<Bits>) {
                        c.reset();
                } else {
                        c.clear();
                }
        }
};

// The set ordering, defined once: the keys in increasing order, lexicographically, over ranges so a block path can replace it.
template<std::ranges::input_range X, std::ranges::input_range Y>
[[nodiscard]] constexpr auto set_three_way(X const& x, Y const& y) noexcept
        -> std::strong_ordering
{
        return std::lexicographical_compare_three_way(
                std::ranges::begin(x), std::ranges::end(x),
                std::ranges::begin(y), std::ranges::end(y)
        );
}

// The same ordering a word at a time; the set HAVING the lowest differing bit is smaller, unless the other is a prefix.
template<block_range Bits>
[[nodiscard]] constexpr auto set_three_way(Bits const& x, Bits const& y) noexcept
        -> std::strong_ordering
{
        using access = block_access<Bits>;
        using block_type = decltype(access::block(x, 0UZ));

        auto const [index, diff] = first_difference(x, y);
        if (diff == block_type{}) {
                return std::strong_ordering::equal;
        }

        auto const offset = detail::bits::countr_zero(diff);
        auto const x_has  = (static_cast<block_type>(access::block(x, index) >> offset) & block_type{1}) != block_type{};

        if (x_has) {
                return any_above(y, index, offset) ? std::strong_ordering::less : std::strong_ordering::greater;
        }
        return any_above(x, index, offset) ? std::strong_ordering::greater : std::strong_ordering::less;
}

template<class Bits_cv, class Bits = std::remove_const_t<Bits_cv>>
concept set_range =
        requires(Bits const& c)
        {
                { set_find<Bits>::first(c) } -> std::convertible_to<std::size_t>;
                { set_find<Bits>::last (c) } -> std::convertible_to<std::size_t>;
        } and
        requires(Bits const& c, std::size_t n)
        {
                { set_find<Bits>::next(c, n) } -> std::convertible_to<std::size_t>;
                { set_find<Bits>::prev(c, n) } -> std::convertible_to<std::size_t>;
        }
;

// Declared here, constraint and all, so set_iterator below can befriend both spellings.
template<set_range Bits> class set_view;

template<class> class set_iterator;
template<class> class set_reference;
template<set_range> struct set_compare;

// Forward-declared so set_iterator's dependent friend template-ids have a template to name; Clang requires it, GCC does not.
template<set_range Bits> [[nodiscard]] constexpr auto set_begin(Bits const& c) noexcept -> set_iterator<Bits>;
template<set_range Bits> [[nodiscard]] constexpr auto set_end  (Bits const& c) noexcept -> set_iterator<Bits>;

template<class Bits>
class set_iterator
{
        Bits const* m_ptr{};
        std::size_t m_idx{};

        friend Bits;
        friend class set_reference<Bits>;
        friend class set_view<Bits>;
        friend class set_view<Bits const>;
        friend constexpr auto set_begin <>(Bits const& c) noexcept -> set_iterator;
        friend constexpr auto set_end   <>(Bits const& c) noexcept -> set_iterator;

        [[nodiscard]] constexpr set_iterator(Bits const* ptr, std::size_t idx) noexcept
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
        using reference         = set_reference<Bits>;

        [[nodiscard]] constexpr set_iterator() noexcept = default;

        [[nodiscard]] friend constexpr auto operator==(set_iterator lhs, set_iterator rhs) noexcept
                -> bool
        {
                assert(lhs.m_ptr == rhs.m_ptr);
                return lhs.m_idx == rhs.m_idx;
        }

        [[nodiscard]] constexpr auto operator*() const noexcept
                -> reference
        {
                assert(m_ptr != nullptr);
                return { *m_ptr, m_idx };
        }

        // The assert is on its own line: the coverage job drops assert branches by matching the start of the line.
        constexpr auto operator++() noexcept
                -> set_iterator&
                requires requires (Bits const& c, std::size_t n) { set_find<Bits>::next(c, n); }
        {
                assert(m_ptr != nullptr);
                m_idx = set_find<Bits>::next(*m_ptr, m_idx);
                return *this;
        }

        constexpr auto operator--() noexcept
                -> set_iterator&
                requires requires (Bits const& c, std::size_t n) { set_find<Bits>::prev(c, n); }
        {
                assert(m_ptr != nullptr);
                m_idx = set_find<Bits>::prev(*m_ptr, m_idx);
                return *this;
        }

        constexpr auto operator++(int) noexcept -> set_iterator { auto nrv = *this; ++*this; return nrv; }
        constexpr auto operator--(int) noexcept -> set_iterator { auto nrv = *this; --*this; return nrv; }
};

template<set_range Bits> [[nodiscard]] constexpr auto set_begin(Bits const& c) noexcept -> set_iterator<Bits> { return { &c, set_find<Bits>::first(c) }; }
template<set_range Bits> [[nodiscard]] constexpr auto set_end  (Bits const& c) noexcept -> set_iterator<Bits> { return { &c, set_find<Bits>::last (c) }; }

template<class Bits>
class set_reference
{
        Bits const& m_ref;
        std::size_t m_idx;

        friend Bits;
        friend class set_iterator<Bits>;

        [[nodiscard]] constexpr set_reference(Bits const& ref, std::size_t idx) noexcept
        :
                m_ref(ref),
                m_idx(idx)
        {}

public:
        using value_type = std::size_t;
        using iterator   = set_iterator<Bits>;

        [[nodiscard]] constexpr auto operator&() const noexcept
                -> iterator
        {
                return { &m_ref, m_idx };
        }

        [[nodiscard]] constexpr explicit(false) operator value_type() const noexcept  // NOLINT(misc-explicit-constructor)
        {
                return m_idx;
        }

        template<std::constructible_from<value_type> T>
        [[nodiscard]] constexpr explicit(not std::is_convertible_v<value_type, T>) operator T() const noexcept(std::is_nothrow_constructible_v<T, value_type>)  // NOLINT(misc-explicit-constructor)
                requires std::is_class_v<T>
        {
                return m_idx;
        }
};

template<class Bits>
[[nodiscard]] constexpr auto format_as(set_reference<Bits> ref) noexcept
        -> set_reference<Bits>::value_type
{
        return ref;
}

// set_compare defaults to trusting Bits' own <=>, which is safe only for types this library controls; foreign ones opt out.
template<set_range Bits>
struct set_compare
{
        [[nodiscard]] static constexpr auto lexicographical_three_way(Bits const& x, Bits const& y) noexcept
                -> std::strong_ordering
        {
                return x <=> y;
        }
};

// The [set] interface over storage it does not own; held by pointer so the view stays movable, with key_type for fmt.
template<set_range Bits>
class set_view : public std::ranges::view_base
{
        using bits_type = std::remove_const_t<Bits>;
        using ops       = set_ops<bits_type>;

        Bits* m_ptr;

        // A view is as block-accessible as the thing it views; constrained, so one over an opaque type keeps the element-wise path.
        [[nodiscard]] friend constexpr auto block_count(set_view v) noexcept
                -> std::size_t
                requires block_range<bits_type>
        {
                return block_access<bits_type>::num_blocks(*v.m_ptr);
        }

        [[nodiscard]] friend constexpr auto block_at(set_view v, std::size_t i) noexcept
                requires block_range<bits_type>
        {
                return block_access<bits_type>::block(*v.m_ptr, i);
        }

public:
        using key_type               = std::size_t;
        using value_type             = std::size_t;
        using key_compare            = std::less<std::size_t>;
        using value_compare          = std::less<std::size_t>;
        using size_type              = std::size_t;
        using difference_type        = std::ptrdiff_t;
        using const_reference        = set_reference<bits_type>;
        using reference              = const_reference;
        using const_iterator         = set_iterator<bits_type>;
        using iterator               = const_iterator;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;
        using reverse_iterator       = const_reverse_iterator;

        [[nodiscard]] constexpr explicit set_view(Bits& c) noexcept : m_ptr(&c) {}

        // begin() and cbegin() coincide, this proxy iteration being read-only; mutation goes through insert and erase.
        [[nodiscard]] constexpr auto begin() const noexcept -> const_iterator { return set_begin(*m_ptr); }
        [[nodiscard]] constexpr auto end()   const noexcept -> const_iterator { return set_end  (*m_ptr); }

        [[nodiscard]] constexpr auto rbegin() const noexcept -> const_reverse_iterator { return std::make_reverse_iterator(end());   }
        [[nodiscard]] constexpr auto rend()   const noexcept -> const_reverse_iterator { return std::make_reverse_iterator(begin()); }

        [[nodiscard]] constexpr auto cbegin()  const noexcept -> const_iterator         { return begin();  }
        [[nodiscard]] constexpr auto cend()    const noexcept -> const_iterator         { return end();    }
        [[nodiscard]] constexpr auto crbegin() const noexcept -> const_reverse_iterator { return rbegin(); }
        [[nodiscard]] constexpr auto crend()   const noexcept -> const_reverse_iterator { return rend();   }

        // A bitset's none() is a set's empty(), and its count() a set's size().
        [[nodiscard]] constexpr auto empty() const noexcept -> bool { return size() == 0UZ; }

        [[nodiscard]] constexpr auto size() const noexcept -> size_type { return ops::size(*m_ptr); }

        // Constant where the Bits knows its own width, per bit_extent.
        [[nodiscard]] constexpr auto max_size() const noexcept -> size_type { return ops::max_size(*m_ptr); }

        // Modifiers, present only where Bits is not const, returning what [set] says they return.
        constexpr auto insert(key_type x) const noexcept  // NOLINT(modernize-use-nodiscard)
                -> std::pair<const_iterator, bool>
                requires (not std::is_const_v<Bits>)
        {
                auto const inserted = not contains(x);
                ops::insert(*m_ptr, x);
                return { const_iterator{ m_ptr, x }, inserted };
        }

        constexpr auto insert(const_iterator, key_type x) const noexcept  // NOLINT(modernize-use-nodiscard)
                -> const_iterator
                requires (not std::is_const_v<Bits>)
        {
                ops::insert(*m_ptr, x);
                return const_iterator{ m_ptr, x };
        }

        template<std::input_iterator I, std::sentinel_for<I> S>
        constexpr void insert(I first, S last) const noexcept
                requires (not std::is_const_v<Bits>)
        {
                for (; first != last; ++first) {
                        ops::insert(*m_ptr, static_cast<key_type>(*first));
                }
        }

        constexpr void insert(std::initializer_list<key_type> ilist) const noexcept
                requires (not std::is_const_v<Bits>)
        {
                insert(ilist.begin(), ilist.end());
        }

        // Not [[nodiscard]], for the reason std::set::erase is not: the count is there for callers who want it.
        constexpr auto erase(key_type x) const noexcept  // NOLINT(modernize-use-nodiscard)
                -> size_type
                requires (not std::is_const_v<Bits>)
        {
                auto const erased = contains(x);
                ops::erase(*m_ptr, x);
                return erased;
        }

        constexpr auto erase(const_iterator pos) const noexcept  // NOLINT(modernize-use-nodiscard)
                -> const_iterator
                requires (not std::is_const_v<Bits>)
        {
                auto nrv = pos;
                ++nrv;
                ops::erase(*m_ptr, *pos);
                return nrv;
        }

        constexpr void clear() const noexcept
                requires (not std::is_const_v<Bits>)
        {
                ops::clear(*m_ptr);
        }

        // Lookup, all of it falling out of set_find's four operations, with no further hook.
        [[nodiscard]] constexpr auto contains(key_type x) const noexcept -> bool { return ops::contains(*m_ptr, x); }

        [[nodiscard]] constexpr auto count(key_type x) const noexcept -> size_type { return contains(x); }

        [[nodiscard]] constexpr auto find(key_type x) const noexcept
                -> const_iterator
        {
                return contains(x) ? const_iterator{ m_ptr, x } : end();
        }

        // The first element not less than x, asked about directly because stepping from x - 1 would underflow at zero.
        [[nodiscard]] constexpr auto lower_bound(key_type x) const noexcept
                -> const_iterator
        {
                return contains(x) ? const_iterator{ m_ptr, x } : upper_bound(x);
        }

        [[nodiscard]] constexpr auto upper_bound(key_type x) const noexcept
                -> const_iterator
        {
                return const_iterator{ m_ptr, set_find<bits_type>::next(*m_ptr, x) };
        }

        [[nodiscard]] constexpr auto equal_range(key_type x) const noexcept
                -> std::pair<const_iterator, const_iterator>
        {
                return { lower_bound(x), upper_bound(x) };
        }

        // Prefer Bits' own == when it has one; equality is unambiguous either way, so this is purely an optimization.
        [[nodiscard]] friend constexpr auto operator==(set_view lhs, set_view rhs) noexcept
                -> bool
        {
                if constexpr (requires { *lhs.m_ptr == *rhs.m_ptr; }) {
                        return *lhs.m_ptr == *rhs.m_ptr;
                } else {
                        return std::ranges::equal(lhs, rhs);
                }
        }

        // Always through set_compare, so specializing that trait changes how any set_view over the Bits orders too.
        [[nodiscard]] friend constexpr auto operator<=>(set_view lhs, set_view rhs) noexcept
                -> std::strong_ordering
        {
                return set_compare<bits_type>::lexicographical_three_way(*lhs.m_ptr, *rhs.m_ptr);
        }

        // Not [set] and under review; three paths, cheapest first: the member, then the bitwise operators, then the element-wise scan.
        [[nodiscard]] constexpr auto is_subset_of(set_view other) const noexcept
                -> bool
        {
                if constexpr (requires { m_ptr->is_subset_of(*other.m_ptr); }) {
                        return m_ptr->is_subset_of(*other.m_ptr);
                } else if constexpr (requires { (*m_ptr & ~*other.m_ptr).none(); }) {
                        return (*m_ptr & ~*other.m_ptr).none();
                } else {
                        return std::ranges::includes(other, *this);
                }
        }

        [[nodiscard]] constexpr auto is_proper_subset_of(set_view other) const noexcept
                -> bool
        {
                if constexpr (requires { m_ptr->is_proper_subset_of(*other.m_ptr); }) {
                        return m_ptr->is_proper_subset_of(*other.m_ptr);
                } else {
                        return is_subset_of(other) and *this != other;
                }
        }

        [[nodiscard]] constexpr auto intersects(set_view other) const noexcept
                -> bool
        {
                if constexpr (requires { m_ptr->intersects(*other.m_ptr); }) {
                        return m_ptr->intersects(*other.m_ptr);
                } else if constexpr (requires { (*m_ptr & *other.m_ptr).any(); }) {
                        return (*m_ptr & *other.m_ptr).any();
                } else {
                        auto first1 = begin();
                        auto last1  = end();
                        auto first2 = other.begin();
                        auto last2  = other.end();
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

// Deduces the constness of the argument, the way span<T> and span<T const> do.
template<class Bits>
set_view(Bits&) -> set_view<Bits>;

} // namespace xstd::ranges

namespace xstd {

using ranges::set_range;
using ranges::set_view;

} // namespace xstd

#endif // XSTD_BITS_RANGES_SET_VIEW_HPP
