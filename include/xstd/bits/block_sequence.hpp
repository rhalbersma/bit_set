//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_BITS_BLOCK_SEQUENCE_HPP
#define XSTD_BITS_BLOCK_SEQUENCE_HPP

#include <boost/hash2/hash_append_fwd.hpp>                     // hash_append, hash_append_tag
#include <xstd/bits/detail/intrin.hpp>                         // countl_zero, countr_zero, popcount
#include <xstd/bits/detail/pred.hpp>                           // intersects, is_subset_of, not_equal_to
#include <xstd/ints/concepts/unsigned_integer.hpp>             // unsigned_integer
#include <xstd/ints/cstdlib/div.hpp>                           // div, div_result
#include <xstd/ints/limits.hpp>                                // numeric_limits
#include <xstd/ints/memory.hpp>                                // align_up
#include <xstd/misc/type_traits/conditional_data_member.hpp>   // XSTD_NO_UNIQUE_ADDRESS, conditional_data_member_t
#include <algorithm>                                           // all_of, any_of, equal, fill, fill_n, find_if, fold_left, max, shift_left, shift_right
#include <array>                                               // array
#include <cassert>                                             // assert
#include <concepts>                                            // swap
#include <cstddef>                                             // ptrdiff_t, size_t
#include <functional>                                          // plus
#include <iterator>                                            // distance, prev, random_access_iterator, sized_sentinel_for
#include <memory>                                              // allocator
#include <ranges>                                              // begin, drop, iota, reverse, size, swap, transform, zip
                                                               // (views::drop_last when P22014R2 is accepted)
#include <span>                                                // dynamic_extent
#include <type_traits>                                         // conditional_t, is_const_v, is_nothrow_swappable_v, remove_reference_t
#include <utility>                                             // pair
#include <vector>                                              // vector

namespace xstd {

// What block_sequence packs bits into: a contiguous, sized range of unsigned integers.
// std::array and std::vector both qualify, and so does std::inplace_vector -- runtime
// width over static capacity, for free.
//
// Storage, not container: xstd::ranges::block_range is the other side of the same word,
// and asks whether a bit container will hand its blocks over. This one asks whether a
// range is blocks. Nothing models both, and no scope sees both unqualified.
template<class R>
concept block_storage =
        std::ranges::contiguous_range<R> and
        std::ranges::sized_range<R> and
        xstd::unsigned_integer<std::ranges::range_value_t<R>>
;

// The block count a width of N bits needs, floored at one so that even a zero-width
// block_sequence has a block to name. Free rather than a member, because block_array below
// has to spell it inside block_sequence's own template argument list.
template<xstd::unsigned_integer Block, std::size_t N>
inline constexpr auto num_blocks_v = std::ranges::max(
        align_up(N, static_cast<std::size_t>(xstd::numeric_limits<Block>::digits)) /
                    static_cast<std::size_t>(xstd::numeric_limits<Block>::digits),
        1UZ
);

// The one storage vehicle. N is the width when that is a constant, and
// std::dynamic_extent when the width is carried at run time instead.
//
// It owns the unused-tail invariant -- every bit at or above size() reads zero, which is
// what makes whole-block comparison, popcount and the block-at-a-time scans mean
// anything -- and the whole primitive vocabulary over it.
//
// No iterators and no proxies: those are readings, and a reading here would be picking
// one. It would also make std::ranges see a sequence of blocks.
//
// The hand-unrolled one- and two-block cases below are where the static performance
// lives, so they stay compile-time branches; the general path they fall through to is
// written over m_blocks as a range and therefore serves the dynamic width unchanged.
template<block_storage Blocks, std::size_t N = std::dynamic_extent>
class block_sequence
{
public:
        using block_type = std::ranges::range_value_t<Blocks>;

        static constexpr auto bits_per_block  = static_cast<std::size_t>(xstd::numeric_limits<block_type>::digits);
        static constexpr auto has_static_size = N != std::dynamic_extent;

private:
        static constexpr auto static_num_bits   = has_static_size ? align_up(N, bits_per_block) : 0UZ;
        static constexpr auto static_num_blocks = has_static_size ? std::ranges::max(static_num_bits / bits_per_block, 1UZ) : 0UZ;
        static constexpr auto static_last_block = static_num_blocks - 1UZ;

        static constexpr auto left_bit = bits_per_block - 1UZ;
        static constexpr auto unit     = static_cast<block_type>( 1);
        static constexpr auto zero     = static_cast<block_type>( 0);
        static constexpr auto ones     = static_cast<block_type>(-1);

