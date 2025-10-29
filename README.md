# TurboSloth-Engine

A modern game engine built with Vulkan and C++20, featuring cross-platform support via MoltenVK on macOS.

## Features

- **Vulkan Renderer**: Modern graphics API with MoltenVK support for macOS
- **GLFW Integration**: Cross-platform window and input management
- **Layer-based Architecture**: Modular application structure with layer system
- **C++20**: Modern C++ features and best practices
- **CMake Build System**: Cross-platform build configuration with presets

## Prerequisites

### macOS

- **Xcode Command Line Tools**: `xcode-select --install`
- **CMake** (3.21+): `brew install cmake`
- **Ninja**: `brew install ninja`
- **Vulkan SDK**: Download from [LunarG](https://vulkan.lunarg.com/sdk/home)
  - Install the macOS SDK (includes MoltenVK)
  - Current tested version: 1.4.328.1

## Environment Setup

Add the following to your `~/.zshrc` (or `~/.bashrc` for Bash):

```bash
export VULKAN_SDK="$HOME/VulkanSDK/1.4.328.1/macOS"
export PATH="$VULKAN_SDK/bin:$PATH"
export DYLD_LIBRARY_PATH="$VULKAN_SDK/lib:$DYLD_LIBRARY_PATH"
export VK_ICD_FILENAMES="$VULKAN_SDK/share/vulkan/icd.d/MoltenVK_icd.json"
export VK_LAYER_PATH="$VULKAN_SDK/share/vulkan/explicit_layer.d"
```

After editing, reload your shell configuration:
```bash
source ~/.zshrc  # or source ~/.bashrc
```

## Building

### macOS

The project includes macOS-specific build presets that use the system AppleClang compiler:

```bash
# Configure (Debug)
cmake --preset macos-debug

# Build
cmake --build build/macos-debug

# Or for Release builds
cmake --preset macos-release
cmake --build build/macos-release
```

The executable will be created at `build/macos-debug/application/khclone` (or `build/macos-release/application/khclone`).

### Alternative: Generic Clang Presets

If you prefer to use a different clang installation:

```bash
cmake --preset clang-debug
cmake --build build/clang-debug
```

## Running

After building, run the application:

```bash
./build/macos-debug/application/khclone
```

You should see output similar to:
```
[Engine] Starting 'KHClone' (1920x1080)
[VulkanRenderer] Creating Vulkan instance...
[AppLayer] Attached to application.
```

## Project Structure

```
TurboSloth-Engine/
├── CMakeLists.txt           # Root CMake configuration
├── CmakePresets.json        # CMake build presets
├── engine/                  # Engine library
│   ├── CMakeLists.txt
│   ├── include/             # Public headers
│   │   └── Engine/
│   │       ├── Core/
│   │       ├── Platform/
│   │       └── Renderer/
│   └── src/                 # Engine implementation
│       ├── Core/
│       ├── Platform/
│       │   └── GLFW/
│       └── Renderer/
└── application/             # Application executable
    ├── CMakeLists.txt
    └── src/
        └── main.cpp         # Application entry point
```

## Technology Stack

- **Graphics API**: Vulkan 1.4 (via MoltenVK on macOS)
- **Windowing**: GLFW 3.4
- **Language**: C++20
- **Build System**: CMake 3.21+ with Ninja generator
- **Platform**: macOS (Apple Silicon and Intel)

## Development

### Adding New Layers

The engine uses a layer-based architecture. To create a new layer:

```cpp
class MyLayer : public Engine::Layer
{
public:
    void OnAttach() override
    {
        // Initialize layer resources
    }

    void OnUpdate(float dt) override
    {
        // Update logic
    }
};

// In main.cpp
app.PushLayer<MyLayer>();
```

### CMake Presets

Available presets:
- `macos-debug`: macOS with system AppleClang (Debug)
- `macos-release`: macOS with system AppleClang (Release)
- `clang-debug`: Generic Clang (Debug)
- `clang-release`: Generic Clang (Release)

### Compiler Warnings

The project is configured with strict compiler warnings:
- GCC/Clang: `-Wall -Wextra -Wpedantic`
- MSVC: `/W4 /permissive-`

## MoltenVK Notes

MoltenVK is a Vulkan Portability implementation that translates Vulkan calls to Metal on Apple platforms. The engine properly configures:

- `VK_KHR_portability_enumeration` extension
- `VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR` flag
- MoltenVK ICD (Installable Client Driver) detection

These are handled automatically by the engine's Vulkan initialization code.

## Troubleshooting

### "ErrorIncompatibleDriver" on macOS

If you see this error, ensure:
1. Vulkan SDK is properly installed
2. Environment variables are set correctly (see Environment Setup)
3. You've reloaded your shell configuration or opened a new terminal

### Build Errors with Homebrew Clang

Use the `macos-debug` or `macos-release` presets instead of `clang-debug`/`clang-release` to use the system AppleClang compiler.

### GLFW Extension Errors

Ensure GLFW is being built with Vulkan support. The engine automatically fetches and builds GLFW with the correct configuration.

## License

[Add your license information here]

## Contributing

[Add contribution guidelines here]
