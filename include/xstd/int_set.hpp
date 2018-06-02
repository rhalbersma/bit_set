#pragma once

//          Copyright Rein Halbersma 2014-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>            // all_of, copy, copy_backward, copy_n, equal, fill_n, lexicographical_compare, max, swap_ranges
#include <array>                // array
#include <cassert>              // assert
#include <cstddef>              // size_t
#include <cstdint>              // uint64_t
#include <functional>           // less, not_fn
#include <initializer_list>     // initializer_list
#include <iterator>             // bidirectional_iterator_tag, begin, end, next, prev, rbegin, rend, reverse_iterator
#include <limits>               // digits
#include <numeric>              // accumulate
#include <tuple>                // tie
#include <type_traits>          // is_integral_v, is_nothrow_swappable_v, is_unsigned_v
#include <utility>              // move, swap

// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/p0202r3.html
#define XSTD_PP_CONSTEXPR_ALGORITHM     /* constexpr */

// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0879r0.html
#define XSTD_PP_CONSTEXPR_SWAP          /* constexpr */

// no WG21 proposal yet
#define XSTD_PP_CONSTEXPR_NUMERIC       /* constexpr */

#if defined(__GNUG__)

#define XSTD_PP_CONSTEXPR_INTRINSIC     constexpr

#elif defined(_MSC_VER)

#define XSTD_PP_CONSTEXPR_INTRINSIC     /* constexpr */

#endif

namespace xstd {
namespace detail {
namespace builtin {

#if defined(__GNUG__)

template<int N>
constexpr auto get(__uint128_t x) noexcept
{
        static_assert(0 <= N); static_assert(N < 2);
        return static_cast<uint64_t>(x >> (64 * N));
}

struct ctznz
{
        XSTD_PP_CONSTEXPR_INTRINSIC auto operator()(unsigned x) const // Throws: Nothing.
        {
                assert(x != 0);
                return __builtin_ctz(x);
        }

        XSTD_PP_CONSTEXPR_INTRINSIC auto operator()(unsigned long x) const // Throws: Nothing.
        {
                assert(x != 0);
                return __builtin_ctzl(x);
        }

        XSTD_PP_CONSTEXPR_INTRINSIC auto operator()(unsigned long long x) const // Throws: Nothing.
        {
                assert(x != 0);
                return __builtin_ctzll(x);
        }

        XSTD_PP_CONSTEXPR_INTRINSIC auto operator()(__uint128_t x) const // Throws: Nothing.
        {
                assert(x != 0);
                return get<0>(x) != 0 ? ctznz{}(get<0>(x)) : ctznz{}(get<1>(x)) + 64;
        }
};

using bsfnz = ctznz;

struct clznz
{
        XSTD_PP_CONSTEXPR_INTRINSIC auto operator()(unsigned x) const // Throws: Nothing.
        {
                assert(x != 0);
                return __builtin_clz(x);
        }

        XSTD_PP_CONSTEXPR_INTRINSIC auto operator()(unsigned long x) const // Throws: Nothing.
        {
                assert(x != 0);
                return __builtin_clzl(x);
        }

        XSTD_PP_CONSTEXPR_INTRINSIC auto operator()(unsigned long long x) const // Throws: Nothing.
        {
                assert(x != 0);
                return __builtin_clzll(x);
        }

        XSTD_PP_CONSTEXPR_INTRINSIC auto operator()(__uint128_t x) const // Throws: Nothing.
        {
                assert(x != 0);
                return get<1>(x) != 0 ? clznz{}(get<1>(x)) : clznz{}(get<0>(x)) + 64;
        }
};

struct popcount
{
        XSTD_PP_CONSTEXPR_INTRINSIC auto operator()(unsigned x) const noexcept
        {
                return __builtin_popcount(x);
        }

        XSTD_PP_CONSTEXPR_INTRINSIC auto operator()(unsigned long x) const noexcept
        {
                return __builtin_popcountl(x);
        }

        XSTD_PP_CONSTEXPR_INTRINSIC auto operator()(unsigned long long x) const noexcept
        {
                return __builtin_popcountll(x);
        }

        XSTD_PP_CONSTEXPR_INTRINSIC auto operator()(__uint128_t x) const noexcept
        {
                return popcount{}(get<0>(x)) + popcount{}(get<1>(x));
        }
};

#elif defined(_MSC_VER)

#include <intrin.h>

#pragma intrinsic(_BitScanForward)
#pragma intrinsic(_BitScanReverse)
#pragma intrinsic(__popcnt)

#if defined(_WIN64)

#pragma intrinsic(_BitScanForward64)
#pragma intrinsic(_BitScanReverse64)
#pragma intrinsic(__popcnt64)

#endif

struct bsfnz
{
        XSTD_PP_CONSTEXPR_INTRINSIC auto operator()(uint32_t x) const noexcept
        {
                assert(x != 0);
                unsigned long index;
                _BitScanForward(&index, static_cast<unsigned long>(x));
                return static_cast<int>(index);
        }

#if defined(_WIN64)

        XSTD_PP_CONSTEXPR_INTRINSIC auto operator()(uint64_t x) const noexcept
        {
                assert(x != 0);
                unsigned long index;
                _BitScanForward64(&index, x);
                return static_cast<int>(index);
        }

#endif

};

using ctznz = bsfnz;

struct bsrnz
{
        XSTD_PP_CONSTEXPR_INTRINSIC auto operator()(uint32_t x) const noexcept
        {
                assert(x != 0);
                unsigned long index;
                _BitScanReverse(&index, static_cast<unsigned long>(x));
                return static_cast<int>(index);
        }

#if defined(_WIN64)

        XSTD_PP_CONSTEXPR_INTRINSIC auto operator()(uint64_t x) const noexcept
        {
                assert(x != 0);
                unsigned long index;
                _BitScanReverse64(&index, x);
                return static_cast<int>(index);
        }

#endif

};

struct popcount
{
        XSTD_PP_CONSTEXPR_INTRINSIC auto operator()(uint32_t x) const noexcept
        {
                return static_cast<int>(__popcnt(static_cast<unsigned>(x)));
        }

#if defined(_WIN64)

