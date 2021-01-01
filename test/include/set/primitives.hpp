#pragma once

//          Copyright Rein Halbersma 2014-2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <iostream.hpp>                 // operator<<, operator>>
#include <traits.hpp>                   // has_back, has_front, is_dereferencable, is_equality_comparable
#include <boost/test/unit_test.hpp>     // BOOST_CHECK, BOOST_CHECK_EQUAL, BOOST_CHECK_EQUAL_COLLECTIONS, BOOST_CHECK_NE
#include <algorithm>                    // count, equal, equal_range, find, for_each, lexicographical_compare,
                                        // lexicographical_compare_three_way, lower_bound, upper_bound
#include <compare>                      // strong_ordering
#include <concepts>                     // signed_integral, unsigned_integral
#include <initializer_list>             // initializer_list
#include <iterator>                     // distance, empty, next, prev, size, ssize
#include <limits>                       // max
#include <memory>                       // addressof
#include <numeric>                      // accumulate
#include <type_traits>                  // is_constructible_v, is_convertible_v, is_default_constructible_v, is_same_v
#include <utility>                      // pair

namespace xstd {

template<class X>
struct nested_types
{
                                                                                // [tab:container.req]
        static_assert(std::is_convertible_v<typename X::iterator, typename X::const_iterator>);
        static_assert(std::is_same_v<typename X::      iterator::value_type, typename X::value_type>);
        static_assert(std::is_same_v<typename X::const_iterator::value_type, typename X::value_type>);
        static_assert(std::signed_integral<typename X::difference_type>);
        static_assert(std::is_same_v<typename X::difference_type, typename std::iterator_traits<typename X::iterator>::difference_type>);
        static_assert(std::unsigned_integral<typename X::size_type>);
        static_assert(std::numeric_limits<typename X::difference_type>::max() <= std::numeric_limits<typename X::size_type>::max());

        static_assert(std::is_same_v<typename X::value_type, int>);

        // we use a proxy reference for xstd::bit_set which convertible to int const&, instead of the same as int const&
        static_assert(std::is_convertible_v<typename X::const_reference, int const&>);

                                                                                // [tab:container.rev.req]
        static_assert(std::is_same_v<typename X::      reverse_iterator, std::reverse_iterator<typename X::      iterator>>);
        static_assert(std::is_same_v<typename X::const_reverse_iterator, std::reverse_iterator<typename X::const_iterator>>);

                                                                                // [associative.reqmts]/6
        static_assert(std::is_same_v<typename std::iterator_traits<typename X::iterator>::iterator_category, std::bidirectional_iterator_tag>);
};

template<class X>
struct constructor
{
        auto operator()() const
        {                                                                       // [tab:container.assoc.req]
                static_assert(std::is_default_constructible_v<typename X::key_compare>);
                X u;
                BOOST_CHECK(u.empty());
                BOOST_CHECK(X().empty());
        }

        auto operator()(auto i, auto j) const
        {                                                                       // [tab:container.assoc.req]
                static_assert(std::is_default_constructible_v<typename X::key_compare>);
                X u(i, j);
                X u1;
                u1.insert(i, j);
                BOOST_CHECK(u == u1);
        }

