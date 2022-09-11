#pragma once

//          Copyright Rein Halbersma 2014-2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <boost/test/unit_test.hpp>     // BOOST_CHECK, BOOST_CHECK_EQUAL, BOOST_CHECK_EQUAL_COLLECTIONS, BOOST_CHECK_NE
#include <algorithm>                    // equal_range, lexicographical_compare_three_way
#include <compare>                      // strong_ordering
#include <concepts>                     // convertible_to, default_initializable, equality_comparable, same_as, signed_integral, unsigned_integral
#include <initializer_list>             // initializer_list
#include <iterator>                     // distance, empty, next, prev, size, ssize
#include <limits>                       // max
#include <memory>                       // addressof
#include <numeric>                      // accumulate
#include <ranges>                       // count, equal, find, lexicographical_compare, lower_bound, , subrange, upper_bound
#include <utility>                      // pair

namespace xstd {

template<class X>
struct nested_types
{
        using T = int;
        static_assert(std::same_as<typename X::value_type, T>);                 // [container.reqmts]/2
        static_assert(requires { std::declval<X>().erase(std::declval<T>()); });// [container.reqmts]/3
                                                                                // [container.reqmts]/5
        // We use a proxy reference for xstd::bit_set which is convertible to rather than the same as int const&.
        static_assert(std::convertible_to<typename X::const_reference, T const&>);

        static_assert(std::forward_iterator<typename X::iterator>);             // [container.reqmts]/6
        static_assert(std::same_as<typename X::iterator::value_type, T>);
        static_assert(std::convertible_to<typename X::iterator, typename X::const_iterator>);

        static_assert(std::forward_iterator<typename X::const_iterator>);       // [container.reqmts]/7
        static_assert(std::same_as<typename X::const_iterator::value_type, T>);

                                                                                // [container.reqmts]/8
        static_assert(std::same_as<typename X::difference_type, typename X::iterator::difference_type>);
        static_assert(std::same_as<typename X::difference_type, typename X::const_iterator::difference_type>);

                                                                                // [container.rev.reqmts]/2
        static_assert(std::bidirectional_iterator<typename X::reverse_iterator>);
        static_assert(std::same_as<typename X::reverse_iterator, std::reverse_iterator<typename X::iterator>>);
        static_assert(std::same_as<typename X::reverse_iterator::value_type, T>);

                                                                                // [container.rev.reqmts]/3
        static_assert(std::bidirectional_iterator<typename X::const_reverse_iterator>);
        static_assert(std::same_as<typename X::const_reverse_iterator, std::reverse_iterator<typename X::const_iterator>>);
        static_assert(std::same_as<typename X::const_reverse_iterator::value_type, T>);

        using Key = int;
        static_assert(std::same_as<typename X::key_type, Key>);                 // [associative.reqmts.general]/9
        static_assert(std::same_as<typename X::value_type, Key>);               // [associative.reqmts.general]/12
                                                                                // [associative.reqmts.general]/13
        static_assert(requires { std::declval<X>().erase(std::declval<Key>()); });

        using Compare = std::less<Key>;
        static_assert(std::same_as<typename X::key_compare, Compare>);          // [associative.reqmts.general]/14
        static_assert(std::copy_constructible<Key>);                            // [associative.reqmts.general]/15
        static_assert(std::same_as<typename X::value_compare, Compare>);        // [associative.reqmts.general]/16
};

template<class X>
struct constructor
{
        auto operator()() const
        {
                X u;
                X u1 = X();
                BOOST_CHECK(u.empty());                                         // [container.reqmts]/10
                BOOST_CHECK(u1.empty());                                        // [container.reqmts]/10
                                                                                // [associative.reqmts.general]/20
                static_assert(std::default_initializable<typename X::key_compare>);
        }

        auto operator()(X a) const
        {
                X u(a);
                X u1 = a;
                BOOST_CHECK(u == a);                                            // [container.reqmts]/10
                BOOST_CHECK(u1 == a);                                           // [container.reqmts]/10
        }