        // The padding above the width, and the mask of the last block that is not padding.
        //
        // num_bits is align_up(N), so num_bits - N is in [0, bits_per_block) and the shift
        // is always in range. Width zero is the one case that form cannot express -- there
        // is nothing to align up, so it reports no padding where in truth the sole block is
        // all of it -- and it gets the selection instead. Naming zero rather than computing
        // it matters on MSVC, which constant-folds both arms of a ?: and answers C4293,
        // shift count too big, on the one it discards.
        static constexpr auto static_num_unused_bits = has_static_size ? static_num_bits - N : 0UZ;
        static constexpr auto static_used_bits       = has_static_size and N == 0 ? zero : static_cast<block_type>(ones >> static_num_unused_bits);
        static constexpr auto static_unused_bits     = static_cast<block_type>(~static_used_bits);
        static constexpr auto static_has_unused_bits = has_static_size and static_used_bits != ones;

        // A defaulted default constructor plus an NSDMI, rather than two constructors
        // constrained on the extent: std::vector default-constructs empty, and the
        // at-least-one-block invariant has to hold from the start.
        [[nodiscard]] static constexpr auto make_blocks(std::size_t n) -> Blocks
        {
                if constexpr (has_static_size) {
                        return Blocks{};
                } else {
                        return Blocks(std::ranges::max(align_up(n, bits_per_block) / bits_per_block, 1UZ));
                }
        }

        Blocks m_blocks = make_blocks(0UZ);

        // Present only for a dynamic width; empty_type<size_tag> otherwise, and the tag is
        // what keeps it distinct from any other absent member in an enclosing layout.
        [[XSTD_NO_UNIQUE_ADDRESS]]
        conditional_data_member_t<not has_static_size, std::size_t, struct size_tag> m_size{};

public:
        block_sequence() = default;

        // The width is a constructor argument exactly when it is not a template argument.
        [[nodiscard]] constexpr explicit block_sequence(std::size_t n)
                requires (not has_static_size)
        :
                m_blocks(make_blocks(n)),
                m_size(n)
        {}

        [[nodiscard]] constexpr auto size() const noexcept
                -> std::size_t
        {
                if constexpr (has_static_size) {
                        return N;
                } else {
                        return m_size;
                }
        }

        [[nodiscard]] constexpr auto num_blocks() const noexcept
                -> std::size_t
        {
                if constexpr (has_static_size) {
                        return static_num_blocks;
                } else {
                        return std::ranges::size(m_blocks);
                }
        }

        // The block, for xstd::ranges::block_access; padding above size() stays zero, which is what makes whole-block comparison mean anything.
        [[nodiscard]] constexpr auto block(std::size_t i) const noexcept
                -> block_type
        {
                assert(i < num_blocks());
                return m_blocks[i];
        }

        // The write side of block(). Not a bit_traits entry: block writes are only ever
        // needed on storage we control, and the unused tail is this class's to keep.
        constexpr void set_block(std::size_t i, block_type value) noexcept
        {
                assert(i < num_blocks());
                m_blocks[i] = value;
                erase_unused();
        }

        [[nodiscard]] friend constexpr auto operator==(block_sequence const& x [[maybe_unused]], block_sequence const& y [[maybe_unused]]) noexcept
                -> bool
        {
                if constexpr (has_static_size and N == 0) {
                        return true;
                } else if constexpr (has_static_size) {
                        return std::ranges::equal(x.m_blocks, y.m_blocks);
                } else {
                        return x.m_size == y.m_size and std::ranges::equal(x.m_blocks, y.m_blocks);
                }
        }

        // No operator<=>: block_sequence is pure storage with no opinion on set-of-indices versus sequence-of-bools order; == is unaffected.

        template<class Provider, class Hash, class Flavor>
        friend constexpr void tag_invoke(boost::hash2::hash_append_tag const&, Provider const&, Hash& h, Flavor const& f, block_sequence const* v) noexcept
        {
                boost::hash2::hash_append(h, f, v->m_blocks);
        }

        [[nodiscard]] constexpr auto find_front() const noexcept
                -> std::size_t
        {
                assert(any());
                if constexpr (has_static_size and static_num_blocks == 1) {
                        return detail::bits::countr_zero(m_blocks[0]);
                } else if constexpr (has_static_size and static_num_blocks == 2) {
                        return m_blocks[0] != zero ? detail::bits::countr_zero(m_blocks[0]) : detail::bits::countr_zero(m_blocks[1]) + bits_per_block;
                } else {
                        auto const front = std::ranges::find_if(m_blocks, [](auto block) { return block != zero; });
                        assert(front != std::ranges::end(m_blocks));
                        return detail::bits::countr_zero(*front) + (bits_per_block * distance(std::ranges::begin(m_blocks), front));
                }
        }

