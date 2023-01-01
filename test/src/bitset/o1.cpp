//          Copyright Rein Halbersma 2014-2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <adaptor/bit_set.hpp>          // xstd::bitset adaptor around xstd::bit_set
#include <adaptor/bitset.hpp>           // bitset
#include <adaptor/dynamic_bitset.hpp>   // dynamic_bitset
#include <bitset/exhaustive.hpp>        // all_cardinality_sets, all_singleton_sets, all_valid, any_value, empty_set, full_set
#include <bitset/primitives.hpp>        // mem_set, mem_reset, op_compl, mem_flip, mem_count, mem_size, mem_test, mem_all, mem_any, mem_none
                                        // mem_at, op_bitand, op_bitor, op_xor, op_minus, mem_is_subset_of, mem_intersects, fn_iostream
#include <boost/dynamic_bitset.hpp>     // dynamic_bitset
#include <boost/mpl/vector.hpp>         // vector
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE_TEMPLATE
#include <bitset>                       // bitset
#include <cstdint>                      // uint8_t, uint16_t, uint32_t, uint64_t

BOOST_AUTO_TEST_SUITE(Linear)

using namespace xstd;

using bitset_types = boost::mpl::vector
<       std::bitset<  0>
,       std::bitset<  1>
,       std::bitset< 64>
,       boost::dynamic_bitset<>
,       bitset< 0, uint8_t>
,       bitset< 1, uint8_t>
,       bitset< 8, uint8_t>
,       bitset< 9, uint8_t>
,       bitset<16, uint8_t>
,       bitset<17, uint8_t>
,       bitset<24, uint8_t>
,       bitset<24, uint16_t>
,       bitset<24, uint32_t>
#if defined(__GNUG__) || defined(_MSC_VER) && defined(WIN64)
,       bitset<24, uint64_t>
#endif
#if defined(__GNUG__)
,       bitset<24, __uint128_t>
#endif
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(BitSet, T, bitset_types)
{
        all_cardinality_sets<T>(mem_set());
        all_singleton_sets<T>(mem_set());
        any_value<T>([](auto pos) {
                empty_set<T>([&](auto& bs0) {
                        mem_set()(bs0, pos);
                });
                empty_set<T>([&](auto& bs0) {
                        mem_set()(bs0, pos, true);
                });
                full_set<T>([&](auto& bsN) {
                        mem_set()(bsN, pos, false);
                });
        });

        all_cardinality_sets<T>(mem_reset());
        all_singleton_sets<T>(mem_reset());
        any_value<T>([](auto pos) {
                full_set<T>([&](auto& bsN) {
                        mem_reset()(bsN, pos);
                });
        });

        all_cardinality_sets<T>(op_compl());
        all_singleton_sets<T>(op_compl());

        all_cardinality_sets<T>(mem_flip());
        all_singleton_sets<T>(mem_flip());
        any_value<T>([](auto pos) {
                empty_set<T>([&](auto& bs0) {
                        mem_flip()(bs0, pos);
                });
                full_set<T>([&](auto& bsN) {
                        mem_flip()(bsN, pos);
                });
       });

        all_cardinality_sets<T>(mem_count());

        all_cardinality_sets<T>(mem_size());
        all_singleton_sets<T>(mem_size());

        any_value<T>([](auto pos) {
                empty_set<T>([&](auto const& bs0) {
                        mem_test()(bs0, pos);
                });
                full_set<T>([&](auto const& bsN) {
                        mem_test()(bsN, pos);
                });
        });

        all_cardinality_sets<T>(mem_all());
        all_cardinality_sets<T>(mem_any());
        all_cardinality_sets<T>(mem_none());

        all_valid<T>([](auto pos) {
                empty_set<T>([&](auto const& bs0) {
                        op_at()(bs0, pos);
                });
                // empty_set<T>([&](auto& bs0) {
                //         op_at()(bs0, pos);
                // });
                // empty_set<T>([&](auto& bs0) {
                //         op_at()(bs0, pos, false);
                // });
                // empty_set<T>([&](auto& bs0) {
                //         op_at()(bs0, pos, true);
                // });
                full_set<T>([&](auto const& bsN) {
                        op_at()(bsN, pos);
                });
                // full_set<T>([&](auto& bsN) {
                //         op_at()(bsN, pos);
                // });
                // full_set<T>([&](auto& bsN) {
                //         op_at()(bsN, pos, false);
                // });
                // full_set<T>([&](auto& bsN) {
                //         op_at()(bsN, pos, true);
                // });
        });

        // gcc-13 bug pending: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=108214
        all_cardinality_sets<T>(fn_iostream());
        all_singleton_sets<T>(fn_iostream());
}

BOOST_AUTO_TEST_SUITE_END()
