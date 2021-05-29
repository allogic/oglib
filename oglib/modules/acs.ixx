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

module;

/*
* Includes.
*/

#include <iostream>
#include <bitset>
#include <string>
#include <array>
#include <map>
#include <set>
#include <unordered_map>
#include <type_traits>
#include <functional>

/*
* Begin module ACS.
*/

export module acs;

/*
* Imports.
*/

import type;

/*
* Global parameters.
*/

constexpr u32 MAX_ACTORS = 100000;

/*
* Forward decls.
*/

export struct Actor;
export struct Component;

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
template<typename T>
concept Componentable = std::is_base_of_v<Component, T>;

/*
* Primitives.
*/

using Actors       = std::map<std::string, Actor*>;
using Components   = std::map<u64, Component*>;
using Transactions = std::map<u64, std::multiset<Actor*>>;

/*
* Interfaces for actors and components.
*/

export struct Actor
{
  u64         mComponentMask  = 0;
  Components* mpComponents    = nullptr;
};
export struct Component
{

};

/*
* Global state registry.
*/

Actors       sActors       = {};
Transactions sTransactions = {};

/*
* Actor specific routines.
*/

export template<Actorable A, typename ... Args>
__forceinline A* Create(std::string const& name, Args&& ... args) noexcept
{
  auto& pActor = sActors[name];
  if (!pActor)
  {
    pActor = new A{ std::forward<Args>(args) ... };
  }
  return (A*)pActor;
}
export template<Actorable A>
__forceinline A* Find(std::string const& name) noexcept
{
  return {};
}
export template<Actorable A>
__forceinline A* Destroy(std::string const& name) noexcept
{
  return {};
}

/*
* Component specific routines.
*/

export template<Componentable C, typename ... Args>
__forceinline C* Attach(Actor* pActor, Args&& ... args) noexcept
{
  if (!pActor->mpComponents)
  {
    pActor->mpComponents = new Components;
  }
  auto& pComponent = (*pActor->mpComponents)[typeid(C).hash_code()];
  if (!pComponent)
  {
    pComponent = new C{ std::forward<Args>(args) ... };
    pActor->mComponentMask |= ~typeid(C).hash_code();
    sTransactions[typeid(C).hash_code()].emplace(pActor);
    sTransactions[pActor->mComponentMask].emplace(pActor);
  }
  return (C*)pComponent;
}
export template<Componentable C>
__forceinline C* Detach() noexcept
{
  return {};
}

/*
* Dispatch specific routines.
*/

export template<Componentable ... Cs>
__forceinline void Dispatch(std::function<void(typename Proxy<Cs>::Ptr ...)>&& predicate) noexcept
{
  u64 const componentHash = ((u64)0u | ... | ~typeid(Cs).hash_code());
  for (auto const& pActor : sTransactions[componentHash])
  {
    predicate(((typename Proxy<Cs>::Ptr)(*pActor->mpComponents)[typeid(Cs).hash_code()]) ...);
  }
}