        [[nodiscard]] constexpr auto find_back() const noexcept
                -> std::size_t
        {
                assert(any());
                if constexpr (has_static_size and static_num_blocks == 1) {
                        return last_bit() - detail::bits::countl_zero(m_blocks[0]);
                } else if constexpr (has_static_size and static_num_blocks == 2) {
                        return m_blocks[1] != zero ? last_bit() - detail::bits::countl_zero(m_blocks[1]) : left_bit - detail::bits::countl_zero(m_blocks[0]);
                } else {
                        auto const rg = m_blocks | std::views::reverse;
                        auto const back = std::ranges::find_if(rg, [](auto block) { return block != zero; });
                        assert(back != std::ranges::end(rg));
                        return last_bit() - detail::bits::countl_zero(*back) - (bits_per_block * distance(std::ranges::begin(rg), back));
                }
        }

        // Its own 0. Now that lower_bound carries the 2-block case too, this emits the same
        // instruction sequence the hand-written version did -- verified, not assumed -- so the
        // duplication goes without costing the unrolling. libstdc++ writes both scans out and
        // repeats the word loop and the ctz-plus-index arithmetic between them; boost factors
        // the shared tail into m_do_find_from, but only at block granularity, so find_next still
        // needs its own prologue for a mid-block start. Factoring at bit granularity subsumes
        // both: a block-aligned start is just offset == 0.
        [[nodiscard]] constexpr auto find_first() const noexcept
                -> std::size_t
        {
                return lower_bound(0UZ);
        }

        [[nodiscard]] constexpr auto find_last() const noexcept
                -> std::size_t
        {
                return size();
        }

        // The first set position at or above n, or size() if there is none. This is the whole
        // scan: find_first is lower_bound(0) and find_next is lower_bound(n + 1). Expressed the
        // other way round -- as a strictly-greater scan, which is what boost::dynamic_bitset and
        // libstdc++'s _M_do_find_next expose -- find_first would have to be find_next(-1), and
        // size_t has no such value. That is why the container had to branch on x == 0.
        //
        // n == size() rather than n >= size(): the precondition is n <= size(), asserted just
        // below, so size() is the only value the scan cannot start from. is_valid does not say
        // this -- it is n < size() -- and size() is a legitimate argument here, meaning "no such
        // position", exactly as it is for find_prev.
        [[nodiscard]] constexpr auto lower_bound(std::size_t n) const noexcept
                -> std::size_t
        {
                assert(n <= size());
                if (n == size()) {
                        return size();
                }
                if constexpr (has_static_size and static_num_blocks == 1) {
                        if (auto const block = static_cast<block_type>(m_blocks[0] >> n); block != zero) {
                                return n + detail::bits::countr_zero(block);
                        }
                } else if constexpr (has_static_size and static_num_blocks == 2) {
                        // Two blocks, so the tail is one named block rather than a range: the drop
                        // and find_if below cost more than the whole scan is worth at this width.
                        // index is a run-time value here -- n is -- but the block count is not, so
                        // the second half is a test and not a loop.
                        auto const [ index, offset ] = index_offset(n);
                        if (index == 0) {
                                if (auto const block = static_cast<block_type>(m_blocks[0] >> offset); block != zero) {
                                        return n + detail::bits::countr_zero(block);
                                }
                                if (m_blocks[1] != zero) {
                                        return bits_per_block + detail::bits::countr_zero(m_blocks[1]);
                                }
                        } else if (auto const block = static_cast<block_type>(m_blocks[1] >> offset); block != zero) {
                                return n + detail::bits::countr_zero(block);
                        }
                } else {
                        auto [ index, offset ] = index_offset(n);
                        if (offset != 0) {
                                if (auto const block = static_cast<block_type>(m_blocks[index] >> offset); block != zero) {
                                        return n + detail::bits::countr_zero(block);
                                }
                                ++index;
                                n += bits_per_block - offset;
                        }
                        auto const rg = m_blocks | std::views::drop(index);
                        // next is an iterator. That std::array's is a pointer is
                        // implementation-defined -- [array.overview] requires only that it model
                        // contiguous_iterator -- so readability-qualified-auto's auto const *const
                        // would encode a detail this code never relies on: next is dereferenced and
                        // passed to distance, both pure iterator operations.
                        if (auto const next = std::ranges::find_if(rg, [](auto block) { return block != zero; }); next != std::ranges::end(rg)) {  // NOLINT(readability-qualified-auto)
                                return n + detail::bits::countr_zero(*next) + (bits_per_block * distance(std::ranges::begin(rg), next));
                        }
                }
                return size();
        }

