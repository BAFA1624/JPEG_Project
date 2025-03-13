# src/common/CMakeLists.txt

set(COMMON_SOURCES common.cpp)

message(STATUS "Creating COMMON shared library, sources: ${COMMON_SOURCES}")
add_library(COMMON SHARED ${COMMON_SOURCES})
target_include_directories(COMMON PRIVATE ${INCLUDE_DIRS})

set(LINK_LIBS ${LINK_LIBS} COMMON PARENT_SCOPE)
set(COMMON_SOURCES ${COMMON_SOURCES} PARENT_SCOPE)
