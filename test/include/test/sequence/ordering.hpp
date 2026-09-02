//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef TEST_SEQUENCE_ORDERING_HPP
#define TEST_SEQUENCE_ORDERING_HPP

#include <boost/test/unit_test.hpp>        // BOOST_CHECK_EQUAL
#include <test/bitset/factory.hpp>         // make_bitset
#include <xstd/bits/ranges/array_view.hpp> // array_view
#include <algorithm>                       // lexicographical_compare
#include <compare>                         // strong_ordering
#include <cstddef>                         // size_t
#include <vector>                          // vector

namespace test::sequence {

// What the sequence reading must order like, against the container defining the relation; both of array_view's routes must agree.
template<class Bits>
auto ordering_agrees_with_vector_bool(std::size_t universe = 4) -> void
{
        auto const bound = 1UZ << universe;
        for (auto i = 0UZ; i < bound; ++i) {
                for (auto j = 0UZ; j < bound; ++j) {
                        auto x = test::bitset::make_bitset<Bits>(universe);
                        auto y = test::bitset::make_bitset<Bits>(universe);

                        // Written through the view; named rather than inlined, because CTAD followed by [k] parses as an array declaration.
                        auto xw = xstd::array_view(x);
                        auto yw = xstd::array_view(y);
                        for (auto k = 0UZ; k < universe; ++k) {
                                xw[k] = (i >> k & 1UZ) != 0UZ;
                                yw[k] = (j >> k & 1UZ) != 0UZ;
                        }

                        auto const xv = xstd::array_view(x);
                        auto const yv = xstd::array_view(y);

                        // The reference holds the same bools at the same positions, over the whole width the view reports.
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
