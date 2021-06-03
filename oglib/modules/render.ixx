/*
* Vulkan renderer.
*/

module;

/*
* Includes.
*/

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#define GLFW_INCLUDE_VULKAN
#include <glfw3.h>

#include <vulkan/vulkan.h>

/*
* Begin module render.
*/

export module render;

/*
* Imports.
*/

import type;
import acs;
import component;

/*
* Global state.
*/

constexpr s8 const* DEBUG_LAYER{ "VK_LAYER_KHRONOS_validation" };

VkDebugReportCallbackEXT sDebugCallback   {};
VkInstance               sVkInstance      {};
VkSurfaceKHR             sVkWindowSurface {};
VkPhysicalDevice         sVkPhysicalDevice{};
VkDevice                 sVkLogicalDevice {};
VkCommandPool            sVkCommandPool   {};

/*
* Debug callback.
*/

u32 DebugCallback(
  VkDebugReportFlagsEXT flags,
  VkDebugReportObjectTypeEXT objType,
  u64 srcObject,
  u32 location,
  u32 msgCode,
  s8 const* pLayerPrefix,
  s8 const* pMessage,
  void* pUserData)
{
  if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
  {
    std::cout << "Error: [" << pLayerPrefix << "] Code " << msgCode << " : " << pMessage << std::endl;
  }
  else if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
  {
    std::cout << "Warning: [" << pLayerPrefix << "] Code " << msgCode << " : " << pMessage << std::endl;
  }
  return 0;
}

/*
* Creational render routines.
*/

void CreateInstance(std::vector<s8 const*> extensions, u32 debug)
{
  // App info
  VkApplicationInfo vkAppInfo{};
  vkAppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  vkAppInfo.pApplicationName = "Vulkan";
  vkAppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  vkAppInfo.pEngineName = "VulkanEngine";
  vkAppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  vkAppInfo.apiVersion = VK_API_VERSION_1_0;
  // Create info
  VkInstanceCreateInfo vkCreateInfo{};
  vkCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  vkCreateInfo.pApplicationInfo = &vkAppInfo;
  vkCreateInfo.enabledExtensionCount = (u32)extensions.size();
  vkCreateInfo.ppEnabledExtensionNames = extensions.data();
  if (debug)
  {
    vkCreateInfo.enabledLayerCount = 1;
    vkCreateInfo.ppEnabledLayerNames = &DEBUG_LAYER;
  }
  // Initialize vulkan
  if (vkCreateInstance(&vkCreateInfo, nullptr, &sVkInstance) != VK_SUCCESS)
  {
    return;
  }
}
void CreateDebugCallback()
{
  // Create info
  VkDebugReportCallbackCreateInfoEXT vkCreateInfo{};
  vkCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
  vkCreateInfo.pfnCallback = (PFN_vkDebugReportCallbackEXT)DebugCallback;
  vkCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
  // Gather create debug routine handle
  auto createDebugReportCallback{ (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(sVkInstance, "vkCreateDebugReportCallbackEXT") };
  if (createDebugReportCallback(sVkInstance, &vkCreateInfo, nullptr, &sDebugCallback) != VK_SUCCESS)
  {
    return;
  }
}
void CreateWindowSurface(GLFWwindow* pWindow)
{
  if (glfwCreateWindowSurface(sVkInstance, pWindow, nullptr, &sVkWindowSurface) != VK_SUCCESS)
  {
    return;
  }
}
void CreatePhysicalDevice()
{
  u32 deviceSize{};
  if (vkEnumeratePhysicalDevices(sVkInstance, &deviceSize, nullptr) != VK_SUCCESS)
  {
    return;
  }
  if (deviceSize <= 0)
  {
    return;
  }
  // Gather first device
  if (vkEnumeratePhysicalDevices(sVkInstance, &deviceSize, &sVkPhysicalDevice) != VK_SUCCESS)
  {
    return;
  }
}
void CreateLogicalDevice(u32 debug)
{
  // Create info
  VkDeviceCreateInfo vkCreateInfo{};
  vkCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  if (debug)
  {
    vkCreateInfo.enabledLayerCount = 1;
    vkCreateInfo.ppEnabledLayerNames = &DEBUG_LAYER;
  }
  // Create device
  if (vkCreateDevice(sVkPhysicalDevice, &vkCreateInfo, nullptr, &sVkLogicalDevice) != VK_SUCCESS)
  {
    return;
  }
}
void CreateCommandPool()
{
  // Create info
  VkCommandPoolCreateInfo vkCreateInfo{};
  vkCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  vkCreateInfo.queueFamilyIndex = -1;
  // Create pool
  if (vkCreateCommandPool(sVkLogicalDevice, &vkCreateInfo, nullptr, &sVkCommandPool) != VK_SUCCESS)
  {
    return;
  }
}

/*
* 
*/

void CreateVertexBuffer()
{
  
}
void CreateUniformBuffer()
{

}

/*
* Debug routines.
*/

void DumpPhysicalDevices()
{
  u32 deviceSize{};
  VkPhysicalDevice vkPhysicalDevice{};
  VkPhysicalDeviceProperties vkDeviceProperties{};
  if (vkEnumeratePhysicalDevices(sVkInstance, &deviceSize, nullptr) != VK_SUCCESS)
  {
    return;
  }
  for (u32 i{ deviceSize }; i <= deviceSize; ++i)
  {
    if (vkEnumeratePhysicalDevices(sVkInstance, &i, &vkPhysicalDevice) != VK_SUCCESS)
    {
      return;
    }
    vkGetPhysicalDeviceProperties(vkPhysicalDevice, &vkDeviceProperties);
    std::cout << "ApiVersion:" << vkDeviceProperties.apiVersion << std::endl;
    std::cout << "DriverVersion:" << vkDeviceProperties.driverVersion << std::endl;
    std::cout << "VendorId:" << vkDeviceProperties.vendorID << std::endl;
    std::cout << "DeviceId:" << vkDeviceProperties.deviceID << std::endl;
    std::cout << "DeviceType:" << vkDeviceProperties.deviceType << std::endl;
    std::cout << "DeviceName:" << vkDeviceProperties.deviceName << std::endl;
  }
}

/*
* Public render routines.
*/

export void CreateRenderer(GLFWwindow* pWindow, std::vector<s8 const*> extensions, u32 debug)
{
  CreateInstance(extensions, debug);
  if (debug)
  {
    CreateDebugCallback();
  }
  CreateWindowSurface(pWindow);
  CreatePhysicalDevice();
  CreateLogicalDevice(debug);
  CreateCommandPool();

  CreateVertexBuffer();
  CreateUniformBuffer();

  //DumpPhysicalDevices();
}
export void DestroyRenderer()
{

}