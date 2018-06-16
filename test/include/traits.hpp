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

template<class IntSet, class = void>
constexpr static auto has_resize_v = false;

template<class IntSet>
constexpr static auto has_resize_v<IntSet, std::void_t<decltype(
        std::declval<IntSet>().resize(std::declval<typename IntSet::size_type>(), std::declval<bool>())
)>> = true;

template<class IntSet, class = void>
constexpr static auto has_empty_v = false;

template<class IntSet>
constexpr static auto has_empty_v<IntSet, std::void_t<decltype(
        std::declval<IntSet>().empty()
)>> = true;

template<class IntSet, class = void>
constexpr static auto has_full_v = false;

template<class IntSet>
constexpr static auto has_full_v<IntSet, std::void_t<decltype(
        std::declval<IntSet>().full()
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
constexpr static auto has_iterator_erase_v = false;

template<class IntSet, class InputIterator>
constexpr static auto has_iterator_erase_v<IntSet, InputIterator, std::void_t<decltype(
        std::declval<IntSet>().erase(std::declval<InputIterator>())
)>> = true;

template<class IntSet, class InputIterator, class = void>
constexpr static auto has_range_erase_v = false;

template<class IntSet, class InputIterator>
constexpr static auto has_range_erase_v<IntSet, InputIterator, std::void_t<decltype(
        std::declval<IntSet>().erase(std::declval<InputIterator>(), std::declval<InputIterator>())
)>> = true;

template<class IntSet, class KeyType, class = void>
constexpr static auto has_find_v = false;

template<class IntSet, class KeyType>
constexpr static auto has_find_v<IntSet, KeyType, std::void_t<decltype(
        std::declval<IntSet>().find(std::declval<KeyType>())
)>> = true;

template<class IntSet, class KeyType, class = void>
constexpr static auto has_count_v = false;

template<class IntSet, class KeyType>
constexpr static auto has_count_v<IntSet, KeyType, std::void_t<decltype(
        std::declval<IntSet>().count(std::declval<KeyType>())
)>> = true;

template<class IntSet, class KeyType, class = void>
constexpr static auto has_lower_bound_v = false;

template<class IntSet, class KeyType>
constexpr static auto has_lower_bound_v<IntSet, KeyType, std::void_t<decltype(
        std::declval<IntSet>().lower_bound(std::declval<KeyType>())
)>> = true;

template<class IntSet, class KeyType, class = void>
constexpr static auto has_upper_bound_v = false;

template<class IntSet, class KeyType>
constexpr static auto has_upper_bound_v<IntSet, KeyType, std::void_t<decltype(
        std::declval<IntSet>().upper_bound(std::declval<KeyType>())
)>> = true;

template<class IntSet, class KeyType, class = void>
constexpr static auto has_equal_range_v = false;

template<class IntSet, class KeyType>
constexpr static auto has_equal_range_v<IntSet, KeyType, std::void_t<decltype(
        std::declval<IntSet>().equal_range(std::declval<KeyType>())
)>> = true;

template<class IntSet, class = void>
constexpr static auto has_op_minus_assign_v = false;

template<class IntSet>
constexpr static auto has_op_minus_assign_v<IntSet, std::void_t<decltype(
        std::declval<IntSet>() -= std::declval<IntSet>()
)>> = true;

}       // namespace tti
}       // namespace xstd