        [[nodiscard]] constexpr auto find_next(std::size_t n) const noexcept
                -> std::size_t
        {
                assert(is_valid(n));
                return lower_bound(n + 1);
        }

        [[nodiscard]] constexpr auto find_prev(std::size_t n) const noexcept
                -> std::size_t
        {
                assert(any());
                --n;
                if constexpr (has_static_size and static_num_blocks == 1) {
                        return n - detail::bits::countl_zero(static_cast<block_type>(m_blocks[0] << (left_bit - n)));
                } else {
                        auto [ index, offset ] = index_offset(n);
                        if (auto const reverse_offset = left_bit - offset; reverse_offset != 0) {
                                if (auto const block = static_cast<block_type>(m_blocks[index] << reverse_offset); block != zero) {
                                        return n - detail::bits::countl_zero(block);
                                }
                                --index;
                                n -= bits_per_block - reverse_offset;
                        }
                        auto const rg = m_blocks | std::views::reverse | std::views::drop(last_block() - index);
                        auto const prev = std::ranges::find_if(rg, [](auto block) { return block != zero; });
                        assert(prev != std::ranges::end(rg));
                        return n - detail::bits::countl_zero(*prev) - (bits_per_block * distance(std::ranges::begin(rg), prev));
                }
        }

        constexpr void operator&=(block_sequence const& other [[maybe_unused]]) noexcept
        {
                assert(this->size() == other.size());
                if constexpr (has_static_size and N > 0 and static_num_blocks == 1) {
                        this->m_blocks[0] &= other.m_blocks[0];
                } else if constexpr (has_static_size and static_num_blocks == 2) {
                        this->m_blocks[0] &= other.m_blocks[0];
                        this->m_blocks[1] &= other.m_blocks[1];
                } else if constexpr (not (has_static_size and N == 0)) {
                        for (auto const i : std::views::iota(0UZ, num_blocks())) {
                                this->m_blocks[i] &= other.m_blocks[i];
                        }
                }
        }

        constexpr void operator|=(block_sequence const& other [[maybe_unused]]) noexcept
        {
                assert(this->size() == other.size());
                if constexpr (has_static_size and N > 0 and static_num_blocks == 1) {
                        this->m_blocks[0] |= other.m_blocks[0];
                } else if constexpr (has_static_size and static_num_blocks == 2) {
                        this->m_blocks[0] |= other.m_blocks[0];
                        this->m_blocks[1] |= other.m_blocks[1];
                } else if constexpr (not (has_static_size and N == 0)) {
                        for (auto const i : std::views::iota(0UZ, num_blocks())) {
                                this->m_blocks[i] |= other.m_blocks[i];
                        }
                }
        }

        constexpr void operator^=(block_sequence const& other [[maybe_unused]]) noexcept
        {
                assert(this->size() == other.size());
                if constexpr (has_static_size and N > 0 and static_num_blocks == 1) {
                        this->m_blocks[0] ^= other.m_blocks[0];
                } else if constexpr (has_static_size and static_num_blocks == 2) {
                        this->m_blocks[0] ^= other.m_blocks[0];
                        this->m_blocks[1] ^= other.m_blocks[1];
                } else if constexpr (not (has_static_size and N == 0)) {
                        for (auto const i : std::views::iota(0UZ, num_blocks())) {
                                this->m_blocks[i] ^= other.m_blocks[i];
                        }
                }
        }

        constexpr void operator-=(block_sequence const& other [[maybe_unused]]) noexcept
        {
                assert(this->size() == other.size());
                if constexpr (has_static_size and N > 0 and static_num_blocks == 1) {
                        this->m_blocks[0] &= static_cast<block_type>(~other.m_blocks[0]);
                } else if constexpr (has_static_size and static_num_blocks == 2) {
                        this->m_blocks[0] &= static_cast<block_type>(~other.m_blocks[0]);
                        this->m_blocks[1] &= static_cast<block_type>(~other.m_blocks[1]);
                } else if constexpr (not (has_static_size and N == 0)) {
                        for (auto const i : std::views::iota(0UZ, num_blocks())) {
                                this->m_blocks[i] &= static_cast<block_type>(~other.m_blocks[i]);
                        }
                }
        }

