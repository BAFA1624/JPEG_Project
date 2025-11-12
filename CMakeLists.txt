# CMakeLists.txt

cmake_minimum_required(VERSION 3.30.5)

project(parse_jpeg)

#CMake includes
include(CTest)
enable_testing()

# Required packages
#find_package(OpenGL REQUIRED)
#find_package(GLUT REQUIRED)

# Compiler info
set(CMAKE_CXX_COMPILER g++-15)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Setting project directories
set(INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/include)
set(TEST_INCLUDE_DIRS ${INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/tests/include )

# Set output directories
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

# Default compiler flags
set(JANKY_VIM_LINTING_FLAGS "-Wno-pragma-once-outside-header")
set(CMAKE_CXX_FLAGS_DEBUG "${JANKY_VIM_LINTING_FLAGS} -Wall -Wextra -Wconversion -pedantic -O0 -ggdb -fpic -fconcepts-diagnostics-depth=2")
set(CMAKE_CXX_FLAGS_RELEASE "-Werror -DNDEBUG -march=native -Wall -Wextra -pedantic -Wconversion -O3 -fpic")

# Default compile features
set(DEFAULT_COMPILE_FEATURES cxx_std_23)

# Important subdirectories
add_subdirectory(src)
add_subdirectory(tests)

add_executable(main src/main.cpp)

target_compile_features(main PUBLIC ${DEFAULT_COMPILE_FEATURES})
target_link_libraries(main PRIVATE ${LINK_LIBS})
target_include_directories(main PRIVATE ${INCLUDE_DIRS})

# Creating a dummy library to force correct clangd
# highlighting in includes that aren't correctly
# indexed by default
add_library(CLANGD_DUMMY SHARED ${CLANGD_DUMMY_SOURCES})
target_compile_features(CLANGD_DUMMY PUBLIC ${DEFAULT_COMPILE_FEATURES})
target_link_libraries(CLANGD_DUMMY PUBLIC ${CLANGD_DUMMY_LIBS})
target_include_directories(CLANGD_DUMMY PUBLIC ${TEST_INCLUDE_DIRS})
