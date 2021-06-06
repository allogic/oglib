#ifndef VK_RENDERER
#define VK_RENDERER

#include "VkCore.h"
#include "VkVertices.h"
#include "VkUniforms.h"

constexpr s8 const* VK_DEBUG_LAYER { "VK_LAYER_KHRONOS_validation" };

class VkRenderer
{
public:
  VkRenderer(u32 width, u32 height, GLFWwindow* pGlfwWindow, u32 debug = 0);
  virtual ~VkRenderer();

private:
  static u32                   DebugCallback(VkDebugReportFlagsEXT vkFlags, VkDebugReportObjectTypeEXT vkObjType, u64 srcObject, u32 location, u32 msgCode, s8 const* pLayerPrefix, s8 const* pMessage, void* pUserData);
  static std::set<std::string> GetSupportedLayers();
  static std::set<std::string> GetRequiredExtensions(u32 debugEnabled);
  static std::set<std::string> GetSupportedExtensions(VkPhysicalDevice vkPhysicalDevice);
  static u32                   GetMemoryType(VkPhysicalDeviceMemoryProperties vkProperties, u32 typeBits, u32 properties, u32* typeIndex);
  static VkSurfaceFormatKHR    GetSurfaceFormat(std::vector<VkSurfaceFormatKHR> const& vkFormats);
  static VkExtent2D            GetSwapExtent(u32 width, u32 height, VkSurfaceCapabilitiesKHR const& vkSurfaceCapabilities);
  static VkPresentModeKHR      GetPresentMode(std::vector<VkPresentModeKHR> const& vkPresentModes);

  void CreateInstance();
  void CreateDebugCallback();
  void CreateWindowSurface();
  void CreatePhysicalDevice();
  void CreateLogicalDevice();
  void CreateCommandPool();
  void CreateSwapChain();

  void CreateVertexBuffer();
  void CreateUniformBuffer();

  void FindQueueFamilies();

  u32                               mDebug                                {};
  u32                               mWidth                                {};
  u32                               mHeight                               {};
  GLFWwindow*                       mpGlfwWindow                          {};

  VkDebugReportCallbackEXT          mVkDebugCallback                      {};
  VkInstance                        mVkInstance                           {};
  VkSurfaceKHR                      mVkWindowSurface                      {};
  VkPhysicalDevice                  mVkPhysicalDevice                     {};
  VkPhysicalDeviceMemoryProperties  mVkPhysicalDeviceMemoryProperties     {};
  VkDevice                          mVkLogicalDevice                      {};
  VkCommandPool                     mVkCommandPool                        {};
  VkQueue                           mVkGraphicsQueue                      {};
  VkQueue                           mVkPresentQueue                       {};

  VkSwapchainKHR                    mVkSwapChainKhr                       {};
  VkSwapchainKHR                    mVkSwapChainKhrOld                    {};
  VkExtent2D                        mVkSwapChainExtend                    {};
  VkFormat                          mVkSwapChainFormat                    {};
  std::vector<VkImage>              mVkSwapChainImages                    {};

  VkDeviceMemory                    mVkVertexBufferMemory                 {};
  VkDeviceMemory                    mVkIndexBufferMemory                  {};
  VkDeviceMemory                    mVkMvpBufferMemory                    {};
  VkBuffer                          mVkVertexBuffer                       {};
  VkBuffer                          mVkIndexBuffer                        {};
  VkBuffer                          mVkMvpBuffer                          {};
  VkVertexInputBindingDescription   mVkVertexInputBindingDescription      {};
  VkVertexInputAttributeDescription mVkVertexInputAttributeDescriptions[4]{};

  // Remove std::optional<>
  std::optional<s32>                mGraphicsQueueFamily                  {};
  std::optional<s32>                mPresentQueueFamily                   {};
};

#endif