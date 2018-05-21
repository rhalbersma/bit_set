#pragma once

//          Copyright Rein Halbersma 2014-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <initializer_list>     // initializer_list
#include <type_traits>          // void_t
#include <utility>              // declval

namespace xstd {
namespace tti {

template<class IntSet, class = void>
constexpr static auto has_op_minus_assign_v = false;

template<class IntSet>
constexpr static auto has_op_minus_assign_v<IntSet, std::void_t<decltype(
        std::declval<IntSet>() -= std::declval<IntSet>()
)>> = true;

template<class IntSet, class = void>
constexpr static auto has_const_iterator_v = false;

template<class IntSet>
constexpr static auto has_const_iterator_v<IntSet, std::void_t<
        typename IntSet::const_iterator
>> = true;

template<class IntSet, class = void>
constexpr static auto has_front_v = false;

template<class IntSet>
constexpr static auto has_front_v<IntSet, std::void_t<decltype(
        std::declval<IntSet>().front()
)>> = true;

template<class IntSet, class = void>
constexpr static auto has_back_v = false;

template<class IntSet>
constexpr static auto has_back_v<IntSet, std::void_t<decltype(
        std::declval<IntSet>().back()
)>> = true;

template<class IntSet, class UnaryPredicate, class = void>
constexpr static auto has_any_of_v = false;

template<class IntSet, class UnaryPredicate>
constexpr static auto has_any_of_v<IntSet, UnaryPredicate, std::void_t<decltype(
        std::declval<IntSet>().any_of(std::declval<UnaryPredicate>())
)>> = true;

template<class IntSet, class UnaryPredicate, class = void>
constexpr static auto has_none_of_v = false;

template<class IntSet, class UnaryPredicate>
constexpr static auto has_none_of_v<IntSet, UnaryPredicate, std::void_t<decltype(
        std::declval<IntSet>().none_of(std::declval<UnaryPredicate>())
)>> = true;

template<class IntSet, class UnaryPredicate, class = void>
constexpr static auto has_all_of_v = false;

template<class IntSet, class UnaryPredicate>
constexpr static auto has_all_of_v<IntSet, UnaryPredicate, std::void_t<decltype(
        std::declval<IntSet>().all_of(std::declval<UnaryPredicate>())
)>> = true;

template<class IntSet, class T, class BinaryOperation, class = void>
constexpr static auto has_accumulate_v = false;

template<class IntSet, class T, class BinaryOperation>
constexpr static auto has_accumulate_v<IntSet, T, BinaryOperation, std::void_t<decltype(
        std::declval<IntSet>().accumulate(std::declval<T>(), std::declval<BinaryOperation>())
)>> = true;

template<class IntSet, class UnaryFunction, class = void>
constexpr static auto has_for_each_v = false;

template<class IntSet, class UnaryFunction>
constexpr static auto has_for_each_v<IntSet, UnaryFunction, std::void_t<decltype(
        std::declval<IntSet>().for_each(std::declval<UnaryFunction>())
)>> = true;

template<class IntSet, class UnaryFunction, class = void>
constexpr static auto has_reverse_for_each_v = false;

template<class IntSet, class UnaryFunction>
constexpr static auto has_reverse_for_each_v<IntSet, UnaryFunction, std::void_t<decltype(
        std::declval<IntSet>().reverse_for_each(std::declval<UnaryFunction>())
)>> = true;

template<class IntSet, class = void>
constexpr static auto has_static_max_size_v = false;

template<class IntSet>
constexpr static auto has_static_max_size_v<IntSet, std::void_t<decltype(
        IntSet::max_size()
)>> = true;

template<class IntSet, class = void>
constexpr static auto has_static_capacity_v = false;

template<class IntSet>
constexpr static auto has_static_capacity_v<IntSet, std::void_t<decltype(
        IntSet::capacity()
)>> = true;

template<class IntSet, class = void>
constexpr static auto has_full_v = false;

template<class IntSet>
constexpr static auto has_full_v<IntSet, std::void_t<decltype(
        std::declval<IntSet>().full()
)>> = true;

template<class IntSet, class = void>
constexpr static auto has_empty_v = false;

template<class IntSet>
constexpr static auto has_empty_v<IntSet, std::void_t<decltype(
        std::declval<IntSet>().empty()
)>> = true;

template<class IntSet, class InputIterator, class = void>
constexpr static auto has_range_assign_v = false;

template<class IntSet, class InputIterator>
constexpr static auto has_range_assign_v<IntSet, InputIterator, std::void_t<decltype(
        std::declval<IntSet>().assign(std::declval<InputIterator>(), std::declval<InputIterator>())
)>> = true;

template<class IntSet, class ValueType, class = void>
constexpr static auto has_ilist_assign_v = false;

template<class IntSet, class ValueType>
constexpr static auto has_ilist_assign_v<IntSet, ValueType, std::void_t<decltype(
        std::declval<IntSet>() = std::declval<std::initializer_list<ValueType>>()
)>> = true;

template<class IntSet, class InputIterator, class = void>
constexpr static auto has_range_insert_v = false;

template<class IntSet, class InputIterator>
constexpr static auto has_range_insert_v<IntSet, InputIterator, std::void_t<decltype(
        std::declval<IntSet>().insert(std::declval<InputIterator>(), std::declval<InputIterator>())
)>> = true;

template<class IntSet, class ValueType, class = void>
constexpr static auto has_ilist_insert_v = false;

template<class IntSet, class ValueType>
constexpr static auto has_ilist_insert_v<IntSet, ValueType, std::void_t<decltype(
        std::declval<IntSet>().insert(std::declval<std::initializer_list<ValueType>>())
)>> = true;

template<class IntSet, class InputIterator, class = void>
constexpr static auto has_range_erase_v = false;

template<class IntSet, class InputIterator>
constexpr static auto has_range_erase_v<IntSet, InputIterator, std::void_t<decltype(
        std::declval<IntSet>().erase(std::declval<InputIterator>(), std::declval<InputIterator>())
)>> = true;

template<class IntSet, class ValueType, class = void>
constexpr static auto has_ilist_erase_v = false;

template<class IntSet, class ValueType>
constexpr static auto has_ilist_erase_v<IntSet, ValueType, std::void_t<decltype(
        std::declval<IntSet>().erase(std::declval<std::initializer_list<ValueType>>())
)>> = true;

}       // namespace tti
}       // namespace xstd
