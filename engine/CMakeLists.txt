# Build the engine as a static lib
add_library(KHEngine STATIC
  src/Core/Application.cpp
)

# Public headers are under engine/include
target_include_directories(KHEngine
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Warnings
if (MSVC)
  target_compile_options(KHEngine PRIVATE /W4 /permissive-)
else()
  target_compile_options(KHEngine PRIVATE -Wall -Wextra -Wpedantic)
endif()

# (Add dependencies here later, e.g., glfw/volk)
# target_link_libraries(KHEngine PUBLIC glfw volk)
