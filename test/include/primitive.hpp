#pragma once

//          Copyright Rein Halbersma 2014-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <traits.hpp>                   // has_range_assign_v, has_ilist_assign_v, has_const_iterator_v, has_front_v, has_back_v,
                                        // has_any_of_v, has_none_of_v, has_all_of_v, has_accumulate_v, has_for_each_v, has_reverse_for_each_v,
                                        // has_op_minus_assign_v, has_range_insert_v, has_ilist_insert_v, has_range_erase_v, has_ilist_erase_v,
                                        // has_max_size_v, has_empty_v, has_full_v
#include <boost/test/unit_test.hpp>     // BOOST_CHECK, BOOST_CHECK_EQUAL, BOOST_CHECK_EQUAL_COLLECTIONS, BOOST_CHECK_NE, BOOST_CHECK_THROW
#include <algorithm>                    // all_of, any_of, copy_if, count, equal, find, for_each, includes, is_sorted, lexicographical_compare,
                                        // none_of, set_difference, set_intersection, set_symmetric_difference, set_union, transform
#include <functional>                   // greater, plus
#include <initializer_list>             // initializer_list
#include <istream>                      // basic_istream
#include <iterator>                     // distance, inserter, next, prev
#include <memory>                       // addressof
#include <numeric>                      // accumulate
#include <set>                          // set
#include <stdexcept>                    // out_of_range
#include <type_traits>                  // is_constructible_v
#include <vector>                       // vector

namespace xstd {

template<class IntSet>
struct constructor
{
        constexpr auto operator()() const noexcept
        {
                IntSet is;
                BOOST_CHECK(empty(is));                                         // [bitset.cons]/1
        }

        // [bitset.cons]/2-7 describe constructors taking unsigned long long, basic_string and char const*

        template<class InputIterator>
        auto operator()(InputIterator first [[maybe_unused]], InputIterator last [[maybe_unused]]) const
        {
                if constexpr (std::is_constructible_v<IntSet, InputIterator, InputIterator>) {
                        auto const dst = IntSet(first, last);
                        std::for_each(first, last, [&](auto const elem) {
                                BOOST_CHECK(dst.contains(elem));
                        });
                        BOOST_CHECK_EQUAL(dst.size(), static_cast<int>(std::distance(first, last)));
                }
        }

        template<class ValueType>
        constexpr auto operator()(std::initializer_list<ValueType> ilist [[maybe_unused]]) const
        {
                if constexpr (std::is_constructible_v<IntSet, std::initializer_list<ValueType>>) {
                        auto const dst = IntSet(ilist);
                        for (auto const elem : ilist) {
                                BOOST_CHECK(dst.contains(elem));
                        }
                        BOOST_CHECK_EQUAL(dst.size(), static_cast<int>(ilist.size()));
                }
        }
};

struct mem_assign
{
        template<class IntSet, class InputIterator>
        auto operator()(IntSet const& is [[maybe_unused]], InputIterator first [[maybe_unused]], InputIterator last [[maybe_unused]]) const
        {
                if constexpr (tti::has_range_assign_v<IntSet, InputIterator>) {
                        auto const src = is;
                        auto dst = src; dst.assign(first, last);
                        std::for_each(first, last, [&](auto const elem) {
                                BOOST_CHECK(dst.contains(elem));
                        });
                        BOOST_CHECK_EQUAL(dst.size(), static_cast<int>(std::distance(first, last)));
                }
        }

