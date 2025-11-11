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
        void CreateLogicalDevice();
        uint32_t FindGraphicsQueueFamilyIdx(vk::raii::PhysicalDevice);

        vk::raii::Context  m_Context;
        std::optional<vk::raii::Instance> m_Instance;
        std::optional<vk::raii::PhysicalDevice> m_PhysicalDevice;
        std::optional<vk::raii::Device> m_Device;

        uint32_t m_GraphicsQueueFamilyIdx = 0;
        std::optional<vk::raii::Queue> m_GraphicsQueue;

        // Intended for use when setting up Vulkan validation layers in instance creation.
        std::vector<char const*> m_ValidationLayers = {
            "VK_LAYER_KHRONOS_validation"
        };

        std::vector<const char*> m_RequestedDeviceExtensions = {
            vk::KHRSwapchainExtensionName,
            vk::KHRSpirv14ExtensionName,
            vk::KHRSynchronization2ExtensionName,
            vk::KHRCreateRenderpass2ExtensionName
        };

        std::vector<const char*> m_OptionalDeviceExtensions = {
            "VK_KHR_portability_subset", // For MoltenVK on macOS
        };

        std::vector<const char*> m_EnabledDeviceExtensions;
    };
}
