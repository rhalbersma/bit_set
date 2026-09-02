//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef TEST_SEQUENCE_ORDERING_HPP
#define TEST_SEQUENCE_ORDERING_HPP

#include <xstd/bits/ranges/array_view.hpp> // array_view
#include <test/bitset/factory.hpp>         // make_bitset
#include <boost/test/unit_test.hpp>        // BOOST_CHECK_EQUAL
#include <algorithm>                       // lexicographical_compare
#include <compare>                         // strong_ordering
#include <cstddef>                         // size_t
#include <vector>                          // vector

namespace test::sequence {

// What the sequence reading of a bitset must order like, checked against the
// standard container that defines the relation rather than against a restatement
// of it. The set counterpart is test/set/ordering.hpp, and this exists for
// the same reason plus one more: array_view reaches its ordering by two routes
// now -- a word at a time where the viewed type says where its blocks are, and
// element by element where it does not -- and both have to give this answer.
//
// The universe is tiny on purpose: every ordered pair is 4^universe comparisons.
template<class Bits>
auto ordering_agrees_with_vector_bool(std::size_t universe = 4) -> void
{
        auto const bound = 1UZ << universe;
        for (auto i = 0UZ; i < bound; ++i) {
                for (auto j = 0UZ; j < bound; ++j) {
                        auto x = test::bitset::make_bitset<Bits>(universe);
                        auto y = test::bitset::make_bitset<Bits>(universe);

                        // Written through the view, in the sequence vocabulary,
                        // which is the interface under test rather than the
                        // bitset's own.
                        // Named rather than written as xstd::array_view(x)[k]:
                        // class template argument deduction followed by [k]
                        // parses as an array declaration, not a subscript.
                        auto xw = xstd::array_view(x);
                        auto yw = xstd::array_view(y);
                        for (auto k = 0UZ; k < universe; ++k) {
                                xw[k] = (i >> k & 1UZ) != 0UZ;
                                yw[k] = (j >> k & 1UZ) != 0UZ;
                        }

                        auto const xv = xstd::array_view(x);
                        auto const yv = xstd::array_view(y);

                        // The reference holds the same bools at the same
                        // positions, over the whole width the view reports.
                        auto vx = std::vector<bool>(xv.size());
                        auto vy = std::vector<bool>(yv.size());
                        for (auto k = 0UZ; k < xv.size(); ++k) { vx[k] = static_cast<bool>(xv[k]); }
                        for (auto k = 0UZ; k < yv.size(); ++k) { vy[k] = static_cast<bool>(yv[k]); }

                        BOOST_CHECK_EQUAL(xv == yv, vx == vy);
                        BOOST_CHECK_EQUAL((xv <=> yv) < 0,
                                          std::lexicographical_compare(vx.begin(), vx.end(), vy.begin(), vy.end()));
                        BOOST_CHECK_EQUAL((xv <=> yv) > 0,
                                          std::lexicographical_compare(vy.begin(), vy.end(), vx.begin(), vx.end()));
                }
        }
}

} // namespace test::sequence

#endif // TEST_SEQUENCE_ORDERING_HPP
