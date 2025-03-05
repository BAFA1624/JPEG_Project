# src/png/CMakeLists.txt

include_directories(../../include/)

set(PNG_SOURCES png_types.cpp crc.cpp cst.cpp)

message(STATUS "Creating PNG shared library, sources:  ${PNG_SOURCES}")
add_library(PNG SHARED ${PNG_SOURCES})
set(LINK_LIBS ${LINK_LIBS} PNG PARENT_SCOPE)
set(PNG_SOURCES ${PNG_SOURCES} PARENT_SCOPE)
