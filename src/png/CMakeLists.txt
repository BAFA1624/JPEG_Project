# src/png/CMakeLists.txt

set(PNG_SOURCES png_types.cpp png_chunk_payload.cpp)

message(STATUS "Creating PNG shared library, sources:  ${PNG_SOURCES}")
add_library(PNG SHARED ${PNG_SOURCES})
target_include_directories(PNG PRIVATE ${INCLUDE_DIRS})
target_link_libraries(PNG PRIVATE COMMON)
target_compile_features(PNG PUBLIC ${DEFAULT_COMPILE_FEATURES})

set(LINK_LIBS ${LINK_LIBS} PNG PARENT_SCOPE)
set(PNG_SOURCES ${PNG_SOURCES} PARENT_SCOPE)