        template<class IntSet, class ValueType>
        constexpr auto operator()(IntSet const& is [[maybe_unused]], std::initializer_list<ValueType> ilist [[maybe_unused]]) const
        {
                if constexpr (tti::has_ilist_assign_v<IntSet, ValueType>) {
                        auto const src = is;
                        auto dst = src; dst = ilist;
                        for (auto const elem : ilist) {
                                BOOST_CHECK(dst.contains(elem));
                        }
                        BOOST_CHECK_EQUAL(dst.size(), static_cast<int>(ilist.size()));
                }
        }
};

struct const_reference
{
        template<class IntSet>
        auto operator()(IntSet const& is [[maybe_unused]]) const noexcept
        {
                if constexpr (tti::has_const_iterator_v<IntSet>) {
                        for (auto first = is.cbegin(), last = is.cend(); first != last; ++first) {
                                auto const ref = *first;

                                BOOST_CHECK(&ref == first);
                                BOOST_CHECK(is.contains(ref));
                        }

                        for (auto const ref : is) {
                                BOOST_CHECK(is.contains(ref));
                        }
                }
        }
};

struct const_iterator
{
        template<class IntSet>
        auto operator()(IntSet const& cis [[maybe_unused]]) const noexcept
        {
                if constexpr (tti::has_const_iterator_v<IntSet>) {
                        auto is = cis;

                        BOOST_CHECK_EQUAL(std::distance( is.  begin(),  is.  end()), cis.size());
                        BOOST_CHECK_EQUAL(std::distance(cis.  begin(), cis.  end()), cis.size());
                        BOOST_CHECK_EQUAL(std::distance( is. rbegin(),  is. rend()), cis.size());
                        BOOST_CHECK_EQUAL(std::distance(cis. rbegin(), cis. rend()), cis.size());
                        BOOST_CHECK_EQUAL(std::distance( is. cbegin(),  is. cend()), cis.size());
                        BOOST_CHECK_EQUAL(std::distance( is.crbegin(),  is.crend()), cis.size());
                        BOOST_CHECK_EQUAL(std::distance(  begin( is),   end( is)), cis.size());
                        BOOST_CHECK_EQUAL(std::distance(  begin(cis),   end(cis)), cis.size());
                        BOOST_CHECK_EQUAL(std::distance( rbegin( is),  rend( is)), cis.size());
                        BOOST_CHECK_EQUAL(std::distance( rbegin(cis),  rend(cis)), cis.size());
                        BOOST_CHECK_EQUAL(std::distance( cbegin( is),  cend( is)), cis.size());
                        BOOST_CHECK_EQUAL(std::distance(crbegin( is), crend( is)), cis.size());

                        BOOST_CHECK(std::is_sorted( is.  begin(),  is.  end()));
                        BOOST_CHECK(std::is_sorted(cis.  begin(), cis.  end()));
                        BOOST_CHECK(std::is_sorted( is. rbegin(),  is. rend(), std::greater{}));
                        BOOST_CHECK(std::is_sorted(cis. rbegin(), cis. rend(), std::greater{}));
                        BOOST_CHECK(std::is_sorted( is. cbegin(),  is. cend()));
                        BOOST_CHECK(std::is_sorted( is.crbegin(),  is.crend(), std::greater{}));
                        BOOST_CHECK(std::is_sorted(  begin( is),   end( is)));
                        BOOST_CHECK(std::is_sorted(  begin(cis),   end(cis)));
                        BOOST_CHECK(std::is_sorted( rbegin( is),  rend( is), std::greater{}));
                        BOOST_CHECK(std::is_sorted( rbegin(cis),  rend(cis), std::greater{}));
                        BOOST_CHECK(std::is_sorted( cbegin( is),  cend( is)));
                        BOOST_CHECK(std::is_sorted(crbegin( is), crend( is), std::greater{}));
                }
        }
};

struct mem_front
{
        template<class IntSet>
        auto operator()(IntSet const& is [[maybe_unused]]) const noexcept
        {
                if constexpr (tti::has_front_v<IntSet> && tti::has_const_iterator_v<IntSet>) {
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
                if constexpr (tti::has_back_v<IntSet> && tti::has_const_iterator_v<IntSet>) {
                        BOOST_CHECK(is.empty() || (is.back() == *is.crbegin()));
                        BOOST_CHECK(is.empty() || (&is.back() == std::next(is.crbegin()).base()));
                }
        }
};

struct mem_any_of
{
        template<class IntSet, class UnaryPredicate>
        auto operator()(IntSet const& is [[maybe_unused]], UnaryPredicate pred [[maybe_unused]]) const noexcept
        {
                if constexpr (tti::has_any_of_v<IntSet, UnaryPredicate> && tti::has_const_iterator_v<IntSet>) {
                        BOOST_CHECK_EQUAL(is.any_of(pred), std::any_of(is.cbegin(), is.cend(), pred));
                }
        }
};

struct mem_none_of
{
        template<class IntSet, class UnaryPredicate>
        auto operator()(IntSet const& is [[maybe_unused]], UnaryPredicate pred [[maybe_unused]]) const noexcept
        {
                if constexpr (tti::has_none_of_v<IntSet, UnaryPredicate> && tti::has_const_iterator_v<IntSet>) {
                        BOOST_CHECK_EQUAL(is.none_of(pred), std::none_of(is.cbegin(), is.cend(), pred));
                }
        }
};

struct mem_all_of
{
        template<class IntSet, class UnaryPredicate>
        auto operator()(IntSet const& is [[maybe_unused]], UnaryPredicate pred [[maybe_unused]]) const noexcept
        {
                if constexpr (tti::has_all_of_v<IntSet, UnaryPredicate> && tti::has_const_iterator_v<IntSet>) {
                        BOOST_CHECK_EQUAL(is.all_of(pred), std::all_of(is.cbegin(), is.cend(), pred));
                }
        }
};

struct mem_accumulate
{
        template<class IntSet, class T, class BinaryOperation = std::plus<>>
        auto operator()(IntSet const& is [[maybe_unused]], T init [[maybe_unused]], BinaryOperation op [[maybe_unused]] = BinaryOperation{}) const noexcept
        {
                if constexpr (tti::has_accumulate_v<IntSet, T, BinaryOperation> && tti::has_const_iterator_v<IntSet>) {
                        BOOST_CHECK_EQUAL(is.accumulate(init, op), std::accumulate(is.cbegin(), is.cend(), init, op));
                }
        }
};

namespace detail {

class tracer
{
        std::vector<int> m_trace;
public:
        auto operator()(int const elem)
        {
                m_trace.push_back(elem);
        }

