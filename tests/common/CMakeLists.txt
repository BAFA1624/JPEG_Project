# tests/common/CMakeLists.txt

include_directories(../../include/)

set(COMMON_SUB_TEST_SOURCES
    common_test.cpp
)
create_test_sourcelist(COMMON_TEST_SOURCES common_tests.cpp ${COMMON_SUB_TEST_SOURCES})

message(STATUS "Making common tests:")
add_executable(common_tests ${COMMON_TEST_SOURCES})
target_link_libraries(common_tests ${LINK_LIBS})

# Add png_types tests
foreach(test ${COMMON_SUB_TEST_SOURCES})
    get_filename_component(test_name ${test} NAME_WE)
    add_test(NAME ${test_name} COMMAND common_tests ${test_name})
endforeach()