        auto operator()(X&& rv) const
        {
                X u(rv);
                X u1 = rv;
                BOOST_CHECK(u == rv);                                            // [container.reqmts]/10
                BOOST_CHECK(u1 == rv);                                           // [container.reqmts]/10
        }

        auto operator()(auto i, auto j) const
        {                                                                       // [associative.reqmts.general]/26
                static_assert(std::default_initializable<typename X::key_compare>);
                X u(i, j);
                X u1;
                u1.insert(i, j);
                BOOST_CHECK(u == u1);                                           // [associative.reqmts.general]/27
        }

        auto operator()(std::initializer_list<typename X::value_type> il) const
        {
                X u(il);
                X u1(il.begin(), il.end());
                BOOST_CHECK(u == u1);                                           // [associative.reqmts.general]/36
        }
};

struct op_assign
{
        template<class X>
        auto operator()(X& r, X a) const
        {
                r = a;
                static_assert(std::same_as<decltype(r), X&>);                   // [container.reqmts]/49
                BOOST_CHECK(r == a);                                            // [container.reqmts]/50
        }

        template<class X>
        auto operator()(X& a, std::initializer_list<typename X::value_type> il) const
        {                                                                       // [tab:container.assoc.req]
                a = il;
                X a1(il);
                BOOST_CHECK(a == a1);
        }
};

struct mem_const_reference
{
        auto operator()(auto const& bs) const noexcept
        {
                for (auto const ref : bs) {
                        BOOST_CHECK(bs.count(ref));
                }
        }
};

struct mem_const_iterator
{
        template<class X>
        auto operator()(X& a) const noexcept
        {
                using Iterator = X::iterator;
                static_assert(std::same_as<decltype(a.begin()), Iterator>);     // [container.reqmts]/24
                static_assert(std::same_as<decltype(a.end())  , Iterator>);     // [container.reqmts]/27

                using ReverseIterator = X::reverse_iterator;                    // [container.rev.reqmts]/4
                static_assert(std::same_as<decltype(a.rbegin()), ReverseIterator>);
                BOOST_CHECK(a.rbegin() == std::reverse_iterator(a.end()));      // [container.rev.reqmts]/5
                                                                                // [container.rev.reqmts]/7
                static_assert(std::same_as<decltype(a.rend())  , ReverseIterator>);
                BOOST_CHECK(a.rend()   == std::reverse_iterator(a.begin()));    // [container.rev.reqmts]/8
        }

        template<class X>
        auto operator()(X const& a) const noexcept
        {
                using ConstIterator = X::const_iterator;                        // [container.reqmts]/24
                static_assert(std::same_as<decltype(a.begin()) , ConstIterator>);
                                                                                // [container.reqmts]/27
                static_assert(std::same_as<decltype(a.end())   , ConstIterator>);
                                                                                // [container.reqmts]/30
                static_assert(std::same_as<decltype(a.cbegin()), ConstIterator>);
                BOOST_CHECK(a.cbegin() == const_cast<X const&>(a).begin());     // [container.reqmts]/31
                                                                                // [container.reqmts]/33
                static_assert(std::same_as<decltype(a.cend())  , ConstIterator>);
                BOOST_CHECK(a.cend()   == const_cast<X const&>(a).end()  );     // [container.reqmts]/34


                using ConstReverseIterator = X::const_reverse_iterator;         // [container.rev.reqmts]/4
                static_assert(std::same_as<decltype(a.rbegin()) , ConstReverseIterator>);
                BOOST_CHECK(a.rbegin() == std::reverse_iterator(a.end()));      // [container.rev.reqmts]/5
                                                                                // [container.rev.reqmts]/7
                static_assert(std::same_as<decltype(a.rend())   , ConstReverseIterator>);
                BOOST_CHECK(a.rend()   == std::reverse_iterator(a.begin()));    // [container.rev.reqmts]/8
                                                                                // [container.rev.reqmts]/10
                static_assert(std::same_as<decltype(a.crbegin()), ConstReverseIterator>);
                BOOST_CHECK(a.crbegin() == const_cast<X const&>(a).rbegin());   // [container.rev.reqmts]/11
                                                                                // [container.rev.reqmts]/13
                static_assert(std::same_as<decltype(a.crend())  , ConstReverseIterator>);
                BOOST_CHECK(a.crend()   == const_cast<X const&>(a).rend()  );   // [container.rev.reqmts]/14
        }