        friend auto operator==(tracer const& lhs, tracer const& rhs) noexcept
        {
                return lhs.m_trace == rhs.m_trace;
        }
};

}       // namespace detail

struct mem_for_each
{
        template<class IntSet>
        auto operator()(IntSet const& is [[maybe_unused]]) const noexcept
        {
                if constexpr (tti::has_for_each_v<IntSet, detail::tracer> && tti::has_const_iterator_v<IntSet>) {
                        BOOST_CHECK(is.for_each(detail::tracer{}) == std::for_each(is.cbegin(), is.cend(), detail::tracer{}));

                        {
                                detail::tracer fun;
                                for (auto first = is.cbegin(), last = is.cend(); first != last; first++ /* post-increment to hit code coverage */) {
                                        fun(*first);
                                }
                                BOOST_CHECK(is.for_each(detail::tracer{}) == std::move(fun));
                        }

                        {
                                detail::tracer fun;
                                for (auto const elem : is) {
                                        fun(elem);
                                }
                                BOOST_CHECK(is.for_each(detail::tracer{}) == std::move(fun));
                        }
                }
        }
};

struct mem_reverse_for_each
{
        template<class IntSet>
        auto operator()(IntSet const& is [[maybe_unused]]) const noexcept
        {
                if constexpr (tti::has_reverse_for_each_v<IntSet, detail::tracer> && tti::has_const_iterator_v<IntSet>) {
                        BOOST_CHECK(is.reverse_for_each(detail::tracer{}) == std::for_each(is.crbegin(), is.crend(), detail::tracer{}));

                        {
                                detail::tracer fun;
                                for (auto first = is.cend(), last = is.cbegin(); first != last; first-- /* post-decrement to hit code coverage */) {
                                        fun(*std::prev(first));
                                }
                                BOOST_CHECK(is.reverse_for_each(detail::tracer{}) == std::move(fun));
                        }
                }
        }
};

struct op_bitand_assign
{
        template<class IntSet>
        constexpr auto operator()(IntSet const& lhs, IntSet const& rhs) const noexcept
        {
                auto const src = lhs;
                auto dst = src;
                auto const& ret = (dst &= rhs);

                for (auto N = max_size(IntSet{}), i = decltype(N){0}; i < N; ++i) {
                                                                                // [bitset.members]/1
                        BOOST_CHECK_EQUAL(contains(dst, i), !contains(rhs, i) ? false : contains(src, i));
                }
                BOOST_CHECK_EQUAL(std::addressof(ret), std::addressof(dst));    // [bitset.members]/2
        }
};

struct op_bitor_assign
{
        template<class IntSet>
        constexpr auto operator()(IntSet const& lhs, IntSet const& rhs) const noexcept
        {
                auto const src = lhs;
                auto dst = src;
                auto const& ret = (dst |= rhs);

                for (auto N = max_size(IntSet{}), i = decltype(N){0}; i < N; ++i) {
                                                                                // [bitset.members]/3
                        BOOST_CHECK_EQUAL(contains(dst, i), contains(rhs, i) ? true : contains(src, i));
                }
                BOOST_CHECK_EQUAL(std::addressof(ret), std::addressof(dst));    // [bitset.members]/4
        }
};

struct op_xor_assign
{
        template<class IntSet>
        constexpr auto operator()(IntSet const& lhs, IntSet const& rhs) const noexcept
        {
                auto const src = lhs;
                auto dst = src;
                auto const& ret = (dst ^= rhs);

                for (auto N = max_size(IntSet{}), i = decltype(N){0}; i < N; ++i) {
                                                                                // [bitset.members]/5
                        BOOST_CHECK_EQUAL(contains(dst, i), contains(rhs, i) ? !contains(src, i) : contains(src, i));
                }
                BOOST_CHECK_EQUAL(std::addressof(ret), std::addressof(dst));    // [bitset.members]/6
        }
};

struct op_minus_assign
{
        template<class IntSet>
        constexpr auto operator()(IntSet const& lhs [[maybe_unused]], IntSet const& rhs [[maybe_unused]]) const noexcept
        {
                if constexpr (tti::has_op_minus_assign_v<IntSet>) {
                        auto const src = lhs;
                        auto dst = src;
                        auto const& ret = (dst -= rhs);

                        for (auto N = max_size(IntSet{}), i = decltype(N){0}; i < N; ++i) {
                                BOOST_CHECK_EQUAL(contains(dst, i), contains(rhs, i) ? false : contains(src, i));
                        }
                        BOOST_CHECK_EQUAL(std::addressof(ret), std::addressof(dst));
                }
        }
};

struct op_shift_left_assign
{
        template<class IntSet, class SizeType>
        auto operator()(IntSet const& is, SizeType const pos) const
        {
                auto const src = is;
                auto dst = src;
                auto const& ret = (dst <<= pos);

                for (auto N = max_size(IntSet{}), I = decltype(N){0}; I < N; ++I) {
                        if (I < pos) {
                                BOOST_CHECK(!contains(dst, I));                 // [bitset.members]/7.1
                        } else {                                                // [bitset.members]/7.2
                                BOOST_CHECK_EQUAL(contains(dst, I), contains(src, I - pos));
                        }
                }
                BOOST_CHECK_EQUAL(std::addressof(ret), std::addressof(dst));    // [bitset.members]/8
        }
};

struct op_shift_right_assign
{
        template<class IntSet, class SizeType>
        auto operator()(IntSet const& is, SizeType const pos) const
        {
                auto const src = is;
                auto dst = src;
                auto const& ret = (dst >>= pos);

                for (auto N = max_size(IntSet{}), I = decltype(N){0}; I < N; ++I) {
                        if (pos >= N - I) {
                                BOOST_CHECK(!contains(dst, I));                 // [bitset.members]/9.1
                        } else {                                                // [bitset.members]/9.2
                                BOOST_CHECK_EQUAL(contains(dst, I), contains(src, I + pos));
                        }
                }
                BOOST_CHECK_EQUAL(std::addressof(ret), std::addressof(dst));    // [bitset.members]/10
        }
};

struct fn_fill
{
        template<class IntSet>
        auto operator()(IntSet const& is) const noexcept
        {
                auto value = is;
                auto const& ret = fill(value);

                BOOST_CHECK(full(value));                                       // [bitset.members]/11
                BOOST_CHECK_EQUAL(std::addressof(ret), std::addressof(value));  // [bitset.members]/12
        }
};

struct fn_insert
{
        template<class IntSet>
        auto operator()(IntSet const& is) const noexcept
        {
                auto value = is;
                                                                                // [bitset.members]/13
                BOOST_CHECK_THROW(insert(value, max_size(is)), std::out_of_range);
        }

