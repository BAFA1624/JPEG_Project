#pragma once

#include "common/test_interface.hpp"
#include "png/png_types.hpp"

#include <functional>

namespace PNG
{

template <typename T>
concept EnumOrIntegral = std::is_enum_v<T> || std::is_integral_v<T>;

template <typename F, typename... Ts>
concept ValidFunction = ( EnumOrIntegral<Ts> && ... )
                        && requires( const F && func, const Ts &&... inputs ) {
                               {
                                   std::forward<const F>( func )(
                                       std::forward<const Ts>( inputs )... )
                               } -> std::same_as<bool>;
                           };

template <typename... Ts>
constexpr auto
args_tuple( const std::tuple<Ts..., bool> & tup ) {
    constexpr std::size_t tup_size =
        std::tuple_size_v<std::tuple<Ts..., bool>> - 1;

    return
        [&tup]<std::size_t... I>( const std::index_sequence<I...> ) constexpr {
            return std::tuple{ std::get<I>( tup )... };
        }( std::make_index_sequence<tup_size>{} );
}

template <typename... Ts, typename F, std::size_t N>
    requires( EnumOrIntegral<Ts> && ... ) && ValidFunction<F, Ts...>
constexpr bool
validate_type( const std::array<std::tuple<Ts..., bool>, N> & test_set,
               const F &&                                     valid_function ) {
    std::size_t test_passes{ 0 };

    for ( const auto & tup : test_set ) {
        constexpr auto bool_position =
            std::tuple_size_v<std::tuple<Ts..., bool>> - 1;
        const bool expected_result = std::get<bool_position>( tup );

        const bool result = std::apply(
            [&valid_function]( const auto &... args ) {
                return std::forward<const F>( valid_function )( args... );
            },
            args_tuple<Ts...>( tup ) );

        test_passes += ( result == expected_result ? 1 : 0 );
    }

    return N == test_passes;
}

// Testing Functions

bool test_png_types();
bool test_ihdr_types();

const auto test_functions = std::vector{ test_png_types, test_ihdr_types };

} // namespace PNG
