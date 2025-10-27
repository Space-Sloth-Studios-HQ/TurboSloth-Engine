#include "Engine/WindowVulkan.h"
#include <GLFW/glfw3.h>
#include <stdexcept>

namespace Engine::WindowVulkan
{
    void GetRequiredVulkanExtensions(const IWindow& window, std::vector<const char*>& extensions)
    {
        // Get the required extensions from GLFW
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        if (glfwExtensions == nullptr)
        {
            throw std::runtime_error("Failed to get required Vulkan extensions from GLFW. Vulkan may not be supported.");
        }

        // Add GLFW extensions to the vector
        for (uint32_t i = 0; i < glfwExtensionCount; ++i)
        {
            extensions.push_back(glfwExtensions[i]);
        }
    }
}