        XSTD_PP_CONSTEXPR_INTRINSIC auto operator()(uint64_t x) const noexcept
        {
                return static_cast<int>(__popcnt64(x));
        }

#endif

};

#endif

}       // namespace builtin

template<class UIntType>
XSTD_PP_CONSTEXPR_INTRINSIC auto ctznz(UIntType x) // Throws: Nothing.
{
        assert(x != 0);
        return builtin::ctznz{}(x);
}

template<class UIntType>
XSTD_PP_CONSTEXPR_INTRINSIC auto bsfnz(UIntType x) // Throws: Nothing.
{
        assert(x != 0);
        return builtin::bsfnz{}(x);
}

template<class UIntType>
XSTD_PP_CONSTEXPR_INTRINSIC auto ctz(UIntType x) noexcept
{
        return x ? ctznz(x) : std::numeric_limits<UIntType>::digits;
}

template<class UIntType>
XSTD_PP_CONSTEXPR_INTRINSIC auto bsf(UIntType x) noexcept
{
        return x ? bsfnz(x) : std::numeric_limits<UIntType>::digits;
}

#if defined(__GNUG__)

template<class UIntType>
XSTD_PP_CONSTEXPR_INTRINSIC auto clznz(UIntType x) // Throws: Nothing.
{
        assert(x != 0);
        return builtin::clznz{}(x);
}

template<class UIntType>
XSTD_PP_CONSTEXPR_INTRINSIC auto bsrnz(UIntType x) // Throws: Nothing.
{
        assert(x != 0);
        return std::numeric_limits<UIntType>::digits - 1 - builtin::clznz{}(x);
}

#elif defined(_MSC_VER)

template<class UIntType>
XSTD_PP_CONSTEXPR_INTRINSIC auto clznz(UIntType x) // Throws: Nothing.
{
        assert(x != 0);
        return std::numeric_limits<UIntType>::digits - 1 - builtin::bsrnz{}(x);
}

template<class UIntType>
XSTD_PP_CONSTEXPR_INTRINSIC auto bsrnz(UIntType x) // Throws: Nothing.
{
        assert(x != 0);
        return builtin::bsrnz{}(x);
}

#endif

template<class UIntType>
XSTD_PP_CONSTEXPR_INTRINSIC auto clz(UIntType x) noexcept
{
        return x ? clznz(x) : std::numeric_limits<UIntType>::digits;
}

template<class UIntType>
XSTD_PP_CONSTEXPR_INTRINSIC auto bsr(UIntType x) noexcept
{
        return x ? bsrnz(x) : -1;
}

template<class UIntType>
XSTD_PP_CONSTEXPR_INTRINSIC auto popcount(UIntType x) noexcept
{
        return builtin::popcount{}(x);
}

}       // namespace detail

template<int /* N */, class /* UIntType */>
class int_set;

template<int N, class UIntType> XSTD_PP_CONSTEXPR_ALGORITHM auto operator==  (int_set<N, UIntType> const& /* lhs */, int_set<N, UIntType> const& /* rhs */) noexcept;
template<int N, class UIntType> XSTD_PP_CONSTEXPR_ALGORITHM auto operator<   (int_set<N, UIntType> const& /* lhs */, int_set<N, UIntType> const& /* rhs */) noexcept;
template<int N, class UIntType> XSTD_PP_CONSTEXPR_ALGORITHM bool is_subset_of(int_set<N, UIntType> const& /* lhs */, int_set<N, UIntType> const& /* rhs */) noexcept;
template<int N, class UIntType> XSTD_PP_CONSTEXPR_ALGORITHM bool intersects  (int_set<N, UIntType> const& /* lhs */, int_set<N, UIntType> const& /* rhs */) noexcept;

template<int N, class UIntType = std::size_t>
class int_set
{
        static_assert(0 <= N);
        static_assert(std::is_unsigned_v<UIntType>);
        static_assert(std::is_integral_v<UIntType>);
        static_assert(sizeof(unsigned) <= sizeof(UIntType));

        #if defined(_MSC_VER)

        static_assert(sizeof(UIntType) <= sizeof(std::size_t));

        #endif

        constexpr static auto block_size = std::numeric_limits<UIntType>::digits;
        constexpr static auto num_blocks = (N - 1 + block_size) / block_size;
        constexpr static auto num_bits = num_blocks * block_size;
        constexpr static auto excess_bits = num_bits - N;

        class proxy_reference;
        class proxy_iterator;

        UIntType m_data[std::max(num_blocks, 1)]{};    // zero-initializated by default
public:
        using key_type               = int;
        using key_compare            = std::less<key_type>;
        using value_type             = int;
        using value_compare          = std::less<value_type>;
        using pointer                = proxy_iterator;
        using const_pointer          = proxy_iterator;
        using reference              = proxy_reference;
        using const_reference        = proxy_reference;
        using size_type              = int;
        using difference_type        = int;
        using iterator               = proxy_iterator;
        using const_iterator         = proxy_iterator;
        using reverse_iterator       = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;
        using block_type             = UIntType;
        using insert_return_type     = void;

        int_set() = default;

        template<class InputIterator>
        constexpr int_set(InputIterator first, InputIterator last) // Throws: Nothing.
        {
                insert(first, last);
        }

        constexpr int_set(std::initializer_list<value_type> ilist) // Throws: Nothing.
        :
                int_set(ilist.begin(), ilist.end())
        {}

        template<class InputIterator>
        auto assign(InputIterator first, InputIterator last) // Throws: Nothing.
        {
                clear();
                insert(first, last);
        }

        auto& operator=(std::initializer_list<value_type> ilist) // Throws: Nothing.
        {
                assign(ilist.begin(), ilist.end());
                return *this;
        }

        XSTD_PP_CONSTEXPR_INTRINSIC auto begin()         noexcept { return       iterator{data(), find_first()}; }
        XSTD_PP_CONSTEXPR_INTRINSIC auto begin()   const noexcept { return const_iterator{data(), find_first()}; }
        constexpr                   auto end()           noexcept { return       iterator{data(), num_bits}; }
        constexpr                   auto end()     const noexcept { return const_iterator{data(), num_bits}; }