        constexpr void operator<<=(std::size_t n [[maybe_unused]]) noexcept
        {
                assert(is_valid(n));
                if constexpr (has_static_size and static_num_blocks == 1) {
                        // m_blocks[0] <<= n narrows the promoted int back to a block_type implicitly, which -fsanitize=implicit-conversion aborts on once a bit shifts out.
                        m_blocks[0] = static_cast<block_type>(m_blocks[0] << n);
                } else {
                        auto const [ n_blocks, L_shift ] = xstd::div(n, bits_per_block);
                        // Implied by is_valid(n) above, and stated again because GCC does not
                        // carry the range through xstd::div's aggregate return: without it,
                        // -Warray-bounds reports the memmove inside shift_right at -O1 and -O2
                        // whenever asserts are live. No CMake build type is that combination --
                        // Debug is -O0 and the optimized ones carry NDEBUG -- but -O2 -g is one
                        // command away, and the bound is worth saying in any case.
                        assert(n_blocks <= last_block());
                        if (L_shift == 0) {
                                std::shift_right(std::ranges::begin(m_blocks), std::ranges::end(m_blocks), static_cast<std::ptrdiff_t>(n_blocks));
                        } else {
                                auto const R_shift = bits_per_block - L_shift;
                                for (auto i = last_block(); i > n_blocks; --i) {
                                        m_blocks[i] = static_cast<block_type>(static_cast<block_type>(m_blocks[i - n_blocks] << L_shift) | static_cast<block_type>(m_blocks[i - n_blocks - 1] >> R_shift));
                                }
                                m_blocks[n_blocks] = static_cast<block_type>(m_blocks[0] << L_shift);
                        }
                        std::ranges::fill_n(std::ranges::begin(m_blocks), static_cast<std::ptrdiff_t>(n_blocks), zero);
                }
                erase_unused();
        }

        constexpr void operator>>=(std::size_t n [[maybe_unused]]) noexcept
        {
                assert(is_valid(n));
                if constexpr (has_static_size and static_num_blocks == 1) {
                        // m_blocks[0] >>= n narrows the promoted int back to a block_type implicitly, which -fsanitize=implicit-conversion instruments.
                        m_blocks[0] = static_cast<block_type>(m_blocks[0] >> n);
                } else {
                        auto const [ n_blocks, R_shift ] = xstd::div(n, bits_per_block);
                        // See operator<<=: the same bound, for the same reason.
                        assert(n_blocks <= last_block());
                        if (R_shift == 0) {
                                std::shift_left(std::ranges::begin(m_blocks), std::ranges::end(m_blocks), static_cast<std::ptrdiff_t>(n_blocks));
                        } else {
                                auto const L_shift = bits_per_block - R_shift;
                                for (auto i = 0UZ; i + n_blocks < last_block(); ++i) {
                                        m_blocks[i] = static_cast<block_type>(static_cast<block_type>(m_blocks[i + n_blocks] >> R_shift) | static_cast<block_type>(m_blocks[i + n_blocks + 1] << L_shift));
                                }
                                m_blocks[last_block() - n_blocks] = static_cast<block_type>(m_blocks[last_block()] >> R_shift);
                        }
                        std::ranges::fill_n(std::ranges::prev(std::ranges::end(m_blocks), static_cast<std::ptrdiff_t>(n_blocks)), static_cast<std::ptrdiff_t>(n_blocks), zero);
                }
        }

        constexpr void set() noexcept
        {
                if constexpr (has_static_size and static_has_unused_bits) {
                        std::ranges::fill_n(std::ranges::begin(m_blocks), static_cast<std::ptrdiff_t>(static_last_block), ones);
                        m_blocks[static_last_block] = static_used_bits;
                } else if constexpr (has_static_size and N > 0) {
                        std::ranges::fill(m_blocks, ones);
                } else if constexpr (not has_static_size) {
                        // Uniform, because used_bits() is the whole block when the width divides
                        // evenly and none of it at width zero, where the sole block is all padding.
                        std::ranges::fill_n(std::ranges::begin(m_blocks), static_cast<std::ptrdiff_t>(last_block()), ones);
                        m_blocks[last_block()] = used_bits();
                }
                assert(all());
        }

        constexpr void reset() noexcept
        {
                std::ranges::fill(m_blocks, zero);
                assert(none());
        }

        constexpr void flip() noexcept
        {
                if constexpr (has_static_size and N > 0 and static_num_blocks == 1) {
                        m_blocks[0] = static_cast<block_type>(~m_blocks[0]);
                } else if constexpr (has_static_size and static_num_blocks == 2) {
                        m_blocks[0] = static_cast<block_type>(~m_blocks[0]);
                        m_blocks[1] = static_cast<block_type>(~m_blocks[1]);
                } else if constexpr (not (has_static_size and N == 0)) {
                        for (auto const i : std::views::iota(0UZ, num_blocks())) {
                                m_blocks[i] = static_cast<block_type>(~m_blocks[i]);
                        }
                }
                erase_unused();
        }

