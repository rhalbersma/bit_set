#pragma once

//          Copyright Rein Halbersma 2014-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>            // all_of, copy_backward, copy_n, equal, fill_n, for_each, lexicographical_compare, max, none_of, swap_ranges
#include <cassert>              // assert
#include <cstddef>              // ptrdiff_t, size_t
#include <cstdint>              // uint64_t
#include <functional>           // less
#include <initializer_list>     // initializer_list
#include <iterator>             // bidirectional_iterator_tag, begin, end, next, prev, rbegin, rend, reverse_iterator
#include <limits>               // digits
#include <numeric>              // accumulate
#include <tuple>                // tie
#include <type_traits>          // enable_if_t, is_integral_v, is_nothrow_swappable_v, is_unsigned_v
#include <utility>              // pair, swap

#if defined(__GNUG__)

#define DO_PRAGMA(X) _Pragma(#X)
#define PRAGMA_GCC_DIAGNOSTIC_PUSH_IGNORED(X)   \
        _Pragma("GCC diagnostic push")          \
        DO_PRAGMA(GCC diagnostic ignored X)

#define PRAGMA_GCC_DIAGNOSTIC_POP               \
        _Pragma("GCC diagnostic pop")

#else

#define PRAGMA_GCC_DIAGNOSTIC_PUSH_IGNORED(X)
#define PRAGMA_GCC_DIAGNOSTIC_POP

#endif

#if defined(_MSC_VER)

#define PRAGMA_VC_WARNING_PUSH_DISABLE(X)       \
        __pragma(warning(push))                 \
        __pragma(warning(disable: X))

#define PRAGMA_VC_WARNING_POP                   \
        __pragma(warning(pop))

#else

#define PRAGMA_VC_WARNING_PUSH_DISABLE(X)
#define PRAGMA_VC_WARNING_POP

#endif

#if defined(__GNUG__)

#define XSTD_PP_CONSTEXPR_INTRINSIC     /* constexpr */

#elif defined(_MSC_VER)

#define XSTD_PP_CONSTEXPR_INTRINSIC     /* constexpr */

#endif

// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/p0202r3.html
#define XSTD_PP_CONSTEXPR_ALGORITHM     /* constexpr */

// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0879r0.html
#define XSTD_PP_CONSTEXPR_SWAP          /* constexpr */

// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0892r2.html
#define XSTD_PP_EXPLICIT_FALSE          /* explicit(false) */

