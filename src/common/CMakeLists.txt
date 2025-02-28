include_directories(../../include/)

set(sources common.cpp)

message(STATUS "Creating COMMON shared library...")
add_library(COMMON SHARED ${sources})
set(LINK_LIBS ${LINK_LIBS} COMMON PARENT_SCOPE)