        template<class X>
        auto operator()(typename X::iterator i, typename X::iterator j) const noexcept
        {                                                                       // [container.reqmts]/37
                if constexpr (std::random_access_iterator<typename X::iterator>) {
                                                                                // [container.reqmts]/36
                        static_assert(std::same_as<decltype(i <=> j), std::strong_ordering>);
                }
        }
};

struct op_equal_to
{
        auto operator()(auto const& a, auto const& b) const noexcept
        {                                                                       // [container.reqmts]/39
                static_assert(std::equality_comparable<typename std::remove_cvref_t<decltype(a)>::value_type>);
                static_assert(std::convertible_to<decltype(a == b), bool>);     // [container.reqmts]/40
                BOOST_CHECK_EQUAL(a == b, std::ranges::equal(a, b));            // [container.reqmts]/41
                using X = std::remove_cvref_t<decltype(a)>;                     // [container.reqmts]/43
                static_assert(std::equivalence_relation<std::equal_to<X>, X, X>);
        }
};

struct op_not_equal_to
{
        auto operator()(auto const& a, auto const& b) const noexcept
        {
                BOOST_CHECK_EQUAL(a != b, !(a == b));                           // [container.reqmts]/44
        }
};

struct mem_swap
{
        auto operator()(auto& a, auto& b) const noexcept
        {
                static_assert(std::same_as<decltype(a.swap(b)), void>);         // [container.reqmts]/45
                auto a1 = a, b1 = b;
                a1.swap(b1);
                BOOST_CHECK(a1 == b && b1 == a);                                // [container.reqmts]/46
        }
};

struct fn_swap
{
        auto operator()(auto& a, auto& b) const noexcept
        {
                auto a1 = a, b1 = b;
                auto a2 = a, b2 = b;
                swap(a1, b1); a2.swap(b2);
                BOOST_CHECK(a1 == a2 && b1 == b2);                              // [container.reqmts]/48
        }
};

struct mem_size
{
        template<class X>
        auto operator()(X const& a) const noexcept
        {                                                                       // [container.reqmts]/52
                static_assert(std::same_as<decltype(a.size()), typename X::size_type>);
                                                                                // [container.reqmts]/53
                BOOST_CHECK_EQUAL(static_cast<std::ptrdiff_t>(a.size()), std::distance(a.begin(), a.end()));
        }
};

struct mem_max_size
{
        template<class X>
        auto operator()(X const& a) const noexcept
        {                                                                       // [container.reqmts]/56
                static_assert(std::same_as<decltype(a.max_size()), typename X::size_type>);
                BOOST_CHECK_LE(a.size(), a.max_size());                         // [container.reqmts]/57
        }
};

struct mem_empty
{
        auto operator()(auto const& a) const noexcept
        {
                static_assert(std::convertible_to<decltype(a.empty()), bool>);  // [container.reqmts]/59
                BOOST_CHECK_EQUAL(a.empty(), a.begin() == a.end());             // [container.reqmts]/60
        }
};

struct mem_front
{
        template<class X>
        auto operator()(X const& a [[maybe_unused]]) const noexcept
        {
                if constexpr (requires { a.front(); }) {
                                                                                // [sequence.reqmts]/70
                        static_assert(std::same_as<decltype(a.front()), typename X::const_reference>);
                        BOOST_CHECK(a.empty() || (a.front() == *a.begin()));    // [sequence.reqmts]/71
                }
        }
};

struct mem_back
{
        template<class X>
        auto operator()(X const& a [[maybe_unused]]) const noexcept
        {
                if constexpr (requires { a.back(); }) {
                                                                                // [sequence.reqmts]/73
                        static_assert(std::same_as<decltype(a.back()), typename X::const_reference>);
                                                                                // [sequence.reqmts]/74
                        BOOST_CHECK(a.empty() || (a.back() == *std::prev(a.end())));
                }
        }
};

struct mem_capacity
{
        template<class X>
        auto operator()(X const& a) const noexcept
        {
                if constexpr (requires { a.capacity(); }) {
                        static_assert(std::same_as<decltype(a.capacity()), typename X::size_type>);
                        BOOST_CHECK_LE(a.size(), a.capacity());
                }
        }
};

struct mem_emplace
{
        template<class X, class... Args>
        auto operator()(X& a, Args&&... args) const
        {                                                                       // [associative.reqmts.general]/47
                static_assert(std::same_as<decltype(a.emplace(std::forward<Args>(args)...)), std::pair<typename X::iterator, bool>>);
                                                                                // [associative.reqmts.general]/48
                static_assert(std::constructible_from<typename X::value_type, Args...>);
                auto emplaced = !a.contains(typename X::value_type(std::forward<Args>(args)...));
                auto r = a.emplace(std::forward<Args>(args)...);                // [associative.reqmts.general]/49
                                                                                // [associative.reqmts.general]/50
                BOOST_CHECK(r == std::make_pair(a.find(typename X::value_type(std::forward<Args>(args)...)), emplaced));
        }
};

struct mem_emplace_hint
{
        template<class X, class... Args>
        auto operator()(X& a, X::iterator p, Args&&... args) const
        {                                                                       // [associative.reqmts.general]/57
                static_assert(std::same_as<decltype(a.emplace_hint(p, std::forward<Args>(args)...)), typename X::iterator>);
                auto r = a.emplace_hint(p, std::forward<Args>(args)...);        // [associative.reqmts.general]/58
                                                                                // [associative.reqmts.general]/59
                BOOST_CHECK(r == a.find(typename X::value_type(std::forward<Args>(args)...)));
        }
};

struct mem_insert
{
        template<class X>
        auto operator()(X& a, X::value_type const& t) const
        {                                                                       // [associative.reqmts.general]/61
                static_assert(std::same_as<decltype(a.insert(t)), std::pair<typename X::iterator, bool>>);
                                                                                // [associative.reqmts.general]/62
                static_assert(std::constructible_from<typename X::value_type, decltype(t)>);
                auto const inserted = !a.contains(t);
                auto const r = a.insert(t);                                     // [associative.reqmts.general]/63
                BOOST_CHECK(r == std::make_pair(a.find(t), inserted));          // [associative.reqmts.general]/64
        }