namespace xstd {
namespace builtin {

#if defined(__GNUG__)

template<int N>
constexpr auto get(__uint128_t const x) noexcept
{
        static_assert(0 <= N); static_assert(N < 2);
        return static_cast<uint64_t>(x >> (64 * N));
}

template<class T, std::enable_if_t<
        std::is_unsigned_v<T> && std::is_integral_v<T>
>...>
XSTD_PP_CONSTEXPR_INTRINSIC auto ctznz(T const x) // Throws: Nothing.
{
        assert(x != 0);
        if constexpr (sizeof(T) < sizeof(unsigned)) {
                return __builtin_ctz(static_cast<unsigned>(x));
        } else if constexpr (sizeof(T) == sizeof(unsigned)) {
                return __builtin_ctz(x);
        } else if constexpr (sizeof(T) == sizeof(unsigned long long)) {
                return __builtin_ctzll(x);
        } else if constexpr (sizeof(T) == 2 * sizeof(unsigned long long)) {
                return get<0>(x) != 0 ? __builtin_ctzll(get<0>(x)) : __builtin_ctzll(get<1>(x)) + 64;
        }
}

template<class T, std::enable_if_t<
        std::is_unsigned_v<T> && std::is_integral_v<T>
>...>
XSTD_PP_CONSTEXPR_INTRINSIC auto bsfnz(T const x) // Throws: Nothing.
{
        assert(x != 0);
        return ctznz(x);
}

template<class T, std::enable_if_t<
        std::is_unsigned_v<T> && std::is_integral_v<T>
>...>
XSTD_PP_CONSTEXPR_INTRINSIC auto clznz(T const x) // Throws: Nothing.
{
        assert(x != 0);
        if constexpr (sizeof(T) < sizeof(unsigned)) {
                constexpr auto padded_zeros = std::numeric_limits<unsigned>::digits - std::numeric_limits<T>::digits;
                return __builtin_clz(static_cast<unsigned>(x)) - padded_zeros;
        } else if constexpr (sizeof(T) == sizeof(unsigned)) {
                return __builtin_clz(x);
        } else if constexpr (sizeof(T) == sizeof(unsigned long long)) {
                return __builtin_clzll(x);
        } else if constexpr (sizeof(T) == 2 * sizeof(unsigned long long)) {
                return get<1>(x) != 0 ? __builtin_clzll(get<1>(x)) : __builtin_clzll(get<0>(x)) + 64;
        }
}

template<class T, std::enable_if_t<
        std::is_unsigned_v<T> && std::is_integral_v<T>
>...>
XSTD_PP_CONSTEXPR_INTRINSIC auto bsrnz(T const x) // Throws: Nothing.
{
        assert(x != 0);
        return std::numeric_limits<T>::digits - 1 - clznz(x);
}

template<class T, std::enable_if_t<
        std::is_unsigned_v<T> && std::is_integral_v<T>
>...>
XSTD_PP_CONSTEXPR_INTRINSIC auto popcount(T const x) noexcept
{
        if constexpr (sizeof(T) < sizeof(unsigned)) {
                return __builtin_popcount(static_cast<unsigned>(x));
        } else if constexpr (sizeof(T) == sizeof(unsigned)) {
                return __builtin_popcount(x);
        } else if constexpr (sizeof(T) == sizeof(unsigned long long)) {
                return __builtin_popcountll(x);
        } else if constexpr (sizeof(T) == 2 * sizeof(unsigned long long)) {
                return __builtin_popcountll(get<0>(x)) + __builtin_popcountll(get<1>(x));
        }
}

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

template<class T, std::enable_if_t<
        std::is_unsigned_v<T> && std::is_integral_v<T>
>...>
XSTD_PP_CONSTEXPR_INTRINSIC auto bsfnz(T const x) // Throws: Nothing.
{
        assert(x != 0);
        unsigned long index;
        if constexpr (sizeof(T) < sizeof(unsigned long)) {
                _BitScanForward(&index, static_cast<unsigned long>(x));
        } else if constexpr (sizeof(T) == sizeof(unsigned long)) {
                _BitScanForward(&index, x);
        } else if constexpr (sizeof(T) == sizeof(uint64_t)) {
                _BitScanForward64(&index, x);
        }
        return static_cast<int>(index);
}

template<class T, std::enable_if_t<
        std::is_unsigned_v<T> && std::is_integral_v<T>
>...>
XSTD_PP_CONSTEXPR_INTRINSIC auto ctznz(T const x) // Throws: Nothing.
{
        return bsfnz(x);
}

template<class T, std::enable_if_t<
        std::is_unsigned_v<T> && std::is_integral_v<T>
>...>
XSTD_PP_CONSTEXPR_INTRINSIC auto bsrnz(T const x) // Throws: Nothing.
{
        assert(x != 0);
        unsigned long index;
        if constexpr (sizeof(T) < sizeof(unsigned long)) {
                _BitScanReverse(&index, static_cast<unsigned long>(x));
        } else if constexpr (sizeof(T) == sizeof(unsigned long)) {
                _BitScanReverse(&index, x);
        } else if constexpr (sizeof(T) == sizeof(uint64_t)) {
                _BitScanReverse64(&index, x);
        }
        return static_cast<int>(index);
}

template<class T, std::enable_if_t<
        std::is_unsigned_v<T> && std::is_integral_v<T>
>...>
XSTD_PP_CONSTEXPR_INTRINSIC auto clznz(T const x) // Throws: Nothing.
{
        assert(x != 0);
        return std::numeric_limits<T>::digits - 1 - bsrnz(x);
}

template<class T, std::enable_if_t<
        std::is_unsigned_v<T> && std::is_integral_v<T>
>...>
XSTD_PP_CONSTEXPR_INTRINSIC auto popcount(T const x) noexcept
{
        if constexpr (sizeof(T) < sizeof(unsigned short)) {
                return static_cast<int>(__popcnt16(static_cast<unsigned short>(x)));
        } else if constexpr (sizeof(T) == sizeof(unsigned short)) {
                return static_cast<int>(__popcnt16(x));
        } else if constexpr (sizeof(T) == sizeof(unsigned)) {
                return static_cast<int>(__popcnt(x));
        } else if constexpr (sizeof(T) == sizeof(uint64_t)) {
                return static_cast<int>(__popcnt64(x));
        }
}

#endif

template<class T, std::enable_if_t<
        std::is_unsigned_v<T> && std::is_integral_v<T>
>...>
XSTD_PP_CONSTEXPR_INTRINSIC auto ctz(T const x) noexcept
{
        return x ? ctznz(x) : std::numeric_limits<T>::digits;
}

template<class T, std::enable_if_t<
        std::is_unsigned_v<T> && std::is_integral_v<T>
>...>
XSTD_PP_CONSTEXPR_INTRINSIC auto bsf(T const x) noexcept
{
        return x ? bsfnz(x) : std::numeric_limits<T>::digits;
}

template<class T, std::enable_if_t<
        std::is_unsigned_v<T> && std::is_integral_v<T>
>...>
XSTD_PP_CONSTEXPR_INTRINSIC auto clz(T const x) noexcept
{
        return x ? clznz(x) : std::numeric_limits<T>::digits;
}

template<class T, std::enable_if_t<
        std::is_unsigned_v<T> && std::is_integral_v<T>
>...>
XSTD_PP_CONSTEXPR_INTRINSIC auto bsr(T const x) noexcept
{
        return x ? bsrnz(x) : -1;
}

}       // namespace builtin

template<int /* N */, class /* Block */>
class int_set;

template<int N, class Block> XSTD_PP_CONSTEXPR_ALGORITHM auto operator==  (int_set<N, Block> const& /* lhs */, int_set<N, Block> const& /* rhs */) noexcept;
template<int N, class Block> XSTD_PP_CONSTEXPR_ALGORITHM auto operator<   (int_set<N, Block> const& /* lhs */, int_set<N, Block> const& /* rhs */) noexcept;
template<int N, class Block> XSTD_PP_CONSTEXPR_ALGORITHM bool is_subset_of(int_set<N, Block> const& /* lhs */, int_set<N, Block> const& /* rhs */) noexcept;
template<int N, class Block> XSTD_PP_CONSTEXPR_ALGORITHM bool intersects  (int_set<N, Block> const& /* lhs */, int_set<N, Block> const& /* rhs */) noexcept;

template<int N, class Block = std::size_t>
class int_set
{
        static_assert(0 <= N);
        static_assert(std::is_unsigned_v<Block>);
        static_assert(std::is_integral_v<Block>);

        constexpr static auto block_size = std::numeric_limits<Block>::digits;
        constexpr static auto num_logical_blocks = (N - 1 + block_size) / block_size;
        constexpr static auto num_storage_blocks = std::max(num_logical_blocks, 1);
        constexpr static auto num_bits = num_logical_blocks * block_size;
        constexpr static auto num_excess_bits = num_bits - N;
        static_assert(0 <= num_excess_bits); static_assert(num_excess_bits < block_size);

        class proxy_reference;
        class proxy_iterator;

        Block m_data[num_storage_blocks]{};     // zero-initialization
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
        using difference_type        = std::ptrdiff_t;
        using iterator               = proxy_iterator;
        using const_iterator         = proxy_iterator;
        using reverse_iterator       = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;
        using block_type             = Block;
        using insert_return_type     = void;

        int_set() = default;                    // zero-initialization

        template<class InputIterator>
        constexpr int_set(InputIterator first, InputIterator last) // Throws: Nothing.
        {
                insert(first, last);
        }

        constexpr int_set(std::initializer_list<value_type> ilist) // Throws: Nothing.
        :
                int_set(ilist.begin(), ilist.end())
        {}

        auto& operator=(std::initializer_list<value_type> ilist) // Throws: Nothing.
        {
                clear();
                insert(ilist.begin(), ilist.end());
                return *this;
        }

        XSTD_PP_CONSTEXPR_INTRINSIC auto begin()         noexcept { return       iterator{this, find_first()}; }
        XSTD_PP_CONSTEXPR_INTRINSIC auto begin()   const noexcept { return const_iterator{this, find_first()}; }
        constexpr                   auto end()           noexcept { return       iterator{this, N}; }
        constexpr                   auto end()     const noexcept { return const_iterator{this, N}; }

