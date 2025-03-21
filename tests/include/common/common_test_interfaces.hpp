#pragma once

#include <concepts>

template <typename TestType, typename TestOutput, typename... TestInput>
concept Test = std::constructible_from<TestType, TestInput...>
               && requires( TestType &       non_const_test,
                            const TestType & const_test, TestInput... inputs ) {
                      {
                          const_test.validate( inputs... )
                      } -> std::same_as<TestOutput>;
                  };