        constexpr                   auto rbegin()        noexcept { return       reverse_iterator{end()}; }
        constexpr                   auto rbegin()  const noexcept { return const_reverse_iterator{end()}; }
        XSTD_PP_CONSTEXPR_INTRINSIC auto rend()          noexcept { return       reverse_iterator{begin()}; }
        XSTD_PP_CONSTEXPR_INTRINSIC auto rend()    const noexcept { return const_reverse_iterator{begin()}; }

        XSTD_PP_CONSTEXPR_INTRINSIC auto cbegin()  const noexcept { return const_iterator{begin()}; }
        XSTD_PP_CONSTEXPR_INTRINSIC auto cend()    const noexcept { return const_iterator{end()};   }
        constexpr                   auto crbegin() const noexcept { return const_reverse_iterator{rbegin()}; }
        XSTD_PP_CONSTEXPR_INTRINSIC auto crend()   const noexcept { return const_reverse_iterator{rend()};   }

        XSTD_PP_CONSTEXPR_INTRINSIC auto front() const // Throws: Nothing.
                -> const_reference
        {
                assert(!empty());
                return { *data(), find_front() };
        }

        XSTD_PP_CONSTEXPR_INTRINSIC auto back() const // Throws: Nothing.
                -> const_reference
        {
                assert(!empty());
                return { *data(), find_back() };
        }

        template<class UnaryPredicate>
        XSTD_PP_CONSTEXPR_INTRINSIC auto any_of(UnaryPredicate pred [[maybe_unused]]) const
        {
                if constexpr (num_blocks == 1) {
                        for (auto block = m_data[0]; block; /* update inside loop */) {
                                auto const first = detail::bsfnz(block);
                                if (pred(first)) {
                                        return true;
                                }
                                block ^= bit1(first);
                        }
                } else if constexpr (num_blocks >= 2) {
                        for (auto i = 0, offset = 0; i < num_blocks; ++i, offset += block_size) {
                                for (auto block = m_data[i]; block; /* update inside loop */) {
                                        auto const first = detail::bsfnz(block);
                                        if (pred(offset + first)) {
                                                return true;
                                        }
                                        block ^= bit1(first);
                                }
                        }
                }
                return false;
        }

        template<class UnaryPredicate>
        XSTD_PP_CONSTEXPR_INTRINSIC auto none_of(UnaryPredicate pred [[maybe_unused]]) const
        {
                return !any_of(pred);
        }

        template<class UnaryPredicate>
        XSTD_PP_CONSTEXPR_INTRINSIC auto all_of(UnaryPredicate pred [[maybe_unused]]) const
        {
                return !any_of(std::not_fn(pred));
        }

        template<class T, class BinaryOperation = std::plus<>>
        XSTD_PP_CONSTEXPR_INTRINSIC auto accumulate(T init, BinaryOperation op [[maybe_unused]] = BinaryOperation{}) const
        {
                auto result = std::move(init);
                if constexpr (num_blocks == 1) {
                        for (auto block = m_data[0]; block; /* update inside loop */) {
                                auto const first = detail::bsfnz(block);
                                result = op(result, first);
                                block ^= bit1(first);
                        }
                } else if constexpr (num_blocks >= 2) {
                        for (auto i = 0, offset = 0; i < num_blocks; ++i, offset += block_size) {
                                for (auto block = m_data[i]; block; /* update inside loop */) {
                                        auto const first = detail::bsfnz(block);
                                        result = op(result, offset + first);
                                        block ^= bit1(first);
                                }
                        }
                }
                return result;
        }

        template<class UnaryFunction>
        XSTD_PP_CONSTEXPR_INTRINSIC auto for_each(UnaryFunction fun) const
        {
                if constexpr (num_blocks == 1) {
                        for (auto block = m_data[0]; block; /* update inside loop */) {
                                auto const first = detail::bsfnz(block);
                                fun(first);
                                block ^= bit1(first);
                        }
                } else if constexpr (num_blocks >= 2) {
                        for (auto i = 0, offset = 0; i < num_blocks; ++i, offset += block_size) {
                                for (auto block = m_data[i]; block; /* update inside loop */) {
                                        auto const first = detail::bsfnz(block);
                                        fun(offset + first);
                                        block ^= bit1(first);
                                }
                        }
                }
                return std::move(fun);
        }

        template<class UnaryFunction>
        XSTD_PP_CONSTEXPR_INTRINSIC auto reverse_for_each(UnaryFunction fun) const
        {
                if constexpr (num_blocks == 1) {
                        for (auto block = m_data[0]; block; /* update inside loop */) {
                                auto const last = detail::bsrnz(block);
                                fun(last);
                                block ^= bit1(last);
                        }
                } else if constexpr (num_blocks >= 2) {
                        for (auto i = num_blocks - 1, offset = (num_blocks - 1) * block_size; i >= 0; --i, offset -= block_size) {
                                for (auto block = m_data[i]; block; /* update inside loop */) {
                                        auto const last = detail::bsrnz(block);
                                        fun(offset + last);
                                        block ^= bit1(last);
                                }
                        }
                }
                return std::move(fun);
        }

        [[nodiscard]] XSTD_PP_CONSTEXPR_ALGORITHM auto empty() const noexcept
                -> bool
        {
                if constexpr (num_blocks == 0) {
                        return true;
                } else if constexpr (num_blocks == 1) {
                        return !m_data[0];
                } else if constexpr (num_blocks == 2) {
                        return !m_data[0] && !m_data[1];
                } else if constexpr (num_blocks >= 3) {
                        return std::none_of(std::cbegin(m_data), std::cend(m_data), [](auto const block) -> bool {
                                return block;
                        });
                }
        }

