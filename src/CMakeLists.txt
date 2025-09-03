# src/CMakeLists.txt

# Add subdirectories
add_subdirectory(common)
add_subdirectory(png)

# Pass source & library lists back to parent scope
set(LINK_LIBS ${LINK_LIBS} PARENT_SCOPE)
set(PNG_SOURCES ${PNG_SOURCES} PARENT_SCOPE)
set(COMMON_SOURCES ${COMMON_SOURCES} PARENT_SCOPE)
