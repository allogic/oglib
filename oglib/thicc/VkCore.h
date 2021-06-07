#ifndef VK_CORE
#define VK_CORE

#include <cstdio>
#include <string>
#include <string_view>
#include <fstream>
#include <sstream>
#include <vector>
#include <array>
#include <map>
#include <set>
#include <iterator>
#include <cmath>
#include <optional>
#include <type_traits>
#include <functional>

#include "VkTypes.h"
#include "VkRegistry.h"

//#define VULKAN_HPP_NO_EXCEPTIONS
//#define VULKAN_HPP_TYPESAFE_CONVERSION
#define VK_USE_PLATFORM_WIN32_KHR
//#include <vulkan/vulkan.hpp>

#define GLFW_INCLUDE_VULKAN
#include <glfw3.h>
#undef GLFW_INCLUDE_VULKAN

#define _STR(VALUE) #VALUE
#define STR(VALUE) _STR(VALUE)

#undef min
#undef max

#define VK_VALIDATE(EXPRESSION)                            \
if (VkResult result{ EXPRESSION }; result != VK_SUCCESS)   \
{                                                          \
  std::printf(STR(EXPRESSION) ":%d:%X\n", result, result); \
  std::exit(1);                                            \
}

#define VK_LOG(FORMAT, ...)       \
std::printf(FORMAT, __VA_ARGS__);

#define VK_MEASURE_BEGIN(NAME)                                 \
auto NAME##Start{ std::chrono::high_resolution_clock::now() };

#define VK_MEASURE_END(NAME)                                                                                   \
auto NAME##End{ std::chrono::high_resolution_clock::now() };                                                   \
auto NAME##Duration{ std::chrono::duration_cast<std::chrono::milliseconds>(NAME##End - NAME##Start).count() }; \
std::printf(STR(NAME) ": %lldms\n", NAME##Duration);

#endif