        constexpr void swap(block_sequence& other) noexcept(std::is_nothrow_swappable_v<Blocks>)
        {
                // m_size is empty_type under a static width, and swapping that is a no-op.
                std::ranges::swap(this->m_blocks, other.m_blocks);
                std::ranges::swap(this->m_size,   other.m_size);
        }

        constexpr void set(std::size_t n) noexcept
        {
                assert(is_valid(n));
                auto&& [ block, mask ] = block_mask(n);
                block |= mask;
                assert((*this)[n]);
        }

        [[nodiscard]] constexpr auto insert(std::size_t n) noexcept
                -> bool
        {
                assert(is_valid(n));
                auto&& [ block, mask ] = block_mask(n);
                auto const inserted = not detail::bits::intersects(block, mask);
                block |= mask;
                assert((*this)[n]);
                return inserted;
        }

        constexpr void reset(std::size_t n) noexcept
        {
                assert(is_valid(n));
                auto&& [ block, mask ] = block_mask(n);
                block &= static_cast<block_type>(~mask);
                assert(not (*this)[n]);
        }

        [[nodiscard]] constexpr auto erase(std::size_t n) noexcept
                -> bool
        {
                assert(is_valid(n));
                auto&& [ block, mask ] = block_mask(n);
                auto const erased = detail::bits::intersects(block, mask);
                block &= static_cast<block_type>(~mask);
                assert(not (*this)[n]);
                return erased;
        }

        constexpr void flip(std::size_t n) noexcept
        {
                assert(is_valid(n));
                auto&& [ block, mask ] = block_mask(n);
                block ^= mask;
        }

        [[nodiscard]] constexpr auto operator[](std::size_t n) const noexcept
                -> bool
        {
                assert(is_valid(n));
                auto&& [ block, mask ] = block_mask(n);
                return detail::bits::intersects(block, mask);
        }

        [[nodiscard]] constexpr auto count() const noexcept
                -> std::size_t
        {
                if constexpr (has_static_size and N == 0) {
                        return 0UZ;
                } else if constexpr (has_static_size and static_num_blocks == 1) {
                        return detail::bits::popcount(m_blocks[0]);
                } else if constexpr (has_static_size and static_num_blocks == 2) {
                        return detail::bits::popcount(m_blocks[0]) + detail::bits::popcount(m_blocks[1]);
                } else {
                        return std::ranges::fold_left(
                                m_blocks | std::views::transform([](auto block) { return detail::bits::popcount(block); }),
                                0UZ, std::plus<>()
                        );
                }
        }

        [[nodiscard]] constexpr auto all() const noexcept
                -> bool
        {
                if constexpr (has_static_size and static_has_unused_bits) {
                        if constexpr (static_num_blocks == 1) {
                                return m_blocks[0] == static_used_bits;
                        } else if constexpr (static_num_blocks == 2) {
                                return m_blocks[0] == ones and m_blocks[1] == static_used_bits;
                        } else {
                                return all_but_last_are_ones() and m_blocks[static_last_block] == static_used_bits;
                        }
                } else if constexpr (has_static_size) {
                        if constexpr (N == 0) {
                                return true;
                        } else if constexpr (static_num_blocks == 1) {
                                return m_blocks[0] == ones;
                        } else if constexpr (static_num_blocks == 2) {
                                return m_blocks[0] == ones and m_blocks[1] == ones;
                        } else {
                                return std::ranges::all_of(m_blocks, [](auto block) { return block == ones; });
                        }
                } else {
                        // One shape for both, because used_bits() names the full block when the
                        // width divides evenly; the static arms keep the split only to stay
                        // compile-time branches.
                        return all_but_last_are_ones() and m_blocks[last_block()] == used_bits();
                }
        }

        [[nodiscard]] constexpr auto any() const noexcept
                -> bool
        {
                return not none();
        }

        [[nodiscard]] constexpr auto none() const noexcept
                -> bool
        {
                if constexpr (has_static_size and N == 0) {
                        return true;
                } else if constexpr (has_static_size and static_num_blocks == 1) {
                        return m_blocks[0] == zero;
                } else if constexpr (has_static_size and static_num_blocks == 2) {
                        return m_blocks[0] == zero and m_blocks[1] == zero;
                } else {
                        return std::ranges::all_of(m_blocks, [](auto block) { return block == zero; });
                }
        }

