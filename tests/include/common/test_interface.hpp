#pragma once

#include "common/common.hpp"
#include "common/common_io.hpp"

#include <algorithm>
#include <concepts>
#include <format>
#include <iostream>
#include <print>
#include <ranges>
#include <string_view>
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

#include <iostream>
template <typename Func, typename Output, typename... Inputs>
    requires ValidTestCallable<Func, Output, Inputs...>
bool
test_function_print( const std::string_view test_name, Func && function,
                     const Output expected_output, Inputs &&... inputs ) {
    std::cout << "TEST - " << test_name << ":\n";
    const auto output{ std::forward<Func>( function )(
        std::forward<Inputs>( inputs )... ) };
    const auto result{ expected_output == output };
    std::cout << "\toutput: " << output << "\n";
    std::cout << "\texpected_output: " << expected_output << "\n";
    return result;
}

constexpr auto
confirm_results( const std::vector<bool> & test_results ) {
    bool test_result{ true };

    for ( const auto & [i, result] : std::views::enumerate( test_results ) ) {
        std::println( " [TEST {}] -> {}", i, ( result ? "PASS" : "FAIL" ) );
        test_result &= result;
    }

    return test_result;
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

class TestFixture
{
    private:
    std::string_view test_id;
    std::ostream &   os;

    public:
    TestFixture( const std::string_view id,
                 std::ostream &         output_stream = std::cerr ) :
        test_id( id ), os( output_stream ) {
        os << std::format( "Created {} test fixture.\n", test_id );
    }

    ~TestFixture() {
        os << std::format( "Finished {} test fixture.\n", test_id )
           << std::flush;
    }

    TestFixture( const TestFixture & ) = default;
    TestFixture & operator=( const TestFixture & ) = default;

    TestFixture( TestFixture && ) noexcept = default;
    TestFixture & operator=( TestFixture && ) noexcept = default;

    std::ostream & operator<<( const std::string_view str ) {
        return os << std::format( " {} | ", test_id ) << str << "\n";
    }
};

} // namespace TEST_INTERFACE
