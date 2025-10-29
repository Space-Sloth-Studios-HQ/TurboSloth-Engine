#include "Engine/Renderer/VulkanRenderer.h"
#include "Engine/WindowVulkan.h"
#include <ranges>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <cstring>

#ifdef NDEBUG
    constexpr bool enableValidationLayers = false;
#else
    constexpr bool enableValidationLayers = true;
#endif

namespace Engine 
{
    void VulkanRenderer::Init(const IWindow& window)
    {
        CreateInstance(window);
    }

    void VulkanRenderer::Shutdown()
    {
        // Implementation for cleaning up Vulkan resources
    }

    void VulkanRenderer::RenderFrame()
    {
        // Implementation for rendering a single frame using Vulkan
    }

    void VulkanRenderer::CreateInstance(const IWindow& window)
    {
        std::cout << "[VulkanRenderer] Creating Vulkan instance...\n";
        vk::ApplicationInfo appInfo(
            "Engine",                      // pApplicationName
            VK_MAKE_VERSION(1, 0, 0),     // applicationVersion
            "No Engine",                   // pEngineName
            VK_MAKE_VERSION(1, 0, 0),     // engineVersion
            vk::ApiVersion14              // apiVersion
        );

        // Get required extensions from GLFW
        std::vector<const char*> extensions;
        Engine::WindowVulkan::GetRequiredVulkanExtensions(window, extensions);

        // Check if the required GLFW extensions are supported by Vulkan implementation
        auto extensionProperties = m_Context.enumerateInstanceExtensionProperties();
        for (const char* requiredExt : extensions)
        {
            if (std::ranges::none_of(extensionProperties,
                [&](const vk::ExtensionProperties& prop) {
                    return std::strcmp(prop.extensionName, requiredExt) == 0;
                }))
            {
                throw std::runtime_error("Required GLFW Vulkan extension not supported: " + std::string(requiredExt));
            }
        }

        // Add portability enumeration extension for MoltenVK on macOS
        extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

        // Add debug utils extension
        if (enableValidationLayers)
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        vk::InstanceCreateInfo instanceCreateInfo(
            vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR,  // flags - required for MoltenVK
            &appInfo,                                     // pApplicationInfo
            0, nullptr,                                   // enabled layers (count, names)
            static_cast<uint32_t>(extensions.size()),    // enabled extensions count
            extensions.data()                             // enabled extension names
        );
        // Create the Vulkan instance
        m_Instance.emplace(m_Context, instanceCreateInfo);
    }
}