        [[nodiscard]] constexpr auto is_subset_of(block_sequence const& other [[maybe_unused]]) const noexcept
                -> bool
        {
                assert(this->size() == other.size());
                if constexpr (has_static_size and N == 0) {
                        return true;
                } else if constexpr (has_static_size and static_num_blocks == 1) {
                        return detail::bits::is_subset_of(this->m_blocks[0], other.m_blocks[0]);
                } else if constexpr (has_static_size and static_num_blocks == 2) {
                        return
                                detail::bits::is_subset_of(this->m_blocks[0], other.m_blocks[0]) and
                                detail::bits::is_subset_of(this->m_blocks[1], other.m_blocks[1])
                        ;
                } else {
                        return std::ranges::all_of(
                                std::views::zip(this->m_blocks, other.m_blocks), [](auto&& _) { auto&& [ lhs, rhs] = _;
                                return detail::bits::is_subset_of(lhs, rhs);
                        });
                }
        }

        [[nodiscard]] constexpr auto is_proper_subset_of(block_sequence const& other [[maybe_unused]]) const noexcept
                -> bool
        {
                assert(this->size() == other.size());
                if constexpr (has_static_size and N == 0) {
                        return false;
                } else if constexpr (has_static_size and static_num_blocks == 1) {
                        return
                                detail::bits::is_subset_of (this->m_blocks[0], other.m_blocks[0]) and
                                detail::bits::not_equal_to(this->m_blocks[0], other.m_blocks[0])
                        ;
                } else if constexpr (has_static_size and static_num_blocks == 2) {
                        if (not detail::bits::is_subset_of(this->m_blocks[0], other.m_blocks[0])) {
                                return false;
                        }
                        if (detail::bits::not_equal_to(this->m_blocks[0], other.m_blocks[0])) {
                                return detail::bits::is_subset_of(this->m_blocks[1], other.m_blocks[1]);
                        }
                        return
                                detail::bits::is_subset_of (this->m_blocks[1], other.m_blocks[1]) and
                                detail::bits::not_equal_to(this->m_blocks[1], other.m_blocks[1])
                        ;
                } else {
                        auto i = 0UZ;
                        while (i < num_blocks()) {
                                if (not detail::bits::is_subset_of(this->m_blocks[i], other.m_blocks[i])) {
                                        return false;
                                }
                                if (    detail::bits::not_equal_to(this->m_blocks[i], other.m_blocks[i])) {
                                        break;
                                }
                                ++i;
                        }
                        return (i == num_blocks()) ? false : std::ranges::all_of(
                                std::views::zip(this->m_blocks, other.m_blocks) | std::views::drop(i), [](auto&& _) { auto&& [ lhs, rhs ] = _;
                                return detail::bits::is_subset_of(lhs, rhs);
                        });
                }
        }

        [[nodiscard]] constexpr auto intersects(block_sequence const& other [[maybe_unused]]) const noexcept
                -> bool
        {
                assert(this->size() == other.size());
                if constexpr (has_static_size and N == 0) {
                        return false;
                } else if constexpr (has_static_size and static_num_blocks == 1) {
                        return detail::bits::intersects(this->m_blocks[0], other.m_blocks[0]);
                } else if constexpr (has_static_size and static_num_blocks == 2) {
                        return
                                detail::bits::intersects(this->m_blocks[0], other.m_blocks[0]) or
                                detail::bits::intersects(this->m_blocks[1], other.m_blocks[1])
                        ;
                } else {
                        return std::ranges::any_of(
                                std::views::zip(this->m_blocks, other.m_blocks), [](auto&& _) { auto&& [ lhs, rhs ] = _;
                                return detail::bits::intersects(lhs, rhs);
                        });
                }
        }

private:
        [[nodiscard]] constexpr auto last_block() const noexcept
                -> std::size_t
        {
                return num_blocks() - 1UZ;
        }

        // Every block but the last, which is the half of all() that padding cannot reach.
        //
        // An iterator pair rather than views::take: libc++ 18 -- Xcode 16.4 on the matrix --
        // writes the return type of views::take's iota_view fast path as
        // decltype(iota_view(*begin(rng), ...)), so forming the adaptor's operator| over a
        // range of 128-bit blocks instantiates an iota_view over that block type, however
        // unlike an iota_view a std::vector is. That trips libc++'s "bigger than integer-like
        // type" static_assert, which is a hard error rather than a substitution failure.
        // views::drop, views::reverse, views::transform and views::zip are all clear; only
        // take carries it, and only until Xcode 16.4 leaves the matrix.
        [[nodiscard]] constexpr auto all_but_last_are_ones() const noexcept
                -> bool
        {
                auto const first = std::ranges::begin(m_blocks);
                return std::ranges::all_of(first, first + static_cast<std::ptrdiff_t>(last_block()), [](auto block) { return block == ones; });
        }