        template<class IntSet, class SizeType>
        auto operator()(IntSet const& is, SizeType const pos) const
        {
                auto const src = is;
                auto dst = src;
                auto const& ret = insert(dst, pos);

                for (auto N = max_size(IntSet{}), i = decltype(N){0}; i < N; ++i) {
                                                                                // [bitset.members]/14
                        BOOST_CHECK_EQUAL(contains(dst, i), i == pos ? true : contains(src, i));
                }
                BOOST_CHECK_EQUAL(std::addressof(ret), std::addressof(dst));    // [bitset.members]/15

                IntSet singlet; insert(singlet, pos);
                BOOST_CHECK(dst == (src | singlet));
        }

        template<class IntSet, class SizeType>
        auto operator()(IntSet const& is, SizeType const pos, bool const val) const
        {
                auto const src = is;
                auto dst = src;
                auto const& ret = insert(dst, pos, val);

                for (auto N = max_size(IntSet{}), i = decltype(N){0}; i < N; ++i) {
                                                                                // [bitset.members]/14
                        BOOST_CHECK_EQUAL(contains(dst, i), i == pos ? val : contains(src, i));
                }
                BOOST_CHECK_EQUAL(std::addressof(ret), std::addressof(dst));    // [bitset.members]/15
        }
};

struct mem_insert
{
        template<class IntSet, class InputIterator>
        auto operator()(IntSet const& is [[maybe_unused]], InputIterator first [[maybe_unused]], InputIterator last [[maybe_unused]]) const
        {
                if constexpr (tti::has_range_insert_v<IntSet, InputIterator>) {
                        auto const src = is;
                        auto dst = src; dst.insert(first, last);
                        std::for_each(first, last, [&](auto const elem) {
                                BOOST_CHECK(dst.contains(elem));
                        });
                        BOOST_CHECK_LE(src.size(), dst.size());
                        BOOST_CHECK_LE(dst.size(), src.size() + static_cast<int>(std::distance(first, last)));
                }
        }

        template<class IntSet, class ValueType>
        constexpr auto operator()(IntSet const& is [[maybe_unused]], std::initializer_list<ValueType> ilist [[maybe_unused]]) const
        {
                if constexpr (tti::has_ilist_insert_v<IntSet, ValueType>) {
                        auto const src = is;
                        auto dst = src; dst.insert(ilist);
                        for (auto const elem : ilist) {
                                BOOST_CHECK(dst.contains(elem));
                        }
                        BOOST_CHECK_LE(src.size(), dst.size());
                        BOOST_CHECK_LE(dst.size(), src.size() + static_cast<int>(ilist.size()));
                }
        }
};

struct fn_clear
{
        template<class IntSet>
        auto operator()(IntSet const& is) const noexcept
        {
                auto value = is;
                auto const& ret = clear(value);

                BOOST_CHECK(empty(value));                                      // [bitset.members]/16
                BOOST_CHECK_EQUAL(std::addressof(ret), std::addressof(value));  // [bitset.members]/17
        }
};

struct fn_erase
{
        template<class IntSet>
        auto operator()(IntSet const& is) const noexcept
        {
                auto value = is;
                                                                                // [bitset.members]/18
                BOOST_CHECK_THROW(erase(value, max_size(is)), std::out_of_range);
        }

        template<class IntSet, class SizeType>
        auto operator()(IntSet const& is, SizeType const pos) const
        {
                auto const src = is;
                auto dst = src;
                auto const& ret = erase(dst, pos);

                for (auto N = max_size(IntSet{}), i = decltype(N){0}; i < N; ++i) {
                                                                                // [bitset.members]/19
                        BOOST_CHECK_EQUAL(contains(dst, i), i == pos ? false : contains(src, i));
                }
                BOOST_CHECK_EQUAL(std::addressof(ret), std::addressof(dst));    // [bitset.members]/20

                IntSet singlet; insert(singlet, pos);
                BOOST_CHECK(dst == src - singlet);
        }
};

struct mem_erase
{
        template<class IntSet, class InputIterator>
        auto operator()(IntSet const& is [[maybe_unused]], InputIterator first [[maybe_unused]], InputIterator last [[maybe_unused]]) const
        {
                if constexpr (tti::has_range_erase_v<IntSet, InputIterator>) {
                        auto const src = is;
                        auto dst = src; dst.erase(first, last);
                        std::for_each(first, last, [&](auto const elem) {
                                BOOST_CHECK(!dst.contains(elem));
                        });
                        BOOST_CHECK_LE(dst.size(), src.size());
                        BOOST_CHECK_LE(src.size(), dst.size() + static_cast<int>(std::distance(first, last)));
                }
        }

        template<class IntSet, class ValueType>
        constexpr auto operator()(IntSet const& is [[maybe_unused]], std::initializer_list<ValueType> ilist [[maybe_unused]]) const
        {
                if constexpr (tti::has_ilist_erase_v<IntSet, ValueType>) {
                        auto const src = is;
                        auto dst = src; dst.erase(ilist);
                        for (auto const elem : ilist) {
                                BOOST_CHECK(!dst.contains(elem));
                        }
                        BOOST_CHECK_LE(dst.size(), src.size());
                        BOOST_CHECK_LE(src.size(), dst.size() + static_cast<int>(ilist.size()));
                }
        }
};

struct fn_swap
{
        template<class IntSet>
        constexpr auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                auto src1 = a, src2 = b;
                using std::swap;
                swap(src1, src2);
                BOOST_CHECK(src1 == b && src2 == a);
        }
};

struct op_compl
{
        template<class IntSet>
        constexpr auto operator()(IntSet const& a) const noexcept
        {
                auto expected = a; complement(expected);
                auto const& ret = ~a;

                BOOST_CHECK_NE(std::addressof(ret), std::addressof(expected));  // [bitset.members]/21
                BOOST_CHECK(ret == expected);                                   // [bitset.members]/22
                BOOST_CHECK(~ret == a);                                         // involution
        }