        [[nodiscard]] XSTD_PP_CONSTEXPR_ALGORITHM auto full() const noexcept
        {
                if constexpr (excess_bits == 0) {
                        if constexpr (num_blocks == 0) {
                                return true;
                        } else if constexpr (num_blocks == 1) {
                                return m_data[0] == ones;
                        } else if constexpr (num_blocks == 2) {
                                return m_data[0] == ones && m_data[1] == ones;
                        } else if constexpr (num_blocks >= 3) {
                                return std::all_of(std::cbegin(m_data), std::cend(m_data), [](auto const block) {
                                        return block == ones;
                                });
                        }
                } else {
                        if constexpr (num_blocks == 1) {
                                return m_data[0] == sane;
                        } else if constexpr (num_blocks == 2) {
                                return m_data[0] == ones && m_data[1] == sane;
                        } else {
                                static_assert(num_blocks >= 3);
                                return
                                        std::all_of(std::cbegin(m_data), std::prev(std::cend(m_data)), [](auto const block) {
                                                return block == ones;
                                        }) && m_data[num_blocks - 1] == sane;
                                ;
                        }
                }
        }

        XSTD_PP_CONSTEXPR_INTRINSIC auto size() const noexcept
        {
                if constexpr (num_blocks == 0) {
                        return 0;
                } else if constexpr (num_blocks == 1) {
                        return detail::popcount(m_data[0]);
                } else if constexpr (num_blocks == 2) {
                        return detail::popcount(m_data[0]) + detail::popcount(m_data[1]);
                } else if constexpr (num_blocks >= 3) {
                        return std::accumulate(std::cbegin(m_data), std::cend(m_data), 0, [](auto const sum, auto const block) {
                                return sum + detail::popcount(block);
                        });
                }
        }

        constexpr auto max_size() const noexcept
        {
                return N;
        }

        constexpr auto& insert(value_type const n) // Throws: Nothing.
        {
                assert(0 <= n); assert(n < N);
                if constexpr (num_blocks == 1) {
                        m_data[0] |= bit1(n);
                } else {
                        m_data[which(n)] |= bit1(where(n));
                }
                assert(contains(n));
                return *this;
        }

        constexpr auto insert(const_iterator /* hint */, value_type const n) // Throws: Nothing.
                -> iterator
        {
                insert(n);
                return { data(), n };
        }

        template<class InputIterator>
        constexpr auto insert(InputIterator first, InputIterator last) // Throws: Nothing.
        {
                while (first != last) {
                        insert(*first++);
                }
        }

        constexpr auto insert(std::initializer_list<value_type> ilist) // Throws: Nothing.
        {
                insert(ilist.begin(), ilist.end());
        }

        XSTD_PP_CONSTEXPR_ALGORITHM auto fill() noexcept
        {
                if constexpr (num_blocks == 1) {
                        m_data[0] = ones;
                } else if constexpr (num_blocks == 2) {
                        m_data[0] = ones;
                        m_data[1] = ones;
                } else if constexpr (num_blocks >= 3) {
                        std::fill_n(std::begin(m_data), num_blocks, ones);
                }
                sanitize_back();
                assert(full());
        }

        constexpr auto& erase(key_type const& x) // Throws: Nothing.
        {
                assert(0 <= x); assert(x < N);
                if constexpr (num_blocks == 1) {
                        m_data[0] &= ~bit1(x);
                } else {
                        m_data[which(x)] &= ~bit1(where(x));
                }
                assert(!contains(x));
                return *this;
        }

        template<class InputIterator>
        constexpr auto erase(InputIterator first, InputIterator last) // Throws: Nothing.
        {
                while (first != last) {
                        erase(*first++);
                }
        }

        constexpr auto erase(std::initializer_list<value_type> ilist) // Throws: Nothing.
        {
                erase(ilist.begin(), ilist.end());
        }

        XSTD_PP_CONSTEXPR_SWAP auto swap(int_set& other [[maybe_unused]]) noexcept(num_blocks == 0 || std::is_nothrow_swappable_v<value_type>)
        {
                if constexpr (num_blocks == 1) {
                        using std::swap;
                        swap(m_data[0], other.m_data[0]);
                } else if constexpr (num_blocks == 2) {
                        using std::swap;
                        swap(m_data[0], other.m_data[0]);
                        swap(m_data[1], other.m_data[1]);
                } else if constexpr (num_blocks >= 3) {
                        std::swap_ranges(std::begin(m_data), std::end(m_data), std::begin(other.m_data));
                }
        }

        XSTD_PP_CONSTEXPR_ALGORITHM auto clear() noexcept
        {
                if constexpr (num_blocks == 1) {
                        m_data[0] = zero;
                } else if constexpr (num_blocks == 2) {
                        m_data[0] = zero;
                        m_data[1] = zero;
                } else if constexpr (num_blocks >= 3) {
                        std::fill_n(std::begin(m_data), num_blocks, zero);
                }
                assert(empty());
        }

        constexpr auto& replace(value_type const n) // Throws: Nothing.
        {
                assert(0 <= n); assert(n < N);
                if constexpr (num_blocks == 1) {
                        m_data[0] ^= bit1(n);
                } else {
                        m_data[which(n)] ^= bit1(where(n));
                }
                return *this;
        }

        constexpr auto complement() noexcept
        {
                if constexpr (num_blocks == 1) {
                        m_data[0] = ~m_data[0];
                } else if constexpr (num_blocks == 2) {
                        m_data[0] = ~m_data[0];
                        m_data[1] = ~m_data[1];
                } else if constexpr (num_blocks >= 3) {
                        for (auto& block : m_data) {
                                block = ~block;
                        }
                }
                sanitize_back();
        }

        [[nodiscard]] constexpr auto contains(key_type const& x) const // Throws: Nothing.
                -> bool
        {
                assert(0 <= x); assert(x < N);
                if constexpr (num_blocks == 1) {
                        return m_data[0] & bit1(x);
                } else {
                        return m_data[which(x)] & bit1(where(x));
                }
        }

        constexpr auto find(key_type const& x) // Throws: Nothing.
        {
                assert(0 <= x); assert(x < N);
                return contains(x) ? iterator{data(), x} : end();
        }

        constexpr auto find(key_type const& x) const // Throws: Nothing.
        {
                assert(0 <= x); assert(x < N);
                return contains(x) ? const_iterator{data(), x} : cend();
        }

        constexpr auto count(key_type const& x) const // Throws: Nothing.
                -> size_type
        {
                assert(0 <= x); assert(x < N);
                return contains(x);
        }

