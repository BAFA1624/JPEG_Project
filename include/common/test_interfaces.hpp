#pragma once

#include <concepts>
#include <expected>
#include <stdexcept>

/* --- Structure: ---\/---
 * Subject to change.
 * - TestError: Generic error type. May represent a number of
 *       of error including, but not limited to, failing to
 *       load input or output data, a failed test run, failure
 *       to connect to the logger, & internal errors for the
 *       test framework.
 * - Test: Managed by a TestEnvironment. Runs a set of inputs
 *       passed from the TestEnvironment for a single test,
 *       passing outputs & errors back to the TestEnvironment.
 * - TestLoader: Managed by a TestEnvironment. Loads data for
 *       a specific test, passing either the data, or a load
 *       error back to the TestEnvironment.
 * - TestEnvironment: Stores functionality to load inputs,
 *       load expected outputs, activate a Test, verify
 *       the results, & handle & store error information.
 * - TestList: Manages a list of TestEnvironments to be run
 *       in the same block of tests. Allows tests to be
 *       executed in sequence, or in parallel.
 * - TestRunner: Manages TestList's. Runs the tests, & collates
 *       results. Manages logger to control test text output.
 */

/* --- TestError Concept Definition (WIP) --- */
template <typename TestErrorType>
concept TestError = std::derived_from<TestErrorType, std::runtime_error>;

class NullError : public std::runtime_error
{};

static_assert( TestError<NullError> );
static_assert( !TestError<int> );

/* --- Test Concept Definition (WIP) --- */
template <typename TestType, typename TestOutput, typename ErrorType,
          typename... TestInputTypes>
concept Test = requires( TestType non_const_test, const TestType const_test,
                         const TestInputTypes... inputs ) {
    { non_const_test.set_inputs( inputs... ) } -> std::same_as<void>;
    { non_const_test.reset() } -> std::same_as<bool>;
    { non_const_test.run() } -> std::same_as<bool>;
    {
        const_test.get_output()
    } -> std::same_as<std::expected<TestOutput, ErrorType>>;
};

/* --- TestLoader Concept Definition (WIP) --- */

// template <typename TestLoaderType, typename TestInputType>
// concept TestLoader = ;

/* --- TestEnvironment Concept Definition (WIP) --- */
/* --- TestList Concept Definition (WIP) --- */
/* --- TestRunner Concept Definition (WIP) --- */
