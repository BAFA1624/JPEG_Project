#pragma once

#include <concepts>
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
run_tests( const std::vector<bool ( * )()> & test_functions ) {
    std::size_t test_passes{ 0 };

    for ( const auto & func : test_functions ) {
        test_passes += ( func() ? 1 : 0 );
    }

    return static_cast<int>( test_functions.size() - test_passes );
}

} // namespace TEST_INTERFACE