        template<class X>
        auto operator()(X& a, X::value_type&& t) const
        {                                                                       // [associative.reqmts.general]/61
                static_assert(std::same_as<decltype(a.insert(t)), std::pair<typename X::iterator, bool>>);
                                                                                // [associative.reqmts.general]/62
                static_assert(std::constructible_from<typename X::value_type, decltype(t)>);
                auto const inserted = !a.contains(t);
                auto const r = a.insert(t);                                     // [associative.reqmts.general]/63
                BOOST_CHECK(r == std::make_pair(a.find(t), inserted));          // [associative.reqmts.general]/64
        }

        template<class X>
        auto operator()(X& a, X::iterator p, X::value_type const& t) const
        {                                                                       // [associative.reqmts.general]/70
                static_assert(std::same_as<decltype(a.insert(p, t)), typename X::iterator>);
                                                                                // [associative.reqmts.general]/71
                static_assert(std::constructible_from<typename X::value_type, decltype(t)>);
                auto r = a.insert(p, t);                                        // [associative.reqmts.general]/72
                BOOST_CHECK(r == a.find(t));                                    // [associative.reqmts.general]/73
        }

        template<class X>
        auto operator()(X& a, X::iterator p, X::value_type&& t) const
        {                                                                       // [associative.reqmts.general]/70
                static_assert(std::same_as<decltype(a.insert(p, t)), typename X::iterator>);
                                                                                // [associative.reqmts.general]/71
                static_assert(std::constructible_from<typename X::value_type, decltype(t)>);
                auto r = a.insert(p, t);                                        // [associative.reqmts.general]/72
                BOOST_CHECK(r == a.find(t));                                    // [associative.reqmts.general]/73
        }

