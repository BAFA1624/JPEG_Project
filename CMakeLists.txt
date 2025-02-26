cmake_minimum_required(VERSION 3.30.5)

project(parse_jpeg)

#CMake includes
include(CTest)

# Required packages
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

# Compiler info
set(CMAKE_CXX_COMPILER g++-14)
set(CMAKE_CXX_STANDARD 23)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set output directories
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

# Default compiler flags
set(CMAKE_CXX_FLAGS_DEBUG "-Wall -Wextra -Wconversion -pedantic -O0 -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "-Werror -DNDEBUG -march=native -Wall -Wextra -pedantic -Wconversion -O3")

#set(LINK_LIBS)

include_directories(./include/ ${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})
add_subdirectory(src)

add_executable(main src/main.cpp)
add_custom_command(
	TARGET main POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json ${CMAKE_SOURCE_DIR}/compile_commands.json
	COMMENT "Transferring compile_commands.json"
)
message(STATUS "LINK_LIBS: ${LINK_LIBS}")
target_link_libraries(main PRIVATE ${LINK_LIBS})