        template<class IntSet>
        constexpr auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                BOOST_CHECK(~(a | b) == (~a & ~b));                             // De Morgan's Laws
                BOOST_CHECK(~(a & b) == (~a | ~b));                             // De Morgan's Laws
        }
};

struct fn_complement
{
        template<class IntSet>
        auto operator()(IntSet const& is) const noexcept
        {
                auto const src = is;
                auto dst = is;
                auto const& ret = complement(dst);

                for (auto N = max_size(IntSet{}), i = decltype(N){0}; i < N; ++i) {
                        BOOST_CHECK_NE(contains(dst, i), contains(src, i));     // [bitset.members]/23
                }
                BOOST_CHECK_EQUAL(std::addressof(ret), std::addressof(dst));    // [bitset.members]/24

                complement(dst);
                BOOST_CHECK(dst == src);                                        // involution
        }
};

struct fn_replace
{
        template<class IntSet>
        auto operator()(IntSet const& is) const noexcept
        {
                auto value = is;
                                                                                // [bitset.members]/25
                BOOST_CHECK_THROW(replace(value, max_size(is)), std::out_of_range);
        }

        template<class IntSet, class SizeType>
        auto operator()(IntSet const& is, SizeType const pos) const
        {
                auto const src = is;
                auto dst = is;
                auto const& ret = replace(dst, pos);
                for (auto N = max_size(IntSet{}), i = decltype(N){0}; i < N; ++i) {
                                                                                // [bitset.members]/26
                        BOOST_CHECK_EQUAL(contains(dst, i), i == pos ? !contains(src, i) : contains(src, i));
                }
                BOOST_CHECK_EQUAL(std::addressof(ret), std::addressof(dst));    // [bitset.members]/27

                IntSet singlet; insert(singlet, pos);
                BOOST_CHECK(dst == (src ^ singlet));
        }
};

// [bitset.members]/28-33 describe to_ulong, to_ullong, to_string

struct fn_size
{
        template<class IntSet>
        auto operator()(IntSet const& is) noexcept
        {
                auto expected = decltype(size(is)){0};
                for (auto N = max_size(IntSet{}), i = decltype(N){0}; i < N; ++i) {
                        expected += contains(is, i);
                }
                BOOST_CHECK_EQUAL(size(is), expected);                          // [bitset.members]/34

                if constexpr (tti::has_const_iterator_v<IntSet>) {
                        BOOST_CHECK_EQUAL(is.size(), std::distance(is.cbegin(), is.cend()));
                }
        }
};

struct fn_max_size
{
        template<class IntSet>
        auto operator()(IntSet const& is) const noexcept
        {
                BOOST_CHECK_EQUAL(max_size(is), max_size(IntSet{}));            // [bitset.members]/35

                if constexpr (tti::has_max_size_v<IntSet>) {
                        BOOST_CHECK_EQUAL(max_size(IntSet{}), is.max_size());
                }
        }
};

struct op_equal_to
{
        template<class IntSet>
        auto operator()(IntSet const& a) const noexcept
        {
                BOOST_CHECK(a == a);                                            // reflexive
        }

        template<class IntSet>
        auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                auto expected = true;
                for (auto N = max_size(IntSet{}), i = decltype(N){0}; i < N; ++i) {
                        expected &= contains(a, i) == contains(b, i);
                }
                BOOST_CHECK_EQUAL(a == b, expected);                            // [bitset.members]/36
                BOOST_CHECK_EQUAL(a == b, b == a);                              // symmetric

                if constexpr (tti::has_const_iterator_v<IntSet>) {
                        BOOST_CHECK_EQUAL(a == b, std::equal(a.cbegin(), a.cend(), b.cbegin(), b.cend()));
                }
        }

        template<class IntSet>
        auto operator()(IntSet const& a, IntSet const& b, IntSet const& c) const noexcept
        {
                auto const ab = a == b;
                auto const bc = b == c;
                auto const ac = a == c;
                BOOST_CHECK(!(ab && bc) || ac);                                 // transitive
        }
};

struct op_not_equal_to
{
        template<class IntSet>
        auto operator()(IntSet const& a) const noexcept
        {
                BOOST_CHECK(!(a != a));                                         // irreflexive
        }

        template<class IntSet>
        auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                BOOST_CHECK_EQUAL(a != b, !(a == b));                           // [bitset.members]/37
                BOOST_CHECK_EQUAL(a != b, b != a);                              // symmetric
        }
};

struct op_less
{
        template<class IntSet>
        auto operator()(IntSet const& a) const noexcept
        {
                BOOST_CHECK(!(a < a));                                          // irreflexive
        }

        template<class IntSet>
        auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                auto expected = false;
                for (auto N = max_size(IntSet{}), i = decltype(N){0}; i < N; ++i) {
                        auto const r = N - 1 - i;
                        if (!contains(a, r) && contains(b, r)) { expected = true; break; }
                        if (!contains(b, r) && contains(a, r)) {                  break; }
                }
                BOOST_CHECK_EQUAL(a < b, expected);

                if constexpr (tti::has_const_iterator_v<IntSet>) {
                        BOOST_CHECK_EQUAL(a < b, std::lexicographical_compare(a.crbegin(), a.crend(), b.crbegin(), b.crend()));
                }
        }

        template<class IntSet>
        auto operator()(IntSet const& a, IntSet const& b, IntSet const& c) const noexcept
        {
                auto const ab = a < b;
                auto const bc = b < c;
                auto const ac = a < c;
                BOOST_CHECK(!(ab && bc) || ac);                                 // transitive
        }
};

