# tests/png/CMakeLists.txt

set(PNG_SUB_TEST_SOURCES
    png_types_test.cpp
    png_chunk_payload_test.cpp
)

set(PNG_SUB_WIP_TEST_SOURCES
    png_chunk_test.cpp
)
create_test_sourcelist(PNG_TEST_SOURCES png_tests.cpp ${PNG_SUB_TEST_SOURCES} ${PNG_SUB_WIP_TEST_SOURCES})

message(STATUS "Making png tests:")
add_executable(png_tests ${PNG_TEST_SOURCES})

target_include_directories(png_tests PRIVATE ${TEST_INCLUDE_DIRS})
target_link_libraries(png_tests ${LINK_LIBS})

# Add png_types tests
foreach(test ${PNG_SUB_TEST_SOURCES})
    get_filename_component(test_name ${test} NAME_WE)
    add_test(NAME ${test_name} COMMAND png_tests ${test_name})
endforeach()

foreach(test ${PNG_SUB_WIP_TEST_SOURCES})
    get_filename_component(test_name ${test} NAME_WE)
    add_test(NAME ${test_name} COMMAND png_tests ${test_name})
    set_tests_properties(${test_name} PROPERTIES WILL_FAIL TRUE)
endforeach()