        constexpr auto& operator&=(int_set const& other [[maybe_unused]]) noexcept
        {
                if constexpr (num_blocks == 1) {
                        m_data[0] &= other.m_data[0];
                } else if constexpr (num_blocks == 2) {
                        m_data[0] &= other.m_data[0];
                        m_data[1] &= other.m_data[1];
                } else if constexpr (num_blocks >= 3) {
                        for (auto i = 0; i < num_blocks; ++i) {
                                m_data[i] &= other.m_data[i];
                        }
                }
                return *this;
        }

        constexpr auto& operator|=(int_set const& other [[maybe_unused]]) noexcept
        {
                if constexpr (num_blocks == 1) {
                        m_data[0] |= other.m_data[0];
                } else if constexpr (num_blocks == 2) {
                        m_data[0] |= other.m_data[0];
                        m_data[1] |= other.m_data[1];
                } else if constexpr (num_blocks >= 3) {
                        for (auto i = 0; i < num_blocks; ++i) {
                                m_data[i] |= other.m_data[i];
                        }
                }
                return *this;
        }

        constexpr auto& operator^=(int_set const& other [[maybe_unused]]) noexcept
        {
                if constexpr (num_blocks == 1) {
                        m_data[0] ^= other.m_data[0];
                } else if constexpr (num_blocks == 2) {
                        m_data[0] ^= other.m_data[0];
                        m_data[1] ^= other.m_data[1];
                } else if constexpr (num_blocks >= 3) {
                        for (auto i = 0; i < num_blocks; ++i) {
                                m_data[i] ^= other.m_data[i];
                        }
                }
                return *this;
        }

        constexpr auto& operator-=(int_set const& other [[maybe_unused]]) noexcept
        {
                if constexpr (num_blocks == 1) {
                        m_data[0] &= ~other.m_data[0];
                } else if constexpr (num_blocks == 2) {
                        m_data[0] &= ~other.m_data[0];
                        m_data[1] &= ~other.m_data[1];
                } else if constexpr (num_blocks >= 3) {
                        for (auto i = 0; i < num_blocks; ++i) {
                                m_data[i] &= ~other.m_data[i];
                        }
                }
                return *this;
        }

        XSTD_PP_CONSTEXPR_ALGORITHM auto& operator<<=(size_type const n [[maybe_unused]]) // Throws: Nothing.
        {
                assert(0 <= n); assert(n < N);
                if constexpr (num_blocks == 1) {
                        m_data[0] <<= n;
                } else if constexpr (num_blocks >= 2) {
                        if (n == 0) { return *this; }

                        auto const n_block = n / block_size;
                        auto const L_shift = n % block_size;

                        if (L_shift == 0) {
                                std::copy_backward(std::cbegin(m_data), std::prev(std::cend(m_data), n_block), std::end(m_data));
                        } else {
                                auto const R_shift = block_size - L_shift;

                                for (auto i = num_blocks - 1; i > n_block; --i) {
                                        m_data[i] =
                                                (m_data[i - n_block    ] << L_shift) |
                                                (m_data[i - n_block - 1] >> R_shift)
                                        ;
                                }
                                m_data[n_block] = m_data[0] << L_shift;
                        }
                        std::fill_n(std::begin(m_data), n_block, zero);
                }
                sanitize_back();
                return *this;
        }

        XSTD_PP_CONSTEXPR_ALGORITHM auto& operator>>=(size_type const n) // Throws: Nothing.
        {
                assert(0 <= n); assert(n < N);
                if constexpr (num_blocks == 1) {
                        m_data[0] >>= n;
                } else if constexpr (num_blocks >= 2) {
                        if (n == 0) { return *this; }

                        auto const n_block = n / block_size;
                        auto const R_shift = n % block_size;

                        if (R_shift == 0) {
                                std::copy_n(std::next(std::cbegin(m_data), n_block), num_blocks - n_block, std::begin(m_data));
                        } else {
                                auto const L_shift = block_size - R_shift;

                                for (auto i = 0; i < num_blocks - 1 - n_block; ++i) {
                                        m_data[i] =
                                                (m_data[i + n_block    ] >> R_shift) |
                                                (m_data[i + n_block + 1] << L_shift)
                                        ;
                                }
                                m_data[num_blocks - 1 - n_block] = m_data[num_blocks - 1] >> R_shift;
                        }
                        std::fill_n(std::rbegin(m_data), n_block, zero);
                }
                return *this;
        }

        template<class HashAlgorithm>
        friend constexpr auto hash_append(HashAlgorithm& h, int_set const& is) noexcept
        {
                h(is.data(), num_bits / std::numeric_limits<unsigned char>::digits);
        }

private:
        constexpr static auto zero = static_cast<block_type>(0);
        constexpr static auto ones = ~zero;
        constexpr static auto sane = ones >> excess_bits;

        constexpr static auto bit1(value_type const n) // Throws: Nothing.
        {
                assert(0 <= n); assert(n < block_size);
                return static_cast<block_type>(1) << n;
        }

        constexpr static auto which(value_type const n) // Throws: Nothing.
        {
                static_assert(num_blocks != 1);
                assert(0 <= n); assert(n < num_bits);
                return n / block_size;
        }

        constexpr static auto where(value_type const n) // Throws: Nothing.
        {
                static_assert(num_blocks != 1);
                assert(0 <= n); assert(n < num_bits);
                return n % block_size;
        }

        constexpr auto sanitize_back() noexcept
        {
                if constexpr (excess_bits != 0) {
                        if constexpr (num_blocks == 1) {
                                m_data[0] &= sane;
                        } else {
                                static_assert(num_blocks >= 2);
                                m_data[num_blocks - 1] &= sane;
                        }
                }
        }

        constexpr auto const* data() const noexcept
        {
                return m_data;
        }

        XSTD_PP_CONSTEXPR_INTRINSIC auto find_first() const noexcept
        {
                if constexpr (num_blocks == 0) {
                        return 0;
                } else if constexpr (num_blocks == 1) {
                        return detail::bsf(m_data[0]);
                } else if constexpr (num_blocks >= 2) {
                        auto offset = 0;
                        for (auto i = 0; i < num_blocks; ++i, offset += block_size) {
                                if (auto const block = m_data[i]; block) {
                                        offset += detail::ctznz(block);
                                        break;
                                }
                        }
                        return offset;
                }
        }

