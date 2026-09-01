//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/bits/ranges.hpp>          // array_view, set_view and their proxies
#include <xstd/bits/ext/std/bitset.hpp>  // the hooks that make std::bitset a bit range
#include <boost/test/unit_test.hpp>      // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE
#include <bitset>                        // bitset
#include <concepts>                      // same_as
#include <cstddef>                       // size_t
#include <type_traits>                   // is_convertible_v

BOOST_AUTO_TEST_SUITE(Bits)
BOOST_AUTO_TEST_SUITE(Ranges)

namespace {

using Bits = std::bitset<64>;

using SetIt   = xstd::ranges::set_iterator<Bits>;
using SetRef  = xstd::ranges::set_reference<Bits>;
using ArrIt   = xstd::ranges::array_iterator<Bits, false>;
using ArrRef  = xstd::ranges::array_reference<Bits, false>;

// Dependent, so a type without the member is a substitution failure rather than
// a hard error: asking `r.operator&()` of a concrete type that lacks it does not
// compile at all, requires-expression or no.
template<class R>
constexpr bool has_address_of = requires(R r) { r.operator&(); };

}       // namespace

// The two views are one shape asked twice, and this is the shape: dereferencing
// an iterator gives a proxy reference, taking the address of a proxy reference
// gives an iterator back, and the value -- a key for the set view, a bool for
// the sequence view -- is reached only by converting the reference.
//
// The standard says nothing about this shape, and its implementations disagree
// about all of it. Measured on the same four questions:
//
//                                              libstdc++   libc++
//   vector<bool>::const_reference is bool         yes        no
//   vector<bool>::reference has operator&         no         yes
//   bitset::reference has operator&               no         yes
//   bitset's const operator[] returns bool        yes        no
//
// libc++ already builds its bit references exactly this way, proxies on both
// paths and an operator& that hands an iterator back; libstdc++ does neither.
// So this is not a novel shape, it is libc++'s -- and because it is a choice
// rather than a rule, none of those four is asserted here. A test that pinned
// them would be pinning whichever standard library it happened to run against,
// which is what the first version of this file did, and Apple Clang said so.
// What is asserted below is what our own views guarantee on every toolchain.
BOOST_AUTO_TEST_CASE(DereferencingYieldsAProxyRatherThanTheValue)
{
        static_assert(std::same_as<decltype(*std::declval<SetIt const&>()), SetRef>);
        static_assert(std::same_as<decltype(*std::declval<ArrIt const&>()), ArrRef>);

        static_assert(not std::same_as<decltype(*std::declval<SetIt const&>()), std::size_t>);
        static_assert(not std::same_as<decltype(*std::declval<ArrIt const&>()), bool>);
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(AddressOfAProxyYieldsAnIterator)
{
        static_assert(std::same_as<decltype(&std::declval<SetRef const&>()), SetIt>);
        static_assert(std::same_as<decltype(&std::declval<ArrRef const&>()), ArrIt>);

        static_assert(has_address_of<SetRef>);
        static_assert(has_address_of<ArrRef>);

        BOOST_CHECK(true);
}

// The const path is a proxy too, and is the same proxy as the mutable one minus
// the assignment -- not a plain bool, which is what libstdc++ hands back and
// libc++ does not.
BOOST_AUTO_TEST_CASE(TheConstPathIsAProxyAsWell)
{
        using ConstArrRef = xstd::ranges::array_reference<Bits, true>;

        static_assert(std::is_convertible_v<ConstArrRef, bool>);
        static_assert(has_address_of<ConstArrRef>);
        static_assert(std::same_as<decltype(&std::declval<ConstArrRef const&>()),
                                   xstd::ranges::array_iterator<Bits, true>>);

        // and it is exactly the assignment that the const one drops.
        static_assert(    std::is_assignable_v<ArrRef const&, bool>);
        static_assert(not std::is_assignable_v<ConstArrRef const&, bool>);

        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(TheValueArrivesByImplicitConversion)
{
        static_assert(std::is_convertible_v<SetRef, std::size_t>);
        static_assert(std::is_convertible_v<ArrRef, bool>);

        auto b = Bits();
        auto const v = xstd::set_view(b);
        v.insert({3, 5, 7});

        // No cast at any of these.
        std::size_t const key = *v.begin();
        BOOST_CHECK_EQUAL(key, 3UZ);
        BOOST_CHECK(*v.begin() == 3UZ);

        auto const a = xstd::array_view(b);
        bool const bit = a[3];
        BOOST_CHECK(bit);
        BOOST_CHECK(a[5] == true);
        BOOST_CHECK(a[4] == false);
}

// & . * on an iterator and * . & on a reference are both the identity, which is
// what makes the pair a round trip rather than two one-way conversions.
BOOST_AUTO_TEST_CASE(TheProxyPairRoundTrips)
{
        auto b = Bits();
        auto const v = xstd::set_view(b);
        v.insert({3, 5, 7, 11});

        for (auto it = v.begin(); it != v.end(); ++it) {
                BOOST_CHECK(&*it == it);
                BOOST_CHECK(*&*it == *it);
        }

        auto const a = xstd::array_view(b);
        for (auto it = a.begin(); it != a.end(); ++it) {
                BOOST_CHECK(&*it == it);
                BOOST_CHECK(static_cast<bool>(*&*it) == static_cast<bool>(*it));
        }

        // and writing goes through the reference the round trip hands back.
        *&a.begin()[4] = true;
        BOOST_CHECK(b.test(4));
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
