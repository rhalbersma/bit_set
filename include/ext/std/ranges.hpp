#ifndef EXT_STD_RANGES_HPP
#define EXT_STD_RANGES_HPP

//          Copyright Rein Halbersma 2014-2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <ranges>       // begin, end, range_adaptor_closure, range_value_t, view_interface
                        // all_t 
                        // input_range, view, viewable_range
#include <utility>      // forward, move                        

namespace xstd {
namespace ranges::views {

template<std::ranges::view V>
        requires std::ranges::input_range<V>
class as_set_view 
: 
        public std::ranges::view_interface<as_set_view<V>> 
{
        V base_ = V();
public:
        using key_type = std::ranges::range_value_t<V>;
        
        [[nodiscard]] constexpr          as_set_view()       noexcept = default;
        [[nodiscard]] constexpr explicit as_set_view(V base) noexcept 
        : 
                base_(std::move(base)) 
        {}

        [[nodiscard]] constexpr auto begin() const noexcept { return std::ranges::begin(base_); }
        [[nodiscard]] constexpr auto end  () const noexcept { return std::ranges::end  (base_); }
};

template<class R>
as_set_view(R&&) -> as_set_view<std::views::all_t<R>>;

namespace detail {

struct as_set_fn 
: 
        std::ranges::range_adaptor_closure<as_set_fn> 
{
        template <std::ranges::viewable_range R>
        [[nodiscard]] constexpr auto operator()(R&& r) const noexcept
                requires requires { as_set_view(std::forward<R>(r)); }
        {
                return as_set_view(std::forward<R>(r));
        }
};

}       // namespace detail

inline constexpr detail::as_set_fn as_set;

}       // namespace ranges::views

namespace views = ranges::views;

}       // namespace xstd

#endif  // include guard
