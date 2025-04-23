# CMakeLists.txt

cmake_minimum_required(VERSION 3.30.5)

project(parse_jpeg)

#CMake includes
include(CTest)
enable_testing()

# Required packages
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

# Compiler info
set(CMAKE_CXX_COMPILER g++-14)
set(CMAKE_CXX_STANDARD 23)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Setting project directories
set(INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/include)
set(TEST_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/tests/include ${INCLUDE_DIRS})

# Set output directories
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

# Default compiler flags
set(CMAKE_CXX_FLAGS_DEBUG "-Wall -Wextra -Wconversion -pedantic -O0 -ggdb -fpic")
set(CMAKE_CXX_FLAGS_RELEASE "-Werror -DNDEBUG -march=native -Wall -Wextra -pedantic -Wconversion -O3 -fpic")

# Include directories
#include_directories(./include/ ${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})

# Important subdirectories
add_subdirectory(src)
add_subdirectory(tests)

add_executable(main src/main.cpp)

target_link_libraries(main PRIVATE ${LINK_LIBS})
target_include_directories(main PRIVATE ${INCLUDE_DIRS})
