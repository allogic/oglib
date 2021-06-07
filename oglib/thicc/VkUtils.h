#ifndef VK_UTILS
#define VK_UTILS

#include "VkCore.h"

/*
* Some utilities.
*/

namespace VkUtils
{
  /*
  * Vulkan helper routines.
  */
  
  static std::vector<VkLayerProperties>     GetLayerProperties()
  {
    u32 layerCount{};
    VK_VALIDATE(vkEnumerateInstanceLayerProperties(&layerCount, nullptr));
    std::vector<VkLayerProperties> vkLayers{ layerCount };
    VK_VALIDATE(vkEnumerateInstanceLayerProperties(&layerCount, vkLayers.data()));
    return vkLayers;
  }
  static std::vector<VkExtensionProperties> GetInstanceExtensionProperties(s8 const* pVkLayerName)
  {
    u32 extensionCount{};
    VK_VALIDATE(vkEnumerateInstanceExtensionProperties(pVkLayerName, &extensionCount, nullptr));
    std::vector<VkExtensionProperties> vkExtensions{ extensionCount };
    VK_VALIDATE(vkEnumerateInstanceExtensionProperties(pVkLayerName, &extensionCount, vkExtensions.data()));
    return vkExtensions;
  }
  static std::vector<VkExtensionProperties> GetSupportedExtensionsProperties(VkPhysicalDevice vkPhysicalDevice)
  {
    u32 extensionCount{};
    VK_VALIDATE(vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &extensionCount, nullptr));
    std::vector<VkExtensionProperties> vkExtensions{ extensionCount };
    VK_VALIDATE(vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &extensionCount, vkExtensions.data()));
    return vkExtensions;
  }

  static std::vector<s8 const*>             GetLayerPropertyNames(std::vector<VkLayerProperties> const& vkLayerProperties)
  {
    std::vector<s8 const*> vkLayerNames{};
    for (auto const& vkLayerProperty : vkLayerProperties)
    {
      vkLayerNames.emplace_back(vkLayerProperty.layerName);
    }
    return vkLayerNames;
  }
  static std::vector<s8 const*>             GetExtensionPropertyNames(std::vector<VkExtensionProperties> const& vkExtensionProperties)
  {
    std::vector<s8 const*> vkExtensionsNames{};
    for (auto const& vkExtensionProperty : vkExtensionProperties)
    {
      vkExtensionsNames.emplace_back(vkExtensionProperty.extensionName);
    }
    return vkExtensionsNames;
  }
  static std::vector<s8 const*>             GetRequiredExtensionNames(u32 debugEnabled)
  {
    u32 extensionCount{};
    s8 const** ppVkExtensions{ glfwGetRequiredInstanceExtensions(&extensionCount) };
    std::vector<s8 const*> vkRequiredExtensions{};
    for (u32 i{}; i < extensionCount; ++i)
    {
      vkRequiredExtensions.emplace_back(ppVkExtensions[i]);
    }
    if (debugEnabled)
    {
      vkRequiredExtensions.emplace_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    }
    return vkRequiredExtensions;
  }
}

#endif