        XSTD_PP_CONSTEXPR_INTRINSIC auto find_front() const // Throws: Nothing.
        {
                assert(!empty());
                if constexpr (num_blocks == 1) {
                        return detail::bsfnz(m_data[0]);
                } else if constexpr (num_blocks == 2) {
                        return
                                m_data[0] ?
                                detail::bsfnz(m_data[0]) :
                                detail::bsfnz(m_data[1]) + block_size
                        ;
                } else {
                        auto offset = 0;
                        for (auto i = 0; i < num_blocks - 1; ++i, offset += block_size) {
                                if (auto const block = m_data[i]; block) {
                                        return offset + detail::ctznz(block);
                                }
                        }
                        return offset + detail::ctznz(m_data[std::max(num_blocks - 1, 0)]);
                }
        }

        XSTD_PP_CONSTEXPR_INTRINSIC auto find_back() const // Throws: Nothing.
        {
                assert(!empty());
                if constexpr (num_blocks == 1) {
                        return detail::bsrnz(m_data[0]);
                } else if constexpr (num_blocks == 2) {
                        return
                                m_data[1] ?
                                detail::bsrnz(m_data[1]) + block_size :
                                detail::bsrnz(m_data[0])
                        ;
                } else {
                        auto offset = num_bits - 1;
                        for (auto i = num_blocks - 1; i > 0; --i, offset -= block_size) {
                                if (auto const block = m_data[i]; block) {
                                        return offset - detail::clznz(block);
                                }
                        }
                        return offset - detail::clznz(m_data[0]);
                }
        }

        class proxy_reference
        {
                block_type const& m_block;
                value_type const m_value;
        public:
                ~proxy_reference() = default;
                proxy_reference(proxy_reference const&) = default;
                proxy_reference(proxy_reference&&) = default;
                proxy_reference& operator=(proxy_reference const&) = delete;
                proxy_reference& operator=(proxy_reference&&) = delete;

                proxy_reference() = delete;

                constexpr proxy_reference(block_type const& b, value_type const v) noexcept
                :
                        m_block{b},
                        m_value{v}
                {
                        assert(0 <= m_value); assert(m_value < N);
                }

                proxy_reference& operator=(value_type const) = delete;

                /* implicit */ constexpr operator value_type() const noexcept
                {
                        return m_value;
                }

                constexpr auto operator&() const noexcept
                        -> proxy_iterator
                {
                        return { &m_block, m_value };
                }
        };

        class proxy_iterator
        {
        public:
                using difference_type   = typename int_set::difference_type;
                using value_type        = typename int_set::value_type;
                using pointer           = proxy_iterator;
                using reference         = proxy_reference;
                using iterator_category = std::bidirectional_iterator_tag;

        private:
                block_type const* m_block;
                value_type m_value;

        public:
                proxy_iterator() = default;

                constexpr proxy_iterator(block_type const* b, value_type const v) // Throws: Nothing.
                :
                        m_block{b},
                        m_value{v}
                {
                        assert(m_block != nullptr);
                        assert(0 <= m_value); assert(m_value < N || m_value == num_bits);
                }

                constexpr auto operator*() const // Throws: Nothing.
                        -> proxy_reference
                {
                        assert(0 <= m_value); assert(m_value < N);
                        return { *m_block, m_value };
                }

                XSTD_PP_CONSTEXPR_INTRINSIC auto& operator++() // Throws: Nothing.
                {
                        assert(0 <= m_value); assert(m_value < N);
                        increment();
                        assert(0 < m_value); assert(m_value < N || m_value == num_bits);
                        return *this;
                }

                XSTD_PP_CONSTEXPR_INTRINSIC auto operator++(int) // Throws: Nothing.
                {
                        auto nrv = *this; ++*this; return nrv;
                }

                XSTD_PP_CONSTEXPR_INTRINSIC auto& operator--() // Throws:Nothing.
                {
                        assert(0 < m_value); assert(m_value < N || m_value == num_bits);
                        decrement();
                        assert(0 <= m_value); assert(m_value < N);
                        return *this;
                }

                XSTD_PP_CONSTEXPR_INTRINSIC auto operator--(int) // Throws: Nothing.
                {
                        auto nrv = *this; --*this; return nrv;
                }

                friend constexpr auto operator==(proxy_iterator const& lhs, proxy_iterator const& rhs) noexcept
                {
                        assert(lhs.m_block == rhs.m_block);
                        return lhs.m_value == rhs.m_value;
                }

                friend constexpr auto operator!=(proxy_iterator const& lhs, proxy_iterator const& rhs) noexcept
                {
                        return !(lhs == rhs);
                }

        private:
                XSTD_PP_CONSTEXPR_INTRINSIC auto increment() // Throws: Nothing.
                {
                        assert(m_value < N);
                        if (++m_value == num_bits) { return; }
                        if constexpr (num_blocks == 1) {
                                if (auto const block = *m_block >> m_value; block) {
                                        m_value += detail::ctznz(block);
                                        return;
                                }
                                m_value = block_size;
                        } else if constexpr (num_blocks >= 2) {
                                auto i = which(m_value);
                                if (auto const offset = where(m_value); offset != 0) {
                                        if (auto const block = m_block[i] >> offset; block) {
                                                m_value += detail::ctznz(block);
                                                return;
                                        }
                                        ++i;
                                        m_value += block_size - offset;
                                }
                                for (/* initialized before loop */; i < num_blocks; ++i, m_value += block_size) {
                                        if (auto const block = m_block[i]; block) {
                                                m_value += detail::ctznz(block);
                                                return;
                                        }
                                }
                        }
                        assert(m_value == num_bits);
                }

