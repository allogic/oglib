#include "VkRenderer.h"

VkRenderer::VkRenderer(u32 width, u32 height, GLFWwindow* pGlfwWindow, u32 debug)
  : mWidth{ width }
  , mHeight{ height }
  , mpGlfwWindow{ pGlfwWindow }
  , mDebug{ debug }
{
  CreateInstance();
  CreateDebugCallback();
  CreateWindowSurface();
  CreatePhysicalDevice();

  FindQueueFamilies();

  CreateLogicalDevice();
  CreateCommandPool();
  CreateSwapChain();

  CreateVertexBuffer();
  CreateUniformBuffer();
}
VkRenderer::~VkRenderer()
{

}

u32 VkRenderer::DebugCallback(VkDebugReportFlagsEXT vkFlags, VkDebugReportObjectTypeEXT vkObjType, u64 srcObject, u32 location, u32 msgCode, s8 const* pLayerPrefix, s8 const* pMessage, void* pUserData)
{
  if (vkFlags & (VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT))
  {
    VK_LOG("%s\n%u\n%s\n", pLayerPrefix, msgCode, pMessage);
  }
  return 0;
}
u32 VkRenderer::GetMemoryType(VkPhysicalDeviceMemoryProperties vkProperties, u32 typeBits, u32 properties, u32* typeIndex)
{
  for (u32 i = 0; i < 32; i++)
  {
    if ((typeBits & 1) == 1)
    {
      if ((vkProperties.memoryTypes[i].propertyFlags & properties) == properties)
      {
        *typeIndex = i;
        return 1;
      }
    }
    typeBits >>= 1;
  }
  return 0;
}
VkSurfaceFormatKHR VkRenderer::GetSurfaceFormat(std::vector<VkSurfaceFormatKHR> const& vkFormats)
{
  if (vkFormats.size() == 1 && vkFormats[0].format == VK_FORMAT_UNDEFINED)
  {
    return { VK_FORMAT_R8G8B8A8_UNORM, VK_COLORSPACE_SRGB_NONLINEAR_KHR };
  }
  for (auto const& vkSurfaceFormat : vkFormats)
  {
    if (vkSurfaceFormat.format == VK_FORMAT_R8G8B8A8_UNORM)
    {
      return vkSurfaceFormat;
    }
  }
  return vkFormats[0];
}
VkExtent2D VkRenderer::GetSwapExtent(u32 width, u32 height, VkSurfaceCapabilitiesKHR const& vkSurfaceCapabilities)
{
  if (vkSurfaceCapabilities.currentExtent.width == -1)
  {
    VkExtent2D vkSwapChainExtent{};
    vkSwapChainExtent.width = std::min(std::max(width, vkSurfaceCapabilities.minImageExtent.width), vkSurfaceCapabilities.maxImageExtent.width);
    vkSwapChainExtent.height = std::min(std::max(height, vkSurfaceCapabilities.minImageExtent.height), vkSurfaceCapabilities.maxImageExtent.height);
    return vkSwapChainExtent;
  }
  else
  {
    return vkSurfaceCapabilities.currentExtent;
  }
}
VkPresentModeKHR VkRenderer::GetPresentMode(std::vector<VkPresentModeKHR> const& vkPresentModes)
{
  for (auto const& vkPresentMode : vkPresentModes)
  {
    if (vkPresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
    {
      return vkPresentMode;
    }
  }
  return VK_PRESENT_MODE_FIFO_KHR;
}

void VkRenderer::CreateInstance()
{
  // Gather validation layers
  mVkLayerProperties = VkUtils::GetLayerProperties();
  mVkLayerPropertyNames = VkUtils::GetLayerPropertyNames(mVkLayerProperties);
  std::printf("Available layers:\n");
  for (auto const& pVkLayerProperyName : mVkLayerPropertyNames)
  {
    std::printf("\t%s\n", pVkLayerProperyName);
  }
  // Gather instance extensions
  std::printf("Instance extensions:\n");
  for (auto const& pVkLayerProperyName : mVkLayerPropertyNames)
  {
    std::printf("\t%s\n", pVkLayerProperyName);
    std::vector<VkExtensionProperties> vkInstanceExtensions{ VkUtils::GetInstanceExtensionProperties(pVkLayerProperyName) };
    for (auto const& vkInstanceExtension : vkInstanceExtensions)
    {
      std::printf("\t\t%s\n", vkInstanceExtension.extensionName);
    }
  }
  // Gather required extensions
  mVkRequiredExtensionPropertyNames = VkUtils::GetRequiredExtensionNames(mDebug);
  std::printf("Required extensions:\n");
  for (auto const& pVkRequiredExtensionPropertyName : mVkRequiredExtensionPropertyNames)
  {
    std::printf("\t%s\n", pVkRequiredExtensionPropertyName);
  }
  // Application info
  VkApplicationInfo vkApplicationInfo{};
  vkApplicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  vkApplicationInfo.pApplicationName = "VulkanApplication";
  vkApplicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  vkApplicationInfo.pEngineName = "VulkanEngine";
  vkApplicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  vkApplicationInfo.apiVersion = VK_API_VERSION_1_0;
  // Instance create info
  VkInstanceCreateInfo vkInstanceCreateInfo{};
  vkInstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  vkInstanceCreateInfo.pApplicationInfo = &vkApplicationInfo;
  vkInstanceCreateInfo.enabledExtensionCount = (u32)mVkRequiredExtensionPropertyNames.size();
  vkInstanceCreateInfo.ppEnabledExtensionNames = mVkRequiredExtensionPropertyNames.data();
  if (mDebug)
  {
    vkInstanceCreateInfo.enabledLayerCount = 1;
    vkInstanceCreateInfo.ppEnabledLayerNames = &VK_DEBUG_LAYER;
  }
  // Initialize vulkan
  VK_VALIDATE(vkCreateInstance(&vkInstanceCreateInfo, nullptr, &mVkInstance));
}
void VkRenderer::CreateDebugCallback()
{
  // Debug report callback create info
  VkDebugReportCallbackCreateInfoEXT vkDebugReportCallbackCreateInfo{};
  vkDebugReportCallbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
  vkDebugReportCallbackCreateInfo.pfnCallback = (PFN_vkDebugReportCallbackEXT)DebugCallback;
  vkDebugReportCallbackCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
  // Gather create debug routine handle
  auto createDebugReportCallback{ (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(mVkInstance, "vkCreateDebugReportCallbackEXT") };
  // Create callback
  VK_VALIDATE(createDebugReportCallback(mVkInstance, &vkDebugReportCallbackCreateInfo, nullptr, &mVkDebugCallback));
}
void VkRenderer::CreateWindowSurface()
{
  VK_VALIDATE(glfwCreateWindowSurface(mVkInstance, mpGlfwWindow, nullptr, &mVkWindowSurface));
}
void VkRenderer::CreatePhysicalDevice()
{
  // Gather physical device counts
  u32 deviceCount{};
  VK_VALIDATE(vkEnumeratePhysicalDevices(mVkInstance, &deviceCount, nullptr));
  if (deviceCount <= 0)
  {
    std::printf("No physical device present\n");
    return;
  }
  // Gather first device
  VK_VALIDATE(vkEnumeratePhysicalDevices(mVkInstance, &deviceCount, &mVkPhysicalDevice));
  // Gather supported extensions
  mVkSupportedExtensionProperties = VkUtils::GetSupportedExtensionsProperties(mVkPhysicalDevice);
  mVkSupportedExtensionPropertyNames = VkUtils::GetExtensionPropertyNames(mVkSupportedExtensionProperties);
  std::printf("Supported extensions:\n");
  for (auto const& pVkSupportedExtensionPropertyName : mVkSupportedExtensionPropertyNames)
  {
    std::printf("\t%s\n", pVkSupportedExtensionPropertyName);
  }
}
void VkRenderer::CreateLogicalDevice()
{
  r32 queuePriority{ 1.f };
  // Device queue create infos
  VkDeviceQueueCreateInfo vkDeviceQueueCreateInfos[2]{};
  vkDeviceQueueCreateInfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  vkDeviceQueueCreateInfos[0].queueFamilyIndex = mGraphicsQueueFamily.value();
  vkDeviceQueueCreateInfos[0].queueCount = 1;
  vkDeviceQueueCreateInfos[0].pQueuePriorities = &queuePriority;
  vkDeviceQueueCreateInfos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  vkDeviceQueueCreateInfos[1].queueFamilyIndex = mPresentQueueFamily.value();
  vkDeviceQueueCreateInfos[1].queueCount = 1;
  vkDeviceQueueCreateInfos[1].pQueuePriorities = &queuePriority;
  // Device create info
  VkDeviceCreateInfo vkDeviceCreateInfo{};
  vkDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  vkDeviceCreateInfo.pQueueCreateInfos = vkDeviceQueueCreateInfos;
  vkDeviceCreateInfo.enabledExtensionCount = (u32)mVkRequiredExtensionPropertyNames.size();
  vkDeviceCreateInfo.ppEnabledExtensionNames = mVkRequiredExtensionPropertyNames.data();
  if (mGraphicsQueueFamily.value() == mPresentQueueFamily.value())
  {
    vkDeviceCreateInfo.queueCreateInfoCount = 1;
  }
  else
  {
    vkDeviceCreateInfo.queueCreateInfoCount = 2;
  }
  if (mDebug)
  {
    vkDeviceCreateInfo.enabledLayerCount = 1;
    vkDeviceCreateInfo.ppEnabledLayerNames = &VK_DEBUG_LAYER;
  }
  // Create device
  VK_VALIDATE(vkCreateDevice(mVkPhysicalDevice, &vkDeviceCreateInfo, nullptr, &mVkLogicalDevice));
  // Gather queues
  vkGetDeviceQueue(mVkLogicalDevice, mGraphicsQueueFamily.value(), 0, &mVkGraphicsQueue);
  vkGetDeviceQueue(mVkLogicalDevice, mPresentQueueFamily.value(), 0, &mVkPresentQueue);
  // Gather device properties
  vkGetPhysicalDeviceMemoryProperties(mVkPhysicalDevice, &mVkPhysicalDeviceMemoryProperties);
}
void VkRenderer::CreateCommandPool()
{
  // Command pool create info
  VkCommandPoolCreateInfo vkCommandPoolCreateInfo{};
  vkCommandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  vkCommandPoolCreateInfo.queueFamilyIndex = mGraphicsQueueFamily.value();
  // Create command pool
  VK_VALIDATE(vkCreateCommandPool(mVkLogicalDevice, &vkCommandPoolCreateInfo, nullptr, &mVkCommandPool));
}
void VkRenderer::CreateSwapChain()
{
  // Find surface capabilities
  VkSurfaceCapabilitiesKHR vkSurfaceCapabilitiesKhr{};
  VK_VALIDATE(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mVkPhysicalDevice, mVkWindowSurface, &vkSurfaceCapabilitiesKhr));
  // Find surface formats
  u32 formatCount{};
  VK_VALIDATE(vkGetPhysicalDeviceSurfaceFormatsKHR(mVkPhysicalDevice, mVkWindowSurface, &formatCount, nullptr));
  std::vector<VkSurfaceFormatKHR> vkSurfaceFormatsKhr{ formatCount };
  VK_VALIDATE(vkGetPhysicalDeviceSurfaceFormatsKHR(mVkPhysicalDevice, mVkWindowSurface, &formatCount, vkSurfaceFormatsKhr.data()));
  // Find supported present modes
  u32 presentModeCount{};
  VK_VALIDATE(vkGetPhysicalDeviceSurfacePresentModesKHR(mVkPhysicalDevice, mVkWindowSurface, &presentModeCount, nullptr));
  std::vector<VkPresentModeKHR> vkPresentModesKhr{};
  VK_VALIDATE(vkGetPhysicalDeviceSurfacePresentModesKHR(mVkPhysicalDevice, mVkWindowSurface, &presentModeCount, vkPresentModesKhr.data()));
  // Gather number of images for swapchain
  u32 requiredImageCount{ vkSurfaceCapabilitiesKhr.minImageCount };
  if ((vkSurfaceCapabilitiesKhr.maxImageCount != 0) && requiredImageCount > vkSurfaceCapabilitiesKhr.maxImageCount)
  {
    requiredImageCount = vkSurfaceCapabilitiesKhr.maxImageCount;
  }
  std::printf("Images required for swapchain %u\n", requiredImageCount);
  // Get swap chain settings
  VkSurfaceFormatKHR vkSurfaceFormatKhr{ GetSurfaceFormat(vkSurfaceFormatsKhr) };
  // Get swap chain size
  mVkSwapChainExtend = GetSwapExtent(mWidth, mHeight, vkSurfaceCapabilitiesKhr);
  // Get default surface transformation
  VkSurfaceTransformFlagBitsKHR vkSurfaceTransformFlagsKhr{ vkSurfaceCapabilitiesKhr.currentTransform };
  if (vkSurfaceCapabilitiesKhr.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
  {
    vkSurfaceTransformFlagsKhr = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
  }
  // Choose presentation mode
  VkPresentModeKHR vkPresentMode{ GetPresentMode(vkPresentModesKhr) };
  // Swap chain create info
  VkSwapchainCreateInfoKHR vkSwapChainCreateInfoKhr{};
  vkSwapChainCreateInfoKhr.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  vkSwapChainCreateInfoKhr.surface = mVkWindowSurface;
  vkSwapChainCreateInfoKhr.minImageCount = requiredImageCount;
  vkSwapChainCreateInfoKhr.imageFormat = vkSurfaceFormatKhr.format;
  vkSwapChainCreateInfoKhr.imageColorSpace = vkSurfaceFormatKhr.colorSpace;
  vkSwapChainCreateInfoKhr.imageExtent = mVkSwapChainExtend;
  vkSwapChainCreateInfoKhr.imageArrayLayers = 1;
  vkSwapChainCreateInfoKhr.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  vkSwapChainCreateInfoKhr.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  vkSwapChainCreateInfoKhr.queueFamilyIndexCount = 0;
  vkSwapChainCreateInfoKhr.pQueueFamilyIndices = nullptr;
  vkSwapChainCreateInfoKhr.preTransform = vkSurfaceTransformFlagsKhr;
  vkSwapChainCreateInfoKhr.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  vkSwapChainCreateInfoKhr.presentMode = vkPresentMode;
  vkSwapChainCreateInfoKhr.clipped = 1;
  vkSwapChainCreateInfoKhr.oldSwapchain = mVkSwapChainKhrOld;
  // Create swap chain
  VK_VALIDATE(vkCreateSwapchainKHR(mVkLogicalDevice, &vkSwapChainCreateInfoKhr, nullptr, &mVkSwapChainKhr));
  // Destroy old swap chain
  if (mVkSwapChainKhrOld)
  {
    vkDestroySwapchainKHR(mVkLogicalDevice, mVkSwapChainKhrOld, nullptr);
  }
  // Reset swap chain
  mVkSwapChainKhrOld = mVkSwapChainKhr;
  mVkSwapChainFormat = vkSurfaceFormatKhr.format;
  // Gather swap chain image count
  u32 currentImageCount{};
  VK_VALIDATE(vkGetSwapchainImagesKHR(mVkLogicalDevice, mVkSwapChainKhr, &currentImageCount, nullptr));
  mVkSwapChainImages.clear();
  mVkSwapChainImages.resize(currentImageCount);
  VK_VALIDATE(vkGetSwapchainImagesKHR(mVkLogicalDevice, mVkSwapChainKhr, &currentImageCount, mVkSwapChainImages.data()));
  std::printf("Images current for swapchain %u\n", currentImageCount);
}

void VkRenderer::CreateVertexBuffer()
{
  std::vector<VertexLambert> vertices
  {
    VertexLambert{ { -0.5f, -0.5f, 0.f }, { 0.f, 0.f, 0.f }, { 0.f, 0.f }, { 1.f, 0.f, 0.f, 1.f } },
    VertexLambert{ { -0.5f,  0.5f, 0.f }, { 0.f, 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 1.f, 0.f, 1.f } },
    VertexLambert{ {  0.5f,  0.5f, 0.f }, { 0.f, 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f, 1.f, 1.f } },
  };
  std::vector<u32> indices{ 0, 1, 2 };
  // Command Buffer allocate info
  VkCommandBufferAllocateInfo vkCommandBufferAllocateInfo{};
  vkCommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  vkCommandBufferAllocateInfo.commandPool = mVkCommandPool;
  vkCommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  vkCommandBufferAllocateInfo.commandBufferCount = 1;
  // Copy command buffer
  VkCommandBuffer vkCommandBufferCopy{};
  VK_VALIDATE(vkAllocateCommandBuffers(mVkLogicalDevice, &vkCommandBufferAllocateInfo, &vkCommandBufferCopy));
  // Staging buffers
  VkDeviceMemory vkVertexDeviceMemory{};
  VkDeviceMemory vkIndexDeviceMemory{};
  VkBuffer vkVertexBuffer{};
  VkBuffer vkIndexBuffer{};
  // Transfer vertices to GPU
  {
    // Vertex buffer create info
    VkBufferCreateInfo vkVertexBufferCreateInfo{};
    vkVertexBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    vkVertexBufferCreateInfo.size = sizeof(VertexLambert) * vertices.size();
    vkVertexBufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    // Host buffer
    VK_VALIDATE(vkCreateBuffer(mVkLogicalDevice, &vkVertexBufferCreateInfo, nullptr, &vkVertexBuffer));
    // Gather buffer requirements
    VkMemoryRequirements vkMemoryRequirements{};
    vkGetBufferMemoryRequirements(mVkLogicalDevice, vkVertexBuffer, &vkMemoryRequirements);
    // Memory allocate info
    VkMemoryAllocateInfo vkMemoryAllocateInfo{};
    vkMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    vkMemoryAllocateInfo.allocationSize = vkMemoryRequirements.size;
    GetMemoryType(mVkPhysicalDeviceMemoryProperties, vkMemoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &vkMemoryAllocateInfo.memoryTypeIndex);
    // Device buffer
    vkAllocateMemory(mVkLogicalDevice, &vkMemoryAllocateInfo, nullptr, &vkVertexDeviceMemory);
    // Copy memory to mapped buffer
    void* pMemory{};
    VK_VALIDATE(vkMapMemory(mVkLogicalDevice, vkVertexDeviceMemory, 0, sizeof(VertexLambert) * vertices.size(), 0, &pMemory));
    std::memcpy(pMemory, vertices.data(), sizeof(VertexLambert) * vertices.size());
    vkUnmapMemory(mVkLogicalDevice, vkVertexDeviceMemory);
    vkBindBufferMemory(mVkLogicalDevice, vkVertexBuffer, vkVertexDeviceMemory, 0);
    // Allocate GPU only buffer
    vkVertexBufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    VK_VALIDATE(vkCreateBuffer(mVkLogicalDevice, &vkVertexBufferCreateInfo, nullptr, &mVkVertexBuffer));
    vkGetBufferMemoryRequirements(mVkLogicalDevice, mVkVertexBuffer, &vkMemoryRequirements);
    vkMemoryAllocateInfo.allocationSize = vkMemoryRequirements.size;
    GetMemoryType(mVkPhysicalDeviceMemoryProperties, vkMemoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vkMemoryAllocateInfo.memoryTypeIndex);
    VK_VALIDATE(vkAllocateMemory(mVkLogicalDevice, &vkMemoryAllocateInfo, nullptr, &mVkVertexBufferMemory));
    VK_VALIDATE(vkBindBufferMemory(mVkLogicalDevice, mVkVertexBuffer, mVkVertexBufferMemory, 0));
  }
  // Transfer indices to GPU
  {
    // Index buffer create info
    VkBufferCreateInfo vkIndexBufferCreateInfo{};
    vkIndexBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    vkIndexBufferCreateInfo.size = sizeof(u32) * indices.size();
    vkIndexBufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    // Host buffer
    VK_VALIDATE(vkCreateBuffer(mVkLogicalDevice, &vkIndexBufferCreateInfo, nullptr, &vkIndexBuffer));
    // Gather buffer requirements
    VkMemoryRequirements vkMemoryRequirements{};
    vkGetBufferMemoryRequirements(mVkLogicalDevice, vkIndexBuffer, &vkMemoryRequirements);
    // Memory allocate info
    VkMemoryAllocateInfo vkMemoryAllocateInfo{};
    vkMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    vkMemoryAllocateInfo.allocationSize = vkMemoryRequirements.size;
    GetMemoryType(mVkPhysicalDeviceMemoryProperties, vkMemoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &vkMemoryAllocateInfo.memoryTypeIndex);
    // Device buffer
    vkAllocateMemory(mVkLogicalDevice, &vkMemoryAllocateInfo, nullptr, &vkIndexDeviceMemory);
    // Copy memory to mapped buffer
    void* pMemory{};
    VK_VALIDATE(vkMapMemory(mVkLogicalDevice, vkIndexDeviceMemory, 0, sizeof(u32) * indices.size(), 0, &pMemory));
    std::memcpy(pMemory, indices.data(), sizeof(u32) * indices.size());
    vkUnmapMemory(mVkLogicalDevice, vkIndexDeviceMemory);
    vkBindBufferMemory(mVkLogicalDevice, vkIndexBuffer, vkIndexDeviceMemory, 0);
    // Allocate GPU only buffer
    vkIndexBufferCreateInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    VK_VALIDATE(vkCreateBuffer(mVkLogicalDevice, &vkIndexBufferCreateInfo, nullptr, &mVkIndexBuffer));
    vkGetBufferMemoryRequirements(mVkLogicalDevice, mVkIndexBuffer, &vkMemoryRequirements);
    vkMemoryAllocateInfo.allocationSize = vkMemoryRequirements.size;
    GetMemoryType(mVkPhysicalDeviceMemoryProperties, vkMemoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vkMemoryAllocateInfo.memoryTypeIndex);
    VK_VALIDATE(vkAllocateMemory(mVkLogicalDevice, &vkMemoryAllocateInfo, nullptr, &mVkIndexBufferMemory));
    VK_VALIDATE(vkBindBufferMemory(mVkLogicalDevice, mVkIndexBuffer, mVkIndexBufferMemory, 0));
  }
  // Command buffer begin info
  VkCommandBufferBeginInfo vkCommandBufferBeginInfo{};
  vkCommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  vkCommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  VK_VALIDATE(vkBeginCommandBuffer(vkCommandBufferCopy, &vkCommandBufferBeginInfo));
  // Issue commands
  VkBufferCopy vkBufferCopy{};
  vkBufferCopy.size = sizeof(VertexLambert) * vertices.size();
  vkCmdCopyBuffer(vkCommandBufferCopy, vkVertexBuffer, mVkVertexBuffer, 1, &vkBufferCopy);
  vkBufferCopy.size = sizeof(u32) * indices.size();
  vkCmdCopyBuffer(vkCommandBufferCopy, vkIndexBuffer, mVkIndexBuffer, 1, &vkBufferCopy);
  VK_VALIDATE(vkEndCommandBuffer(vkCommandBufferCopy));
  // Submit commands
  VkSubmitInfo vkSubmitInfo{};
  vkSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  vkSubmitInfo.commandBufferCount = 1;
  vkSubmitInfo.pCommandBuffers = &vkCommandBufferCopy;
  VK_VALIDATE(vkQueueSubmit(mVkGraphicsQueue, 1, &vkSubmitInfo, VK_NULL_HANDLE));
  VK_VALIDATE(vkQueueWaitIdle(mVkGraphicsQueue));
  vkFreeCommandBuffers(mVkLogicalDevice, mVkCommandPool, 1, &vkCommandBufferCopy);
  // Cleanup
  vkDestroyBuffer(mVkLogicalDevice, vkVertexBuffer, nullptr);
  vkDestroyBuffer(mVkLogicalDevice, vkIndexBuffer, nullptr);
  vkFreeMemory(mVkLogicalDevice, vkVertexDeviceMemory, nullptr);
  vkFreeMemory(mVkLogicalDevice, vkIndexDeviceMemory, nullptr);

  mVkVertexInputBindingDescription.binding = 0;
  mVkVertexInputBindingDescription.stride = sizeof(VertexLambert);
  mVkVertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  mVkVertexInputAttributeDescriptions[0].binding = 0;
  mVkVertexInputAttributeDescriptions[0].location = 0;
  mVkVertexInputAttributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
  mVkVertexInputAttributeDescriptions[0].offset = 0;

  mVkVertexInputAttributeDescriptions[1].binding = 0;
  mVkVertexInputAttributeDescriptions[1].location = 1;
  mVkVertexInputAttributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
  mVkVertexInputAttributeDescriptions[1].offset = sizeof(r32) * 3;

  mVkVertexInputAttributeDescriptions[2].binding = 0;
  mVkVertexInputAttributeDescriptions[2].location = 2;
  mVkVertexInputAttributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
  mVkVertexInputAttributeDescriptions[2].offset = (sizeof(r32) * 3) + (sizeof(r32) * 3);

  mVkVertexInputAttributeDescriptions[3].binding = 0;
  mVkVertexInputAttributeDescriptions[3].location = 3;
  mVkVertexInputAttributeDescriptions[3].format = VK_FORMAT_R32G32B32A32_SFLOAT;
  mVkVertexInputAttributeDescriptions[3].offset = (sizeof(r32) * 3) + (sizeof(r32) * 3) + (sizeof(r32) * 2);
}
void VkRenderer::CreateUniformBuffer()
{
  // Buffer create info.
  VkBufferCreateInfo vkBufferCreateInfo{};
  vkBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  vkBufferCreateInfo.size = sizeof(UniformMvp);
  vkBufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
  VK_VALIDATE(vkCreateBuffer(mVkLogicalDevice, &vkBufferCreateInfo, nullptr, &mVkMvpBuffer));
  VkMemoryRequirements vkMemoryRequirements{};
  vkGetBufferMemoryRequirements(mVkLogicalDevice, mVkMvpBuffer, &vkMemoryRequirements);
  VkMemoryAllocateInfo vkMemoryAllocateInfo{};
  vkMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  vkMemoryAllocateInfo.allocationSize = vkMemoryRequirements.size;
  GetMemoryType(mVkPhysicalDeviceMemoryProperties, vkMemoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &vkMemoryRequirements.memoryTypeBits);
  vkAllocateMemory(mVkLogicalDevice, &vkMemoryAllocateInfo, nullptr, &mVkMvpBufferMemory);
  vkBindBufferMemory(mVkLogicalDevice, mVkMvpBuffer, mVkMvpBufferMemory, 0);
  // Update uniform data
  
}

void VkRenderer::FindQueueFamilies()
{
  // Gather queue family counts
  u32 queueFamilyCount{};
  vkGetPhysicalDeviceQueueFamilyProperties(mVkPhysicalDevice, &queueFamilyCount, nullptr);
  if (queueFamilyCount <= 0)
  {
    std::printf("No queue family present\n");
    return;
  }
  // Gather queue families
  std::vector<VkQueueFamilyProperties> queueFamilies{ queueFamilyCount };
  vkGetPhysicalDeviceQueueFamilyProperties(mVkPhysicalDevice, &queueFamilyCount, queueFamilies.data());
  u32 presentSupport{};
  for (u32 i{}; i < queueFamilyCount; ++i)
  {
    VK_VALIDATE(vkGetPhysicalDeviceSurfaceSupportKHR(mVkPhysicalDevice, i, mVkWindowSurface, &presentSupport));
    if (queueFamilies[i].queueCount > 0 && queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
    {
      mGraphicsQueueFamily = i;
      if (presentSupport)
      {
        mPresentQueueFamily = i;
        break;
      }
    }
  }
  VK_LOG("Graphics queue family %d\n", mGraphicsQueueFamily.value_or(-1));
  VK_LOG("Present queue family %d\n", mPresentQueueFamily.value_or(-1));
}