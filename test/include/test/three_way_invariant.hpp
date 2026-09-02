//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef TEST_THREE_WAY_INVARIANT_HPP
#define TEST_THREE_WAY_INVARIANT_HPP

#include <algorithm>                // lexicographical_compare_three_way
#include <compare>                  // strong_ordering
#include <concepts>                 // same_as
#include <ranges>                   // begin, end, input_range, range_value_t

namespace test {

// The invariant the block-streaming path has to keep: a type's operator<=> means
// the same as comparing its own iterator range lexicographically. Whenever the
// blocks can be reached the comparison streams them a word at a time and never
// iterates at all, so this is the statement that the optimization is invisible.
//
// It is checked against the type's own iteration rather than against a std::set
// or std::vector<bool> holding the same content, which the ordering tests
// elsewhere do. Those say the ordering is the right relation; this says the fast
// path computes the same relation as the slow one, with no separately built
// reference in between that could be wrong in the same direction.
//
// It holds trivially where there is no fast path, and is applied there anyway:
// what makes it an invariant is that it is not conditional on which path a type
// happens to take today.
template<std::ranges::input_range C>
[[nodiscard]] constexpr std::strong_ordering three_way_by_iteration(C const& x, C const& y) noexcept
{
        using value_type = std::ranges::range_value_t<C>;
        return std::lexicographical_compare_three_way(
                std::ranges::begin(x), std::ranges::end(x),
                std::ranges::begin(y), std::ranges::end(y),
                [](auto const& a, auto const& b) static noexcept {
                        // Through value_type, because dereferencing yields a proxy
                        // reference and the comparison is meant to be of the values
                        // it stands for. bool goes via int: the built-in <=> is
                        // clearer about int than about bool.
                        if constexpr (std::same_as<value_type, bool>) {
                                return static_cast<int>(static_cast<value_type>(a)) <=> static_cast<int>(static_cast<value_type>(b));
                        } else {
                                return static_cast<value_type>(a) <=> static_cast<value_type>(b);
                        }
                }
        );
}

} // namespace test

#endif // TEST_THREE_WAY_INVARIANT_HPP
