//          Copyright Rein Halbersma 2014-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <set/exhaustive.hpp>                   // empty_set, full_set, all_values, all_cardinality_sets,
                                                // all_singleton_arrays, all_singleton_ilists, all_singleton_sets
#include <set/primitives.hpp>                   // constructor, mem_const_reference, mem_const_iterator, mem_front, mem_back, mem_empty, mem_size,
                                                // mem_max_size, mem_insert, mem_erase, mem_clear, op_equal, op_less, fn_iterator, fn_size, fn_empty
#include <xstd/int_set.hpp>                     // int_set
#include <boost/container/flat_set.hpp>         // flat_set
#include <boost/mpl/vector.hpp>                 // vector
#include <boost/test/test_case_template.hpp>    // BOOST_AUTO_TEST_CASE_TEMPLATE
#include <boost/test/unit_test.hpp>             // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END
#include <set>                                  // bitset

BOOST_AUTO_TEST_SUITE(Linear)

using namespace xstd;

using IntSetTypes = boost::mpl::vector
<       std::set<int>
,       boost::container::flat_set<int>
,       int_set<  0, uint32_t>
,       int_set<  1, uint32_t>
,       int_set<  2, uint32_t>
,       int_set< 32, uint32_t>
,       int_set< 33, uint32_t>
,       int_set< 34, uint32_t>
,       int_set< 64, uint32_t>
,       int_set< 65, uint32_t>
,       int_set< 66, uint32_t>
,       int_set< 96, uint32_t>
,       int_set<128>
#if defined(__GNUG__)
,       int_set<128, __uint128_t>
,       int_set<256, __uint128_t>
#endif
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(IntSet, T, IntSetTypes)
{
        all_singleton_arrays<T>([](auto const& a1) {
                constructor<T>{}(a1.begin(), a1.end());
        });
        all_singleton_ilists<T>([](auto ilist1) {
                constructor<T>{}(ilist1);
        });

        all_cardinality_sets<T>(mem_const_reference{});
        all_singleton_sets<T>(mem_const_reference{});

        all_cardinality_sets<T>([](auto& is) {
                mem_const_iterator{}(is);
        });
        all_cardinality_sets<T>([](auto const& is) {
                mem_const_iterator{}(is);
        });
        all_singleton_sets<T>([](auto& is) {
                mem_const_iterator{}(is);
        });
        all_singleton_sets<T>([](auto const& is) {
                mem_const_iterator{}(is);
        });

        all_cardinality_sets<T>(mem_front{});
        all_singleton_sets<T>(mem_front{});

        all_cardinality_sets<T>(mem_back{});
        all_singleton_sets<T>(mem_back{});

        all_cardinality_sets<T>(mem_empty{});
        all_cardinality_sets<T>(mem_size{});
        all_cardinality_sets<T>(mem_max_size{});

        all_values<T>([](auto const& t) {
                empty_set<T>([=](auto& is) {
                        mem_insert{}(is, t);
                });
        });
        all_values<T>([](auto const& t) {
                empty_set<T>([=](auto& is) {
                        mem_insert{}(is, is.end(), t);
                });
        });
        all_singleton_arrays<T>([](auto const& a1) {
                empty_set<T>([&](auto& is) {
                        mem_insert{}(is, a1.begin(), a1.end());
                });
        });
        all_singleton_ilists<T>([](auto ilist1) {
                empty_set<T>([&](auto& is) {
                        mem_insert{}(is, ilist1);
                });
        });

        full_set<T>([](auto& is) {
                all_values<T>([&](auto const& k) {
                        mem_erase{}(is, k);
                });
        });

        // boost::container::flat_set<int>::erase invalidates iterators
        if constexpr (!std::is_same_v<T, boost::container::flat_set<int>>) {
                full_set<T>([](auto& is) {
                        for (auto first = is.begin(), last = is.end(); first != last; /* update inside loop */) {
                                mem_erase{}(is, first++);
                        }
                });
                full_set<T>([](auto& is) {
                        mem_erase{}(is, is.begin(), is.end());
                });
        }

        all_cardinality_sets<T>(mem_clear{});
        all_singleton_sets<T>(mem_clear{});

        all_cardinality_sets<T>(op_equal_to{});
        all_singleton_sets<T>(op_equal_to{});

        all_cardinality_sets<T>(op_less{});
        all_singleton_sets<T>(op_less{});

        all_cardinality_sets<T>([](auto& is) {
                fn_iterator{}(is);
        });
        all_cardinality_sets<T>([](auto const& is) {
                fn_iterator{}(is);
        });

        all_cardinality_sets<T>(fn_size{});
        all_cardinality_sets<T>(fn_empty{});
}

BOOST_AUTO_TEST_SUITE_END()
