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

# Create "empty" library to for correct Clangd highlighting in includes
add_library(UNUSED_TEST_INTERFACE SHARED ${CMAKE_CURRENT_SOURCE_DIR}/test_interface.cpp)
target_include_directories(UNUSED_TEST_INTERFACE PRIVATE ${TEST_INCLUDE_DIRS})
target_compile_features(UNUSED_TEST_INTERFACE PRIVATE ${DEFAULT_COMPILE_FEATURES})
set(CLANGD_DUMMY_LIBS ${CLANGD_DUMMY_LIBS} UNUSED_TEST_INTERFACE)

# Add png_types tests
foreach(test ${COMMON_SUB_TEST_SOURCES})
    get_filename_component(test_name ${test} NAME_WE)
    add_test(NAME ${test_name} COMMAND common_tests ${test_name})
endforeach()

set(CLANGD_DUMMY_LIBS ${CLANGD_DUMMY_LIBS} PARENT_SCOPE)
