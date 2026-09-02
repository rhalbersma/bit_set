//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef TEST_SET_ORDERING_HPP
#define TEST_SET_ORDERING_HPP

#include <xstd/bits/ranges/set_view.hpp>  // set_view
#include <test/bitset/factory.hpp>        // make_bitset
#include <boost/test/unit_test.hpp>       // BOOST_CHECK_EQUAL
#include <algorithm>                      // lexicographical_compare
#include <compare>                        // strong_ordering
#include <cstddef>                        // size_t
#include <set>                            // set

namespace test::set {

// What the set reading of a bitset must order like, checked against the
// standard container that defines the relation rather than against a
// restatement of it.
//
// This exists because the default of xstd::ranges::set_compare is to trust the
// viewed type's own <=>, and a type that has one is not thereby a type whose one
// is right: boost::dynamic_bitset's disagrees with the ordering of the same keys
// on a third of all pairs. Nothing but remembering to write a specialization
// stands between that default and a silently wrong ordering, so the guard is
// here, applied per viewed type, rather than left to the memory of whoever adds
// the next adaptor.
//
// The universe is tiny on purpose: every ordered pair of subsets is 4^universe
// comparisons, and the relation is settled by the short prefixes.
template<class Bits>
auto ordering_agrees_with_std_set(std::size_t universe = 4) -> void
{
        auto const bound = 1UZ << universe;
        for (auto i = 0UZ; i < bound; ++i) {
                for (auto j = 0UZ; j < bound; ++j) {
                        auto x = test::bitset::make_bitset<Bits>(universe);
                        auto y = test::bitset::make_bitset<Bits>(universe);
                        auto kx = std::set<std::size_t>();
                        auto ky = std::set<std::size_t>();

                        // Written through the view, in the set vocabulary, which is
                        // the interface under test rather than the bitset's own.
                        for (auto k = 0UZ; k < universe; ++k) {
                                if (i >> k & 1UZ) { xstd::set_view(x).insert(k); kx.insert(k); }
                                if (j >> k & 1UZ) { xstd::set_view(y).insert(k); ky.insert(k); }
                        }

                        auto const xv = xstd::set_view(x);
                        auto const yv = xstd::set_view(y);

                        BOOST_CHECK_EQUAL(xv == yv, kx == ky);
                        BOOST_CHECK_EQUAL((xv <=> yv) < 0,
                                          std::lexicographical_compare(kx.begin(), kx.end(), ky.begin(), ky.end()));
                        BOOST_CHECK_EQUAL((xv <=> yv) > 0,
                                          std::lexicographical_compare(ky.begin(), ky.end(), kx.begin(), kx.end()));
                }
        }
}

} // namespace test::set

#endif // TEST_SET_ORDERING_HPP
