#pragma once

namespace Engine 
{
    class VulkanRenderer
    {
    public:
        void Init(void* nativeWindowHandle, uint32_t width, uint32_t height);
        void Shutdown();
        void RenderFrame();
    }
}