        constexpr                   auto rbegin()        noexcept { return       reverse_iterator{end()}; }
        constexpr                   auto rbegin()  const noexcept { return const_reverse_iterator{end()}; }
        XSTD_PP_CONSTEXPR_INTRINSIC auto rend()          noexcept { return       reverse_iterator{begin()}; }
        XSTD_PP_CONSTEXPR_INTRINSIC auto rend()    const noexcept { return const_reverse_iterator{begin()}; }

        XSTD_PP_CONSTEXPR_INTRINSIC auto cbegin()  const noexcept { return const_iterator{begin()}; }
        constexpr                   auto cend()    const noexcept { return const_iterator{end()};   }
        constexpr                   auto crbegin() const noexcept { return const_reverse_iterator{rbegin()}; }
        XSTD_PP_CONSTEXPR_INTRINSIC auto crend()   const noexcept { return const_reverse_iterator{rend()};   }

        XSTD_PP_CONSTEXPR_INTRINSIC auto front() const // Throws: Nothing.
                -> const_reference
        {
                assert(!empty());
                return { *this, find_front() };
        }

        XSTD_PP_CONSTEXPR_INTRINSIC auto back() const // Throws: Nothing.
                -> const_reference
        {
                assert(!empty());
                return { *this, find_back() };
        }

        [[nodiscard]] XSTD_PP_CONSTEXPR_ALGORITHM auto empty() const noexcept
                -> bool
        {
                if constexpr (num_logical_blocks == 0) {
                        return true;
                } else if constexpr (num_logical_blocks == 1) {
                        return !m_data[0];
                } else if constexpr (num_logical_blocks == 2) {
                        return !m_data[0] && !m_data[1];
                } else if constexpr (num_logical_blocks >= 3) {
                        return std::none_of(std::begin(m_data), std::end(m_data), [](auto const block) -> bool {
                                return block;
                        });
                }
        }

        [[nodiscard]] XSTD_PP_CONSTEXPR_ALGORITHM auto full() const noexcept
        {
                if constexpr (num_excess_bits == 0) {
                        if constexpr (num_logical_blocks == 0) {
                                return true;
                        } else if constexpr (num_logical_blocks == 1) {
                                return m_data[0] == ones;
                        } else if constexpr (num_logical_blocks == 2) {
                                return m_data[0] == ones && m_data[1] == ones;
                        } else if constexpr (num_logical_blocks >= 3) {
                                return std::all_of(std::begin(m_data), std::end(m_data), [](auto const block) {
                                        return block == ones;
                                });
                        }
                } else {
                        static_assert(num_logical_blocks >= 1);
                        if constexpr (num_logical_blocks == 1) {
                                return m_data[0] == no_excess_bits;
                        } else if constexpr (num_logical_blocks == 2) {
                                return m_data[0] == ones && m_data[1] == no_excess_bits;
                        } else if constexpr (num_logical_blocks >= 3) {
                                return
                                        std::all_of(std::begin(m_data), std::prev(std::end(m_data)), [](auto const block) {
                                                return block == ones;
                                        }) && m_data[num_logical_blocks - 1] == no_excess_bits
                                ;
                        }
                }
        }

        XSTD_PP_CONSTEXPR_INTRINSIC auto size() const noexcept
                -> size_type
        {
                if constexpr (num_logical_blocks == 0) {
                        return 0;
                } else if constexpr (num_logical_blocks == 1) {
                        return builtin::popcount(m_data[0]);
                } else if constexpr (num_logical_blocks == 2) {
                        return builtin::popcount(m_data[0]) + builtin::popcount(m_data[1]);
                } else if constexpr (num_logical_blocks >= 3) {
                        return std::accumulate(std::begin(m_data), std::end(m_data), 0, [](auto const sum, auto const block) {
                                return sum + builtin::popcount(block);
                        });
                }
        }

        constexpr static auto max_size() noexcept
                -> size_type
        {
                return N;
        }

        constexpr auto insert(value_type const x) // Throws: Nothing.
                -> std::pair<iterator, bool>
        {
                assert(0 <= x); assert(x < N);
                if constexpr (num_logical_blocks >= 1) {
                        PRAGMA_GCC_DIAGNOSTIC_PUSH_IGNORED("-Wconversion")
                        m_data[which(x)] |= single_bit_mask(where(x));
                        PRAGMA_GCC_DIAGNOSTIC_POP
                }
                assert(contains(x));
                return { { this, x }, true };
        }

