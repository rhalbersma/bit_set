#pragma once

//          Copyright Rein Halbersma 2014-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <traits.hpp>                   // has_back_v, has_front_v
#include <boost/test/unit_test.hpp>     // BOOST_CHECK, BOOST_CHECK_EQUAL, BOOST_CHECK_EQUAL_COLLECTIONS, BOOST_CHECK_NE
#include <algorithm>                    // all_of, any_of, copy_if, count, equal, equal_range, find, for_each, includes, lexicographical_compare, lower_bound,
                                        // none_of, set_difference, set_intersection, set_symmetric_difference, set_union, transform, upper_bound
#include <initializer_list>             // initializer_list
#include <iterator>                     // distance, inserter, next, prev
#include <memory>                       // addressof
#include <numeric>                      // accumulate
#include <set>                          // set
#include <type_traits>                  // is_constructible_v, is_convertible_v, is_default_constructrible_v, is_integral_v, is_same_v, is_signed_v,
#include <utility>                      // pair

namespace xstd {

template<class X>
struct nested_types
{
                                                                                // [container.requirements.general] Table 83
        static_assert(std::is_convertible_v<typename X::iterator, typename X::const_iterator>);
        static_assert(std::is_same_v<typename X::      iterator::value_type, typename X::value_type>);
        static_assert(std::is_same_v<typename X::const_iterator::value_type, typename X::value_type>);
        static_assert(std::is_signed_v<typename X::difference_type>);
        static_assert(std::is_integral_v<typename X::difference_type>);

        // we use a signed instead of an unsigned size_type for xstd::int_set
        static_assert(std::is_integral_v<typename X::size_type>);

        static_assert(std::is_same_v<typename X::value_type, int>);

        // we use a proxy reference for xstd::int_set which convertible to int const&, instead of the same as int const&
        static_assert(std::is_convertible_v<typename X::const_reference, int const&>);

                                                                                // [container.requirements.general] Table 84
        static_assert(std::is_same_v<typename X::      reverse_iterator, std::reverse_iterator<typename X::      iterator>>);
        static_assert(std::is_same_v<typename X::const_reverse_iterator, std::reverse_iterator<typename X::const_iterator>>);

                                                                                // [associative.reqmts]/6
        static_assert(std::is_same_v<typename std::iterator_traits<typename X::iterator>::iterator_category, std::bidirectional_iterator_tag>);
};

template<class X>
struct constructor
{
        constexpr auto operator()() const
        {                                                                       // [associative.reqmts] Table 90
                static_assert(std::is_default_constructible_v<typename X::key_compare>);
                X u;
                BOOST_CHECK(u.empty());
                BOOST_CHECK(X().empty());
        }

        template<class InputIterator>
        auto operator()(InputIterator i, InputIterator j) const
        {                                                                       // [associative.reqmts] Table 90
                static_assert(std::is_default_constructible_v<typename X::key_compare>);
                X u(i, j);
                X u1;
                u1.insert(i, j);
                BOOST_CHECK(u == u1);
        }

        constexpr auto operator()(std::initializer_list<typename X::value_type> il) const
        {                                                                       // [associative.reqmts] Table 90
                X u(il);
                X u1(il.begin(), il.end());
                BOOST_CHECK(u == u1);
        }
};

struct op_assign
{
        template<class X>
        constexpr auto operator()(X& a, std::initializer_list<typename X::value_type> il) const
        {                                                                       // [associative.reqmts] Table 90
                a = il;
                X a1(il);
                BOOST_CHECK(a == a1);
        }
};

struct mem_const_reference
{
        template<class IntSet>
        auto operator()(IntSet const& is) const noexcept
        {
                for (auto const ref : is) {
                        BOOST_CHECK(is.count(ref));
                }
        }
};

struct mem_const_iterator
{
        template<class X>
        auto operator()(X& a) const noexcept
        {                                                                       // [container.requirements.general] Table 83
                static_assert(std::is_same_v<decltype(a.begin()), typename X::iterator>);
                static_assert(std::is_same_v<decltype(a.end())  , typename X::iterator>);

                                                                                // [container.requirements.general] Table 84
                static_assert(std::is_same_v<decltype(a.rbegin()), typename X::reverse_iterator>);
                static_assert(std::is_same_v<decltype(a.rend())  , typename X::reverse_iterator>);
        }