        auto operator()(std::initializer_list<typename X::value_type> il) const
        {                                                                       // [tab:container.assoc.req]
                X u(il);
                X u1(il.begin(), il.end());
                BOOST_CHECK(u == u1);
        }
};

struct op_assign
{
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
        {                                                                       // [tab:container.req]
                static_assert(std::is_same_v<decltype(a.begin()), typename X::iterator>);
                static_assert(std::is_same_v<decltype(a.end())  , typename X::iterator>);

                using Iterator = typename X::iterator;                          // [iterator.iterators] Table 94
                using Reference = typename std::iterator_traits<Iterator>::reference;
                using ValueType = typename std::iterator_traits<Iterator>::value_type;

                static_assert(tti::is_dereferenceable<Iterator>);
                static_assert(std::is_same_v<decltype(++std::declval<Iterator>()), Iterator&>);

                using InputIterator = Iterator;                                // [input.iterators] Table 95
                static_assert(tti::is_equality_comparable<InputIterator>);
                {
                        auto r = a.begin();
                        auto s = a.end();
                        BOOST_CHECK_EQUAL(r != s, !(r == s));
                }
                static_assert(std::is_same_v       <decltype(*std::declval<InputIterator>()), Reference>);
                static_assert(std::is_convertible_v<decltype(*std::declval<InputIterator>()), ValueType>);
                {
                        if (a.begin() != a.end()) {
                                auto r = a.begin();
                                auto s = a.begin();
                                r++;
                                ++s;
                                BOOST_CHECK(r == s);
                        }
                }
                static_assert(std::is_convertible_v<decltype(*std::declval<InputIterator>()++), ValueType>);
                {
                        if (a.begin() != a.end()) {
                                auto r = a.begin();
                                auto s = a.begin();
                                auto tmp = *s;
                                ++s;
                                BOOST_CHECK_EQUAL(*r++, tmp);
                                BOOST_CHECK(r == s);
                        }
                }

                using ForwardIterator = InputIterator;                          // [forward.iterators] Table 97
                static_assert(std::is_convertible_v<decltype( std::declval<ForwardIterator>()++), ForwardIterator const&>);
                static_assert(std::is_same_v       <decltype(*std::declval<ForwardIterator>()++), Reference>);

                using BidirectionalIterator = ForwardIterator;                  // [bidirectional.iterators] Table 98
                static_assert(std::is_same_v       <decltype(--std::declval<BidirectionalIterator>()  ), BidirectionalIterator&>);
                static_assert(std::is_convertible_v<decltype(  std::declval<BidirectionalIterator>()--), BidirectionalIterator const&>);
                static_assert(std::is_same_v       <decltype( *std::declval<BidirectionalIterator>()--), Reference>);
                {
                        if (a.begin() != a.end()) {
                                auto r = a.end();
                                auto s = a.end();
                                auto tmp = s;
                                --s;
                                BOOST_CHECK(r-- == tmp);
                                BOOST_CHECK(r == s);
                        }
                }

                                                                                // [tab:container.rev.req]
                static_assert(std::is_same_v<decltype(a.rbegin()), typename X::reverse_iterator>);
                static_assert(std::is_same_v<decltype(a.rend())  , typename X::reverse_iterator>);
        }