        template<class X, class InputIterator>
        auto operator()(X& a, InputIterator i, InputIterator j) const
        {
                static_assert(std::same_as<decltype(a.insert(i, j)), void>);    // [associative.reqmts.general]/75
                                                                                // [associative.reqmts.general]/76
                static_assert(std::constructible_from<typename X::value_type, decltype(*i)>);
                auto a1 = a;
                a.insert(i, j);
                for (auto const& t : std::ranges::subrange(i, j)) {
                        a1.insert(t);
                }
                BOOST_CHECK(a == a1);                                           // [associative.reqmts.general]/77
        }

        template<class X, class Range>
                requires std::ranges::range<Range>
        auto operator()(X& a, Range&& rg) const
        {                                                                       // [associative.reqmts.general]/79
                static_assert(std::same_as<decltype(a.insert_range(rg)), void>);
                                                                                // [associative.reqmts.general]/80
                static_assert(std::constructible_from<typename X::value_type, decltype(*rg.begin())>);
                auto a1 = a;
                a.insert_range(rg);
                for (auto const& t : rg) {
                        a1.insert(t);
                }
                BOOST_CHECK(a == a1);                                           // [associative.reqmts.general]/81
        }

        template<class X>
        auto operator()(X& a, std::initializer_list<typename X::value_type> il) const
        {
                auto a1 = a;
                a.insert(il);
                a1.insert(il.begin(), il.end());
                BOOST_CHECK(a == a1);                                           // [associative.reqmts.general]/83
        }
};

struct mem_erase
{
        template<class X>
        auto operator()(X& a, X::key_type const& k) const
        {                                                                       // [associative.reqmts.general]/118
                static_assert(std::same_as<decltype(a.erase(k)), typename X::size_type>);
                auto const erased = a.count(k);
                auto const returns = a.erase(k);                                // [associative.reqmts.general]/119
                BOOST_CHECK(returns == erased);                                 // [associative.reqmts.general]/120
        }

        template<class X>
        auto operator()(X& a, X::const_iterator q) const
        {                                                                       // [associative.reqmts.general]/126
                static_assert(std::same_as<decltype(a.erase(q)), typename X::iterator>);
                BOOST_CHECK(q != a.end());
                auto const expected = std::next(q);                             // assumes erase does not invalidate iterators
                auto const returns = a.erase(q);                                // [associative.reqmts.general]/127
                BOOST_CHECK(returns == expected);                               // [associative.reqmts.general]/128
        }

        template<class X>
        auto operator()(X& a, X::const_iterator q1, X::const_iterator q2) const
        {                                                                       // [associative.reqmts.general]/134
                static_assert(std::same_as<decltype(a.erase(q1, q2)), typename X::iterator>);
                auto const expected = q2;                                       // assumes erase does not invalidate iterators
                auto const returns = a.erase(q1, q2);                           // [associative.reqmts.general]/135
                BOOST_CHECK(returns == expected);                               // [associative.reqmts.general]/136
        }
};

struct mem_clear
{
        auto operator()(auto& a) const noexcept
        {
                auto a1 = a;
                a.clear();
                a1.erase(a1.begin(), a1.end());
                BOOST_CHECK(a == a1);                                           // [associative.reqmts.general]/138
                BOOST_CHECK(a.empty());                                         // [associative.reqmts.general]/139
        }
};

struct mem_find
{
        template<class X>
        auto operator()(X& b, X::key_type const& k) const
        {                                                                       // [associative.reqmts.general]/141
                static_assert(std::same_as<decltype(b.find(k)), typename X::iterator>);
                BOOST_CHECK(b.find(k) == std::ranges::find(b, k));              // [associative.reqmts.general]/142
        }