        constexpr auto insert(const_iterator /* hint */, value_type const x) // Throws: Nothing.
                -> iterator
        {
                assert(0 <= x); assert(x < N);
                insert(x);
                return { this, x };
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

        XSTD_PP_CONSTEXPR_ALGORITHM auto& fill() noexcept
        {
                if constexpr (num_excess_bits == 0) {
                        if constexpr (num_logical_blocks == 1) {
                                m_data[0] = ones;
                        } else if constexpr (num_logical_blocks == 2) {
                                m_data[0] = ones;
                                m_data[1] = ones;
                        } else if constexpr (num_logical_blocks >= 3) {
                                std::fill_n(std::begin(m_data), num_logical_blocks, ones);
                        }
                } else {
                        if constexpr (num_logical_blocks == 1) {
                                m_data[0] = no_excess_bits;
                        } else if constexpr (num_logical_blocks == 2) {
                                m_data[0] = ones;
                                m_data[1] = no_excess_bits;
                        } else if constexpr (num_logical_blocks >= 3) {
                                std::fill_n(std::begin(m_data), num_logical_blocks - 1, ones);
                                m_data[num_logical_blocks - 1] = no_excess_bits;
                        }
                }
                assert(full());
                return *this;
        }

        constexpr auto erase(key_type const& x) // Throws: Nothing.
        {
                assert(0 <= x); assert(x < N);
                if constexpr (num_logical_blocks >= 1) {
                        PRAGMA_GCC_DIAGNOSTIC_PUSH_IGNORED("-Wconversion")
                        m_data[which(x)] &= ~single_bit_mask(where(x));
                        PRAGMA_GCC_DIAGNOSTIC_POP
                }
                assert(!contains(x));
                return 1;
        }

        constexpr auto erase(const_iterator pos) // Throws: Nothing.
        {
                assert(pos != end());
                erase(*pos++);
                return pos;
        }

        constexpr auto erase(const_iterator first, const_iterator last) // Throws: Nothing.
        {
                while (first != last) {
                        erase(*first++);
                }
                return first;
        }

        XSTD_PP_CONSTEXPR_SWAP auto swap(int_set& other [[maybe_unused]]) noexcept(num_logical_blocks == 0 || std::is_nothrow_swappable_v<value_type>)
        {
                if constexpr (num_logical_blocks == 1) {
                        using std::swap;
                        swap(m_data[0], other.m_data[0]);
                } else if constexpr (num_logical_blocks == 2) {
                        using std::swap;
                        swap(m_data[0], other.m_data[0]);
                        swap(m_data[1], other.m_data[1]);
                } else if constexpr (num_logical_blocks >= 3) {
                        std::swap_ranges(std::begin(m_data), std::end(m_data), std::begin(other.m_data));
                }
        }

        XSTD_PP_CONSTEXPR_ALGORITHM auto& clear() noexcept
        {
                if constexpr (num_logical_blocks == 1) {
                        m_data[0] = zero;
                } else if constexpr (num_logical_blocks == 2) {
                        m_data[0] = zero;
                        m_data[1] = zero;
                } else if constexpr (num_logical_blocks >= 3) {
                        std::fill_n(std::begin(m_data), num_logical_blocks, zero);
                }
                assert(empty());
                return *this;
        }

        constexpr auto& replace(value_type const n [[maybe_unused]]) // Throws: Nothing.
        {
                assert(0 <= n); assert(n < N);
                if constexpr (num_logical_blocks >= 1) {
                        PRAGMA_GCC_DIAGNOSTIC_PUSH_IGNORED("-Wconversion")
                        m_data[which(n)] ^= single_bit_mask(where(n));
                        PRAGMA_GCC_DIAGNOSTIC_POP
                }
                return *this;
        }

        constexpr auto find(key_type const& x) // Throws: Nothing.
        {
                assert(0 <= x); assert(x < N);
                return contains(x) ? iterator{this, x} : end();
        }

        constexpr auto find(key_type const& x) const // Throws: Nothing.
        {
                assert(0 <= x); assert(x < N);
                return contains(x) ? const_iterator{this, x} : cend();
        }

        constexpr auto count(key_type const& x) const // Throws: Nothing.
                -> size_type
        {
                assert(0 <= x); assert(x < N);
                return contains(x);
        }

        [[nodiscard]] constexpr auto contains(key_type const& x) const // Throws: Nothing.
        {
                assert(0 <= x); assert(x < N);
                if constexpr (num_logical_blocks >= 1) {
                        if (m_data[which(x)] & single_bit_mask(where(x))) {
                                return true;
                        }
                }
                return false;
        }

        XSTD_PP_CONSTEXPR_INTRINSIC auto lower_bound(key_type const& x) // Throws: Nothing.
                -> iterator
        {
                assert(0 <= x); assert(x < N);
                return { this, find_first_from(x) };
        }

        XSTD_PP_CONSTEXPR_INTRINSIC auto lower_bound(key_type const& x) const // Throws: Nothing.
                -> const_iterator
        {
                assert(0 <= x); assert(x < N);
                return { this, find_first_from(x) };
        }

        XSTD_PP_CONSTEXPR_INTRINSIC auto upper_bound(key_type const& x) // Throws: Nothing.
                -> iterator
        {
                assert(0 <= x); assert(x < N);
                return { this, find_first_from(x + 1) };
        }

        XSTD_PP_CONSTEXPR_INTRINSIC auto upper_bound(key_type const& x) const // Throws: Nothing.
                -> const_iterator
        {
                assert(0 <= x); assert(x < N);
                return { this, find_first_from(x + 1) };
        }

        XSTD_PP_CONSTEXPR_INTRINSIC auto equal_range(key_type const& x) // Throws: Nothing.
                -> std::pair<iterator, iterator>
        {
                assert(0 <= x); assert(x < N);
                return { lower_bound(x), upper_bound(x) };
        }

        XSTD_PP_CONSTEXPR_INTRINSIC auto equal_range(key_type const& x) const // Throws: Nothing.
                -> std::pair<const_iterator, const_iterator>
        {
                assert(0 <= x); assert(x < N);
                return { lower_bound(x), upper_bound(x) };
        }

        constexpr auto& complement() noexcept
        {
                if constexpr (num_excess_bits == 0) {
                        if constexpr (num_logical_blocks == 1) {
                                m_data[0] = static_cast<block_type>(~m_data[0]);
                        } else if constexpr (num_logical_blocks == 2) {
                                m_data[0] = static_cast<block_type>(~m_data[0]);
                                m_data[1] = static_cast<block_type>(~m_data[1]);
                        } else if constexpr (num_logical_blocks >= 3) {
                                for (auto& block : m_data) {
                                        block = static_cast<block_type>(~block);
                                }
                        }
                } else {
                        if constexpr (num_logical_blocks == 1) {
                                m_data[0] = static_cast<block_type>(~m_data[0]) & no_excess_bits;
                        } else if constexpr (num_logical_blocks == 2) {
                                m_data[0] = static_cast<block_type>(~m_data[0]);
                                m_data[1] = static_cast<block_type>(~m_data[1]) & no_excess_bits;
                        } else if constexpr (num_logical_blocks >= 3) {
                                for (auto i = 0; i < num_logical_blocks - 1; ++i) {
                                        m_data[i] = static_cast<block_type>(~m_data[i]);
                                }
                                m_data[num_logical_blocks - 1] = static_cast<block_type>(~m_data[num_logical_blocks - 1]) & no_excess_bits;
                        }
                }
                return *this;
        }

        constexpr auto& operator&=(int_set const& other [[maybe_unused]]) noexcept
        {
                if constexpr (num_logical_blocks == 1) {
                        m_data[0] &= other.m_data[0];
                } else if constexpr (num_logical_blocks == 2) {
                        m_data[0] &= other.m_data[0];
                        m_data[1] &= other.m_data[1];
                } else if constexpr (num_logical_blocks >= 3) {
                        for (auto i = 0; i < num_logical_blocks; ++i) {
                                m_data[i] &= other.m_data[i];
                        }
                }
                return *this;
        }

        constexpr auto& operator|=(int_set const& other [[maybe_unused]]) noexcept
        {
                if constexpr (num_logical_blocks == 1) {
                        m_data[0] |= other.m_data[0];
                } else if constexpr (num_logical_blocks == 2) {
                        m_data[0] |= other.m_data[0];
                        m_data[1] |= other.m_data[1];
                } else if constexpr (num_logical_blocks >= 3) {
                        for (auto i = 0; i < num_logical_blocks; ++i) {
                                m_data[i] |= other.m_data[i];
                        }
                }
                return *this;
        }

        constexpr auto& operator^=(int_set const& other [[maybe_unused]]) noexcept
        {
                if constexpr (num_logical_blocks == 1) {
                        m_data[0] ^= other.m_data[0];
                } else if constexpr (num_logical_blocks == 2) {
                        m_data[0] ^= other.m_data[0];
                        m_data[1] ^= other.m_data[1];
                } else if constexpr (num_logical_blocks >= 3) {
                        for (auto i = 0; i < num_logical_blocks; ++i) {
                                m_data[i] ^= other.m_data[i];
                        }
                }
                return *this;
        }

        constexpr auto& operator-=(int_set const& other [[maybe_unused]]) noexcept
        {
                if constexpr (num_logical_blocks == 1) {
                        PRAGMA_GCC_DIAGNOSTIC_PUSH_IGNORED("-Wconversion")
                        m_data[0] &= ~other.m_data[0];
                        PRAGMA_GCC_DIAGNOSTIC_POP
                } else if constexpr (num_logical_blocks == 2) {
                        PRAGMA_GCC_DIAGNOSTIC_PUSH_IGNORED("-Wconversion")
                        m_data[0] &= ~other.m_data[0];
                        m_data[1] &= ~other.m_data[1];
                        PRAGMA_GCC_DIAGNOSTIC_POP
                } else if constexpr (num_logical_blocks >= 3) {
                        for (auto i = 0; i < num_logical_blocks; ++i) {
                                PRAGMA_GCC_DIAGNOSTIC_PUSH_IGNORED("-Wconversion")
                                m_data[i] &= ~other.m_data[i];
                                PRAGMA_GCC_DIAGNOSTIC_POP
                        }
                }
                return *this;
        }

        XSTD_PP_CONSTEXPR_ALGORITHM auto& operator<<=(size_type const n [[maybe_unused]]) // Throws: Nothing.
        {
                assert(0 <= n); assert(n < N);
                if constexpr (num_logical_blocks == 1) {
                        PRAGMA_GCC_DIAGNOSTIC_PUSH_IGNORED("-Wconversion")
                        m_data[0] >>= n;
                        PRAGMA_GCC_DIAGNOSTIC_POP
                } else if constexpr (num_logical_blocks >= 2) {
                        if (n == 0) {
                                return *this;
                        }

                        auto const n_block = n / block_size;
                        auto const R_shift = n % block_size;

                        if (R_shift == 0) {
                                std::copy_backward(std::begin(m_data), std::prev(std::end(m_data), n_block), std::end(m_data));
                        } else {
                                auto const L_shift = block_size - R_shift;

                                for (auto i = num_logical_blocks - 1; i > n_block; --i) {
                                        m_data[i] =
                                                static_cast<block_type>(m_data[i - n_block    ] >> R_shift) |
                                                static_cast<block_type>(m_data[i - n_block - 1] << L_shift)
                                        ;
                                }
                                m_data[n_block] = static_cast<block_type>(m_data[0] >> R_shift);
                        }
                        std::fill_n(std::begin(m_data), n_block, zero);
                }
                clear_excess_bits();
                return *this;
        }

        XSTD_PP_CONSTEXPR_ALGORITHM auto& operator>>=(size_type const n [[maybe_unused]]) // Throws: Nothing.
        {
                assert(0 <= n); assert(n < N);
                if constexpr (num_logical_blocks == 1) {
                        PRAGMA_GCC_DIAGNOSTIC_PUSH_IGNORED("-Wconversion")
                        m_data[0] <<= n;
                        PRAGMA_GCC_DIAGNOSTIC_POP
                } else if constexpr (num_logical_blocks >= 2) {
                        if (n == 0) {
                                return *this;
                        }

                        auto const n_block = n / block_size;
                        auto const L_shift = n % block_size;

                        if (L_shift == 0) {
                                std::copy_n(std::next(std::begin(m_data), n_block), num_logical_blocks - n_block, std::begin(m_data));
                        } else {
                                auto const R_shift = block_size - L_shift;

                                for (auto i = 0; i < num_logical_blocks - 1 - n_block; ++i) {
                                        m_data[i] =
                                                static_cast<block_type>(m_data[i + n_block    ] << L_shift) |
                                                static_cast<block_type>(m_data[i + n_block + 1] >> R_shift)
                                        ;
                                }
                                m_data[num_logical_blocks - 1 - n_block] = static_cast<block_type>(m_data[num_logical_blocks - 1] << L_shift);
                        }
                        std::fill_n(std::rbegin(m_data), n_block, zero);
                }
                clear_excess_bits();
                return *this;
        }

private:
        constexpr static auto zero = static_cast<block_type>( 0);
        constexpr static auto ones = static_cast<block_type>(-1);

        PRAGMA_VC_WARNING_PUSH_DISABLE(4309)
        constexpr static auto no_excess_bits = static_cast<block_type>(ones << num_excess_bits);
        PRAGMA_VC_WARNING_POP

        static_assert(num_excess_bits ^ (ones == no_excess_bits));
        constexpr static auto unit = static_cast<block_type>(static_cast<block_type>(1) << (block_size - 1));

        constexpr static auto single_bit_mask(value_type const n) // Throws: Nothing.
        {
                static_assert(num_logical_blocks >= 1);
                assert(0 <= n); assert(n < block_size);
                return static_cast<block_type>(unit >> n);
        }

        constexpr static auto which(value_type const n [[maybe_unused]]) // Throws: Nothing.
        {
                static_assert(num_logical_blocks >= 1);
                assert(0 <= n); assert(n < N);
                if constexpr (num_logical_blocks == 1) {
                        return 0;
                } else {
                        return n / block_size;
                }
        }

        constexpr static auto where(value_type const n) // Throws: Nothing.
        {
                static_assert(num_logical_blocks >= 1);
                assert(0 <= n); assert(n < N);
                if constexpr (num_logical_blocks == 1) {
                        return n;
                } else {
                        return n % block_size;
                }
        }

        constexpr auto clear_excess_bits() noexcept
        {
                if constexpr (num_excess_bits != 0) {
                        static_assert(num_logical_blocks >= 1);
                        m_data[num_logical_blocks - 1] &= no_excess_bits;
                }
        }

        XSTD_PP_CONSTEXPR_INTRINSIC auto find_front() const // Throws: Nothing.
        {
                assert(!empty());
                if constexpr (num_logical_blocks == 1) {
                        return builtin::clznz(m_data[0]);
                } else if constexpr (num_logical_blocks == 2) {
                        return
                                m_data[0] ?
                                builtin::clznz(m_data[0]) :
                                builtin::clznz(m_data[1]) + block_size
                        ;
                } else {
                        auto n = 0;
                        for (auto i = 0; i < num_storage_blocks - 1; ++i, n += block_size) {
                                if (auto const block = m_data[i]; block) {
                                        return n + builtin::clznz(block);
                                }
                        }
                        return n + builtin::clznz(m_data[num_storage_blocks - 1]);
                }
        }

        XSTD_PP_CONSTEXPR_INTRINSIC auto find_back() const // Throws: Nothing.
        {
                assert(!empty());
                if constexpr (num_logical_blocks == 1) {
                        return num_bits - 1 - builtin::ctznz(m_data[0]);
                } else if constexpr (num_logical_blocks == 2) {
                        return
                                m_data[1] ?
                                num_bits - 1 - builtin::ctznz(m_data[1]) :
                                block_size - 1 - builtin::ctznz(m_data[0])
                        ;
                } else {
                        auto n = num_bits - 1;
                        for (auto i = num_storage_blocks - 1; i > 0; --i, n -= block_size) {
                                if (auto const block = m_data[i]; block) {
                                        return n - builtin::ctznz(block);
                                }
                        }
                        return n - builtin::ctznz(m_data[0]);
                }
        }

        XSTD_PP_CONSTEXPR_INTRINSIC auto find_first() const noexcept
        {
                if constexpr (num_logical_blocks == 1) {
                        if (m_data[0]) {
                                return builtin::clznz(m_data[0]);
                        }
                } else if constexpr (num_logical_blocks >= 2) {
                        auto n = 0;
                        for (auto i = 0; i < num_logical_blocks; ++i, n += block_size) {
                                if (auto const block = m_data[i]; block) {
                                        return n + builtin::clznz(block);
                                }
                        }
                }
                return N;
        }

        XSTD_PP_CONSTEXPR_INTRINSIC auto find_first_from(int n) const // Throws: Nothing.
        {
                assert(0 <= n); assert(n <= N);
                if (n == N) {
                        return N;
                }
                if constexpr (num_logical_blocks == 1) {
                        if (auto const block = static_cast<block_type>(m_data[0] << n); block) {
                                return n + builtin::clznz(block);
                        }
                } else if constexpr (num_logical_blocks >= 2) {
                        auto i = which(n);
                        if (auto const offset = where(n); offset) {
                                if (auto const block = static_cast<block_type>(m_data[i] << offset); block) {
                                        return n + builtin::clznz(block);
                                }
                                ++i;
                                n += block_size - offset;
                        }
                        for (/* initialized before loop */; i < num_logical_blocks; ++i, n += block_size) {
                                if (auto const block = m_data[i]; block) {
                                        return n + builtin::clznz(block);
                                }
                        }
                }
                return N;
        }

        XSTD_PP_CONSTEXPR_INTRINSIC auto find_last_from(int n) const // Throws: Nothing.
        {
                assert(0 <= n); assert(n < N);
                if constexpr (num_logical_blocks == 1) {
                        return n - builtin::ctznz(static_cast<block_type>(m_data[0] >> (block_size - 1 - n)));
                } else {
                        if constexpr (num_logical_blocks >= 2) {
                                auto i = which(n);
                                if (auto const offset = block_size - 1 - where(n); offset) {
                                        if (auto const block = static_cast<block_type>(m_data[i] >> offset); block) {
                                                return n - builtin::ctznz(block);
                                        }
                                        --i;
                                        n -= block_size - offset;
                                }
                                for (/* initialized before loop */; i > 0; --i, n -= block_size) {
                                        if (auto const block = m_data[i]; block) {
                                                return n - builtin::ctznz(block);
                                        }
                                }
                        }
                        return n - builtin::ctznz(m_data[0]);
                }
        }

        class proxy_reference
        {
                int_set const& m_is;
                value_type const m_value;
        public:
                ~proxy_reference() = default;
                proxy_reference(proxy_reference const&) = default;
                proxy_reference(proxy_reference&&) = default;
                proxy_reference& operator=(proxy_reference const&) = delete;
                proxy_reference& operator=(proxy_reference&&) = delete;

                proxy_reference() = delete;

                constexpr proxy_reference(int_set const& is, value_type const v) noexcept
                :
                        m_is{is},
                        m_value{v}
                {
                        assert(0 <= m_value); assert(m_value < N);
                }

                proxy_reference& operator=(value_type const) = delete;

                XSTD_PP_EXPLICIT_FALSE constexpr operator value_type() const noexcept
                {
                        return m_value;
                }

                template<class T, std::enable_if_t<
                        std::is_class_v<T> &&
                        std::is_constructible_v<T, value_type>
                >...>
                XSTD_PP_EXPLICIT_FALSE constexpr operator T() const noexcept(noexcept(T(m_value)))
                {
                        return m_value;
                }

                constexpr auto operator&() const noexcept
                        -> proxy_iterator
                {
                        return { &m_is, m_value };
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
                int_set const* m_is;
                value_type m_value;

        public:
                proxy_iterator() = default;

                constexpr proxy_iterator(int_set const* is, value_type const v) // Throws: Nothing.
                :
                        m_is{is},
                        m_value{v}
                {
                        assert(0 <= m_value); assert(m_value <= N);
                }

                constexpr auto operator*() const // Throws: Nothing.
                        -> proxy_reference
                {
                        assert(0 <= m_value); assert(m_value < N);
                        return { *m_is, m_value };
                }

                XSTD_PP_CONSTEXPR_INTRINSIC auto& operator++() // Throws: Nothing.
                {
                        assert(0 <= m_value); assert(m_value < N);
                        m_value = m_is->find_first_from(m_value + 1);
                        assert(0 < m_value); assert(m_value <= N);
                        return *this;
                }

                XSTD_PP_CONSTEXPR_INTRINSIC auto operator++(int) // Throws: Nothing.
                {
                        auto nrv = *this; ++*this; return nrv;
                }

                XSTD_PP_CONSTEXPR_INTRINSIC auto& operator--() // Throws: Nothing.
                {
                        assert(0 < m_value); assert(m_value <= N);
                        m_value = m_is->find_last_from(m_value - 1);
                        assert(0 <= m_value); assert(m_value < N);
                        return *this;
                }

                XSTD_PP_CONSTEXPR_INTRINSIC auto operator--(int) // Throws: Nothing.
                {
                        auto nrv = *this; --*this; return nrv;
                }

                friend constexpr auto operator==(proxy_iterator const& lhs, proxy_iterator const& rhs) noexcept
                {
                        assert(lhs.m_is == rhs.m_is);
                        return lhs.m_value == rhs.m_value;
                }

                friend constexpr auto operator!=(proxy_iterator const& lhs, proxy_iterator const& rhs) noexcept
                {
                        return !(lhs == rhs);
                }
        };

        friend XSTD_PP_CONSTEXPR_ALGORITHM auto operator==  <>(int_set const& /* lhs */, int_set const& /* rhs */) noexcept;
        friend XSTD_PP_CONSTEXPR_ALGORITHM auto operator<   <>(int_set const& /* lhs */, int_set const& /* rhs */) noexcept;
        friend XSTD_PP_CONSTEXPR_ALGORITHM bool is_subset_of<>(int_set const& /* lhs */, int_set const& /* rhs */) noexcept;
        friend XSTD_PP_CONSTEXPR_ALGORITHM bool intersects  <>(int_set const& /* lhs */, int_set const& /* rhs */) noexcept;
};

template<int N, class Block>
XSTD_PP_CONSTEXPR_ALGORITHM auto operator==(int_set<N, Block> const& lhs [[maybe_unused]], int_set<N, Block> const& rhs [[maybe_unused]]) noexcept
{
        constexpr auto num_logical_blocks = int_set<N, Block>::num_logical_blocks;
        if constexpr (num_logical_blocks == 0) {
                return true;
        } else if constexpr (num_logical_blocks == 1) {
                return lhs.m_data[0] == rhs.m_data[0];
        } else if constexpr (num_logical_blocks == 2) {
                constexpr auto tied = [](auto const& is) {
                        return std::tie(is.m_data[0], is.m_data[1]);
                };
                return tied(lhs) == tied(rhs);
        } else if constexpr (num_logical_blocks >= 3) {
                return std::equal(
                        std::begin(lhs.m_data), std::end(lhs.m_data),
                        std::begin(rhs.m_data), std::end(rhs.m_data)
                );
        }
}

template<int N, class Block>
XSTD_PP_CONSTEXPR_ALGORITHM auto operator!=(int_set<N, Block> const& lhs, int_set<N, Block> const& rhs) noexcept
{
        return !(lhs == rhs);
}

template<int N, class Block>
XSTD_PP_CONSTEXPR_ALGORITHM auto operator<(int_set<N, Block> const& lhs [[maybe_unused]], int_set<N, Block> const& rhs [[maybe_unused]]) noexcept
{
        constexpr auto num_logical_blocks = int_set<N, Block>::num_logical_blocks;
        if constexpr (num_logical_blocks == 0) {
                return false;
        } else if constexpr (num_logical_blocks == 1) {
                return rhs.m_data[0] < lhs.m_data[0];
        } else if constexpr (num_logical_blocks == 2) {
                constexpr auto tied = [](auto const& is) {
                        return std::tie(is.m_data[0], is.m_data[1]);
                };
                return tied(rhs) < tied(lhs);
        } else if constexpr (num_logical_blocks >= 3) {
                return std::lexicographical_compare(
                        std::begin(rhs.m_data), std::end(rhs.m_data),
                        std::begin(lhs.m_data), std::end(lhs.m_data)
                );
        }
}

template<int N, class Block>
XSTD_PP_CONSTEXPR_ALGORITHM auto operator>(int_set<N, Block> const& lhs, int_set<N, Block> const& rhs) noexcept
{
        return rhs < lhs;
}

template<int N, class Block>
XSTD_PP_CONSTEXPR_ALGORITHM auto operator>=(int_set<N, Block> const& lhs, int_set<N, Block> const& rhs) noexcept
{
        return !(lhs < rhs);
}

template<int N, class Block>
XSTD_PP_CONSTEXPR_ALGORITHM auto operator<=(int_set<N, Block> const& lhs, int_set<N, Block> const& rhs) noexcept
{
        return !(rhs < lhs);
}

template<int N, class Block>
constexpr auto operator~(int_set<N, Block> const& lhs) noexcept
{
        auto nrv{lhs}; nrv.complement(); return nrv;
}

template<int N, class Block>
constexpr auto operator&(int_set<N, Block> const& lhs, int_set<N, Block> const& rhs) noexcept
{
        auto nrv{lhs}; nrv &= rhs; return nrv;
}

template<int N, class Block>
constexpr auto operator|(int_set<N, Block> const& lhs, int_set<N, Block> const& rhs) noexcept
{
        auto nrv{lhs}; nrv |= rhs; return nrv;
}

template<int N, class Block>
constexpr auto operator^(int_set<N, Block> const& lhs, int_set<N, Block> const& rhs) noexcept
{
        auto nrv{lhs}; nrv ^= rhs; return nrv;
}

template<int N, class Block>
constexpr auto operator-(int_set<N, Block> const& lhs, int_set<N, Block> const& rhs) noexcept
{
        auto nrv{lhs}; nrv -= rhs; return nrv;
}

template<int N, class Block>
XSTD_PP_CONSTEXPR_ALGORITHM auto operator<<(int_set<N, Block> const& lhs, int const n) // Throws: Nothing.
{
        assert(0 <= n); assert(n < N);
        auto nrv{lhs}; nrv <<= n; return nrv;
}

template<int N, class Block>
XSTD_PP_CONSTEXPR_ALGORITHM auto operator>>(int_set<N, Block> const& lhs, int const n) // Throws: Nothing.
{
        assert(0 <= n); assert(n < N);
        auto nrv{lhs}; nrv >>= n; return nrv;
}

template<int N, class Block>
XSTD_PP_CONSTEXPR_ALGORITHM auto is_subset_of(int_set<N, Block> const& lhs [[maybe_unused]], int_set<N, Block> const& rhs [[maybe_unused]]) noexcept
        -> bool
{
        constexpr auto num_logical_blocks = int_set<N, Block>::num_logical_blocks;
        if constexpr (num_logical_blocks == 0) {
                return true;
        } else if constexpr (num_logical_blocks == 1) {
                return !(lhs.m_data[0] & ~rhs.m_data[0]);
        } else if constexpr (num_logical_blocks == 2) {
                return
                        !(lhs.m_data[0] & ~rhs.m_data[0]) &&
                        !(lhs.m_data[1] & ~rhs.m_data[1])
                ;
        } else if constexpr (num_logical_blocks >= 3) {
                return std::equal(
                        std::begin(lhs.m_data), std::end(lhs.m_data),
                        std::begin(rhs.m_data), std::end(rhs.m_data),
                        [](auto const wL, auto const wR)
                                -> bool
                        {
                                return !(wL & ~wR);
                        }
                );
        }
}

template<int N, class Block>
XSTD_PP_CONSTEXPR_ALGORITHM auto is_superset_of(int_set<N, Block> const& lhs, int_set<N, Block> const& rhs) noexcept
{
        return is_subset_of(rhs, lhs);
}

template<int N, class Block>
XSTD_PP_CONSTEXPR_ALGORITHM auto is_proper_subset_of(int_set<N, Block> const& lhs, int_set<N, Block> const& rhs) noexcept
{
        return is_subset_of(lhs, rhs) && !is_subset_of(rhs, lhs);
}

template<int N, class Block>
XSTD_PP_CONSTEXPR_ALGORITHM auto is_proper_superset_of(int_set<N, Block> const& lhs, int_set<N, Block> const& rhs) noexcept
{
        return is_superset_of(lhs, rhs) && !is_superset_of(rhs, lhs);
}

template<int N, class Block>
XSTD_PP_CONSTEXPR_ALGORITHM auto intersects(int_set<N, Block> const& lhs [[maybe_unused]], int_set<N, Block> const& rhs [[maybe_unused]]) noexcept
        -> bool
{
        constexpr auto num_logical_blocks = int_set<N, Block>::num_logical_blocks;
        if constexpr (num_logical_blocks == 0) {
                return false;
        } else if constexpr (num_logical_blocks == 1) {
                return lhs.m_data[0] & rhs.m_data[0];
        } else if constexpr (num_logical_blocks == 2) {
                return
                        (lhs.m_data[0] & rhs.m_data[0]) ||
                        (lhs.m_data[1] & rhs.m_data[1])
                ;
        } else if constexpr (num_logical_blocks >= 3) {
                return !std::equal(
                        std::begin(lhs.m_data), std::end(lhs.m_data),
                        std::begin(rhs.m_data), std::end(rhs.m_data),
                        [](auto const wL, auto const wR)
                                -> bool
                        {
                                return !(wL & wR);
                        }
                );
        }
}

template<int N, class Block>
XSTD_PP_CONSTEXPR_ALGORITHM auto disjoint(int_set<N, Block> const& lhs, int_set<N, Block> const& rhs) noexcept
{
        return !intersects(lhs, rhs);
}

template<int N, class Block>
XSTD_PP_CONSTEXPR_SWAP auto swap(int_set<N, Block>& lhs, int_set<N, Block>& rhs) noexcept(noexcept(lhs.swap(rhs)))
{
        lhs.swap(rhs);
}

template<int N, class Block>
XSTD_PP_CONSTEXPR_INTRINSIC auto begin(int_set<N, Block>& is) noexcept
        -> decltype(is.begin())
{
        return is.begin();
}

template<int N, class Block>
XSTD_PP_CONSTEXPR_INTRINSIC auto begin(int_set<N, Block> const& is) noexcept
        -> decltype(is.begin())
{
        return is.begin();
}

template<int N, class Block>
constexpr auto end(int_set<N, Block>& is) noexcept
        -> decltype(is.end())
{
        return is.end();
}

template<int N, class Block>
constexpr auto end(int_set<N, Block> const& is) noexcept
        -> decltype(is.end())
{
        return is.end();
}

template<int N, class Block>
constexpr auto rbegin(int_set<N, Block>& is) noexcept
        -> decltype(is.rbegin())
{
        return is.rbegin();
}

template<int N, class Block>
constexpr auto rbegin(int_set<N, Block> const& is) noexcept
        -> decltype(is.rbegin())
{
        return is.rbegin();
}

template<int N, class Block>
XSTD_PP_CONSTEXPR_INTRINSIC auto rend(int_set<N, Block>& is) noexcept
        -> decltype(is.rend())
{
        return is.rend();
}

template<int N, class Block>
XSTD_PP_CONSTEXPR_INTRINSIC auto rend(int_set<N, Block> const& is) noexcept
        -> decltype(is.rend())
{
        return is.rend();
}

template<int N, class Block>
XSTD_PP_CONSTEXPR_INTRINSIC auto cbegin(int_set<N, Block> const& is) noexcept
        -> decltype(xstd::begin(is))
{
        return xstd::begin(is);
}

template<int N, class Block>
constexpr auto cend(int_set<N, Block> const& is) noexcept
        -> decltype(xstd::end(is))
{
        return xstd::end(is);
}

template<int N, class Block>
constexpr auto crbegin(int_set<N, Block> const& is) noexcept
        -> decltype(xstd::rbegin(is))
{
        return xstd::rbegin(is);
}

template<int N, class Block>
XSTD_PP_CONSTEXPR_INTRINSIC auto crend(int_set<N, Block> const& is) noexcept
        -> decltype(xstd::rend(is))
{
        return xstd::rend(is);
}

template<int N, class Block>
XSTD_PP_CONSTEXPR_ALGORITHM auto size(int_set<N, Block> const& is) noexcept
        -> decltype(is.size())
{
        return is.size();
}

template<int N, class Block>
[[nodiscard]] XSTD_PP_CONSTEXPR_ALGORITHM auto empty(int_set<N, Block> const& is) noexcept
        -> decltype(is.empty())
{
        return is.empty();
}

}       // namespace xstd
