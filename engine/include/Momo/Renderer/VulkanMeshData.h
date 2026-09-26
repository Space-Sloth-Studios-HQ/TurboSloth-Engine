#pragma once

#include <vulkan/vulkan_raii.hpp>
#include <cstdint>
#include <optional>
#define GLM_FORCE_DEPTH_ZERO_TO_ONE // Vulkan depth [0, 1] range
#include <glm/glm.hpp>

namespace Momo {
namespace Renderer {
struct PushConstantData
{
    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
    glm::mat4 modelMatrix;
    glm::vec4 baseColorFactor;
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
    AllocatedBuffer vertexBuffer;
    AllocatedBuffer indexBuffer;
    glm::mat4 localTransform;
    glm::vec4 baseColorFactor;
};

struct VulkanModelData
{
    std::vector<VulkanMeshData> meshes;
    std::optional<AllocatedImage> textureImage;
};
} // namespace Renderer
} // namespace Momo