include_directories(../../include/)

set(sources PNG.cpp CRC.cpp CST.cpp)

message(STATUS "Creating PNG shared libraries...")
add_library(PNG SHARED ${sources})
set(LINK_LIBS ${LINK_LIBS} PNG PARENT_SCOPE)
