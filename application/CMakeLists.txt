# Builds the main app for now. Later you can link an engine lib here.
add_executable(KHApp
    src/main.cpp
)

# Warnings
if (MSVC)
  target_compile_options(KHApp PRIVATE /W4 /permissive-)
else()
  target_compile_options(KHApp PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Nice output name
set_target_properties(KHApp PROPERTIES OUTPUT_NAME "khclone")

target_link_libraries(KHApp PRIVATE KHEngine)