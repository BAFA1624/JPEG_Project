# src/common/CMakeLists.txt

set(COMMON_SOURCES common.cpp crc.cpp)

message(STATUS "Creating COMMON shared library, sources: ${COMMON_SOURCES}")
add_library(COMMON SHARED ${COMMON_SOURCES})
target_include_directories(COMMON PRIVATE ${INCLUDE_DIRS})
target_compile_features(COMMON PUBLIC ${DEFAULT_COMPILE_FEATURES})

set(LINK_LIBS ${LINK_LIBS} COMMON PARENT_SCOPE)
set(COMMON_SOURCES ${COMMON_SOURCES} PARENT_SCOPE)
