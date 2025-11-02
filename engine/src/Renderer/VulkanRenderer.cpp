#include "Engine/Renderer/VulkanRenderer.h"
#include "Engine/WindowVulkan.h"
#include <ranges>
#include <stdexcept>
#include <vector>
#include <map>
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
        PickPhysicalDevice();
    }

    void VulkanRenderer::Shutdown()
    {
        // RAII wrappers (vk::raii::Instance) handle destruction automatically
        // Explicitly reset the optional to destroy the instance now
        // This also implicitly destroys the VkPhysicalDevice so no need to set it here
    }

    void VulkanRenderer::RenderFrame()
    {
        // Implementation for rendering a single frame using Vulkan
    }

    void VulkanRenderer::PickPhysicalDevice()
    {
        std::vector<vk::raii::PhysicalDevice> devices = m_Instance.enumeratePhysicalDevices();
        if (devices.empty()) 
        {
            throw std::runtime_error("Failed to find GPUs with Vulkan support!");
        }
        
        // TODO: Maybe application can dictate specific requirements needed 
        // Most of the time we would just choose the on-board GPU and call it a day
        const auto devIter = std::ranges::find_if(devices,
            [&](auto const & device) 
            {
                std::cout << "[VulkanRenderer] Device found: " <<  device.getProperties().deviceName << std::endl;
                auto queueFamiles = device.getQueueFamilyProperties();
                bool isSuitable = device.getProperties().apiVersion >= VK_API_VERSION_1_3;
                const auto qfpIter = std::ranges::find_if(queueFamiles,
                    [](vk::QueueFamilyProperties const & qfp)
                    {
                        return (qfp.queueFlags & vk::QueueFlagBits::eGraphics) != static_cast<vk::QueueFlags>(0);
                    });
                isSuitable = isSuitable && (qfpIter != queueFamiles.end());
                auto extensions = device.enumerateDeviceExtensionProperties();
                bool found = true;
                for (auto const & extension : m_DeviceExtensions)
                {
                    auto extensionIter = std::ranges::find_if(extensions, [extension](auto const & ext) { return strcmp(ext.extensionName, extension) == 0; });
                    found = found && extensionIter != extensions.end();
                }
                isSuitable = isSuitable && found;
                if (isSuitable)
                {
                    m_PhysicalDevice = std::make_unique<vk::raii::PhysicalDevice>(device);
                }
                return isSuitable;
            }
        );

        if (devIter == devices.end()) 
        {
            throw std::runtime_error("Failed to find suitable GPU");
        }

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
        m_Instance = vk::raii::Instance(m_Context, instanceCreateInfo);
    }
}
