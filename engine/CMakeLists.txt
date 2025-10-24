# Build the engine as a static lib
add_library(KHEngine STATIC
  src/Core/Application.cpp
  src/Platform/GLFW/GLFWWindow.cpp
)

# Public headers are under engine/include
target_include_directories(KHEngine
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# GLFW via FetchContent
include(FetchContent)

FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)

# Link GLFW privately (engine API does not expose GLFW headers)
target_link_libraries(KHEngine
  PRIVATE
    glfw
)

# Windows-friendly defines (avoid <windows.h> min/max macro conflicts if you include it later)
if (WIN32)
  target_compile_definitions(KHEngine PRIVATE NOMINMAX)
endif()

# Warnings
if (MSVC)
  target_compile_options(KHEngine PRIVATE /W4 /permissive-)
else()
  target_compile_options(KHEngine PRIVATE -Wall -Wextra -Wpedantic)
endif()

# (Add dependencies here later, e.g., glfw/volk)
# target_link_libraries(KHEngine PUBLIC glfw volk)