        template<class X>
        auto operator()(X const& b, X::key_type const& k) const
        {                                                                       // [associative.reqmts.general]/141
                static_assert(std::same_as<decltype(b.find(k)), typename X::const_iterator>);
                BOOST_CHECK(b.find(k) == std::ranges::find(b, k));              // [associative.reqmts.general]/142
        }
};

struct mem_count
{
        template<class X>
        auto operator()(X const& b, X::key_type const& k) const
        {                                                                       // [associative.reqmts.general]/147
                static_assert(std::same_as<decltype(b.count(k)), typename X::size_type>);
                                                                                // [associative.reqmts.general]/148
                BOOST_CHECK_EQUAL(static_cast<std::ptrdiff_t>(b.count(k)), std::ranges::count(b, k));
        }
};

struct mem_contains
{
        template<class X>
        auto operator()(X const& b, X::key_type const& k) const
        {
                static_assert(std::same_as<decltype(b.contains(k)), bool>);
                BOOST_CHECK_EQUAL(b.contains(k), b.find(k) != b.end());         // [associative.reqmts.general]/154
        }
};

struct mem_lower_bound
{
        template<class X>
        auto operator()(X& b, X::key_type const& k) const
        {                                                                       // [associative.reqmts.general]/157
                static_assert(std::same_as<decltype(b.lower_bound(k)), typename X::iterator>);
                                                                                // [associative.reqmts.general]/158
                BOOST_CHECK(b.lower_bound(k) == std::ranges::lower_bound(b, k));
        }

        template<class X>
        auto operator()(X const& b, X::key_type const& k) const
        {                                                                       // [associative.reqmts.general]/157
                static_assert(std::same_as<decltype(b.lower_bound(k)), typename X::const_iterator>);
                                                                                // [associative.reqmts.general]/158
                BOOST_CHECK(b.lower_bound(k) == std::ranges::lower_bound(b, k));
        }
};

struct mem_upper_bound
{
        template<class X>
        auto operator()(X& b, X::key_type const& k) const
        {                                                                       // [associative.reqmts.general]/163
                static_assert(std::same_as<decltype(b.upper_bound(k)), typename X::iterator>);
                                                                                // [associative.reqmts.general]/164
                BOOST_CHECK(b.upper_bound(k) == std::ranges::upper_bound(b, k));
        }

        template<class X>
        auto operator()(X const& b, X::key_type const& k) const
        {                                                                       // [associative.reqmts.general]/163
                static_assert(std::same_as<decltype(b.upper_bound(k)), typename X::const_iterator>);
                                                                                // [associative.reqmts.general]/164
                BOOST_CHECK(b.upper_bound(k) == std::ranges::upper_bound(b, k));
        }
};

struct mem_equal_range
{
        template<class X>
        auto operator()(X& b, X::key_type const& k) const
        {
                using iterator = X::iterator;                                   // [associative.reqmts.general]/169
                static_assert(std::same_as<decltype(b.equal_range(k)), std::pair<iterator, iterator>>);
                                                                                // [associative.reqmts.general]/170
                BOOST_CHECK(b.equal_range(k) == std::make_pair(b.lower_bound(k), b.upper_bound(k)));
        }

        template<class X>
        auto operator()(X const& b, X::key_type const& k) const
        {
                using const_iterator = X::const_iterator;                       // [associative.reqmts.general]/169
                static_assert(std::same_as<decltype(b.equal_range(k)), std::pair<const_iterator, const_iterator>>);
                                                                                // [associative.reqmts.general]/170
                BOOST_CHECK(b.equal_range(k) == std::make_pair(b.lower_bound(k), b.upper_bound(k)));
        }
};

struct op_compare_three_way
{
        auto operator()(auto const& a) const noexcept
        {
                // std::strong_ordering does not have output streaming operator<< required for BOOST_CHECK_EQUAL
                BOOST_CHECK((a <=> a) == std::strong_ordering::equal);          // reflexive
        }

        auto operator()(auto const& a, auto const& b) const noexcept
        {                                                                       // [tab:container.req]
                static_assert(std::same_as<decltype(a <=> b), std::strong_ordering>);
                                                                                // [tab:container.opt]
                // std::strong_ordering does not have output streaming operator<< required for BOOST_CHECK_EQUAL
                BOOST_CHECK((a <=> b) == std::lexicographical_compare_three_way(a.begin(), a.end(), b.begin(), b.end()));
        }
};

struct op_less
{
        auto operator()(auto const& a) const noexcept
        {                                                                       // [tab:container.opt]
                BOOST_CHECK(!(a < a));                                          // irreflexive
        }

