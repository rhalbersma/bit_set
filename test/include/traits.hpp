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
constexpr static auto has_forward_iterator_v = false;

template<class IntSet>
constexpr static auto has_forward_iterator_v<IntSet, std::void_t<decltype(
        ++begin(std::declval<IntSet>()) == end(std::declval<IntSet>())
)>> = true;

template<class IntSet, class = void>
constexpr static auto has_hinted_insert_v = false;

template<class IntSet>
constexpr static auto has_hinted_insert_v<IntSet, std::void_t<decltype(
        std::declval<IntSet>().insert(std::declval<typename IntSet::iterator>(), std::declval<typename IntSet::value_type>())
)>> = true;

template<class IntSet, class = void>
constexpr static auto has_ilist_erase_v = false;

template<class IntSet>
constexpr static auto has_ilist_erase_v<IntSet, std::void_t<decltype(
        std::declval<IntSet>().erase(std::declval<std::initializer_list<typename IntSet::key_type>>())
)>> = true;

template<class IntSet, class = void>
constexpr static auto has_op_minus_assign_v = false;

template<class IntSet>
constexpr static auto has_op_minus_assign_v<IntSet, std::void_t<decltype(
        std::declval<IntSet>() -= std::declval<IntSet>()
)>> = true;

template<class IntSet, class = void>
constexpr static auto has_resize_v = false;

template<class IntSet>
constexpr static auto has_resize_v<IntSet, std::void_t<decltype(
        std::declval<IntSet>().resize(std::declval<typename IntSet::size_type>(), std::declval<bool>())
)>> = true;

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

}       // namespace tti
}       // namespace xstd