        template<class X>
        auto operator()(X const& a) const noexcept
        {                                                                       // [container.requirements.general] Table 83
                static_assert(std::is_same_v<decltype(a.begin()) , typename X::const_iterator>);
                static_assert(std::is_same_v<decltype(a.end())   , typename X::const_iterator>);
                static_assert(std::is_same_v<decltype(a.cbegin()), typename X::const_iterator>);
                static_assert(std::is_same_v<decltype(a.cend())  , typename X::const_iterator>);
                BOOST_CHECK(a.cbegin() == const_cast<X const&>(a).begin());
                BOOST_CHECK(a.cend()   == const_cast<X const&>(a).end()  );

                                                                                // [container.requirements.general] Table 84
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
        auto operator()(IntSet const& is [[maybe_unused]]) const noexcept
        {
                if constexpr (tti::has_front_v<IntSet>) {
                        BOOST_CHECK(is.empty() || (is.front() == *is.cbegin()));
                        BOOST_CHECK(is.empty() || (&is.front() == is.cbegin()));
                }
        }
};

struct mem_back
{
        template<class IntSet>
        auto operator()(IntSet const& is [[maybe_unused]]) const noexcept
        {
                if constexpr (tti::has_back_v<IntSet>) {
                        BOOST_CHECK(is.empty() || (is.back() == *is.crbegin()));
                        BOOST_CHECK(is.empty() || (&is.back() == std::next(is.crbegin()).base()));
                }
        }
};

struct mem_empty
{
        template<class X>
        auto operator()(X const& a) const noexcept
        {                                                                       // [container.requirements.general] Table 83
                static_assert(std::is_convertible_v<decltype(a.empty()), bool>);
                BOOST_CHECK_EQUAL(a.empty(), a.begin() == a.end());
        }
};

struct mem_size
{
        template<class X>
        auto operator()(X const& a) const noexcept
        {                                                                       // [container.requirements.general] Table 83
                static_assert(std::is_same_v<decltype(a.size()), typename X::size_type>);
                BOOST_CHECK_EQUAL(static_cast<std::ptrdiff_t>(a.size()), std::distance(a.begin(), a.end()));
        }
};

struct mem_max_size
{
        template<class X>
        auto operator()(X const& a) const noexcept
        {                                                                       // [container.requirements.general] Table 83
                static_assert(std::is_same_v<decltype(a.max_size()), typename X::size_type>);
                BOOST_CHECK_EQUAL(a.max_size(), X{}.max_size());
        }
};

struct mem_insert
{
        template<class X>
        auto operator()(X& a, typename X::value_type const t) const
        {                                                                       // [associative.reqmts] Table 90
                static_assert(std::is_same_v<decltype(a.insert(t)), std::pair<typename X::iterator, bool>>);
                auto r = a.insert(t);
                BOOST_CHECK(r == std::make_pair(a.find(t), true));
        }

        template<class X>
        auto operator()(X& a, typename X::iterator p, typename X::value_type const t) const
        {                                                                       // [associative.reqmts] Table 90
                static_assert(std::is_same_v<decltype(a.insert(p, t)), typename X::iterator>);
                auto r = a.insert(p, t);
                BOOST_CHECK(r == a.find(t));
        }

