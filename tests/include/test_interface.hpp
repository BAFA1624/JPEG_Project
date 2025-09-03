#pragma once

#include <concepts>

namespace TEST_INTERFACE
{

template <typename T, typename Output, typename... Inputs>
concept ValidTestCallable =
    requires( T && f, const Output & output, Inputs &&... inputs ) {
        { f( std::forward<Inputs>( inputs )... ) } -> std::same_as<Output>;
        {
            output == f( std::forward<Inputs>( inputs )... )
        } -> std::convertible_to<bool>;
    };

template <typename Func, typename Output, typename... Inputs>
    requires ValidTestCallable<Func, Output, Inputs...>
constexpr bool
test_function( Func && function, const Output expected_output,
               Inputs &&... inputs ) {
    return expected_output == std::forward<Func>( function )( inputs... );
}

} // namespace TEST_INTERFACE
