//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_BITS_RANGES_SET_VIEW_HPP
#define XSTD_BITS_RANGES_SET_VIEW_HPP

#include <xstd/bits/ranges/bit_extent.hpp> // bit_extent, static_bit_extent
#include <algorithm>                       // includes
#include <cassert>                         // assert
#include <compare>                         // strong_ordering
#include <concepts>                        // constructible_from, convertible_to
#include <cstddef>                         // ptrdiff_t, size_t
#include <functional>                      // less
#include <initializer_list>                // initializer_list
#include <iterator>                        // bidirectional_iterator_tag, make_reverse_iterator, reverse_iterator
#include <ranges>                          // distance, equal, view_base
#include <type_traits>                     // is_class_v, is_const_v, is_convertible_v, is_nothrow_constructible_v, remove_const_t
#include <utility>                          // pair

// A std::set<std::size_t> over a sequence of bits it does not own.
//
// The hook points this way round, unlike std::string_view's. A string opts into
// being viewed by carrying operator basic_string_view() itself; neither type this
// view exists for can do that -- std::bitset<N>'s only associated namespace is
// std, where [namespace.std] forbids a program adding anything, and
// boost::dynamic_bitset<> is not ours either. std::span is the precedent that
// works: the view carries the converting constructor and the viewed type carries
// nothing, so everything a Bits has to say arrives through the traits below.
//
// Non-owning and mutable through, again like span rather than string_view. The
// point is not to freeze a bitset but to rename it: a bitset's count() is a set's
// size(), a bitset's size() is a set's max_size(), set(pos) is insert(pos), and
// reset(pos) is erase(pos). There is a set inside a bitset, and this is what lets
// it out.
//
// It dangles over a temporary exactly as string_view does over s + "x". Viewing an
// rvalue within one full-expression is legitimate, so there is no deleted overload
// here, only this paragraph.
namespace xstd::ranges {

// Where the elements are.
//
// Bits customizes its iteration either by providing hidden friends
// find_first/find_last/find_next/find_prev discoverable via ADL (the default
// below, delegating to them - used by xstd's own types, whose associated
// namespace is xstd and can legitimately hold them), or by giving
// set_find<Bits> an explicit specialization for a foreign type that cannot
// provide those via ADL - e.g. std::bitset<N>, per [namespace.std] above; or
// boost::dynamic_bitset<>, whose own member begin()/end() can shadow a
// same-named ADL free function. Calls to find_first(c) and friends below are
// dependent (c's type is the template parameter Bits), so non-ADL unqualified
// lookup for them is fixed at this point of definition and can never see a
// later header's free functions - only ADL, deferred to each point of
// instantiation, can. set_find<Bits>'s specializations aren't subject to that:
// specialization matching considers any specialization visible before the point
// of use, regardless of which header declares it, so it works for foreign types
// where ADL cannot.
//
// Each member below is individually constrained on the underlying ADL call
// actually being valid, rather than just declared with a fixed return type.
// Without that, set_find<Bits>::first(c) would be a well-formed *expression* for
// any Bits at all (the declaration alone doesn't depend on whether find_first(c)
// in the body compiles - body instantiation is lazy and, on failure, a hard
// error rather than SFINAE), which made set_range<Bits> a false positive for
// every Bits, including set_reference<Bits> itself: nothing stopped
// set_reference<set_reference<Bits>> from being formed, recursively without end.
// Constraining each member here makes it (and so set_range) correctly SFINAE
// away when Bits doesn't actually provide the customization.
template<class Bits>
struct set_find
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

// Which of the two vocabularies a Bits speaks.
//
// count() with no argument is the discriminator: only a bitset has one, because
// std::set::count takes a key. Asked once, here, rather than per member -- and
// that is not tidiness. Probing member by member reads whichever name matches
// first, and the two vocabularies share names that mean different things:
// boost::dynamic_bitset::clear() erases the bits and leaves size() at zero, where
// std::set::clear() empties the set and leaves the universe alone. A per-member
// probe picks that clear() and silently destroys the container it was asked to
// empty. Deciding the language first and then speaking only that one cannot.
template<class Bits>
concept bitset_vocabulary = requires(Bits const& c)
{
        { c.count() } -> std::convertible_to<std::size_t>;
};

// The vocabulary, rewired.
//
// Unlike set_find above, this needs no ADL and no specialization for the types it
// serves: a member call is always found, whoever declared the member. So a type
// speaking either vocabulary is served unspecialized, and a type speaking neither
// specializes this.
template<class Bits>
struct set_ops
{
        // A bitset's count() is a set's size().
        [[nodiscard]] static constexpr std::size_t size(Bits const& c) noexcept
        {
                if constexpr (bitset_vocabulary<Bits>) {
                        return c.count();
                } else {
                        return c.size();
                }
        }

