# tests/common/CMakeLists.txt

set(COMMON_SUB_TEST_SOURCES
    common_test.cpp
    crc_test.cpp
)
create_test_sourcelist(COMMON_TEST_SOURCES common_tests.cpp ${COMMON_SUB_TEST_SOURCES})

message(STATUS "Making common tests:")
add_executable(common_tests ${COMMON_TEST_SOURCES})

target_include_directories(common_tests PRIVATE ${TEST_INCLUDE_DIRS})
target_link_libraries(common_tests ${LINK_LIBS})

# Add png_types tests
foreach(test ${COMMON_SUB_TEST_SOURCES})
    get_filename_component(test_name ${test} NAME_WE)
    add_test(NAME ${test_name} COMMAND common_tests ${test_name})
endforeach()

# Creating a dummy library to force correct clangd highlighting
# for tests/common/test_interface.hpp
add_library(TEST_INTERFACE SHARED test_interface.cpp)
target_include_directories(TEST_INTERFACE PUBLIC ${TEST_INCLUDE_DIRS})
target_compile_features(TEST_INTERFACE PUBLIC ${DEFAULT_COMPILE_FEATURES})
