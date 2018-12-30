//          Copyright Rein Halbersma 2014-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/bit_set.hpp>                     // ctznz, clznz, popcount, bsfnz, bsrnz, ctz, clz, bsf, bsr, bit1
#include <boost/mpl/vector.hpp>                 // vector
#include <boost/test/test_case_template.hpp>    // BOOST_AUTO_TEST_CASE_TEMPLATE
#include <boost/test/unit_test.hpp>             // BOOST_AUTO_TEST_SUITE, BOOST_CHECK, BOOST_CHECK_EQUAL, BOOST_AUTO_TEST_SUITE_END
#include <limits>                               // digits

BOOST_AUTO_TEST_SUITE(Builtin)

#if defined(__GNUG__)

using unsigned_integer_types = boost::mpl::vector
<       uint8_t
,       uint16_t
,       uint32_t
,       uint64_t
,       __uint128_t
>;

#elif defined(_MSC_VER)

using unsigned_integer_types = boost::mpl::vector
<       uint8_t
,       uint16_t
,       uint32_t
#if defined(WIN64)
,       uint64_t
#endif
>;

#endif

using namespace xstd::builtin;

template<class T> constexpr auto zero = static_cast<T>( 0);
template<class T> constexpr auto ones = static_cast<T>(-1);

template<class T>
constexpr auto bit1(int const n)
{
        return static_cast<T>(static_cast<T>(1) << n);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(CountTrailingZerosNonZero, T, unsigned_integer_types)
{
        BOOST_CHECK_EQUAL(ctznz(ones<T>), 0);

        for (auto i = 0; i < std::numeric_limits<T>::digits; ++i) {
                auto const b = bit1<T>(i);
                BOOST_CHECK_EQUAL(ctznz(b), i);
        }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(CountLeadingZerosNonZero, T, unsigned_integer_types)
{
        BOOST_CHECK_EQUAL(clznz(ones<T>), 0);

        for (auto i = 0; i < std::numeric_limits<T>::digits; ++i) {
                auto const b = bit1<T>(i);
                BOOST_CHECK_EQUAL(clznz(b), std::numeric_limits<T>::digits - 1 - i);
        }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(Popcount, T, unsigned_integer_types)
{
        BOOST_CHECK_EQUAL(popcount(zero<T>), 0);
        BOOST_CHECK_EQUAL(popcount(ones<T>), std::numeric_limits<T>::digits);

        for (auto i = 0; i < std::numeric_limits<T>::digits; ++i) {
                auto const b = static_cast<T>(~(ones<T> << i));
                BOOST_CHECK_EQUAL(popcount(b), i);
        }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(BitScanForwardNonZero, T, unsigned_integer_types)
{
        BOOST_CHECK_EQUAL(bsfnz(ones<T>), 0);

        for (auto i = 0; i < std::numeric_limits<T>::digits; ++i) {
                auto const b = bit1<T>(i);
                BOOST_CHECK_EQUAL(bsfnz(b), i);
        }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(BitScanReverseNonZero, T, unsigned_integer_types)
{
        BOOST_CHECK_EQUAL(bsrnz(ones<T>), std::numeric_limits<T>::digits - 1);

        for (auto i = 0; i < std::numeric_limits<T>::digits; ++i) {
                auto const b = bit1<T>(i);
                BOOST_CHECK_EQUAL(bsrnz(b), i);
        }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(CountTrailingZeros, T, unsigned_integer_types)
{
        BOOST_CHECK_EQUAL(ctz(zero<T>), std::numeric_limits<T>::digits);
        BOOST_CHECK_EQUAL(ctz(ones<T>), 0);

        for (auto i = 0; i < std::numeric_limits<T>::digits; ++i) {
                auto const b = bit1<T>(i);
                BOOST_CHECK_EQUAL(ctz(b), i);
        }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(CountLeadingZeros, T, unsigned_integer_types)
{
        BOOST_CHECK_EQUAL(clz(zero<T>), std::numeric_limits<T>::digits);
        BOOST_CHECK_EQUAL(clz(ones<T>), 0);

        for (auto i = 0; i < std::numeric_limits<T>::digits; ++i) {
                auto const b = bit1<T>(i);
                BOOST_CHECK_EQUAL(clz(b), std::numeric_limits<T>::digits - 1 - i);
        }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(BitScanForward, T, unsigned_integer_types)
{
        BOOST_CHECK_EQUAL(bsf(zero<T>), std::numeric_limits<T>::digits);
        BOOST_CHECK_EQUAL(bsf(ones<T>), 0);

        for (auto i = 0; i < std::numeric_limits<T>::digits; ++i) {
                auto const b = bit1<T>(i);
                BOOST_CHECK_EQUAL(bsf(b), i);
        }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(BitScanReverse, T, unsigned_integer_types)
{
        BOOST_CHECK_EQUAL(bsr(zero<T>), -1);
        BOOST_CHECK_EQUAL(bsr(ones<T>), std::numeric_limits<T>::digits - 1);

        for (auto i = 0; i < std::numeric_limits<T>::digits; ++i) {
                auto const b = bit1<T>(i);
                BOOST_CHECK_EQUAL(bsr(b), i);
        }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(Bit1, T, unsigned_integer_types)
{
        for (auto i = 0; i < std::numeric_limits<T>::digits; ++i) {
                auto const b = bit1<T>(i);
                BOOST_CHECK_EQUAL(popcount(b), 1);

                // __uint128_t does not have output streaming operator<< required for BOOST_CHECK_EQUAL
                BOOST_CHECK(b == static_cast<T>(1) << i);
        }
}

BOOST_AUTO_TEST_SUITE_END()