        // The top bit of the last block, padding included. find_back and find_prev count
        // down from it and both assert any(), so the zero-width case never reaches here.
        [[nodiscard]] constexpr auto last_bit() const noexcept
                -> std::size_t
        {
                return (num_blocks() * bits_per_block) - 1UZ;
        }

        // static_used_bits at a run-time width, and the same two cases. Above width zero the
        // padding is num_blocks() * bits_per_block - size(), which is in [0, bits_per_block)
        // and so always a shift in range; at width zero the sole block is entirely padding,
        // which that expression would report as none, so the selection answers instead.
        [[nodiscard]] constexpr auto used_bits() const noexcept
                -> block_type
        {
                return size() == 0 ? zero : static_cast<block_type>(ones >> ((num_blocks() * bits_per_block) - size()));
        }

        [[nodiscard]] constexpr auto is_valid(std::size_t n [[maybe_unused]]) const noexcept
                -> bool
        {
                if constexpr (has_static_size and N == 0) {
                        // Unreachable: only an assert calls is_valid, and a zero-width block_sequence has no member that reaches one. Not removable either - MSVC's /W4 rejects a bare n < N as always false (C4296).
                        return false;                   // GCOVR_EXCL_LINE
                } else {
                        return n < size();
                }
        }

        [[nodiscard]] static constexpr auto index_offset(std::size_t n) noexcept
                -> xstd::div_result<std::size_t>
        {
                if constexpr (has_static_size and static_num_blocks == 1) {
                        return { .quotient = 0UZ, .remainder = n };
                } else {
                        return xstd::div(n, bits_per_block);
                }
        }

        // cl rejects a member of the explicit object parameter in a trailing return type (C2228); bit_array.hpp's result_t is the same idiom.
        template<class Self>
        using block_reference_t = std::conditional_t<
                std::is_const_v<std::remove_reference_t<Self>>, block_type const&, block_type&>;

        [[nodiscard]] constexpr auto block_mask(this auto&& self, std::size_t n) noexcept
                -> std::pair<block_reference_t<decltype(self)>, block_type>
        {
                auto const [ index, offset ] = index_offset(n);
                return { std::forward<decltype(self)>(self).m_blocks[index], static_cast<block_type>(unit << offset) };
        }

        constexpr void erase_unused() noexcept
        {
                if constexpr (has_static_size and static_has_unused_bits) {
                        m_blocks[static_last_block] &= static_used_bits;
                        assert(not detail::bits::intersects(m_blocks[static_last_block], static_unused_bits));
                } else if constexpr (not has_static_size) {
                        m_blocks[last_block()] &= used_bits();
                }
        }

        // Iterators are taken by value, as std::ranges::distance itself takes them.
        //
        // performance-unnecessary-value-param flags both parameters, and only for the
        // reverse_iterator instantiations. libstdc++ hand-writes that class's copy
        // constructor -- a pre-"= default" idiom; the copy assignment beside it is
        // defaulted -- so it is not trivially copyable, where libc++'s is and the check
        // stays silent. The verdict is a property of the standard library rather than of
        // this signature.
        //
        // Nor is the cost it names paid here. Not trivially copyable means non-trivial
        // for the purposes of calls under the Itanium ABI, so an out-of-line callee takes
        // a pointer to a caller-built temporary where a trivially copyable type of the
        // same eight bytes arrives in a register. This is a static constexpr one-liner:
        // at -O2 it inlines and both iterators fold away to a pointer subtraction.
        template<std::random_access_iterator I, std::sized_sentinel_for<I> S>
        [[nodiscard]] static constexpr auto distance(I first, S last) noexcept  // NOLINT(performance-unnecessary-value-param)
        {
                return static_cast<std::size_t>(std::ranges::distance(first, last));
        }
};

// The two storage vehicles the library ships, each named and ordered after what it holds.
// std::inplace_vector needs no alias of its own: it satisfies block_storage, so
// block_sequence<std::inplace_vector<Block, K>> already gives a runtime width over static
// capacity.
//
// Block leads in both, as it does in std::array and std::vector. N cannot be defaulted
// behind it, but nothing wants to: the three containers pass their own N and Block on.
template<xstd::unsigned_integer Block, std::size_t N>
using block_array = block_sequence<std::array<Block, num_blocks_v<Block, N>>, N>;

template<xstd::unsigned_integer Block = std::size_t, class Allocator = std::allocator<Block>>
using block_vector = block_sequence<std::vector<Block, Allocator>>;

}       // namespace xstd

#endif  // XSTD_BITS_BLOCK_SEQUENCE_HPP
