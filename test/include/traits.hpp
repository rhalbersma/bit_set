#pragma once

//          Copyright Rein Halbersma 2014-2020.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <initializer_list>     // initializer_list
#include <type_traits>          // void_t
#include <utility>              // declval

namespace xstd::tti {

template<class IntSet, class = void>
inline constexpr auto has_back = false;

template<class IntSet>
inline constexpr auto has_back<IntSet, std::void_t<decltype(
        std::declval<IntSet>().back()
)>> = true;

template<class IntSet, class = void>
inline constexpr auto has_capacity = false;

template<class IntSet>
inline constexpr auto has_capacity<IntSet, std::void_t<decltype(
        std::declval<IntSet>().capacity()
)>> = true;

template<class IntSet, class = void>
inline constexpr auto has_forward_iterator = false;

template<class IntSet>
inline constexpr auto has_forward_iterator<IntSet, std::void_t<decltype(
        ++begin(std::declval<IntSet>()) == end(std::declval<IntSet>())
)>> = true;

template<class IntSet, class = void>
inline constexpr auto has_front = false;

template<class IntSet>
inline constexpr auto has_front<IntSet, std::void_t<decltype(
        std::declval<IntSet>().front()
)>> = true;

template<class IntSet, class = void>
inline constexpr auto has_hinted_insert = false;

template<class IntSet>
inline constexpr auto has_hinted_insert<IntSet, std::void_t<decltype(
        std::declval<IntSet>().insert(std::declval<typename IntSet::iterator>(), std::declval<typename IntSet::value_type>())
)>> = true;

template<class IntSet, class = void>
inline constexpr auto has_ilist_erase = false;

template<class IntSet>
inline constexpr auto has_ilist_erase<IntSet, std::void_t<decltype(
        std::declval<IntSet>().erase(std::declval<std::initializer_list<typename IntSet::key_type>>())
)>> = true;

template<class IntSet, class = void>
inline constexpr auto has_op_minus_assign = false;

template<class IntSet>
inline constexpr auto has_op_minus_assign<IntSet, std::void_t<decltype(
        std::declval<IntSet>() -= std::declval<IntSet>()
)>> = true;

template<class IntSet, class = void>
inline constexpr auto has_resize = false;

template<class IntSet>
inline constexpr auto has_resize<IntSet, std::void_t<decltype(
        std::declval<IntSet>().resize(std::declval<typename IntSet::size_type>(), std::declval<bool>())
)>> = true;

template<class Iterator, class = void>
inline constexpr auto is_dereferenceable = false;

template<class Iterator>
inline constexpr auto is_dereferenceable<Iterator, std::void_t<
        decltype(*std::declval<Iterator>())
>> = true;

template<class Iterator, class = void>
inline constexpr auto is_equality_comparable = false;

template<class Iterator>
inline constexpr auto is_equality_comparable<Iterator, std::void_t<
        decltype(std::declval<Iterator>() != std::declval<Iterator>())
>> = true;

}       // namespace xstd::tti