struct op_greater
{
        template<class IntSet>
        auto operator()(IntSet const& a) const noexcept
        {
                BOOST_CHECK(!(a > a));                                          // irreflexive
        }

        template<class IntSet>
        auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                BOOST_CHECK_EQUAL(a > b, b < a);
        }

        template<class IntSet>
        auto operator()(IntSet const& a, IntSet const& b, IntSet const& c) const noexcept
        {
                auto const ab = a > b;
                auto const bc = b > c;
                auto const ac = a > c;
                BOOST_CHECK(!(ab && bc) || ac);                                 // transitive
        }
};

struct op_greater_equal
{
        template<class IntSet>
        auto operator()(IntSet const& a) const noexcept
        {
                BOOST_CHECK(a >= a);                                            // reflexive
        }

        template<class IntSet>
        auto operator()(IntSet const& lhs, IntSet const& rhs) const noexcept
        {
                BOOST_CHECK_EQUAL(lhs >= rhs, !(lhs < rhs));
        }

        template<class IntSet>
        auto operator()(IntSet const& a, IntSet const& b, IntSet const& c) const noexcept
        {
                auto const ab = a >= b;
                auto const bc = b >= c;
                auto const ac = a >= c;
                BOOST_CHECK(!(ab && bc) || ac);                                 // transitive
        }
};

struct op_less_equal
{
        template<class IntSet>
        auto operator()(IntSet const& a) const noexcept
        {
                BOOST_CHECK(a <= a);                                            // reflexive
        }

        template<class IntSet>
        auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                BOOST_CHECK_EQUAL(a <= b, !(b < a));
        }

        template<class IntSet>
        auto operator()(IntSet const& a, IntSet const& b, IntSet const& c) const noexcept
        {
                auto const ab = a <= b;
                auto const bc = b <= c;
                auto const ac = a <= c;
                BOOST_CHECK(!(ab && bc) || ac);                                 // transitive
        }
};

struct fn_contains
{
        template<class IntSet>
        auto operator()(IntSet const& is) const noexcept
        {                                                                       // [bitset.members]/38
                BOOST_CHECK_THROW(contains(is, max_size(is)), std::out_of_range);
        }

        template<class IntSet, class SizeType>
        auto operator()(IntSet const& is, SizeType const pos) const noexcept
        {
                auto value = is; clear(value); insert(value, pos);
                for (auto N = max_size(IntSet{}), i = decltype(N){0}; i < N; ++i) {
                        BOOST_CHECK_EQUAL(contains(value, i), i == pos);        // [bitset.members]/39
                }
        }
};

struct fn_full
{
        template<class IntSet>
        auto operator()(IntSet const& is) const noexcept
        {
                BOOST_CHECK_EQUAL(full(is), size(is) == max_size(IntSet{}));    // [bitset.members]/40

                if constexpr (tti::has_full_v<IntSet>) {
                        BOOST_CHECK_EQUAL(full(is), is.full());
                }
        }
};

struct fn_not_empty
{
        template<class IntSet>
        auto operator()(IntSet const& is) const noexcept
        {
                BOOST_CHECK_EQUAL(not_empty(is), size(is) != 0);                // [bitset.members]/41

                if constexpr (tti::has_empty_v<IntSet>) {
                        BOOST_CHECK_EQUAL(not_empty(is), !is.empty());
                }
        }
};

struct fn_empty
{
        template<class IntSet>
        auto operator()(IntSet const& is) const noexcept
        {
                BOOST_CHECK_EQUAL(empty(is), size(is) == 0);                    // [bitset.members]/42

                if constexpr (tti::has_empty_v<IntSet>) {
                        BOOST_CHECK_EQUAL(empty(is), is.empty());
                }
        }
};

struct op_shift_left
{
        template<class IntSet, class SizeType>
        auto operator()(IntSet const& is, SizeType const n) const
        {
                auto expected = is; expected <<= n;
                BOOST_CHECK(is << n == expected);                               // [bitset.members]/43

                if constexpr (tti::has_const_iterator_v<IntSet>) {
                        auto const lhs = is << n;
                        std::set<int> tmp, rhs;
                        std::transform(is.cbegin(), is.cend(), std::inserter(tmp, tmp.end()), [=](auto const x) {
                                return x + n;
                        });
                        std::copy_if(tmp.cbegin(), tmp.cend(), std::inserter(rhs, rhs.end()), [&](auto const x) {
                                return x < is.max_size();
                        });
                        BOOST_CHECK_EQUAL_COLLECTIONS(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
                }
        }
};

struct op_shift_right
{
        template<class IntSet, class SizeType>
        auto operator()(IntSet const& is, SizeType const n) const
        {
                auto expected = is; expected >>= n;
                BOOST_CHECK(is >> n == expected);                               // [bitset.members]/44

                if constexpr (tti::has_const_iterator_v<IntSet>) {
                        auto const lhs = is >> n;
                        std::set<int> tmp, rhs;
                        std::transform(is.cbegin(), is.cend(), std::inserter(tmp, tmp.end()), [=](auto const x) {
                                return x - n;
                        });
                        std::copy_if(tmp.cbegin(), tmp.cend(), std::inserter(rhs, rhs.end()), [](auto const x) {
                                return 0 <= x;
                        });
                        BOOST_CHECK_EQUAL_COLLECTIONS(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
                }
        }
};

struct op_at
{
        template<class IntSet, class SizeType>
        auto operator()(IntSet const& is, SizeType const pos) const
        {
                BOOST_CHECK(0 <= pos && pos < max_size(is));                    // [bitset.members]/45
                BOOST_CHECK_EQUAL(at(is, pos), contains(is, pos));              // [bitset.members]/46
                                                                                // [bitset.members]/47: at(is, max_size(is)) does not throw
        }

