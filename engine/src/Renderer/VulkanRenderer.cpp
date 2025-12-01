#include "Engine/Renderer/VulkanRenderer.h"
#include "Engine/WindowVulkan.h"
#include <GLFW/glfw3.h>
#include <ranges>
#include <stdexcept>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <cstring>
#include <set>
#include <algorithm>

#ifdef NDEBUG
    constexpr bool enableValidationLayers = false;
#else
    constexpr bool enableValidationLayers = true;
#endif

namespace 
{
    struct QueueFamilyIndices 
    {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete() const 
        {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }

        std::set<uint32_t> UniqueFamilies() const 
        {
            return {graphicsFamily.value(), presentFamily.value()};
        }
    };

    QueueFamilyIndices FindQueueFamilies(const vk::raii::PhysicalDevice& physicalDevice, const vk::raii::SurfaceKHR& surface)
    {
        QueueFamilyIndices indices;

        std::vector<vk::QueueFamilyProperties> queueFamilies = physicalDevice.getQueueFamilyProperties();

        int i = 0;
        for (const auto& queueFamily : queueFamilies) 
        {
            if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) 
            {
                indices.graphicsFamily = i;
            }

            VkBool32 presentSupport = physicalDevice.getSurfaceSupportKHR(i, static_cast<VkSurfaceKHR>(*surface));

            if (presentSupport) 
            {
                indices.presentFamily = i;
            }

            if (indices.isComplete()) 
            {
                break;
            }

            i++;
        }