        // and a bitset's size() is a set's max_size(), constant where the type
        // declares its width.
        [[nodiscard]] static constexpr std::size_t max_size(Bits const& c) noexcept
        {
                if constexpr (static_bit_extent<Bits>) {
                        return bit_extent<Bits>;
                } else if constexpr (bitset_vocabulary<Bits>) {
                        return c.size();
                } else {
                        return c.max_size();
                }
        }

        [[nodiscard]] static constexpr bool contains(Bits const& c, std::size_t n) noexcept
        {
                if constexpr (bitset_vocabulary<Bits>) {
                        return c.test(n);
                } else {
                        return c.contains(n);
                }
        }

        static constexpr void insert(Bits& c, std::size_t n) noexcept
        {
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

        // reset(), never clear(): see bitset_vocabulary above for what
        // boost::dynamic_bitset::clear() would have done here.
        static constexpr void clear(Bits& c) noexcept
        {
                if constexpr (bitset_vocabulary<Bits>) {
                        c.reset();
                } else {
                        c.clear();
                }
        }
};

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

// Declared here, constraint and all, so set_iterator below can befriend both
// spellings: a redeclaration with different constraints would be a different
// template, not this one.
template<set_range Bits> class set_view;

template<class> class set_iterator;
template<class> class set_reference;
template<set_range> struct set_compare;

// Forward-declared so the dependent friend template-id declarations inside
// set_iterator below (set_begin<>, set_end<>) have a template to refer to:
// [temp.friend] requires that form to name an already-visible template, not one
// declared later in the same header. GCC tolerates the forward reference; Clang
// rejects it ("no candidate function template was found for dependent friend
// function template specialization") per the stricter reading.
template<set_range Bits> [[nodiscard]] constexpr set_iterator<Bits> set_begin(Bits const& c) noexcept;
template<set_range Bits> [[nodiscard]] constexpr set_iterator<Bits> set_end  (Bits const& c) noexcept;

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

        [[nodiscard]] friend constexpr bool operator==(set_iterator lhs, set_iterator rhs) noexcept
        {
                assert(lhs.m_ptr == rhs.m_ptr);
                return lhs.m_idx == rhs.m_idx;
        }

        [[nodiscard]] constexpr reference operator*() const noexcept
        {
                assert(m_ptr != nullptr);
                return { *m_ptr, m_idx };
        }

        // The assert is on its own line, as in operator* above: the coverage
        // job drops assert branches by matching the start of the line, so an
        // assert sharing a line with real code keeps its never-taken branch.
        constexpr set_iterator& operator++() noexcept
                requires requires (Bits const& c, std::size_t n) { set_find<Bits>::next(c, n); }
        {
                assert(m_ptr != nullptr);
                m_idx = set_find<Bits>::next(*m_ptr, m_idx);
                return *this;
        }

        constexpr set_iterator& operator--() noexcept
                requires requires (Bits const& c, std::size_t n) { set_find<Bits>::prev(c, n); }
        {
                assert(m_ptr != nullptr);
                m_idx = set_find<Bits>::prev(*m_ptr, m_idx);
                return *this;
        }

        constexpr set_iterator operator++(int) noexcept { auto nrv = *this; ++*this; return nrv; }
        constexpr set_iterator operator--(int) noexcept { auto nrv = *this; --*this; return nrv; }
};

template<set_range Bits> [[nodiscard]] constexpr set_iterator<Bits> set_begin(Bits const& c) noexcept { return { &c, set_find<Bits>::first(c) }; }
template<set_range Bits> [[nodiscard]] constexpr set_iterator<Bits> set_end  (Bits const& c) noexcept { return { &c, set_find<Bits>::last (c) }; }

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