                XSTD_PP_CONSTEXPR_INTRINSIC auto decrement() // Throws: Nothing.
                {
                        assert(0 < m_value);
                        --m_value;
                        if constexpr (num_blocks == 1) {
                                m_value -= detail::clznz(*m_block << (block_size - 1 - m_value));
                        } else if constexpr (num_blocks >= 2) {
                                auto i = which(m_value);
                                if (auto const offset = where(m_value); offset != block_size - 1) {
                                        if (auto const block = m_block[i] << (block_size - 1 - offset); block) {
                                                m_value -= detail::clznz(block);
                                                return;
                                        }
                                        --i;
                                        m_value -= offset + 1;
                                }
                                for (/* initialized before loop */; i >= 0; --i, m_value -= block_size) {
                                        if (auto const block = m_block[i]; block) {
                                                m_value -= detail::clznz(block);
                                                return;
                                        }
                                }
                        }
                }
        };

        friend XSTD_PP_CONSTEXPR_ALGORITHM auto operator==  <>(int_set const& /* lhs */, int_set const& /* rhs */) noexcept;
        friend XSTD_PP_CONSTEXPR_ALGORITHM auto operator<   <>(int_set const& /* lhs */, int_set const& /* rhs */) noexcept;
        friend XSTD_PP_CONSTEXPR_ALGORITHM bool is_subset_of<>(int_set const& /* lhs */, int_set const& /* rhs */) noexcept;
        friend XSTD_PP_CONSTEXPR_ALGORITHM bool intersects  <>(int_set const& /* lhs */, int_set const& /* rhs */) noexcept;
};

template<int N, class UIntType>
XSTD_PP_CONSTEXPR_ALGORITHM auto operator==(int_set<N, UIntType> const& lhs [[maybe_unused]], int_set<N, UIntType> const& rhs [[maybe_unused]]) noexcept
{
        constexpr auto num_blocks = int_set<N, UIntType>::num_blocks;
        if constexpr (num_blocks == 0) {
                return true;
        } else if constexpr (num_blocks == 1) {
                return lhs.m_data[0] == rhs.m_data[0];
        } else if constexpr (num_blocks == 2) {
                constexpr auto tied = [](auto const& is) {
                        return std::tie(is.m_data[0], is.m_data[1]);
                };
                return tied(lhs) == tied(rhs);
        } else if constexpr (num_blocks >= 3) {
                return std::equal(
                        std::cbegin(lhs.m_data), std::cend(lhs.m_data),
                        std::cbegin(rhs.m_data), std::cend(rhs.m_data)
                );
        }
}

template<int N, class UIntType>
XSTD_PP_CONSTEXPR_ALGORITHM auto operator!=(int_set<N, UIntType> const& lhs, int_set<N, UIntType> const& rhs) noexcept
{
        return !(lhs == rhs);
}

template<int N, class UIntType>
XSTD_PP_CONSTEXPR_ALGORITHM auto operator<(int_set<N, UIntType> const& lhs [[maybe_unused]], int_set<N, UIntType> const& rhs [[maybe_unused]]) noexcept
{
        constexpr auto num_blocks = int_set<N, UIntType>::num_blocks;
        if constexpr (num_blocks == 0) {
                return false;
        } else if constexpr (num_blocks == 1) {
                return lhs.m_data[0] < rhs.m_data[0];
        } else if constexpr (num_blocks == 2) {
                constexpr auto tied = [](auto const& is) {
                        return std::tie(is.m_data[1], is.m_data[0]);
                };
                return tied(lhs) < tied(rhs);
        } else if constexpr (num_blocks >= 3) {
                return std::lexicographical_compare(
                        std::crbegin(lhs.m_data), std::crend(lhs.m_data),
                        std::crbegin(rhs.m_data), std::crend(rhs.m_data)
                );
        }
}

template<int N, class UIntType>
XSTD_PP_CONSTEXPR_ALGORITHM auto operator>(int_set<N, UIntType> const& lhs, int_set<N, UIntType> const& rhs) noexcept
{
        return rhs < lhs;
}

template<int N, class UIntType>
XSTD_PP_CONSTEXPR_ALGORITHM auto operator>=(int_set<N, UIntType> const& lhs, int_set<N, UIntType> const& rhs) noexcept
{
        return !(lhs < rhs);
}

template<int N, class UIntType>
XSTD_PP_CONSTEXPR_ALGORITHM auto operator<=(int_set<N, UIntType> const& lhs, int_set<N, UIntType> const& rhs) noexcept
{
        return !(rhs < lhs);
}

template<int N, class UIntType>
constexpr auto operator~(int_set<N, UIntType> const& lhs) noexcept
{
        auto nrv{lhs}; nrv.complement(); return nrv;
}

template<int N, class UIntType>
constexpr auto operator&(int_set<N, UIntType> const& lhs, int_set<N, UIntType> const& rhs) noexcept
{
        auto nrv{lhs}; nrv &= rhs; return nrv;
}

template<int N, class UIntType>
constexpr auto operator|(int_set<N, UIntType> const& lhs, int_set<N, UIntType> const& rhs) noexcept
{
        auto nrv{lhs}; nrv |= rhs; return nrv;
}

template<int N, class UIntType>
constexpr auto operator^(int_set<N, UIntType> const& lhs, int_set<N, UIntType> const& rhs) noexcept
{
        auto nrv{lhs}; nrv ^= rhs; return nrv;
}

template<int N, class UIntType>
constexpr auto operator-(int_set<N, UIntType> const& lhs, int_set<N, UIntType> const& rhs) noexcept
{
        auto nrv{lhs}; nrv -= rhs; return nrv;
}

template<int N, class UIntType>
XSTD_PP_CONSTEXPR_ALGORITHM auto operator<<(int_set<N, UIntType> const& lhs, int const n) // Throws: Nothing.
{
        assert(0 <= n); assert(n < N);
        auto nrv{lhs}; nrv <<= n; return nrv;
}

template<int N, class UIntType>
XSTD_PP_CONSTEXPR_ALGORITHM auto operator>>(int_set<N, UIntType> const& lhs, int const n) // Throws: Nothing.
{
        assert(0 <= n); assert(n < N);
        auto nrv{lhs}; nrv >>= n; return nrv;
}

template<int N, class UIntType>
XSTD_PP_CONSTEXPR_ALGORITHM auto is_subset_of(int_set<N, UIntType> const& lhs [[maybe_unused]], int_set<N, UIntType> const& rhs [[maybe_unused]]) noexcept
        -> bool
{
        constexpr static auto num_blocks = int_set<N, UIntType>::num_blocks;
        if constexpr (num_blocks == 0) {
                return true;
        } else if constexpr (num_blocks == 1) {
                return !(lhs.m_data[0] & ~rhs.m_data[0]);
        } else if constexpr (num_blocks == 2) {
                return
                        !(lhs.m_data[0] & ~rhs.m_data[0]) &&
                        !(lhs.m_data[1] & ~rhs.m_data[1])
                ;
        } else if constexpr (num_blocks >= 3) {
                return std::equal(
                        std::cbegin(lhs.m_data), std::cend(lhs.m_data),
                        std::cbegin(rhs.m_data), std::cend(rhs.m_data),
                        [](auto const wL, auto const wR) -> bool {
                                return !(wL & ~wR);
                        }
                );
        }
}

template<int N, class UIntType>
XSTD_PP_CONSTEXPR_ALGORITHM auto is_superset_of(int_set<N, UIntType> const& lhs, int_set<N, UIntType> const& rhs) noexcept
{
        return is_subset_of(rhs, lhs);
}

template<int N, class UIntType>
XSTD_PP_CONSTEXPR_ALGORITHM auto is_proper_subset_of(int_set<N, UIntType> const& lhs, int_set<N, UIntType> const& rhs) noexcept
{
        return is_subset_of(lhs, rhs) && !is_subset_of(rhs, lhs);
}

template<int N, class UIntType>
XSTD_PP_CONSTEXPR_ALGORITHM auto is_proper_superset_of(int_set<N, UIntType> const& lhs, int_set<N, UIntType> const& rhs) noexcept
{
        return is_superset_of(lhs, rhs) && !is_superset_of(rhs, lhs);
}

template<int N, class UIntType>
XSTD_PP_CONSTEXPR_ALGORITHM auto intersects(int_set<N, UIntType> const& lhs [[maybe_unused]], int_set<N, UIntType> const& rhs [[maybe_unused]]) noexcept
        -> bool
{
        constexpr static auto num_blocks = int_set<N, UIntType>::num_blocks;
        if constexpr (num_blocks == 0) {
                return false;
        } else if constexpr (num_blocks == 1) {
                return lhs.m_data[0] & rhs.m_data[0];
        } else if constexpr (num_blocks == 2) {
                return
                        (lhs.m_data[0] & rhs.m_data[0]) ||
                        (lhs.m_data[1] & rhs.m_data[1])
                ;
        } else if constexpr (num_blocks >= 3) {
                return !std::equal(
                        std::cbegin(lhs.m_data), std::cend(lhs.m_data),
                        std::cbegin(rhs.m_data), std::cend(rhs.m_data),
                        [](auto const wL, auto const wR) -> bool {
                                return !(wL & wR);
                        }
                );
        }
}

template<int N, class UIntType>
XSTD_PP_CONSTEXPR_ALGORITHM auto disjoint(int_set<N, UIntType> const& lhs, int_set<N, UIntType> const& rhs) noexcept
{
        return !intersects(lhs, rhs);
}

template<int N, class UIntType>
XSTD_PP_CONSTEXPR_SWAP auto swap(int_set<N, UIntType>& lhs, int_set<N, UIntType>& rhs) noexcept(noexcept(lhs.swap(rhs)))
{
        lhs.swap(rhs);
}

template<int N, class UIntType>
XSTD_PP_CONSTEXPR_INTRINSIC auto begin(int_set<N, UIntType>& is)
        -> decltype(is.begin())
{
        return is.begin();
}

template<int N, class UIntType>
XSTD_PP_CONSTEXPR_INTRINSIC auto begin(int_set<N, UIntType> const& is)
        -> decltype(is.begin())
{
        return is.begin();
}

template<int N, class UIntType>
constexpr auto end(int_set<N, UIntType>& is)
        -> decltype(is.end())
{
        return is.end();
}

template<int N, class UIntType>
constexpr auto end(int_set<N, UIntType> const& is)
        -> decltype(is.end())
{
        return is.end();
}

template<int N, class UIntType>
XSTD_PP_CONSTEXPR_INTRINSIC auto cbegin(int_set<N, UIntType> const& is) noexcept(noexcept(xstd::begin(is)))
        -> decltype(xstd::begin(is))
{
        return xstd::begin(is);
}

template<int N, class UIntType>
constexpr auto cend(int_set<N, UIntType> const& is) noexcept(noexcept(xstd::end(is)))
        -> decltype(xstd::end(is))
{
        return xstd::end(is);
}

template<int N, class UIntType>
constexpr auto rbegin(int_set<N, UIntType>& is)
        -> decltype(is.rbegin())
{
        return is.rbegin();
}

template<int N, class UIntType>
constexpr auto rbegin(int_set<N, UIntType> const& is)
        -> decltype(is.rbegin())
{
        return is.rbegin();
}

template<int N, class UIntType>
XSTD_PP_CONSTEXPR_INTRINSIC auto rend(int_set<N, UIntType>& is)
        -> decltype(is.rend())
{
        return is.rend();
}

template<int N, class UIntType>
XSTD_PP_CONSTEXPR_INTRINSIC auto rend(int_set<N, UIntType> const& is)
        -> decltype(is.rend())
{
        return is.rend();
}

template<int N, class UIntType>
constexpr auto crbegin(int_set<N, UIntType> const& is)
        -> decltype(xstd::rbegin(is))
{
        return xstd::rbegin(is);
}

template<int N, class UIntType>
XSTD_PP_CONSTEXPR_INTRINSIC auto crend(int_set<N, UIntType> const& is)
        -> decltype(xstd::rend(is))
{
        return xstd::rend(is);
}

template<int N, class UIntType>
[[nodiscard]] XSTD_PP_CONSTEXPR_ALGORITHM auto empty(int_set<N, UIntType> const& is)
        -> decltype(is.empty())
{
        return is.empty();
}

template<int N, class UIntType>
XSTD_PP_CONSTEXPR_ALGORITHM auto size(int_set<N, UIntType> const& is)
        -> decltype(is.size())
{
        return is.size();
}

}       // namespace xstd
