//          Copyright Rein Halbersma 2014-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/bit_set.hpp>             // bit_set
#include <boost/container/flat_set.hpp> // flat_set
#include <boost/mp11/list.hpp>          // mp_list
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE_TEMPLATE, BOOST_CHECK_EQUAL_COLLECTIONS
#include <algorithm>                    // copy
#include <concepts>                     // integral
#include <cstddef>                      // size_t
#include <cstdint>                      // uint8_t, uint16_t, uint32_t, uint64_t
#include <iterator>                     // inserter
#include <set>                          // set

BOOST_AUTO_TEST_SUITE(Implicit)

using namespace xstd;

using Key = int;
using Types = boost::mp11::mp_list
<       std::set<Key>
,       boost::container::flat_set<Key>
,       bit_set<Key,  32, uint16_t>
,       bit_set<Key,  33, uint16_t>
,       bit_set<Key,  48, uint16_t>
,       bit_set<Key,  32, uint32_t>
,       bit_set<Key,  33, uint32_t>
,       bit_set<Key,  64, uint32_t>
,       bit_set<Key,  65, uint32_t>
,       bit_set<Key,  96, uint32_t>
,       bit_set<Key,  64, uint64_t>
,       bit_set<Key,  65, uint64_t>
,       bit_set<Key, 128, uint64_t>
,       bit_set<Key, 129, uint64_t>
,       bit_set<Key, 192, uint64_t>
#if defined(__GNUG__)
,       bit_set<Key, 128, __uint128_t>
,       bit_set<Key, 129, __uint128_t>
,       bit_set<Key, 256, __uint128_t>
,       bit_set<Key, 257, __uint128_t>
,       bit_set<Key, 384, __uint128_t>
#endif
>;

template<std::integral Key>
class Implicit
{
        Key m_key;

public:
        [[nodiscard]] constexpr explicit(false) Implicit(Key k)      noexcept : m_key(k) {}
        [[nodiscard]] constexpr explicit(false) operator Key() const noexcept { return m_key; }
};

BOOST_AUTO_TEST_CASE_TEMPLATE(ImplicitConstructible, T, Types)
{
        auto src = T({ 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31 });
        std::set<Implicit<Key>> dst;
        std::ranges::copy(src, std::inserter(dst, dst.end()));
        //BOOST_CHECK_EQUAL_COLLECTIONS(src.begin(), src.end(), dst.begin(), dst.end());
}

BOOST_AUTO_TEST_SUITE_END()
