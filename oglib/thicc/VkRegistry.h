#ifndef VK_REGISTRY
#define VK_REGISTRY

#include "VkCore.h"

/*
* Instance registry.
*/

namespace
{
  /*
  * Global state.
  */

  std::map<std::string, void*> sRegistry{};

  /*
  * Public routines.
  */

  template<typename T, typename ... Args>
  __forceinline T* At(std::string const& name, Args&& ... args) noexcept
  {
    auto& pInstance{ sRegistry[name] };
    if (!pInstance)
    {
      pInstance = new T{ std::forward<Args>(args) ... };
    }
    return (T*)pInstance;
  }
}

#endif