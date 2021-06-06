#ifndef VK_UTILS
#define VK_UTILS

#include "VkCore.h"

/*
* Some utilities.
*/

namespace
{
  std::vector<s8 const*> TransformView(std::set<std::string> const& set)
  {
    std::vector<s8 const*> result{};
    for (auto const& value : set)
    {
      result.emplace_back(value.c_str());
    }
    return result;
  }
}

#endif