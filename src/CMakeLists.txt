# src/CMakeLists.txt

# Add subdirectories
add_subdirectory(common)
add_subdirectory(png)

# Add files to CLANGD_DUMMY_SOURCES for correct Clangd highlighting
set(CLANGD_DUMMY_SOURCES ${CLANGD_DUMMY_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/dummy.cpp)

# Pass source & library lists back to parent scope
set(LINK_LIBS ${LINK_LIBS} PARENT_SCOPE)
set(PNG_SOURCES ${PNG_SOURCES} PARENT_SCOPE)
set(COMMON_SOURCES ${COMMON_SOURCES} PARENT_SCOPE)
set(CLANGD_DUMMY_SOURCES ${CLANGD_DUMMY_SOURCES} PARENT_SCOPE)
set(CLANGD_DUMMY_LIBS ${CLANGD_DUMMY_LIBS} PARENT_SCOPE)
