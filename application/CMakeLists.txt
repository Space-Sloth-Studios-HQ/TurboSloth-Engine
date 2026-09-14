# Builds the main app for now. Later you can link an engine lib here.
add_executable(KHApp
    src/main.cpp
)

if (WIN32)
  target_compile_definitions(KHApp PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

# Warnings
if (MSVC)
  target_compile_options(KHApp PRIVATE /W4 /permissive-)
else()
  target_compile_options(KHApp PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Nice output name
set_target_properties(KHApp PROPERTIES OUTPUT_NAME "khclone")

target_link_libraries(KHApp PRIVATE MomoEngine)
# Convenience target: build, then run from the binary dir so the relative
# "shaders/*.spv" paths in VulkanRenderer resolve.
#   cmake --build build/clang-debug --target run
add_custom_target(run
  COMMAND $<TARGET_FILE:KHApp>
  DEPENDS KHApp
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  USES_TERMINAL
  COMMENT "Running khclone"
)