        template<class IntSet, class SizeType>
        auto operator()(IntSet& is, SizeType const pos, bool const val) const
        {
                BOOST_CHECK(0 <= pos && pos < max_size(is));                    // [bitset.members]/48
                BOOST_CHECK_EQUAL(at(is, pos), contains(is, pos));              // [bitset.members]/49
                auto src = is; insert(src, pos, val);
                at(is, pos, val);
                BOOST_CHECK(is == src);                                         // [bitset.members]/49
                                                                                // [bitset.members]/50: at(is, max_size(is), val) does not throw
        }
};

struct mem_find
{
        template<class IntSet, class KeyType>
        auto operator()(IntSet& is, KeyType const& x) const
        {
                if constexpr (tti::has_find_v<IntSet, KeyType>) {
                        BOOST_CHECK(is.find(x) == std::find(is.cbegin(), is.cend(), x));
                }
        }

        template<class IntSet, class KeyType>
        auto operator()(IntSet const& is, KeyType const& x) const
        {
                if constexpr (tti::has_find_v<IntSet, KeyType>) {
                        BOOST_CHECK(is.find(x) == std::find(is.cbegin(), is.cend(), x));
                }
        }
};

struct mem_count
{
        template<class IntSet, class KeyType>
        auto operator()(IntSet const& is, KeyType const& x) const
        {
                if constexpr (tti::has_count_v<IntSet, KeyType>) {
                        BOOST_CHECK(is.count(x) == std::count(is.cbegin(), is.cend(), x));
                }
        }
};

// [bitset.hash]/1 describes std::hash specialization

struct op_bitand
{
        template<class IntSet>
        constexpr auto operator()(IntSet const& a) const noexcept
        {
                BOOST_CHECK((a & a) == a);                                      // idempotent
        }

        template<class IntSet>
        auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                auto expected = a; expected &= b;
                BOOST_CHECK((a & b) == expected);                               // [bitset.operators]/1
                BOOST_CHECK((a & b) == (b & a));                                // commutative
                BOOST_CHECK(is_subset_of(a & b, a));
                BOOST_CHECK(is_subset_of(a & b, b));
                BOOST_CHECK_LE(size(a & b), std::min(size(a), size(b)));

                if constexpr (tti::has_const_iterator_v<IntSet>) {
                        auto const lhs = a & b;
                        IntSet rhs;
                        std::set_intersection(a.cbegin(), a.cend(), b.cbegin(), b.cend(), std::inserter(rhs, rhs.end()));
                        BOOST_CHECK(lhs == rhs);
                }
        }

        template<class IntSet>
        constexpr auto operator()(IntSet const& a, IntSet const& b, IntSet const& c) const noexcept
        {
                BOOST_CHECK(((a & b) & c) == (a & (b & c)));                    // associative
                BOOST_CHECK((a & (b | c)) == ((a & b) | (a & c)));              // distributive
        }
};

struct op_bitor
{
        template<class IntSet>
        constexpr auto operator()(IntSet const& a) const noexcept
        {
                BOOST_CHECK((a | a) == a);                                      // idempotent
        }

        template<class IntSet>
        auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                auto expected = a; expected |= b;
                BOOST_CHECK((a | b) == expected);                               // [bitset.operators]/2
                BOOST_CHECK((a | b) == (b | a));                                // commutative
                BOOST_CHECK(is_subset_of(a, a | b));
                BOOST_CHECK(is_subset_of(b, a | b));
                BOOST_CHECK_EQUAL(size(a | b), size(a) + size(b) - size(a & b));

                if constexpr (tti::has_const_iterator_v<IntSet>) {
                        auto const lhs = a | b;
                        IntSet rhs;
                        std::set_union(a.cbegin(), a.cend(), b.cbegin(), b.cend(), std::inserter(rhs, rhs.end()));
                        BOOST_CHECK(lhs == rhs);
                }
        }

        template<class IntSet>
        constexpr auto operator()(IntSet const& a, IntSet const& b, IntSet const& c) const noexcept
        {
                BOOST_CHECK(((a | b) | c) == (a | (b | c)));                    // associative
                BOOST_CHECK((a | (b & c)) == ((a | b) & (a | c)));              // distributive
        }
};

struct op_xor
{
        template<class IntSet>
        constexpr auto operator()(IntSet const& a) const noexcept
        {
                BOOST_CHECK(empty(a ^ a));                                      // nilpotent
        }

        template<class IntSet>
        auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                auto expected = a; expected ^= b;
                BOOST_CHECK((a ^ b) == expected);                               // [bitset.operators]/3
                BOOST_CHECK((a ^ b) == (b ^ a));                                // commutative
                BOOST_CHECK((a ^ b) == ((a - b) | (b - a)));
                BOOST_CHECK((a ^ b) == (a | b) - (a & b));

                if constexpr (tti::has_const_iterator_v<IntSet>) {
                        auto const lhs = a ^ b;
                        IntSet rhs;
                        std::set_symmetric_difference(a.cbegin(), a.cend(), b.cbegin(), b.cend(), std::inserter(rhs, rhs.end()));
                        BOOST_CHECK(lhs == rhs);
                }
        }

        template<class IntSet>
        constexpr auto operator()(IntSet const& a, IntSet const& b, IntSet const& c) const noexcept
        {
                BOOST_CHECK(((a ^ b) ^ c) == (a ^ (b ^ c)));                    // associative
                BOOST_CHECK((a & (b ^ c)) == ((a & b) ^ (a & c)));              // distributive
        }
};

