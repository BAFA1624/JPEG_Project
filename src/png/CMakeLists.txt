include_directories(../../include/)

set(sources png_types.cpp crc.cpp cst.cpp)

message(STATUS "Creating PNG shared library...")
add_library(PNG SHARED ${sources})
set(LINK_LIBS ${LINK_LIBS} PNG PARENT_SCOPE)
