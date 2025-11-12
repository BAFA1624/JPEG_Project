# src/common/CMakeLists.txt

set(COMMON_SOURCES common.cpp crc.cpp)

message(STATUS "Creating COMMON shared library, sources: ${COMMON_SOURCES}")
add_library(COMMON SHARED ${COMMON_SOURCES})
target_include_directories(COMMON PRIVATE ${INCLUDE_DIRS})
target_compile_features(COMMON PUBLIC ${DEFAULT_COMPILE_FEATURES})

# Create "empty" library to for correct Clangd highlighting in includes
add_library(UNUSED_COMMON_IO SHARED ${CMAKE_CURRENT_SOURCE_DIR}/common_io.cpp)
target_include_directories(UNUSED_COMMON_IO PRIVATE ${INCLUDE_DIRS})
target_compile_features(UNUSED_COMMON_IO PRIVATE ${DEFAULT_COMPILE_FEATURES})
set(CLANGD_DUMMY_LIBS ${CLANGD_DUMMY_LIBS} UNUSED_COMMON_IO)

set(LINK_LIBS ${LINK_LIBS} COMMON PARENT_SCOPE)
set(COMMON_SOURCES ${COMMON_SOURCES} PARENT_SCOPE)
set(CLANGD_DUMMY_LIB ${CLANGD_DUMMY_LIB} PARENT_SCOPE)
