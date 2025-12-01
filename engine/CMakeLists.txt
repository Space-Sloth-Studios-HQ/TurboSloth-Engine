# Build the engine as a static lib
add_library(KHEngine STATIC
  src/Core/Application.cpp
  src/Platform/GLFW/GLFWWindow.cpp
  src/Platform/GLFW/WindowVulkan.cpp
  src/Renderer/VulkanRenderer.cpp
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

# target_compile_definitions(KHEngine PRIVATE glfw)

# ----------- Vulkan SDK / Loader -----------------
# Help FindVulkan on Windows if VULKAN_SDK is set
if (WIN32 AND DEFINED ENV{VULKAN_SDK})
  # Typical SDK layout: C:\VulkanSDK\1.x.x.x\{Include,Lib}
  set(Vulkan_INCLUDE_DIR "$ENV{VULKAN_SDK}/Include" CACHE PATH "")
  # Use the 64-bit lib; if you're targeting Win32, switch to /Lib32/
  set(Vulkan_LIBRARY     "$ENV{VULKAN_SDK}/Lib/vulkan-1.lib" CACHE FILEPATH "")
endif()

# Requires Vulkan SDK installed (LunarG). On macOS this is MoltenVK via the SDK.
# CMake's FindVulkan provides Vulkan::Vulkan if found.
find_package(Vulkan REQUIRED)

# Link the Vulkan loader and GLFW
# Use Vulkan::Vulkan to be cross-platform (Win: vulkan-1.lib, Linux: libvulkan.so, macOS: MoltenVK dylib)
# Vulkan is PUBLIC because VulkanRenderer.h is included in public headers (Application.h)
target_link_libraries(KHEngine
  PUBLIC
    Vulkan::Vulkan
  PRIVATE
    glfw
)

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

# ----------- Shader Compilation with Slang -----------------
# Find slangc compiler (from Vulkan SDK or system PATH)
find_program(SLANGC_EXECUTABLE
  NAMES slangc slangc.exe
  PATHS
    "$ENV{VULKAN_SDK}/Bin"
    "$ENV{SLANG_DIR}/bin"
  DOC "Path to slangc compiler"
)

if(NOT SLANGC_EXECUTABLE)
  message(FATAL_ERROR "slangc not found. Please install Slang or set SLANG_DIR environment variable.")
endif()

message(STATUS "Found slangc: ${SLANGC_EXECUTABLE}")

# Shader source and output directories
set(SHADER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
set(SHADER_OUTPUT_DIR ${CMAKE_BINARY_DIR}/shaders)

# Ensure output directory exists
file(MAKE_DIRECTORY ${SHADER_OUTPUT_DIR})

# Define shader compilation function
function(compile_slang_shader SHADER_SOURCE)
  get_filename_component(SHADER_NAME ${SHADER_SOURCE} NAME_WE)

  # Output files for vertex and fragment stages
  set(VERT_OUTPUT ${SHADER_OUTPUT_DIR}/${SHADER_NAME}.vert.spv)
  set(FRAG_OUTPUT ${SHADER_OUTPUT_DIR}/${SHADER_NAME}.frag.spv)

  # Compile vertex shader
  add_custom_command(
    OUTPUT ${VERT_OUTPUT}
    COMMAND ${SLANGC_EXECUTABLE}
      -target spirv
      -profile spirv_1_5
      -emit-spirv-directly
      -stage vertex
      -entry vertMain
      -fvk-use-entrypoint-name
      -o ${VERT_OUTPUT}
      ${SHADER_SOURCE}
    DEPENDS ${SHADER_SOURCE}
    COMMENT "Compiling vertex shader: ${SHADER_NAME}.vert.spv"
    VERBATIM
  )

  # Compile fragment shader
  add_custom_command(
    OUTPUT ${FRAG_OUTPUT}
    COMMAND ${SLANGC_EXECUTABLE}
      -target spirv
      -profile spirv_1_5
      -emit-spirv-directly
      -stage fragment
      -entry fragMain
      -fvk-use-entrypoint-name
      -o ${FRAG_OUTPUT}
      ${SHADER_SOURCE}
    DEPENDS ${SHADER_SOURCE}
    COMMENT "Compiling fragment shader: ${SHADER_NAME}.frag.spv"
    VERBATIM
  )

  # Add to parent scope
  set(COMPILED_SHADERS ${COMPILED_SHADERS} ${VERT_OUTPUT} ${FRAG_OUTPUT} PARENT_SCOPE)
endfunction()

# Compile triangle.slang
set(COMPILED_SHADERS "")
compile_slang_shader(${SHADER_SOURCE_DIR}/common/triangle.slang)

# Create custom target for all shaders
add_custom_target(EngineShaders ALL
  DEPENDS ${COMPILED_SHADERS}
  COMMENT "Building engine shaders"
)

# Make the engine depend on shaders being built
add_dependencies(KHEngine EngineShaders)

# Install shaders to output directory (optional, for deployment)
install(DIRECTORY ${SHADER_OUTPUT_DIR}/
  DESTINATION shaders
  FILES_MATCHING PATTERN "*.spv"
)
