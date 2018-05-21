#pragma once

//          Copyright Rein Halbersma 2014-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <traits.hpp>                   // has_range_assign_v, has_ilist_assign_v, has_const_iterator_v, has_front_v, has_back_v,
                                        // has_any_of_v, has_none_of_v, has_all_of_v, has_accumulate_v, has_for_each_v, has_reverse_for_each_v,
                                        // has_op_minus_assign_v, has_range_insert_v, has_ilist_insert_v, has_range_erase_v, has_ilist_erase_v,
                                        // has_static_max_size_v, has_static_capacity_v, has_full_v, has_empty_v
#include <boost/test/unit_test.hpp>     // BOOST_CHECK, BOOST_CHECK_EQUAL, BOOST_CHECK_NE, BOOST_CHECK_THROW
#include <algorithm>                    // all_of, any_of, none_of, equal, for_each, includes, is_sorted, lexicographical_compare,
                                        // set_intersection, set_union, set_symmetric_difference, set_difference
#include <functional>                   // greater, plus
#include <initializer_list>             // initializer_list
#include <istream>                      // basic_istream
#include <iterator>                     // distance, inserter, next
#include <memory>                       // addressof
#include <numeric>                      // accumulate
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
                BOOST_CHECK(none(is));                                          // [bitset.cons]/1
        }

        template<class InputIterator>
        auto operator()(InputIterator first [[maybe_unused]], InputIterator last [[maybe_unused]]) const
        {
                if constexpr (std::is_constructible_v<IntSet, InputIterator, InputIterator>) {
                        auto const dst = IntSet(first, last);
                        std::for_each(first, last, [&](auto&& elem) {
                                BOOST_CHECK(dst.contains(elem));
                        });
                        BOOST_CHECK_EQUAL(dst.count(), static_cast<int>(std::distance(first, last)));
                }
        }

        template<class ValueType>
        constexpr auto operator()(std::initializer_list<ValueType> ilist [[maybe_unused]]) const
        {
                if constexpr (std::is_constructible_v<IntSet, std::initializer_list<ValueType>>) {
                        auto const dst = IntSet(ilist);
                        for (auto&& elem : ilist) {
                                BOOST_CHECK(dst.contains(elem));
                        }
                        BOOST_CHECK_EQUAL(dst.count(), static_cast<int>(ilist.size()));
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
                        std::for_each(first, last, [&](auto&& elem) {
                                BOOST_CHECK(dst.contains(elem));
                        });
                        BOOST_CHECK_EQUAL(dst.count(), static_cast<int>(std::distance(first, last)));
                }
        }

        template<class IntSet, class ValueType>
        constexpr auto operator()(IntSet const& is [[maybe_unused]], std::initializer_list<ValueType> ilist [[maybe_unused]]) const
        {
                if constexpr (tti::has_ilist_assign_v<IntSet, ValueType>) {
                        auto const src = is;
                        auto dst = src; dst = ilist;
                        for (auto&& elem : ilist) {
                                BOOST_CHECK(dst.contains(elem));
                        }
                        BOOST_CHECK_EQUAL(dst.count(), static_cast<int>(ilist.size()));
                }
        }
};