        [[nodiscard]] constexpr iterator operator&() const noexcept
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

// set_compare<Bits>::lexicographical_three_way defaults to trusting Bits' own
// <=>: xstd's own bit_finite_set/bitset (detail::bits::array's operator<=>) is meant to
// already compute std::set<int>-equivalent ordering, word-parallel, for every
// cardinality - so the default here is just to call it directly rather than pay
// for iterating through the view.
//
// This mirrors set_find<>'s reason for existing, aimed at the opposite failure
// mode: set_find<> exists because boost::dynamic_bitset<> silently started
// shadowing this library's ADL begin()/end() the moment it grew its own
// same-named members (a foreign type's own thing quietly taking over).
// set_compare<> guards against a foreign type's own <=> quietly taking over with
// the WRONG semantics later - std::bitset<N> has none today, and
// boost::dynamic_bitset<> could add one upstream, but neither is under any
// obligation to make it std::set<int>-equivalent (it could just as well be
// sequence-of-bool, matching the type's own element/index order instead).
// Trusting Bits' <=> by default is only safe for types this library controls;
// std::bitset<N> and boost::dynamic_bitset<> instead opt in to the safe,
// iteration-based fallback via explicit set_compare<Bits> specializations (see
// xstd/bits/ext/std/bitset.hpp and its neighbour).
template<set_range Bits>
struct set_compare
{
        [[nodiscard]] static constexpr std::strong_ordering lexicographical_three_way(Bits const& x, Bits const& y) noexcept
        {
                return x <=> y;
        }
};

// The [set] interface, over storage this does not own.
//
// Bits may be const-qualified, as span's element type may be: set_view<B> is
// mutable through and set_view<B const> is not, and CTAD picks whichever the
// argument was. Every trait above is consulted on the unqualified type, so one
// set_find/set_ops/bit_extent specialization serves both.
//
// Stored as a pointer, not a reference: std::ranges::view requires std::movable,
// which requires assignable_from<T&, T> - a reference data member makes copy and
// move assignment implicitly deleted (references cannot be rebound), which would
// make this fail the view concept entirely and break composition with
// std::views::take_while and the other adaptors. A pointer keeps the defaulted
// assignment working while the constructor still takes Bits& so construction
// reads like any other reference-taking adaptor.
//
// key_type mirrors bit_finite_set's own: fmt's range formatter (fmt/ranges.h)
// detects "format like a set" purely by checking for a nested key_type, so a
// set_view formats with {} delimiters on its own.
template<set_range Bits>
class set_view : public std::ranges::view_base
{
        using bits_type = std::remove_const_t<Bits>;
        using ops       = set_ops<bits_type>;

        Bits* m_ptr;

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

        // begin()/cbegin() (and end()/cend()) coincide: this proxy iteration is
        // inherently read-only - set_reference only converts to std::size_t,
        // there is no assignment through an iterator here - the same way
        // bit_finite_set's own cbegin()/cend() are plain aliases rather than a
        // distinct const-iteration path. Mutation goes through insert and erase.
        [[nodiscard]] constexpr const_iterator begin() const noexcept { return set_begin(*m_ptr); }
        [[nodiscard]] constexpr const_iterator end()   const noexcept { return set_end  (*m_ptr); }

