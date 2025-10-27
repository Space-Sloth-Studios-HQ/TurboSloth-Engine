#pragma once
#include <vulkan/vulkan_raii.hpp>
#include <vector>
#include <optional>
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
        std::optional<vk::raii::Instance> m_Instance;
        // Intended for use when setting up Vulkan validation layers in instance creation.
        std::vector<char const*> m_ValidationLayers = {
            "VK_LAYER_KHRONOS_validation"
        };
    };
}