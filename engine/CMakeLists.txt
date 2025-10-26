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

# ----------- GLFW via FetchContent -----------------
include(FetchContent)

FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)

target_compile_definitions(KHEngine PRIVATE glfw)

# ----------- Vulkan SDK / Loader -----------------
# Requires Vulkan SDK installed (LunarG). On macOS this is MoltenVK via the SDK.
# CMake's FindVulkan provides Vulkan::Vulkan if found.
find_package(Vulkan REQUIRED)

# Link the Vulkan loader +
# Use Vulkan::Vulkan to be cross-platform (Win: vulkan-1.lib, Linux: libvulkan.so, macOS: MoltenVK dylib)
target_link_libraries(KHEngine PRIVATE Vulkan::Vulkan)

# If your GLFW code includes <GLFW/glfw3.h> and you want it to include <vulkan/vulkan.h> for you:
# You can define this globally for the engine sources:
target_compile_definitions(KHEngine PRIVATE GLFW_INCLUDE_VULKAN)

message(STATUS "Vulkan include: ${Vulkan_INCLUDE_DIRS}")
message(STATUS "Vulkan libs: ${Vulkan_LIBRARIES}")

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
