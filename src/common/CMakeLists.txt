# src/common/CMakeLists.txt

include_directories(${INCLUDE_DIRS})

set(COMMON_SOURCES common.cpp)

message(STATUS "Creating COMMON shared library, sources: ${COMMON_SOURCES}")
add_library(COMMON SHARED ${COMMON_SOURCES})
set(LINK_LIBS ${LINK_LIBS} COMMON PARENT_SCOPE)
set(COMMON_SOURCES ${COMMON_SOURCES} PARENT_SCOPE)
