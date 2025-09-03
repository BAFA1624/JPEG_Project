#pragma once

#include <concepts>

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

} // namespace TEST_INTERFACE
