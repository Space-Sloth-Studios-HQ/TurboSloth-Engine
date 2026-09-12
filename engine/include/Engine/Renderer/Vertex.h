#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace Engine {
class Vertex {
public:
    glm::vec3 position;
    glm::vec3 color;
    glm::vec3 normal;
    // glm::vec2 texCoord;

    static vk::VertexInputBindingDescription GetBindingDescription() {
        vk::VertexInputBindingDescription bindingDescription(
            0,                          // binding
            sizeof(Vertex),             // stride
            vk::VertexInputRate::eVertex // inputRate
        );
        return bindingDescription;
    }

    static std::array<vk::VertexInputAttributeDescription, 3> GetAttributeDescriptions() {
        std::array<vk::VertexInputAttributeDescription, 3> attributeDescriptions = {};
        attributeDescriptions[0] = vk::VertexInputAttributeDescription(
            0,                          // location
            0,                          // binding
            vk::Format::eR32G32B32Sfloat,  // format
            offsetof(Vertex, position)  // offset
        );
        attributeDescriptions[1] = vk::VertexInputAttributeDescription(
            1,                          // location
            0,                          // binding
            vk::Format::eR32G32B32Sfloat, // format
            offsetof(Vertex, color)    // offset
        );
        attributeDescriptions[2] = vk::VertexInputAttributeDescription(
            2,                          // location
            0,                          // binding
            vk::Format::eR32G32B32Sfloat, // format
            offsetof(Vertex, normal)   // offset
        );
        return attributeDescriptions;
    }
};
} // namespace Engine