        return indices;
    }

    vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats)
    {
        for (const auto& availableFormat : availableFormats) 
        {
            if (availableFormat.format == vk::Format::eR8G8B8A8Srgb && 
                availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) 
            {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes)
    {
        for (const auto& availablePresentMode : availablePresentModes) 
        {
            // Prefer Mailbox if available and energy is not a concern
            // Otherwise fall back to FIFO which is guaranteed to be available
            if (availablePresentMode == vk::PresentModeKHR::eMailbox) 
            {
                return availablePresentMode;
            }
        }

        return vk::PresentModeKHR::eFifo;
    }

    vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height)
    {
        if (capabilities.currentExtent.width != UINT32_MAX) 
        {
            return capabilities.currentExtent;
        } 
        else 
        {
            vk::Extent2D actualExtent = {
                std::clamp(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
                std::clamp(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
            };
            return actualExtent;
        }
    }

    static std::vector<char> ReadFile(const std::string& filename)
    {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open()) 
        {
            throw std::runtime_error("Failed to open file: " + filename);
        }

        std::vector<char> buffer(file.tellg());
        file.seekg(0);
        file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
        return buffer;
    }
}

namespace Engine
{
    void VulkanRenderer::Init(const IWindow& window)
    {
        CreateInstance(window);
        CreateSurface(window);
        PickPhysicalDevice();
        CreateLogicalDevice();
        CreateSwapChain(window);
        CreateImageView();
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

    void VulkanRenderer::CreateGraphicsPipeline()
    {
        auto vertShaderCode = ReadFile("shaders/triangle.vert.spv");
        auto fragShaderCode = ReadFile("shaders/triangle.frag.spv");
        vk::raii::ShaderModule vertShaderModule = CreateShaderModule(vertShaderCode);
        vk::raii::ShaderModule fragShaderModule = CreateShaderModule(fragShaderCode);

        // Create shader stage create infos
        vk::PipelineShaderStageCreateInfo vertShaderStageInfo(
            {},                                  // flags
            vk::ShaderStageFlagBits::eVertex,   // stage
            *vertShaderModule,                   // module
            "vertMain"                           // pName (entry point)
        );

        vk::PipelineShaderStageCreateInfo fragShaderStageInfo(
            {},                                  // flags
            vk::ShaderStageFlagBits::eFragment, // stage
            *fragShaderModule,                   // module
            "fragMain"                           // pName (entry point)
        );

        std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStages = {
            vertShaderStageInfo,
            fragShaderStageInfo
        };

        // TODO: Continue with pipeline creation using shaderStages
    }

    vk::raii::ShaderModule VulkanRenderer::CreateShaderModule(const std::vector<char>& code)
    {
        vk::ShaderModuleCreateInfo createInfo(
            {},                                 // flags
            code.size(),                       // codeSize
            reinterpret_cast<const uint32_t*>(code.data()) // pCode
        );

        vk::raii::ShaderModule shaderModule(m_Device.value(), createInfo);
        return shaderModule;
    }

    void VulkanRenderer::CreateImageView()
    {
        m_SwapchainImageViews.clear();
        
        // Our images will be used as color targets without any mipmapping levels or multiple layers.
        vk::ImageSubresourceRange subresourceRange(
            vk::ImageAspectFlagBits::eColor,  // aspectMask
            0,                                 // baseMipLevel
            1,                                 // levelCount
            0,                                 // baseArrayLayer
            1                                  // layerCount
        );

        /************************************************************************
        Default components mapping:
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        ************************************************************************/
        vk::ImageViewCreateInfo imageViewCreateInfo(
            {},                                // flags
            {},                                // image (will be set per-image)
            vk::ImageViewType::e2D,           // viewType
            m_SwapchainImageFormat.format,    // format
            {},                                // components (default mapping)
            subresourceRange                   // subresourceRange
        );

        for (const auto& swapchainImage : m_SwapchainImages)
        {
            imageViewCreateInfo.image = swapchainImage;
            vk::raii::ImageView imageView(m_Device.value(), imageViewCreateInfo);
            m_SwapchainImageViews.push_back(std::move(imageView));
            // Store or use the imageView as needed
            // For example, you might want to keep them in a member variable
        }
    }

    void VulkanRenderer::CreateSwapChain(const IWindow& window)
    {
        auto surfaceCapabilities = m_PhysicalDevice->getSurfaceCapabilitiesKHR(*m_Surface);
        m_SwapchainImageFormat = ChooseSwapSurfaceFormat(
            m_PhysicalDevice->getSurfaceFormatsKHR(*m_Surface)
        );
        m_SwapchainPresentMode = ChooseSwapPresentMode(
            m_PhysicalDevice->getSurfacePresentModesKHR(*m_Surface)
        );
        m_SwapchainExtent = ChooseSwapExtent(surfaceCapabilities, window.GetWidth(), window.GetHeight());

        uint32_t minImageCount = (surfaceCapabilities.maxImageCount > 0) 
            ? std::min(surfaceCapabilities.maxImageCount, std::max(surfaceCapabilities.minImageCount + 1, 2u)) 
            : std::max(surfaceCapabilities.minImageCount + 1, 2u);

        vk::SwapchainCreateInfoKHR swapChainCreateInfo(
            {},                                     // flags
            *m_Surface,                            // surface
            minImageCount,                        // minImageCount
            m_SwapchainImageFormat.format,        // imageFormat
            m_SwapchainImageFormat.colorSpace,    // imageColorSpace
            m_SwapchainExtent,                    // imageExtent
            1,                                    // imageArrayLayers -- always 1 unless you're developing stereoscopic 3D app
            vk::ImageUsageFlagBits::eColorAttachment, // imageUsage
            vk::SharingMode::eExclusive,          // imageSharingMode
            0, nullptr,                           // queueFamilyIndexCount, pQueueFamilyIndices
            surfaceCapabilities.currentTransform, // preTransform
            vk::CompositeAlphaFlagBitsKHR::eOpaque,    // compositeAlpha
            m_SwapchainPresentMode,               // presentMode
            VK_TRUE,                              // clipped
            nullptr                               // oldSwapchain
        );

        if (m_GraphicsQueueFamilyIdx != m_PresentQueueFamilyIdx)
        {
            uint32_t queueFamilyIndices[] = { m_GraphicsQueueFamilyIdx, m_PresentQueueFamilyIdx };
            swapChainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
            swapChainCreateInfo.queueFamilyIndexCount = 2;
            swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else
        {
            swapChainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
            swapChainCreateInfo.queueFamilyIndexCount = 0; // Optional
            swapChainCreateInfo.pQueueFamilyIndices = nullptr; // Optional
        }

        m_Swapchain = vk::raii::SwapchainKHR(m_Device.value(), swapChainCreateInfo);
        m_SwapchainImages = m_Swapchain->getImages();
    }

    void VulkanRenderer::CreateSurface(const IWindow& window)
    {
        // Create a Vulkan surface using the native window handle
        VkSurfaceKHR surface;
        GLFWwindow* glfwWindow = static_cast<GLFWwindow*>(window.GetNativeHandle());
        if (glfwCreateWindowSurface(static_cast<VkInstance>(**m_Instance), glfwWindow, nullptr, &surface) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create window surface!");
        }
        // Store or use the surface as needed
        m_Surface = vk::raii::SurfaceKHR(m_Instance.value(), surface);
    }

    void VulkanRenderer::CreateLogicalDevice()
    {
        auto indices = FindQueueFamilies(m_PhysicalDevice.value(), m_Surface.value());

        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;

       // query for Vulkan 1.3 features
        vk::StructureChain <
            vk::PhysicalDeviceFeatures2,
            vk::PhysicalDeviceVulkan13Features,
            vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT
        > featureChain;

        // TODO: Figure out what to do with these features.
        auto& features2             = featureChain.get<vk::PhysicalDeviceFeatures2>();
        auto& vulkan13Features      = featureChain.get<vk::PhysicalDeviceVulkan13Features>();
        auto& dynamicStateFeatures  = featureChain.get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();

        // create a Device
        float                     queuePriority = 1.0f;

        for (uint32_t queueFamily : indices.UniqueFamilies())
        {
            vk::DeviceQueueCreateInfo deviceQueueCreateInfo(
                {},
                queueFamily,
                1,
                &queuePriority
            );
            queueCreateInfos.push_back(deviceQueueCreateInfo);
        }

        // ... features setup ...

        vk::DeviceCreateInfo deviceCreateInfo(
            {},                                                 // flags
            static_cast<uint32_t>(queueCreateInfos.size()),     // queueCreateInfoCount
            queueCreateInfos.data(),                           // pQueueDeviceCreateInfos
            0, nullptr,                                         // EnabledLayerCount / EnabledLayerNames
            static_cast<uint32_t>(m_EnabledDeviceExtensions.size()),   // enabledExtensionCount
            m_EnabledDeviceExtensions.data(),                          // ppEnabledExtensionNames
            {},                                                 // pEnabledFeatures
            features2                                           // pNext
        );

        m_Device = vk::raii::Device(m_PhysicalDevice.value(), deviceCreateInfo);
        if (!m_Device)
        {
            throw std::runtime_error("Failed to create logical device!");
        }

        m_GraphicsQueue = vk::raii::Queue(m_Device.value(), m_GraphicsQueueFamilyIdx, 0);
        if (!m_GraphicsQueue)
        {
            throw std::runtime_error("Failed to retrieve graphics queue!");
        }

        m_PresentQueue = vk::raii::Queue(m_Device.value(), m_PresentQueueFamilyIdx, 0);
        if (!m_PresentQueue)
        {
            throw std::runtime_error("Failed to retrieve present queue!");
        }
    }

    void VulkanRenderer::PickPhysicalDevice()
    {
        std::vector<vk::raii::PhysicalDevice> devices = m_Instance->enumeratePhysicalDevices();
        if (devices.empty())
        {
            throw std::runtime_error("Failed to find GPUs with Vulkan support!");
        }

        // Separate function to check device suitability
        auto isDeviceSuitable = [&](const vk::raii::PhysicalDevice& device) -> bool
        {
            std::cout << "[VulkanRenderer] Evaluating device: " << device.getProperties().deviceName << std::endl;
            
            // Check API version
            if (device.getProperties().apiVersion < VK_API_VERSION_1_3)
            {
                std::cout << "[VulkanRenderer]   ❌ Does not support Vulkan 1.3\n";
                return false;
            }
            std::cout << "[VulkanRenderer]   ✓ Supports Vulkan 1.3\n";
            
            // Check for graphics and presentation queue
            auto indices = FindQueueFamilies(device, m_Surface.value());
            if (!indices.isComplete())
            {
                std::cout << "[VulkanRenderer]   ❌ Missing required queue families\n";
                return false;
            }

            std::cout << "[VulkanRenderer]   ✓ Graphics queue family found " << indices.graphicsFamily.value() << "\n";
            std::cout << "[VulkanRenderer]   ✓ Present queue family found " << indices.presentFamily.value() << "\n";

            if (indices.graphicsFamily.value() != indices.presentFamily.value())
            {
                std::cout << "[VulkanRenderer]   ℹ️  Graphics and Present queues are different\n";
            }
            else
            {
                std::cout << "[VulkanRenderer]   ℹ️  Graphics and Present queues are the same\n";
            }

            
            // Check extensions
            auto extensions = device.enumerateDeviceExtensionProperties();
            for (const char* requiredExt : m_RequestedDeviceExtensions)
            {
                auto extIter = std::ranges::find_if(extensions, [requiredExt](const auto& ext) {
                    return strcmp(ext.extensionName, requiredExt) == 0;
                });
                
                if (extIter == extensions.end())
                {
                    std::cout << "[VulkanRenderer]   ❌ Missing extension: " << requiredExt << "\n";
                    return false;
                }
                std::cout << "[VulkanRenderer]   ✓ Supports " << requiredExt << "\n";
            }
            
            return true;
        };
        
        // Find first suitable device
        auto deviceIter = std::ranges::find_if(devices, isDeviceSuitable);
        
        if (deviceIter == devices.end())
        {
            throw std::runtime_error("Failed to find suitable GPU");
        }
        
        // Only NOW do we assign and populate enabled extensions
        m_PhysicalDevice = *deviceIter;
        m_EnabledDeviceExtensions = m_RequestedDeviceExtensions;  // All were validated

        // Check and add optional extensions if available
        auto availableExtensions = m_PhysicalDevice->enumerateDeviceExtensionProperties();
        for (const char* optionalExt : m_OptionalDeviceExtensions)
        {
            auto extIter = std::ranges::find_if(availableExtensions, [optionalExt](const auto& ext) {
                return strcmp(ext.extensionName, optionalExt) == 0;
            });
            
            if (extIter != availableExtensions.end())
            {
                std::cout << "[VulkanRenderer]   ✓ Enabling optional extension: " << optionalExt << "\n";
                m_EnabledDeviceExtensions.push_back(optionalExt);
            }
            else
            {
                std::cout << "[VulkanRenderer]   ℹ️  Optional extension not available: " << optionalExt << "\n";
            }
        }
        
        std::cout << "[VulkanRenderer] ✅ Selected device: " 
                << m_PhysicalDevice->getProperties().deviceName << std::endl;
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
