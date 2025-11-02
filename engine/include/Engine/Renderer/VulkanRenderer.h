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
        void PickPhysicalDevice();

        vk::raii::Context  m_Context;
        vk::raii::Instance m_Instance = nullptr;
        std::unique_ptr<vk::raii::PhysicalDevice> m_PhysicalDevice = nullptr;
        // Intended for use when setting up Vulkan validation layers in instance creation.
        std::vector<char const*> m_ValidationLayers = {
            "VK_LAYER_KHRONOS_validation"
        };

        std::vector<const char*> m_DeviceExtensions = {
            vk::KHRSwapchainExtensionName,
            vk::KHRSpirv14ExtensionName,
            vk::KHRSynchronization2ExtensionName,
            vk::KHRCreateRenderpass2ExtensionName
        };
    };
}
