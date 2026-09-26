#pragma once
#include <vulkan/vulkan_raii.hpp>
#include <vector>
#include <optional>
#include <cstdint>
#include <unordered_map>
#define GLM_FORCE_DEPTH_ZERO_TO_ONE // Vulkan depth [0, 1] range
#include <glm/glm.hpp>
#include "Momo/Window.h"
#include "Momo/Geometry/Vertex.h"
#include "VulkanMeshData.h"
#include "Momo/Assets/ModelData.h"

namespace Momo {
namespace Renderer {

class VulkanRenderer
{
public:
    void Init(const IWindow& window);
    void Shutdown();
    void RenderFrame(Renderer::VulkanModelData& modelData, glm::mat4 viewMatrix, glm::mat4 modelMatrix);

    AllocatedBuffer CreateVertexBuffer(const std::vector<Momo::Geometry::Vertex>& vertices);
    AllocatedBuffer CreateIndexBuffer(const std::vector<uint32_t>& indices);
    AllocatedImage  CreateTextureImage(const Assets::TextureData& texture);
private:
    void CreateInstance();
    void PickPhysicalDevice();
    void CreateSurface();
    void CreateSwapChain();
    void RecreateSwapchain();
    void CreateLogicalDevice();
    void CreateImageView();
    void CreateGraphicsPipeline();
    void CreateCommandPool();
    void CreateCommandBuffer();
    void CreateSyncObjects();
    void CreateImageAvailableSemaphore();
    void CreateInFlightFence();
    void CreateRenderFinishedSemaphores();
    void CreateTextureSampler();
    void CreateDescriptorSetLayout();

    void BeginFrame(uint32_t imageIndex);
    void EndFrame(uint32_t imageIndex);
    vk::raii::ShaderModule CreateShaderModule(const std::vector<char>& code);
    uint32_t FindGraphicsQueueFamilyIdx(vk::raii::PhysicalDevice);
    uint32_t FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);

    AllocatedBuffer CreateBuffer(const void* data, vk::DeviceSize size, vk::BufferUsageFlags usage, uint32_t indexCount);
    AllocatedImage  CreateDepthBuffer();

    AllocatedBuffer CreateTextureStagingBuffer(const Assets::TextureData& texture);

    vk::raii::CommandBuffer BeginSingleUseCommandBuffer();
    void EndSingleUseCommandBuffer(vk::raii::CommandBuffer& commandBuffer);

    const IWindow* m_Window = nullptr;
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
    bool m_SubOptimal = false;

    // Depth buffer resources
    vk::Format m_DepthFormat;
    std::optional<AllocatedImage> m_DepthBuffer;

    // Texture Image resources
    std::optional<vk::raii::Sampler> m_TextureSampler;
    std::optional<vk::raii::DescriptorSetLayout> m_DescriptorSetLayout;
    std::optional<vk::raii::DescriptorPool> m_DescriptorPool;
    std::optional<vk::raii::DescriptorSet> m_DescriptorSet;

    std::unordered_map<Assets::TextureHandle, AllocatedImage>           m_TextureCache;
    std::unordered_map<Assets::MaterialHandle, vk::raii::DescriptorSet> m_MaterialDescriptorSets;

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

    glm::mat4 m_ProjectionMatrix;
};
} // namespace Renderer
} // namespace Momo