        auto operator()(auto const& a, auto const& b) const noexcept
        {                                                                       // [tab:container.opt]
                static_assert(std::convertible_to<decltype(a < b), bool>);
                BOOST_CHECK_EQUAL(a < b, (a <=> b) < 0);
                BOOST_CHECK_EQUAL(a < b, std::ranges::lexicographical_compare(a, b));
                BOOST_CHECK(!(a < b) || !(b < a));                              // asymmetric
        }

        auto operator()(auto const& a, auto const& b, auto const& c) const noexcept
        {                                                                       // [tab:container.opt]
                BOOST_CHECK(!(a < b && b < c) || a < c);                        // transitive
        }
};

struct op_greater
{
        auto operator()(auto const& a, auto const& b) const noexcept
        {                                                                       // [tab:container.opt]
                static_assert(std::convertible_to<decltype(a > b), bool>);
                BOOST_CHECK_EQUAL(a > b, (a <=> b) > 0);
                BOOST_CHECK_EQUAL(a > b, b < a);
        }
};

struct op_less_equal
{
        auto operator()(auto const& a, auto const& b) const noexcept
        {                                                                       // [tab:container.opt]
                static_assert(std::convertible_to<decltype(a <= b), bool>);
                BOOST_CHECK_EQUAL(a <= b, (a <=> b) <= 0);
                BOOST_CHECK_EQUAL(a <= b, !(b < a));
        }
};

struct op_greater_equal
{
        auto operator()(auto const& a, auto const& b) const noexcept
        {                                                                       // [tab:container.opt]
                static_assert(std::convertible_to<decltype(a >= b), bool>);
                BOOST_CHECK_EQUAL(a >= b, (a <=> b) >= 0);
                BOOST_CHECK_EQUAL(a >= b, !(a < b));
        }
};

struct fn_iterator
{
        auto operator()(auto& c) const noexcept
        {
                BOOST_CHECK( begin(c) == c.begin());                            // [iterator.range]/2
                BOOST_CHECK(   end(c) == c.end());                              // [iterator.range]/3

                BOOST_CHECK(rbegin(c) == c.rbegin());                           // [iterator.range]/8
                BOOST_CHECK(  rend(c) == c.rend());                             // [iterator.range]/9
        }

        auto operator()(auto const& c) const noexcept
        {
                BOOST_CHECK(  begin(c) == c.begin());                           // [iterator.range]/2
                BOOST_CHECK(    end(c) == c.end());                             // [iterator.range]/3

                BOOST_CHECK( cbegin(c) == begin(c));                            // [iterator.range]/6
                BOOST_CHECK(   cend(c) == end(c));                              // [iterator.range]/7

                BOOST_CHECK( rbegin(c) == c.rbegin());                          // [iterator.range]/8
                BOOST_CHECK(   rend(c) == c.rend());                            // [iterator.range]/9

                BOOST_CHECK(crbegin(c) == rbegin(c));                           // [iterator.range]/14
                BOOST_CHECK(  crend(c) == rend(c));                             // [iterator.range]/15
        }
};

struct fn_size
{
        auto operator()(auto const& c) const noexcept
        {
                BOOST_CHECK_EQUAL(size(c), c.size());                           // [iterator.range]/16
        }
};

struct fn_ssize
{
        auto operator()(auto const& c) const noexcept
        {
                using R = std::common_type_t<std::ptrdiff_t, std::make_signed_t<decltype(c.size())>>;
                BOOST_CHECK_EQUAL(ssize(c), static_cast<R>(c.size()));          // [iterator.range]/18
        }
};

struct fn_empty
{
        auto operator()(auto const& c) const noexcept
        {
                BOOST_CHECK_EQUAL(empty(c), c.empty());                         // [iterator.range]/20
        }
};

}       // namespace xstd
