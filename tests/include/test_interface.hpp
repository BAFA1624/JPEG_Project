#pragma once

#include <concepts>
#include <format>
#include <iostream>
#include <print>
#include <vector>

namespace TEST_INTERFACE
{

template <typename T, typename Output, typename... Inputs>
concept ValidTestCallable = requires( T && func, const Output & output,
                                      Inputs &&... inputs ) {
    {
        std::forward<T>( func )( std::forward<Inputs>( inputs )... )
    } -> std::same_as<Output>;
    {
        output == std::forward<T>( func )( std::forward<Inputs>( inputs )... )
    } -> std::convertible_to<bool>;
};

template <typename Func, typename Output, typename... Inputs>
    requires ValidTestCallable<Func, Output, Inputs...>
constexpr bool
test_function( Func && function, const Output expected_output,
               Inputs &&... inputs ) {
    return expected_output
           == std::forward<Func>( function )(
               std::forward<Inputs>( inputs )... );
}

inline int
run_tests( const std::string_view            test_name,
           const std::vector<bool ( * )()> & test_functions ) {
    std::size_t test_passes{ 0 };

    std::println( "Running {} tests:", test_name );
    for ( const auto & func : test_functions ) {
        const auto pass = func();
        test_passes += ( pass ? 1 : 0 );
        std::println( "  - Test Status: {}.", ( pass ? "Success" : "Fail" ) );
    }
    std::println( "Done." );

    return static_cast<int>( test_functions.size() - test_passes );
}

} // namespace TEST_INTERFACE
