# Build the engine as a static lib
add_library(KHEngine STATIC
  src/Core/Application.cpp
  src/Platform/GLFW/GLFWWindow.cpp
  src/Rendering/BgfxRenderer.cpp
)

# Public headers are under engine/include
target_include_directories(KHEngine
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# GLFW via FetchContent
include(FetchContent)

# GLFW (windowing library)
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)

# bx (foundation library)
FetchContent_Declare(
  bx
  GIT_REPOSITORY https://github.com/bkaradzic/bx.git
  GIT_TAG master
)
FetchContent_MakeAvailable(bx)

# bimg (image library)
FetchContent_Declare(
  bimg
  GIT_REPOSITORY https://github.com/bkaradzic/bimg.git
  GIT_TAG master
)
FetchContent_MakeAvailable(bimg)

# bgfx (rendering library)
FetchContent_Declare(
  bgfx
  GIT_REPOSITORY https://github.com/bkaradzic/bgfx.cmake.git
  GIT_TAG master
)
FetchContent_MakeAvailable(bgfx)

# Link GLFW privately (engine API does not expose GLFW headers)
target_link_libraries(KHEngine
  PRIVATE
    glfw
  PUBLIC
    bgfx
    bimg
    bx
)

# Windows-friendly defines (avoid <windows.h> min/max macro conflicts if you include it later)
if (WIN32)
  target_compile_definitions(KHEngine PRIVATE 
  BGFX_CONFIG_RENDERER_VULKAN=1
  BGFX_CONFIG_DEBUG=1
  NOMINMAX)
endif()

# Warnings
if (MSVC)
  target_compile_options(KHEngine PRIVATE /W4 /permissive-)
else()
  target_compile_options(KHEngine PRIVATE -Wall -Wextra -Wpedantic)
endif()
