#pragma once
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <cstdint>

namespace Engine
{
    class BgfxRenderer
    {
    public:
        void Init(void* nativeWindowHandle, uint32_t width, uint32_t height);
        void Shutdown();
        void RenderFrame();
    private:
        bool m_Initialized = false;
    };
}