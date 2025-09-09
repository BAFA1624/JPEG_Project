#pragma once

#include "common/test_interfaces.hpp"
#include "png/png_types.hpp"

#include <functional>

namespace PNG
{

template <typename T>
concept EnumOrIntegral = std::is_enum_v<T> || std::is_integral_v<T>;

template <typename... Ts>
constexpr auto
args_tuple( const std::tuple<Ts..., bool> & tup ) {
    constexpr std::size_t N = std::tuple_size_v<std::tuple<Ts..., bool>> - 1;

    return [&tup]<std::size_t... I>(
               const std::index_sequence<I...> sequence ) constexpr {
        std::cout << sequence.size() << std::endl;
        return std::tuple{ std::get<I>( tup )... };
    }( std::make_index_sequence<N>{} );
}

template <typename... Ts, std::size_t N>
    requires( EnumOrIntegral<Ts> && ... ) && requires( const Ts &... args ) {
        { is_valid( args... ) } -> std::same_as<bool>;
    }
constexpr bool
validate_type( const std::array<std::tuple<Ts..., bool>, N> & test_set,
               const std::function<bool( const Ts... )> &     valid_function ) {
    std::size_t test_passes{ 0 };

    for ( const auto & tup : test_set ) {
        constexpr auto bool_position =
            std::tuple_size_v<std::tuple<Ts..., bool>> - 1;
        const bool expected_result = std::get<bool_position>( tup );

        const bool result = std::apply(
            [&valid_function]( const auto &... args ) {
                return valid_function( args... );
            },
            args_tuple<Ts...>( tup ) );

        test_passes += ( result == expected_result ? 1 : 0 );
    }

    return N == test_passes;
}

// Testing Functions

constexpr bool test_png_types();
constexpr bool test_ihdr_types();
constexpr bool test_plte_types();

constexpr auto test_functions =
    std::array{ test_png_types, test_ihdr_types, test_plte_types };

} // namespace PNG
