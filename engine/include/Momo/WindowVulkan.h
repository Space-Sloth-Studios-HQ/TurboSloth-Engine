#pragma once
#include "Momo/Window.h"
#include <vulkan/vulkan.h>
#include <vector>

namespace Momo::WindowVulkan
{
    void GetRequiredVulkanExtensions(const IWindow& window, std::vector<const char*>& extensions);
}