struct const_reference
{
        template<class IntSet>
        auto operator()(IntSet const& is [[maybe_unused]]) const noexcept
        {
                if constexpr (tti::has_const_iterator_v<IntSet>) {
                        for (auto first = is.begin(), last = is.end(); first != last; ++first) {
                                auto const ref = *first;

                                BOOST_CHECK(&ref == first);
                                BOOST_CHECK(is.contains(ref));
                        }

                        for (auto&& ref : is) {
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

                        BOOST_CHECK_EQUAL(std::distance( is.  begin(),  is.  end()), cis.count());
                        BOOST_CHECK_EQUAL(std::distance(cis.  begin(), cis.  end()), cis.count());
                        BOOST_CHECK_EQUAL(std::distance( is. rbegin(),  is. rend()), cis.count());
                        BOOST_CHECK_EQUAL(std::distance(cis. rbegin(), cis. rend()), cis.count());
                        BOOST_CHECK_EQUAL(std::distance( is. cbegin(),  is. cend()), cis.count());
                        BOOST_CHECK_EQUAL(std::distance( is.crbegin(),  is.crend()), cis.count());
                        BOOST_CHECK_EQUAL(std::distance(  begin( is),   end( is)), cis.count());
                        BOOST_CHECK_EQUAL(std::distance(  begin(cis),   end(cis)), cis.count());
                        BOOST_CHECK_EQUAL(std::distance( rbegin( is),  rend( is)), cis.count());
                        BOOST_CHECK_EQUAL(std::distance( rbegin(cis),  rend(cis)), cis.count());
                        BOOST_CHECK_EQUAL(std::distance( cbegin( is),  cend( is)), cis.count());
                        BOOST_CHECK_EQUAL(std::distance(crbegin( is), crend( is)), cis.count());

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
                        BOOST_CHECK(is.empty() || (is.front() == *is.begin()));
                        BOOST_CHECK(is.empty() || (&is.front() == is.begin()));
                }
        }
};

struct mem_back
{
        template<class IntSet>
        auto operator()(IntSet const& is [[maybe_unused]]) const noexcept
        {
                if constexpr (tti::has_back_v<IntSet> && tti::has_const_iterator_v<IntSet>) {
                        BOOST_CHECK(is.empty() || (is.back() == *is.rbegin()));
                        BOOST_CHECK(is.empty() || (&is.back() == std::next(is.rbegin()).base()));
                }
        }
};

struct mem_any_of
{
        template<class IntSet, class UnaryPredicate>
        auto operator()(IntSet const& is [[maybe_unused]], UnaryPredicate pred [[maybe_unused]]) const noexcept
        {
                if constexpr (tti::has_any_of_v<IntSet, UnaryPredicate> && tti::has_const_iterator_v<IntSet>) {
                        BOOST_CHECK_EQUAL(is.any_of(pred), std::any_of(is.begin(), is.end(), pred));
                }
        }
};

struct mem_none_of
{
        template<class IntSet, class UnaryPredicate>
        auto operator()(IntSet const& is [[maybe_unused]], UnaryPredicate pred [[maybe_unused]]) const noexcept
        {
                if constexpr (tti::has_none_of_v<IntSet, UnaryPredicate> && tti::has_const_iterator_v<IntSet>) {
                        BOOST_CHECK_EQUAL(is.none_of(pred), std::none_of(is.begin(), is.end(), pred));
                }
        }
};

struct mem_all_of
{
        template<class IntSet, class UnaryPredicate>
        auto operator()(IntSet const& is [[maybe_unused]], UnaryPredicate pred [[maybe_unused]]) const noexcept
        {
                if constexpr (tti::has_all_of_v<IntSet, UnaryPredicate> && tti::has_const_iterator_v<IntSet>) {
                        BOOST_CHECK_EQUAL(is.all_of(pred), std::all_of(is.begin(), is.end(), pred));
                }
        }
};

struct mem_accumulate
{
        template<class IntSet, class T, class BinaryOperation = std::plus<>>
        auto operator()(IntSet const& is [[maybe_unused]], T init [[maybe_unused]], BinaryOperation op [[maybe_unused]] = BinaryOperation{}) const noexcept
        {
                if constexpr (tti::has_accumulate_v<IntSet, T, BinaryOperation> && tti::has_const_iterator_v<IntSet>) {
                        BOOST_CHECK_EQUAL(is.accumulate(init, op), std::accumulate(is.begin(), is.end(), init, op));
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
                        BOOST_CHECK(is.for_each(detail::tracer{}) == std::for_each(is.begin(), is.end(), detail::tracer{}));

                        {
                                detail::tracer fun;
                                for (auto first = is.begin(), last = is.end(); first != last; first++ /* post-increment to hit code coverage */) {
                                        fun(*first);
                                }
                                BOOST_CHECK(is.for_each(detail::tracer{}) == std::move(fun));
                        }

                        {
                                detail::tracer fun;
                                for (auto&& elem : is) {
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
                        BOOST_CHECK(is.reverse_for_each(detail::tracer{}) == std::for_each(is.rbegin(), is.rend(), detail::tracer{}));

                        {
                                detail::tracer fun;
                                for (auto first = is.end(), last = is.begin(); first != last; first-- /* post-decrement to hit code coverage */) {
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

                for (auto N = size(IntSet{}), i = decltype(N){0}; i < N; ++i) { // [bitset.members]/1
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

                for (auto N = size(IntSet{}), i = decltype(N){0}; i < N; ++i) { // [bitset.members]/3
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

                for (auto N = size(IntSet{}), i = decltype(N){0}; i < N; ++i) { // [bitset.members]/5
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

                        for (auto N = size(IntSet{}), i = decltype(N){0}; i < N; ++i) {
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

                for (auto N = size(is), I = decltype(N){0}; I < N; ++I) {
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

                for (auto N = size(is), I = decltype(N){0}; I < N; ++I) {
                        if (pos >= N - I) {
                                BOOST_CHECK(!contains(dst, I));                 // [bitset.members]/9.1
                        } else {                                                // [bitset.members]/9.2
                                BOOST_CHECK_EQUAL(contains(dst, I), contains(src, I + pos));
                        }
                }
                BOOST_CHECK_EQUAL(std::addressof(ret), std::addressof(dst));    // [bitset.members]/10
        }
};

struct fn_set
{
        template<class IntSet>
        auto operator()(IntSet const& is) const noexcept
        {
                auto value = is;
                auto const& ret = set(value);

                BOOST_CHECK(all(value));                                        // [bitset.members]/11
                BOOST_CHECK_EQUAL(std::addressof(ret), std::addressof(value));  // [bitset.members]/12
                BOOST_CHECK_THROW(set(value, size(is)), std::out_of_range);     // [bitset.members]/13
        }

        template<class IntSet, class SizeType>
        auto operator()(IntSet const& is, SizeType const pos) const
        {
                auto const src = is;
                auto dst = src;
                auto const& ret = set(dst, pos);

                for (auto N = size(is), i = decltype(N){0}; i < N; ++i) {       // [bitset.members]/14
                        BOOST_CHECK_EQUAL(contains(dst, i), i == pos ? true : contains(src, i));
                }
                BOOST_CHECK_EQUAL(std::addressof(ret), std::addressof(dst));    // [bitset.members]/15
        }

        template<class IntSet, class SizeType>
        auto operator()(IntSet const& is, SizeType const pos, bool const val) const
        {
                auto const src = is;
                auto dst = src;
                auto const& ret = set(dst, pos, val);

                for (auto N = size(is), i = decltype(N){0}; i < N; ++i) {       // [bitset.members]/14
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
                        std::for_each(first, last, [&](auto&& elem) {
                                BOOST_CHECK(dst.contains(elem));
                        });
                        BOOST_CHECK_LE(src.count(), dst.count());
                        BOOST_CHECK_LE(dst.count(), src.count() + static_cast<int>(std::distance(first, last)));
                }
        }

        template<class IntSet, class ValueType>
        constexpr auto operator()(IntSet const& is [[maybe_unused]], std::initializer_list<ValueType> ilist [[maybe_unused]]) const
        {
                if constexpr (tti::has_ilist_insert_v<IntSet, ValueType>) {
                        auto const src = is;
                        auto dst = src; dst.insert(ilist);
                        for (auto&& elem : ilist) {
                                BOOST_CHECK(dst.contains(elem));
                        }
                        BOOST_CHECK_LE(src.count(), dst.count());
                        BOOST_CHECK_LE(dst.count(), src.count() + static_cast<int>(ilist.size()));
                }
        }
};

struct fn_reset
{
        template<class IntSet>
        auto operator()(IntSet const& is) const noexcept
        {
                auto value = is;
                auto const& ret = reset(value);

                BOOST_CHECK(none(value));                                       // [bitset.members]/16
                BOOST_CHECK_EQUAL(std::addressof(ret), std::addressof(value));  // [bitset.members]/17
                BOOST_CHECK_THROW(reset(value, size(is)), std::out_of_range);   // [bitset.members]/18
        }

        template<class IntSet, class SizeType>
        auto operator()(IntSet const& is, SizeType const pos) const
        {
                auto const src = is;
                auto dst = src;
                auto const& ret = reset(dst, pos);

                for (auto N = size(is), i = decltype(N){0}; i < N; ++i) {       // [bitset.members]/19
                        BOOST_CHECK_EQUAL(contains(dst, i), i == pos ? false : contains(src, i));
                }
                BOOST_CHECK_EQUAL(std::addressof(ret), std::addressof(dst));    // [bitset.members]/20
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
                        std::for_each(first, last, [&](auto&& elem) {
                                BOOST_CHECK(!dst.contains(elem));
                        });
                        BOOST_CHECK_LE(dst.count(), src.count());
                        BOOST_CHECK_LE(src.count(), dst.count() + static_cast<int>(std::distance(first, last)));
                }
        }

        template<class IntSet, class ValueType>
        constexpr auto operator()(IntSet const& is [[maybe_unused]], std::initializer_list<ValueType> ilist [[maybe_unused]]) const
        {
                if constexpr (tti::has_ilist_erase_v<IntSet, ValueType>) {
                        auto const src = is;
                        auto dst = src; dst.erase(ilist);
                        for (auto&& elem : ilist) {
                                BOOST_CHECK(!dst.contains(elem));
                        }
                        BOOST_CHECK_LE(dst.count(), src.count());
                        BOOST_CHECK_LE(src.count(), dst.count() + static_cast<int>(ilist.size()));
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
                auto expected = a; flip(expected);
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

struct fn_flip
{
        template<class IntSet>
        auto operator()(IntSet const& is) const noexcept
        {
                auto const src = is;
                auto dst = is;
                auto const& ret = flip(dst);

                for (auto N = size(is), i = decltype(N){0}; i < N; ++i) {
                        BOOST_CHECK_NE(contains(dst, i), contains(src, i));     // [bitset.members]/23
                }
                BOOST_CHECK_EQUAL(std::addressof(ret), std::addressof(dst));    // [bitset.members]/24
                BOOST_CHECK_THROW(flip(dst, size(is)), std::out_of_range);      // [bitset.members]/25

                flip(dst);
                BOOST_CHECK(dst == src);                                        // involution
        }

        template<class IntSet, class SizeType>
        auto operator()(IntSet const& is, SizeType const pos) const
        {
                auto const src = is;
                auto dst = is;
                auto const& ret = flip(dst, pos);
                for (auto N = size(is), i = decltype(N){0}; i < N; ++i) {       // [bitset.members]/26
                        BOOST_CHECK_EQUAL(contains(dst, i), i == pos ? !contains(src, i) : contains(src, i));
                }
                BOOST_CHECK_EQUAL(std::addressof(ret), std::addressof(dst));    // [bitset.members]/27

                flip(dst, pos);
                BOOST_CHECK(dst == src);                                        // involution
        }
};

struct mem_count
{
        template<class IntSet>
        auto operator()(IntSet const& is) noexcept
        {
                auto expected = decltype(is.count()){0};
                for (auto N = size(is), i = decltype(N){0}; i < N; ++i) {
                        expected += contains(is, i);
                }
                BOOST_CHECK_EQUAL(is.count(), expected);                        // [bitset.members]/34

                if constexpr (tti::has_const_iterator_v<IntSet>) {
                        BOOST_CHECK_EQUAL(is.count(), std::distance(is.begin(), is.end()));
                }
        }
};

struct fn_size
{
        template<class IntSet>
        auto operator()(IntSet const& is) const noexcept
        {
                BOOST_CHECK_EQUAL(size(is), size(IntSet{}));                    // [bitset.members]/35

                if constexpr (tti::has_static_max_size_v<IntSet>) {
                        BOOST_CHECK_EQUAL(size(is), is.max_size());
                }
        }
};

template<class IntSet>
struct mem_capacity
{
        auto operator()() const noexcept
        {
                if constexpr (tti::has_static_capacity_v<IntSet>) {
                        using block_t = typename IntSet::block_type;
                        constexpr auto block_size = std::numeric_limits<block_t>::digits;

                        BOOST_CHECK_EQUAL(IntSet::capacity() % block_size, 0);
                        BOOST_CHECK_EQUAL(IntSet::capacity() / block_size, (IntSet::max_size() - 1 + block_size) / block_size);
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
                for (auto N = size(IntSet{}), i = decltype(N){0}; i < N; ++i) {
                        expected &= contains(a, i) == contains(b, i);
                }
                BOOST_CHECK_EQUAL(a == b, expected);                            // [bitset.members]/36
                BOOST_CHECK_EQUAL(a == b, b == a);                              // symmetric

                if constexpr (tti::has_const_iterator_v<IntSet>) {
                        BOOST_CHECK_EQUAL(a == b, std::equal(a.begin(), a.end(), b.begin(), b.end()));
                }
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
                for (auto N = size(IntSet{}), i = decltype(N){0}; i < N; ++i) {
                        auto const r = N - 1 - i;
                        if (!contains(a, r) && contains(b, r)) { expected = true; break; }
                        if (!contains(b, r) && contains(a, r)) {                  break; }
                }
                BOOST_CHECK_EQUAL(a < b, expected);

                if constexpr (tti::has_const_iterator_v<IntSet>) {
                        BOOST_CHECK_EQUAL(a < b, std::lexicographical_compare(a.rbegin(), a.rend(), b.rbegin(), b.rend()));
                }
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
                for (auto N = size(IntSet{}), i = decltype(N){0}; i < N; ++i) {
                        expected &= !contains(a, i) || contains(b, i);
                }
                BOOST_CHECK_EQUAL(is_subset_of(a, b), expected);

                if constexpr (tti::has_const_iterator_v<IntSet>) {
                        BOOST_CHECK_EQUAL(is_subset_of(a, b), std::includes(a.begin(), a.end(), b.begin(), b.end()));
                }
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
};

struct fn_intersects
{
        template<class IntSet>
        constexpr auto operator()(IntSet const& a) const noexcept
        {
                BOOST_CHECK(intersects(a, a) || none(a));
        }

        template<class IntSet>
        constexpr auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                BOOST_CHECK_EQUAL(intersects(a, b), any(a & b));
                BOOST_CHECK_EQUAL(intersects(a, b), intersects(b, a));
        }
};

struct fn_disjoint
{
        template<class IntSet>
        constexpr auto operator()(IntSet const& a) const noexcept
        {
                BOOST_CHECK(!disjoint(a, a) || none(a));
        }

        template<class IntSet>
        constexpr auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                BOOST_CHECK_EQUAL(disjoint(a, b), !intersects(a, b));
                BOOST_CHECK_EQUAL(disjoint(a, b), disjoint(b, a));
        }
};

struct fn_test
{
        template<class IntSet>
        auto operator()(IntSet const& is) const noexcept
        {
                BOOST_CHECK_THROW(test(is, size(is)), std::out_of_range);       // [bitset.members]/38
        }

        template<class IntSet, class SizeType>
        auto operator()(IntSet const& is, SizeType const pos) const noexcept
        {
                auto value = is; reset(value); set(value, pos);
                for (auto N = size(is), i = decltype(N){0}; i < N; ++i) {
                        BOOST_CHECK_EQUAL(i == pos, test(value, i));            // [bitset.members]/39
                }
        }
};

struct fn_all
{
        template<class IntSet>
        auto operator()(IntSet const& is) const noexcept
        {
                BOOST_CHECK_EQUAL(all(is), is.count() == size(is));             // [bitset.members]/40

                if constexpr (tti::has_full_v<IntSet>) {
                        BOOST_CHECK_EQUAL(all(is), is.full());
                }
        }
};

struct fn_any
{
        template<class IntSet>
        auto operator()(IntSet const& is) const noexcept
        {
                BOOST_CHECK_EQUAL(any(is), is.count() != 0);                    // [bitset.members]/41

                if constexpr (tti::has_empty_v<IntSet>) {
                        BOOST_CHECK_EQUAL(any(is), !is.empty());
                }
        }
};

struct fn_none
{
        template<class IntSet>
        auto operator()(IntSet const& is) const noexcept
        {
                BOOST_CHECK_EQUAL(none(is), is.count() == 0);                   // [bitset.members]/42

                if constexpr (tti::has_empty_v<IntSet>) {
                        BOOST_CHECK_EQUAL(none(is), is.empty());
                        BOOST_CHECK_EQUAL(none(is), empty(is));
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
        }
};

struct op_shift_right
{
        template<class IntSet, class SizeType>
        auto operator()(IntSet const& is, SizeType const n) const
        {
                auto expected = is; expected >>= n;
                BOOST_CHECK(is >> n == expected);                               // [bitset.members]/44
        }
};

struct op_at
{
        template<class IntSet, class SizeType>
        auto operator()(IntSet const& is, SizeType const pos) const
        {
                BOOST_CHECK(0 <= pos && pos < size(is));                        // [bitset.members]/45
                BOOST_CHECK_EQUAL(contains(is, pos), test(is, pos));            // [bitset.members]/46
        }
};

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

                if constexpr (tti::has_const_iterator_v<IntSet>) {
                        auto const lhs = a & b;
                        IntSet rhs;
                        std::set_intersection(a.begin(), a.end(), b.begin(), b.end(), std::inserter(rhs, rhs.begin()));
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

                if constexpr (tti::has_const_iterator_v<IntSet>) {
                        auto const lhs = a | b;
                        IntSet rhs;
                        std::set_union(a.begin(), a.end(), b.begin(), b.end(), std::inserter(rhs, rhs.begin()));
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
                BOOST_CHECK(none(a ^ a));                                       // nilpotent
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
                        std::set_symmetric_difference(a.begin(), a.end(), b.begin(), b.end(), std::inserter(rhs, rhs.begin()));
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
                BOOST_CHECK(none(a - a));                                       // nilpotent
        }

        template<class IntSet>
        auto operator()(IntSet const& a, IntSet const& b) const noexcept
        {
                if constexpr (tti::has_op_minus_assign_v<IntSet>) {
                        auto expected = a; expected -= b;
                        BOOST_CHECK(a - b == expected);
                }

                BOOST_CHECK(a - b == (a & ~b));
                BOOST_CHECK(a - b == (a | b) - b);
                BOOST_CHECK(a - b == a - (a & b));
                BOOST_CHECK(is_subset_of(a - b, a));
                BOOST_CHECK(disjoint(a - b, b));

                if constexpr (tti::has_const_iterator_v<IntSet>) {
                        auto const lhs = a - b;
                        IntSet rhs;
                        std::set_difference(a.begin(), a.end(), b.begin(), b.end(), std::inserter(rhs, rhs.begin()));
                        BOOST_CHECK(lhs == rhs);
                }
        }
};

}       // namespace xstd
