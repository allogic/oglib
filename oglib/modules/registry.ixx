/*
* Instance registry.
*/

module;

/*
* Includes.
*/

#include <string>
#include <map>

/*
* Begin module registry.
*/

export module registry;

/*
* Global state.
*/

std::map<std::string, void*> sRegistry{};

/*
* Public routines.
*/

export template<typename T, typename ... Args>
T* At(std::string const& name, Args&& ... args)
{
  auto& pInstance{ sRegistry[name] };
  if (!pInstance)
  {
    pInstance = new T{ std::forward<Args>(args) ... };
  }
  return (T*)pInstance;
}