        [[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept { return std::make_reverse_iterator(end());   }
        [[nodiscard]] constexpr const_reverse_iterator rend()   const noexcept { return std::make_reverse_iterator(begin()); }

        [[nodiscard]] constexpr const_iterator         cbegin()  const noexcept { return begin();  }
        [[nodiscard]] constexpr const_iterator         cend()    const noexcept { return end();    }
        [[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept { return rbegin(); }
        [[nodiscard]] constexpr const_reverse_iterator crend()   const noexcept { return rend();   }

        // A bitset's none() is a set's empty(), and its count() a set's size().
        [[nodiscard]] constexpr bool empty() const noexcept { return size() == 0UZ; }

        [[nodiscard]] constexpr size_type size() const noexcept { return ops::size(*m_ptr); }

        // Constant where the Bits knows its own width, per bit_extent.
        [[nodiscard]] constexpr size_type max_size() const noexcept { return ops::max_size(*m_ptr); }

        // Modifiers, present only where Bits is not const - and returning what
        // [set] says they return, which is what makes the harness able to run
        // one body over a set_view and a std::set alike.
        constexpr std::pair<const_iterator, bool> insert(key_type x) const noexcept
                requires (not std::is_const_v<Bits>)
        {
                auto const inserted = not contains(x);
                ops::insert(*m_ptr, x);
                return { const_iterator{ m_ptr, x }, inserted };
        }

        constexpr const_iterator insert(const_iterator, key_type x) const noexcept
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

        // Not [[nodiscard]], for the reason std::set::erase is not: the count is
        // there for callers who want it and discarded by the ones who don't.
        // modernize-use-nodiscard reaches it only because a view's modifiers are
        // const members, writing through the pointer rather than to it.
        constexpr size_type erase(key_type x) const noexcept  // NOLINT(modernize-use-nodiscard)
                requires (not std::is_const_v<Bits>)
        {
                auto const erased = contains(x);
                ops::erase(*m_ptr, x);
                return erased;
        }

        constexpr const_iterator erase(const_iterator pos) const noexcept
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

        // Lookup. Every one of these falls out of set_find's four operations, so
        // a Bits that can be iterated can be searched, with no further hook.
        [[nodiscard]] constexpr bool contains(key_type x) const noexcept { return ops::contains(*m_ptr, x); }

        [[nodiscard]] constexpr size_type count(key_type x) const noexcept { return contains(x); }

        [[nodiscard]] constexpr const_iterator find(key_type x) const noexcept
        {
                return contains(x) ? const_iterator{ m_ptr, x } : end();
        }

        // The first element not less than x. next() steps strictly past its
        // argument, so x itself is asked about separately rather than by
        // stepping from x - 1, which would underflow at x == 0.
        [[nodiscard]] constexpr const_iterator lower_bound(key_type x) const noexcept
        {
                return contains(x) ? const_iterator{ m_ptr, x } : upper_bound(x);
        }

        [[nodiscard]] constexpr const_iterator upper_bound(key_type x) const noexcept
        {
                return const_iterator{ m_ptr, set_find<bits_type>::next(*m_ptr, x) };
        }

        [[nodiscard]] constexpr std::pair<const_iterator, const_iterator> equal_range(key_type x) const noexcept
        {
                return { lower_bound(x), upper_bound(x) };
        }

        // Prefer Bits' own == when it has one (cheaper than iterating through
        // the proxy iterators above), else compare elementwise. Equality of two
        // sets is unambiguous regardless of any bitset's internal bit-layout
        // convention, so this is purely an optimization.
        [[nodiscard]] friend constexpr bool operator==(set_view lhs, set_view rhs) noexcept
        {
                if constexpr (requires { *lhs.m_ptr == *rhs.m_ptr; }) {
                        return *lhs.m_ptr == *rhs.m_ptr;
                } else {
                        return std::ranges::equal(lhs, rhs);
                }
        }

        // Ordering is not as unambiguous as == - see set_compare<Bits> above for
        // why the default trusts Bits' own <=> and why some Bits opt out. This
        // never duplicates that decision here: it always goes through
        // set_compare, so specializing the trait changes how any set_view over
        // that Bits orders too.
        [[nodiscard]] friend constexpr std::strong_ordering operator<=>(set_view lhs, set_view rhs) noexcept
        {
                return set_compare<bits_type>::lexicographical_three_way(*lhs.m_ptr, *rhs.m_ptr);
        }

        // Not [set], and under review: lazy set intersection and union may be a
        // better home for what these do. They stay for now because the types
        // viewed here have them natively, and routing through std::ranges::
        // includes would make the view slower than the thing it views.
        // Three paths, cheapest first. A Bits with the relation as a member has
        // already done the work word-at-a-time -- ours through detail::bits::array,
        // boost::dynamic_bitset through its own. A Bits with only the bitwise
        // operators can still answer a whole word at a time: std::bitset<N> has
        // no is_subset_of, but (a & ~b).none() is the same question and does not
        // walk the bits one at a time. Only a Bits with neither pays for the
        // element-wise scan, which is the case Hinnant measured at up to two
        // orders of magnitude (doc/design.md).
        [[nodiscard]] constexpr bool is_subset_of(set_view other) const noexcept
        {
                if constexpr (requires { m_ptr->is_subset_of(*other.m_ptr); }) {
                        return m_ptr->is_subset_of(*other.m_ptr);
                } else if constexpr (requires { (*m_ptr & ~*other.m_ptr).none(); }) {
                        return (*m_ptr & ~*other.m_ptr).none();
                } else {
                        return std::ranges::includes(other, *this);
                }
        }

        [[nodiscard]] constexpr bool is_proper_subset_of(set_view other) const noexcept
        {
                if constexpr (requires { m_ptr->is_proper_subset_of(*other.m_ptr); }) {
                        return m_ptr->is_proper_subset_of(*other.m_ptr);
                } else {
                        return is_subset_of(other) and *this != other;
                }
        }

        [[nodiscard]] constexpr bool intersects(set_view other) const noexcept
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