        template<class X>
        auto operator()(X const& a) const noexcept
        {                                                                       // [tab:container.req]
                static_assert(std::is_same_v<decltype(a.begin()) , typename X::const_iterator>);
                static_assert(std::is_same_v<decltype(a.end())   , typename X::const_iterator>);
                static_assert(std::is_same_v<decltype(a.cbegin()), typename X::const_iterator>);
                static_assert(std::is_same_v<decltype(a.cend())  , typename X::const_iterator>);
                BOOST_CHECK(a.cbegin() == const_cast<X const&>(a).begin());
                BOOST_CHECK(a.cend()   == const_cast<X const&>(a).end()  );

                                                                                // [tab:container.rev.req]
                static_assert(std::is_same_v<decltype(a.rbegin()) , typename X::const_reverse_iterator>);
                static_assert(std::is_same_v<decltype(a.rend())   , typename X::const_reverse_iterator>);
                static_assert(std::is_same_v<decltype(a.crbegin()), typename X::const_reverse_iterator>);
                static_assert(std::is_same_v<decltype(a.crend())  , typename X::const_reverse_iterator>);
                BOOST_CHECK(a.crbegin() == const_cast<X const&>(a).rbegin());
                BOOST_CHECK(a.crend()   == const_cast<X const&>(a).rend()  );
        }
};

struct mem_front
{
        template<class IntSet>
        auto operator()(IntSet const& bs [[maybe_unused]]) const noexcept
        {
                if constexpr (tti::has_front<IntSet>) {
                        BOOST_CHECK(bs.empty() || (bs.front() == *bs.cbegin()));
                        BOOST_CHECK(bs.empty() || (&bs.front() == bs.cbegin()));
                }
        }
};

struct mem_back
{
        template<class IntSet>
        auto operator()(IntSet const& bs [[maybe_unused]]) const noexcept
        {
                if constexpr (tti::has_back<IntSet>) {
                        BOOST_CHECK(bs.empty() || (bs.back() == *bs.crbegin()));
                        BOOST_CHECK(bs.empty() || (&bs.back() == std::next(bs.crbegin()).base()));
                }
        }
};

struct mem_empty
{
        auto operator()(auto const& a) const noexcept
        {                                                                       // [tab:container.req]
                static_assert(std::is_convertible_v<decltype(a.empty()), bool>);
                BOOST_CHECK_EQUAL(a.empty(), a.begin() == a.end());
        }
};

struct mem_size
{
        template<class X>
        auto operator()(X const& a) const noexcept
        {                                                                       // [tab:container.req]
                static_assert(std::is_same_v<decltype(a.size()), typename X::size_type>);
                BOOST_CHECK_EQUAL(static_cast<std::ptrdiff_t>(a.size()), std::distance(a.begin(), a.end()));
        }
};

struct mem_max_size
{
        template<class X>
        auto operator()(X const& a) const noexcept
        {                                                                       // [tab:container.req]
                static_assert(std::is_same_v<decltype(a.max_size()), typename X::size_type>);
                BOOST_CHECK_EQUAL(a.max_size(), X().max_size());
                BOOST_CHECK_LE(a.size(), a.max_size());
        }
};

struct mem_capacity
{
        template<class X>
        auto operator()(X const& a) const noexcept
        {
                if constexpr (tti::has_capacity<X>) {                         // [tab:container.req]
                        static_assert(std::is_same_v<decltype(a.capacity()), typename X::size_type>);
                        BOOST_CHECK_LE(a.size(), a.capacity());
                }
        }
};

struct mem_emplace
{
        template<class X, class... Args>
        auto operator()(X& a, Args&&... args) const
        {                                                                       // [tab:container.assoc.req]
                static_assert(std::is_same_v<decltype(a.emplace(std::forward<Args>(args)...)), std::pair<typename X::iterator, bool>>);
                auto r = a.emplace(std::forward<Args>(args)...);
                BOOST_CHECK(r == std::make_pair(a.find(typename X::value_type(std::forward<Args>(args)...)), true));
        }
};

struct mem_emplace_hint
{
        template<class X, class... Args>
        auto operator()(X& a, typename X::iterator p, Args&&... args) const
        {                                                                       // [tab:container.assoc.req]
                static_assert(std::is_same_v<decltype(a.emplace_hint(p, std::forward<Args>(args)...)), typename X::iterator>);
                auto r = a.emplace_hint(p, std::forward<Args>(args)...);
                BOOST_CHECK(r == a.find(typename X::value_type(std::forward<Args>(args)...)));
        }
};

struct mem_insert
{
        template<class X>
        auto operator()(X& a, typename X::value_type const& t) const
        {                                                                       // [tab:container.assoc.req]
                static_assert(std::is_same_v<decltype(a.insert(t)), std::pair<typename X::iterator, bool>>);
                auto r = a.insert(t);
                BOOST_CHECK(r == std::make_pair(a.find(t), true));
        }

        template<class X>
        auto operator()(X& a, typename X::iterator p, typename X::value_type const& t) const
        {                                                                       // [tab:container.assoc.req]
                static_assert(std::is_same_v<decltype(a.insert(p, t)), typename X::iterator>);
                auto r = a.insert(p, t);
                BOOST_CHECK(r == a.find(t));
        }

        template<class X, class InputIterator>
        auto operator()(X& a, InputIterator i, InputIterator j) const
        {                                                                       // [tab:container.assoc.req]
                static_assert(std::is_convertible_v<typename std::iterator_traits<InputIterator>::value_type, typename X::value_type>);
                static_assert(std::is_same_v<decltype(a.insert(i, j)), void>);
                auto a1 = a;
                a.insert(i, j);
                std::for_each(i, j, [&](auto const& t) {
                        a1.insert(t);
                });
                BOOST_CHECK(a == a1);
        }

        template<class X>
        auto operator()(X& a, std::initializer_list<typename X::value_type> il) const
        {                                                                       // [tab:container.assoc.req]
                static_assert(std::is_same_v<decltype(a.insert(il)), void>);
                auto a1 = a;
                a.insert(il);
                a1.insert(il.begin(), il.end());
                BOOST_CHECK(a == a1);
        }
};

struct mem_erase
{
        template<class X>
        auto operator()(X& a, typename X::key_type const& k) const
        {                                                                       // [tab:container.assoc.req]
                static_assert(std::is_same_v<decltype(a.erase(k)), typename X::size_type>);
                auto const expected = a.count(k);
                auto const returns = a.erase(k);
                BOOST_CHECK(returns == expected);
        }

