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
        void CreateSurface(const IWindow& window);
        void CreateSwapChain(const IWindow& window);
        void RecreateSwapchain(uint32_t width, uint32_t height);
        void CreateLogicalDevice();
        void CreateImageView();
        void CreateGraphicsPipeline();
        void CreateCommandPool();
        void CreateCommandBuffer();
        void CreateSyncObjects();
        vk::raii::ShaderModule CreateShaderModule(const std::vector<char>& code);
        uint32_t FindGraphicsQueueFamilyIdx(vk::raii::PhysicalDevice);

        vk::raii::Context  m_Context;
        std::optional<vk::raii::Instance> m_Instance;
        std::optional<vk::raii::SurfaceKHR> m_Surface;
        std::optional<vk::raii::PhysicalDevice> m_PhysicalDevice;
        std::optional<vk::raii::Device> m_Device;

        // Swapchain details
        std::optional<vk::raii::SwapchainKHR> m_Swapchain;
        vk::SurfaceFormatKHR m_SwapchainImageFormat;
        vk::PresentModeKHR m_SwapchainPresentMode;
        vk::Extent2D m_SwapchainExtent;
        std::vector<vk::Image> m_SwapchainImages;
        std::vector<vk::raii::ImageView> m_SwapchainImageViews;

        // Graphics pipeline
        std::optional<vk::raii::PipelineLayout> m_PipelineLayout;
        std::optional<vk::raii::Pipeline> m_GraphicsPipeline;

        // Command buffers
        std::optional<vk::raii::CommandPool> m_CommandPool;
        std::optional<vk::raii::CommandBuffer> m_CommandBuffer;

        // Sync objects
        std::optional<vk::raii::Semaphore> m_ImageAvailableSemaphore;
        std::vector<vk::raii::Semaphore> m_RenderFinishedSemaphores;
        std::optional<vk::raii::Fence> m_InFlightFence;

        uint32_t m_GraphicsQueueFamilyIdx = 0;
        uint32_t m_PresentQueueFamilyIdx = 0;
        std::optional<vk::raii::Queue> m_GraphicsQueue;
        std::optional<vk::raii::Queue> m_PresentQueue;

        // Intended for use when setting up Vulkan validation layers in instance creation.
        std::vector<char const*> m_ValidationLayers = {
            "VK_LAYER_KHRONOS_validation"
        };

        std::vector<const char*> m_RequestedDeviceExtensions = {
            vk::KHRSwapchainExtensionName,
            vk::KHRDynamicRenderingExtensionName
        };

        std::vector<const char*> m_OptionalDeviceExtensions = {
            "VK_KHR_portability_subset", // For MoltenVK on macOS
        };

        std::vector<const char*> m_EnabledDeviceExtensions;
    };
}
