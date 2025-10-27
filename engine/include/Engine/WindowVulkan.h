#pragma once
#include "Engine/Window.h"
#include <vulkan/vulkan.h>
#include <vector>

namespace Engine::WindowVulkan
{
    void GetRequiredVulkanExtensions(const IWindow& window, std::vector<const char*>& extensions);
}