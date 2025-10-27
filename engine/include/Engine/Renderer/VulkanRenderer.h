#pragma once
#include <vulkan/vulkan_raii.hpp>
#include <cstdint>
#include "Engine/Window.h"

namespace Engine 
{
    class VulkanRenderer
    {
    public:
        void Init(const IWindow& window);
        void Shutdown();
        void RenderFrame();
    private:
        void CreateInstance(const IWindow& window);

        vk::raii::Context  m_Context;
        vk::raii::Instance m_Instance;
    };
}