        template<class X, class InputIterator>
        auto operator()(X& a, InputIterator i, InputIterator j) const
        {                                                                       // [associative.reqmts] Table 90
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
        constexpr auto operator()(X& a, std::initializer_list<typename X::value_type> il) const
        {                                                                       // [associative.reqmts] Table 90
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
        {                                                                       // [associative.reqmts] Table 90
                static_assert(std::is_same_v<decltype(a.erase(k)), typename X::size_type>);
                auto const expected = a.count(k);
                auto const returns = a.erase(k);
                BOOST_CHECK(returns == expected);
        }

        template<class X>
        auto operator()(X& a, typename X::const_iterator q) const
        {                                                                       // [associative.reqmts] Table 90
                static_assert(std::is_same_v<decltype(a.erase(q)), typename X::iterator>);
                BOOST_CHECK(q != a.end());
                auto const expected = std::next(q);                             // assumes erase does not invalidate iterators
                auto const returns = a.erase(q);
                BOOST_CHECK(returns == expected);
        }

        template<class X>
        auto operator()(X& a, typename X::const_iterator q1, typename X::const_iterator q2) const
        {                                                                       // [associative.reqmts] Table 90
                static_assert(std::is_same_v<decltype(a.erase(q1, q2)), typename X::iterator>);
                auto const expected = q2;                                       // assumes erase does not invalidate iterators
                auto const returns = a.erase(q1, q2);
                BOOST_CHECK(returns == expected);
        }
};

struct mem_swap
{
        template<class X>
        constexpr auto operator()(X& a, X& b) const noexcept
        {                                                                       // [container.requirements.general] Table 83
                auto a1 = a, b1 = b;
                static_assert(std::is_same_v<decltype(a.swap(b)), void>);
                a1.swap(b1);
                BOOST_CHECK(a1 == b);
                BOOST_CHECK(b1 == a);
        }
};

struct mem_clear
{
        template<class X>
        auto operator()(X& a) const noexcept
        {                                                                       // [associative.reqmts] Table 90
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
        {                                                                       // [associative.reqmts] Table 90
                static_assert(std::is_same_v<decltype(b.find(k)), typename X::iterator>);
                BOOST_CHECK(b.find(k) == std::find(b.begin(), b.end(), k));
        }

        template<class X>
        auto operator()(X const& b, typename X::key_type const& k) const
        {                                                                       // [associative.reqmts] Table 90
                static_assert(std::is_same_v<decltype(b.find(k)), typename X::const_iterator>);
                BOOST_CHECK(b.find(k) == std::find(b.begin(), b.end(), k));
        }
};

struct mem_count
{
        template<class X>
        auto operator()(X const& b, typename X::key_type const& k) const
        {                                                                       // [associative.reqmts] Table 90
                static_assert(std::is_same_v<decltype(b.count(k)), typename X::size_type>);
                BOOST_CHECK_EQUAL(static_cast<std::ptrdiff_t>(b.count(k)), std::count(b.begin(), b.end(), k));
        }
};

struct mem_lower_bound
{
        template<class X>
        auto operator()(X& b, typename X::key_type const& k) const
        {                                                                       // [associative.reqmts] Table 90
                static_assert(std::is_same_v<decltype(b.lower_bound(k)), typename X::iterator>);
                BOOST_CHECK(b.lower_bound(k) == std::lower_bound(b.begin(), b.end(), k));
        }

        template<class X>
        auto operator()(X const& b, typename X::key_type const& k) const
        {                                                                       // [associative.reqmts] Table 90
                static_assert(std::is_same_v<decltype(b.lower_bound(k)), typename X::const_iterator>);
                BOOST_CHECK(b.lower_bound(k) == std::lower_bound(b.begin(), b.end(), k));
        }
};

struct mem_upper_bound
{
        template<class X>
        auto operator()(X& b, typename X::key_type const& k) const
        {                                                                       // [associative.reqmts] Table 90
                static_assert(std::is_same_v<decltype(b.upper_bound(k)), typename X::iterator>);
                BOOST_CHECK(b.upper_bound(k) == std::upper_bound(b.begin(), b.end(), k));
        }

        template<class X>
        auto operator()(X const& b, typename X::key_type const& k) const
        {                                                                       // [associative.reqmts] Table 90
                static_assert(std::is_same_v<decltype(b.upper_bound(k)), typename X::const_iterator>);
                BOOST_CHECK(b.upper_bound(k) == std::upper_bound(b.begin(), b.end(), k));
        }
};

struct mem_equal_range
{
        template<class X>
        auto operator()(X& b, typename X::key_type const& k) const
        {                                                                       // [associative.reqmts] Table 90
                using iterator = typename X::iterator;
                static_assert(std::is_same_v<decltype(b.equal_range(k)), std::pair<iterator, iterator>>);
                BOOST_CHECK(b.equal_range(k) == std::make_pair(b.lower_bound(k), b.upper_bound(k)));

                BOOST_CHECK(b.equal_range(k) == std::equal_range(b.begin(), b.end(), k));
        }

        template<class X>
        auto operator()(X const& b, typename X::key_type const& k) const
        {                                                                       // [associative.reqmts] Table 90
                using const_iterator = typename X::const_iterator;
                static_assert(std::is_same_v<decltype(b.equal_range(k)), std::pair<const_iterator, const_iterator>>);
                BOOST_CHECK(b.equal_range(k) == std::make_pair(b.lower_bound(k), b.upper_bound(k)));

                BOOST_CHECK(b.equal_range(k) == std::equal_range(b.begin(), b.end(), k));
        }
};

struct op_equal_to
{
        template<class X>
        auto operator()(X const& a) const noexcept
        {                                                                       // [container.requirements.general] Table 83
                BOOST_CHECK(a == a);                                            // reflexive
        }

        template<class X>
        auto operator()(X const& a, X const& b) const noexcept
        {                                                                       // [container.requirements.general] Table 83
                static_assert(std::is_convertible_v<decltype(a == b), bool>);
                BOOST_CHECK_EQUAL(a == b, std::equal(a.begin(), a.end(), b.begin(), b.end()));
                BOOST_CHECK(!(a == b) || b == a);                               // symmetric
        }

        template<class X>
        auto operator()(X const& a, X const& b, X const& c) const noexcept
        {                                                                       // [container.requirements.general] Table 83
                BOOST_CHECK(!(a == b && b == c) || a == c);                     // transitive
        }
};

struct op_not_equal_to
{
        template<class X>
        auto operator()(X const& a, X const& b) const noexcept
        {                                                                       // [container.requirements.general] Table 83
                static_assert(std::is_convertible_v<decltype(a != b), bool>);
                BOOST_CHECK_EQUAL(a != b, !(a == b));
        }
};

struct op_less
{
        template<class X>
        auto operator()(X const& a) const noexcept
        {                                                                       // [container.requirements.general] Table 85
                BOOST_CHECK(!(a < a));                                          // irreflexive
        }

        template<class X>
        auto operator()(X const& a, X const& b) const noexcept
        {                                                                       // [container.requirements.general] Table 85
                static_assert(std::is_convertible_v<decltype(a < b), bool>);
                BOOST_CHECK_EQUAL(a < b, std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end()));
                BOOST_CHECK(!(a < b) || !(b < a));                              // asymmetric
        }

        template<class X>
        auto operator()(X const& a, X const& b, X const& c) const noexcept
        {                                                                       // [container.requirements.general] Table 85
                BOOST_CHECK(!(a < b && b < c) || a < c);                        // transitive
        }
};

struct op_greater
{
        template<class X>
        auto operator()(X const& a, X const& b) const noexcept
        {                                                                       // [container.requirements.general] Table 85
                static_assert(std::is_convertible_v<decltype(a > b), bool>);
                BOOST_CHECK_EQUAL(a > b, b < a);
        }
};

struct op_less_equal
{
        template<class X>
        auto operator()(X const& a, X const& b) const noexcept
        {                                                                       // [container.requirements.general] Table 85
                static_assert(std::is_convertible_v<decltype(a <= b), bool>);
                BOOST_CHECK_EQUAL(a <= b, !(b < a));
        }
};

struct op_greater_equal
{
        template<class X>
        auto operator()(X const& a, X const& b) const noexcept
        {                                                                       // [container.requirements.general] Table 85
                static_assert(std::is_convertible_v<decltype(a >= b), bool>);
                BOOST_CHECK_EQUAL(a >= b, !(a < b));
        }
};

struct fn_swap
{
        template<class X>
        constexpr auto operator()(X& a, X& b) const noexcept
        {                                                                       // [container.requirements.general] Table 83
                auto a1 = a, b1 = b;
                auto a2 = a, b2 = b;
                static_assert(std::is_same_v<decltype(swap(a, b)), void>);
                swap(a1, b1); a2.swap(b2);
                BOOST_CHECK(a1 == a2); BOOST_CHECK(b1 == b2);                   // [set.special]/1
        }
};

struct fn_iterator
{
        template<class C>
        auto operator()(C& c) const noexcept
        {
                BOOST_CHECK( begin(c) == c.begin());                            // [iterator.range]/2
                BOOST_CHECK(   end(c) == c.end());                              // [iterator.range]/3
                BOOST_CHECK(rbegin(c) == c.rbegin());                           // [iterator.range]/8
                BOOST_CHECK(  rend(c) == c.rend());                             // [iterator.range]/9
        }

        template<class C>
        auto operator()(C const& c) const noexcept
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
        template<class C>
        auto operator()(C const& c) const noexcept
        {
                BOOST_CHECK_EQUAL(size(c), c.size());                           // [iterator.container]/2
        }
};

struct fn_empty
{
        template<class C>
        auto operator()(C const& c) const noexcept
        {
                BOOST_CHECK_EQUAL(empty(c), c.empty());                         // [iterator.container]/4
        }
};

}       // namespace xstd