struct op_minus
{
        template<class IntSet>
        constexpr auto operator()(IntSet const& a) const noexcept
        {
                BOOST_CHECK(empty(a - a));                                      // nilpotent
        }

        template<class IntSet>
        auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                if constexpr (tti::has_op_minus_assign_v<IntSet>) {
                        auto expected = a; expected -= b;
                        BOOST_CHECK(a - b == expected);
                }

                BOOST_CHECK(a - b == (a & ~b));
                BOOST_CHECK(~(a - b) == (~a | b));
                BOOST_CHECK(~a - ~b == b - a);
                BOOST_CHECK(a - b == (a | b) - b);
                BOOST_CHECK(a - b == a - (a & b));
                BOOST_CHECK(is_subset_of(a - b, a));
                BOOST_CHECK(disjoint(a - b, b));

                if constexpr (tti::has_const_iterator_v<IntSet>) {
                        auto const lhs = a - b;
                        IntSet rhs;
                        std::set_difference(a.cbegin(), a.cend(), b.cbegin(), b.cend(), std::inserter(rhs, rhs.end()));
                        BOOST_CHECK(lhs == rhs);
                }
        }
};

struct fn_is_subset_of
{
        template<class IntSet>
        auto operator()(IntSet const& a) const noexcept
        {
                BOOST_CHECK(is_subset_of(a, a));                                // reflexive
        }

        template<class IntSet>
        auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                auto expected = true;
                for (auto N = max_size(IntSet{}), i = decltype(N){0}; i < N; ++i) {
                        expected &= !contains(a, i) || contains(b, i);
                }
                BOOST_CHECK_EQUAL(is_subset_of(a, b), expected);

                if constexpr (tti::has_const_iterator_v<IntSet>) {
                        BOOST_CHECK_EQUAL(is_subset_of(a, b), std::includes(a.cbegin(), a.cend(), b.cbegin(), b.cend()));
                }
        }

        template<class IntSet>
        auto operator()(IntSet const& a, IntSet const& b, IntSet const& c) const noexcept
        {
                auto const ab = is_subset_of(a, b);
                auto const bc = is_subset_of(b, c);
                auto const ac = is_subset_of(a, c);
                BOOST_CHECK(!(ab && bc) || ac);                                 // transitive
        }
};

struct fn_is_superset_of
{
        template<class IntSet>
        auto operator()(IntSet const& a) const noexcept
        {
                BOOST_CHECK(is_superset_of(a, a));                              // reflexive
        }

        template<class IntSet>
        auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                BOOST_CHECK_EQUAL(is_superset_of(a, b), is_subset_of(b, a));
        }

        template<class IntSet>
        auto operator()(IntSet const& a, IntSet const& b, IntSet const& c) const noexcept
        {
                auto const ab = is_superset_of(a, b);
                auto const bc = is_superset_of(b, c);
                auto const ac = is_superset_of(a, c);
                BOOST_CHECK(!(ab && bc) || ac);                                 // transitive
        }
};

struct fn_is_proper_subset_of
{
        template<class IntSet>
        auto operator()(IntSet const& a) const noexcept
        {
                BOOST_CHECK(!is_proper_subset_of(a, a));                        // irreflexive
        }

        template<class IntSet>
        auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                BOOST_CHECK_EQUAL(is_proper_subset_of(a, b), is_subset_of(a, b) && !is_subset_of(b, a));
        }

        template<class IntSet>
        auto operator()(IntSet const& a, IntSet const& b, IntSet const& c) const noexcept
        {
                auto const ab = is_proper_subset_of(a, b);
                auto const bc = is_proper_subset_of(b, c);
                auto const ac = is_proper_subset_of(a, c);
                BOOST_CHECK(!(ab && bc) || ac);                                 // transitive
        }
};

struct fn_is_proper_superset_of
{
        template<class IntSet>
        auto operator()(IntSet const& a) const noexcept
        {
                BOOST_CHECK(!is_proper_superset_of(a, a));                      // irreflexive
        }

        template<class IntSet>
        auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                BOOST_CHECK_EQUAL(is_proper_superset_of(a, b), is_proper_subset_of(b, a));
        }

        template<class IntSet>
        auto operator()(IntSet const& a, IntSet const& b, IntSet const& c) const noexcept
        {
                auto const ab = is_proper_superset_of(a, b);
                auto const bc = is_proper_superset_of(b, c);
                auto const ac = is_proper_superset_of(a, c);
                BOOST_CHECK(!(ab && bc) || ac);                                 // transitive
        }
};

struct fn_intersects
{
        template<class IntSet>
        constexpr auto operator()(IntSet const& a) const noexcept
        {
                BOOST_CHECK(intersects(a, a) || empty(a));
        }

        template<class IntSet>
        constexpr auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                BOOST_CHECK_EQUAL(intersects(a, b), not_empty(a & b));
                BOOST_CHECK_EQUAL(intersects(a, b), intersects(b, a));
        }
};

struct fn_disjoint
{
        template<class IntSet>
        constexpr auto operator()(IntSet const& a) const noexcept
        {
                BOOST_CHECK(!disjoint(a, a) || empty(a));
        }

        template<class IntSet>
        constexpr auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                BOOST_CHECK_EQUAL(disjoint(a, b), !intersects(a, b));
                BOOST_CHECK_EQUAL(disjoint(a, b), disjoint(b, a));
        }
};

// [bitset.operators]/4-8 describe operator>> and operator<<

}       // namespace xstd
