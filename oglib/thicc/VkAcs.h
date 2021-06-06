#ifndef VK_ACS
#define VK_ACS

/*
* Actor component system.
*
* Actor structure:
* ---A-----------------------------
*    |
*    Transform
*    |
*    Renderable
*
* Transaction structure:
* ---C0-----------C0C1-------------
*    |            |
*    [Actor, ...] [Actor, ...]
*/

#include "VkCore.h"

namespace acs
{
  /*
  * Global parameters.
  */
  
  constexpr u32 MAX_ACTORS{ 100000 };
  
  /*
  * Forward decls.
  */
  
  struct Actor;
  
  /*
  * Type deduction utilities.
  */
  
  template<typename T>
  struct Proxy
{
  using Ref  = T&;
  using CRef = T const&;
  using Ptr  = T*;
  using CPtr = T const*;
};
  
  template<typename T>
  concept Actorable = std::is_base_of_v<Actor, T>;
  
  /*
  * Primitives.
  */
  
  using Actors       = std::map<std::string, Actor*>;
  using Components   = std::map<u64, void*>;
  using Transactions = std::map<u64, std::multiset<Actor*>>;
  
  /*
  * Interfaces for actors and components.
  */
  
  struct Actor
  {
    u64         mComponentMask{};
    Components* mpComponents  {};
  };
  
  /*
  * Global state.
  */
  
  Actors       sActors      {};
  Transactions sTransactions{};
  
  /*
  * Actor specific routines.
  */
  
  template<Actorable A, typename ... Args>
  __forceinline A* Create(std::string const& name, Args&& ... args) noexcept
  {
    auto& pActor{ sActors[name] };
    if (!pActor)
    {
      pActor = new A{ std::forward<Args>(args) ... };
    }
    return (A*)pActor;
  }
  template<Actorable A>
  __forceinline A* Find(std::string const& name) noexcept
  {
    return {};
  }
  template<Actorable A>
  __forceinline A* Destroy(std::string const& name) noexcept
  {
    return {};
  }
  
  /*
  * Component specific routines.
  */
  
  template<typename C, typename ... Args>
  __forceinline C* Attach(Actor* pActor, Args&& ... args) noexcept
  {
    if (!pActor->mpComponents)
    {
      pActor->mpComponents = new Components;
    }
    auto& pComponent{ (*pActor->mpComponents)[typeid(C).hash_code()] };
    if (!pComponent)
    {
      pComponent = new C{ std::forward<Args>(args) ... };
      pActor->mComponentMask |= ~typeid(C).hash_code();
      sTransactions[typeid(C).hash_code()].emplace(pActor);
      sTransactions[pActor->mComponentMask].emplace(pActor);
    }
    return (C*)pComponent;
  }
  template<typename C>
  __forceinline C* Detach() noexcept
  {
    return {};
  }
  
  /*
  * Dispatch specific routines.
  */
  
  template<typename ... Cs>
  __forceinline void Dispatch(std::function<void(typename Proxy<Cs>::Ptr ...)>&& predicate) noexcept
  {
    u64 const componentHash{ ((u64)0u | ... | ~typeid(Cs).hash_code()) };
    for (auto const& pActor : sTransactions[componentHash])
    {
      predicate(((typename Proxy<Cs>::Ptr)(*pActor->mpComponents)[typeid(Cs).hash_code()]) ...);
    }
  }
}

#endif