        template<class X>
        auto operator()(X& a, typename X::const_iterator q) const
        {                                                                       // [tab:container.assoc.req]
                static_assert(std::is_same_v<decltype(a.erase(q)), typename X::iterator>);
                BOOST_CHECK(q != a.end());
                auto const expected = std::next(q);                             // assumes erase does not invalidate iterators
                auto const returns = a.erase(q);
                BOOST_CHECK(returns == expected);
        }

        template<class X>
        auto operator()(X& a, typename X::const_iterator q1, typename X::const_iterator q2) const
        {                                                                       // [tab:container.assoc.req]
                static_assert(std::is_same_v<decltype(a.erase(q1, q2)), typename X::iterator>);
                auto const expected = q2;                                       // assumes erase does not invalidate iterators
                auto const returns = a.erase(q1, q2);
                BOOST_CHECK(returns == expected);
        }
};

struct mem_swap
{
        auto operator()(auto& a, auto& b) const noexcept
        {                                                                       // [tab:container.req]
                auto a1 = a, b1 = b;
                static_assert(std::is_same_v<decltype(a.swap(b)), void>);
                a1.swap(b1);
                BOOST_CHECK(a1 == b);
                BOOST_CHECK(b1 == a);
        }
};

struct mem_clear
{
        auto operator()(auto& a) const noexcept
        {                                                                       // [tab:container.assoc.req]
                auto a1 = a;
                //static_assert(std::is_same_v<decltype(a.clear()), void>);
                a.clear();
                a1.erase(a1.begin(), a1.end());
                BOOST_CHECK(a == a1);
                BOOST_CHECK(a.empty());
        }
};

struct mem_find
{
        template<class X>
        auto operator()(X& b, typename X::key_type const& k) const
        {                                                                       // [tab:container.assoc.req]
                static_assert(std::is_same_v<decltype(b.find(k)), typename X::iterator>);
                BOOST_CHECK(b.find(k) == std::ranges::find(b, k));
        }

        template<class X>
        auto operator()(X const& b, typename X::key_type const& k) const
        {                                                                       // [tab:container.assoc.req]
                static_assert(std::is_same_v<decltype(b.find(k)), typename X::const_iterator>);
                BOOST_CHECK(b.find(k) == std::ranges::find(b, k));
        }
};

struct mem_count
{
        template<class X>
        auto operator()(X const& b, typename X::key_type const& k) const
        {                                                                       // [tab:container.assoc.req]
                static_assert(std::is_same_v<decltype(b.count(k)), typename X::size_type>);
                BOOST_CHECK_EQUAL(static_cast<std::ptrdiff_t>(b.count(k)), std::ranges::count(b, k));
        }
};

struct mem_contains
{
        template<class X>
        auto operator()(X const& b, typename X::key_type const& k) const
        {                                                                       // [tab:container.assoc.req]
                static_assert(std::is_same_v<decltype(b.contains(k)), bool>);
                BOOST_CHECK_EQUAL(b.contains(k), b.find(k) != b.end());
        }
};

struct mem_lower_bound
{
        template<class X>
        auto operator()(X& b, typename X::key_type const& k) const
        {                                                                       // [tab:container.assoc.req]
                static_assert(std::is_same_v<decltype(b.lower_bound(k)), typename X::iterator>);
                BOOST_CHECK(b.lower_bound(k) == std::ranges::lower_bound(b, k));
        }

        template<class X>
        auto operator()(X const& b, typename X::key_type const& k) const
        {                                                                       // [tab:container.assoc.req]
                static_assert(std::is_same_v<decltype(b.lower_bound(k)), typename X::const_iterator>);
                BOOST_CHECK(b.lower_bound(k) == std::ranges::lower_bound(b, k));
        }
};

struct mem_upper_bound
{
        template<class X>
        auto operator()(X& b, typename X::key_type const& k) const
        {                                                                       // [tab:container.assoc.req]
                static_assert(std::is_same_v<decltype(b.upper_bound(k)), typename X::iterator>);
                BOOST_CHECK(b.upper_bound(k) == std::ranges::upper_bound(b, k));
        }

