#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>         // must come first
#include <GLFW/glfw3native.h>   // then platform-specific functions

#include "Engine/Rendering/BgfxRenderer.h"
#include <iostream>

namespace Engine
{
    void BgfxRenderer::Init(void* nativeWindowHandle, uint32_t width, uint32_t height)
    {
        if (m_Initialized)
        {
            std::cerr << "[BgfxRenderer] Already initialized.\n";
            return;
        }

        // nativeWindowHandle is expected to be a GLFWwindow* from IWindow::GetNativeHandle()
        GLFWwindow* glfwWindow = static_cast<GLFWwindow*>(nativeWindowHandle);
        HWND hwnd = glfwGetWin32Window(glfwWindow);

        bgfx::Init initParams;
# if defined(_WIN32)
        initParams.type = bgfx::RendererType::Vulkan; // Choose Vulkan for Windows
# else
        initParams.type = bgfx::RendererType::OpenGL; // Default to OpenGL for other platforms
# endif
        initParams.platformData.nwh = hwnd;
        initParams.resolution.width  = width;
        initParams.resolution.height = height;
        initParams.resolution.reset  = BGFX_RESET_VSYNC;

        if (!bgfx::init(initParams))
        {
            throw std::runtime_error("Failed to initialize bgfx.");
        }

        m_Initialized = true;
        std::cout << "[BgfxRenderer] Initialized with Vulkan backend.\n";
    }

    void BgfxRenderer::Shutdown()
    {
        if (!m_Initialized) return;
        bgfx::shutdown();
        m_Initialized = false;
        std::cout << "[BgfxRenderer] Shutdown completed.\n";
    }

    void BgfxRenderer::RenderFrame()
    {
        if (!m_Initialized)
        {
            std::cerr << "[BgfxRenderer] Not initialized. Cannot render frame.\n";
            return;
        }

        bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);
        bgfx::setViewRect(0, 0, 0, UINT16_MAX, UINT16_MAX);
        bgfx::touch(0); // Ensure view 0 is cleared
        bgfx::frame();
    }
}