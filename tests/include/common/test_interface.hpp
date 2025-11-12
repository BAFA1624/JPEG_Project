#pragma once

#include <algorithm>
#include <concepts>
#include <format>
#include <iostream>
#include <print>
#include <ranges>
#include <string_view>
#include <vector>

// This is temporarily here till common_io is finished.
template <typename T>
concept Streamable = requires( const T x ) { std::cout << x; };
template <typename T>
concept Formattable = requires( const T x ) { std::print( "{}", x ); };
template <typename T>
concept Printable = Streamable<T> && Formattable<T>;

template <Streamable T, std::size_t N>
std::ostream &
operator<<( std::ostream & os, const std::array<T, N> & arr ) {
    const auto width{ os.width() };
    os.width( 2 );
    os << "{ " << std::hex;
    for ( std::size_t i{ 0 }; i < N - 1; ++i ) { os << arr[i] << ", "; }
    os << arr[N - 1] << std::dec << " }";
    os.width( width );
    return os;
}

template <typename T1, typename T2>
concept ComparableElements = requires( const T1 lhs, const T2 rhs ) {
    { lhs == rhs } -> std::same_as<bool>;
};

template <typename T1, typename T2, std::size_t N>
/*requires ComparableElements<T1, T2>*/
constexpr bool
operator==( const std::array<T1, N> & lhs, const std::array<T2, N> & rhs ) {
    bool result = true;
    for ( const auto & [lhs_element, rhs_element] :
          std::views::zip( lhs, rhs ) ) {
        result &= ( lhs_element == rhs_element );
    }
    return result;
}

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