        template<class X>
        auto operator()(X const& b, typename X::key_type const& k) const
        {                                                                       // [tab:container.assoc.req]
                static_assert(std::is_same_v<decltype(b.upper_bound(k)), typename X::const_iterator>);
                BOOST_CHECK(b.upper_bound(k) == std::ranges::upper_bound(b, k));
        }
};

struct mem_equal_range
{
        template<class X>
        auto operator()(X& b, typename X::key_type const& k) const
        {                                                                       // [tab:container.assoc.req]
                using iterator = typename X::iterator;
                static_assert(std::is_same_v<decltype(b.equal_range(k)), std::pair<iterator, iterator>>);
                BOOST_CHECK(b.equal_range(k) == std::make_pair(b.lower_bound(k), b.upper_bound(k)));
                BOOST_CHECK(b.equal_range(k) == std::equal_range(b.begin(), b.end(), k));
        }

        template<class X>
        auto operator()(X const& b, typename X::key_type const& k) const
        {                                                                       // [tab:container.assoc.req]
                using const_iterator = typename X::const_iterator;
                static_assert(std::is_same_v<decltype(b.equal_range(k)), std::pair<const_iterator, const_iterator>>);
                BOOST_CHECK(b.equal_range(k) == std::make_pair(b.lower_bound(k), b.upper_bound(k)));
                BOOST_CHECK(b.equal_range(k) == std::equal_range(b.begin(), b.end(), k));
        }
};

struct op_equal_to
{
        auto operator()(auto const& a) const noexcept
        {                                                                       // [tab:container.req]
                BOOST_CHECK(a == a);                                            // reflexive
        }

        auto operator()(auto const& a, auto const& b) const noexcept
        {                                                                       // [tab:container.req]
                static_assert(std::is_convertible_v<decltype(a == b), bool>);
                BOOST_CHECK_EQUAL(a == b, std::ranges::equal(a, b));
                BOOST_CHECK(!(a == b) || b == a);                               // symmetric
        }

        auto operator()(auto const& a, auto const& b, auto const& c) const noexcept
        {                                                                       // [tab:container.req]
                BOOST_CHECK(!(a == b && b == c) || a == c);                     // transitive
        }
};

struct op_not_equal_to
{
        auto operator()(auto const& a, auto const& b) const noexcept
        {                                                                       // [tab:container.req]
                static_assert(std::is_convertible_v<decltype(a != b), bool>);
                BOOST_CHECK_EQUAL(a != b, !(a == b));
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
                static_assert(std::is_same_v<decltype(a <=> b), std::strong_ordering>);
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
                static_assert(std::is_convertible_v<decltype(a < b), bool>);
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
                static_assert(std::is_convertible_v<decltype(a > b), bool>);
                BOOST_CHECK_EQUAL(a > b, (a <=> b) > 0);
                BOOST_CHECK_EQUAL(a > b, b < a);
        }
};

struct op_less_equal
{
        auto operator()(auto const& a, auto const& b) const noexcept
        {                                                                       // [tab:container.opt]
                static_assert(std::is_convertible_v<decltype(a <= b), bool>);
                BOOST_CHECK_EQUAL(a <= b, (a <=> b) <= 0);
                BOOST_CHECK_EQUAL(a <= b, !(b < a));
        }
};

struct op_greater_equal
{
        auto operator()(auto const& a, auto const& b) const noexcept
        {                                                                       // [tab:container.opt]
                static_assert(std::is_convertible_v<decltype(a >= b), bool>);
                BOOST_CHECK_EQUAL(a >= b, (a <=> b) >= 0);
                BOOST_CHECK_EQUAL(a >= b, !(a < b));
        }
};

struct fn_swap
{
        auto operator()(auto& a, auto& b) const noexcept
        {                                                                       // [tab:container.req]
                auto a1 = a, b1 = b;
                auto a2 = a, b2 = b;
                static_assert(std::is_same_v<decltype(swap(a, b)), void>);
                swap(a1, b1); a2.swap(b2);
                BOOST_CHECK(a1 == a2); BOOST_CHECK(b1 == b2);                   // [set.special]/1
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
                BOOST_CHECK( cbegin(c) == c.cbegin());                          // [iterator.range]/6
                BOOST_CHECK(   cend(c) == c.cend());                            // [iterator.range]/7
                BOOST_CHECK( rbegin(c) == c.rbegin());                          // [iterator.range]/8
                BOOST_CHECK(   rend(c) == c.rend());                            // [iterator.range]/9
                BOOST_CHECK(crbegin(c) == c.crbegin());                         // [iterator.range]/14
                BOOST_CHECK(  crend(c) == c.crend());                           // [iterator.range]/15
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
