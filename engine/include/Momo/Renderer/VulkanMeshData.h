#pragma once

#include <vulkan/vulkan_raii.hpp>
#include <cstdint>
#define GLM_FORCE_DEPTH_ZERO_TO_ONE // Vulkan depth [0, 1] range
#include <glm/glm.hpp>

namespace Momo {
namespace Renderer {
struct PushConstantData
{
    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
    glm::mat4 modelMatrix;
};

struct AllocatedImage
{
    vk::raii::DeviceMemory memory;
    vk::raii::Image image;
    vk::raii::ImageView imageView;
};
struct AllocatedBuffer
{
    vk::raii::DeviceMemory memory;
    vk::raii::Buffer buffer;
    uint32_t indexCount;
};

struct VulkanMeshData
{
    AllocatedBuffer m_VertexBuffer;
    AllocatedBuffer m_IndexBuffer;
};
} // namespace Renderer
} // namespace Momo