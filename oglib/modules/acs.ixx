/*
* Actor component system.
*/

module;

/*
* Includes.
*/

#include <string>
#include <array>
#include <map>
#include <type_traits>
#include <functional>

/*
* Begin module ACS.
*/

export module acs;

/*
* Imports.
*/

import math;

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
struct Identity
{
  using Type = T;
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
* GPU acceleration abstraction types.
*/

#pragma pack(push, 1)
struct Transform
{
  r32v4 mPosition;
  r32v4 mRotation;
  r32v4 mScale;
};
#pragma pack(pop)

/*
* Primitives.
*/

using Components = std::map<u64, Component*>;

/*
* Interfaces for actors and components.
*/

export struct Actor
{
  u32         mTransformIndex = 0;
  Components* mpComponents    = nullptr;
};
export struct Component
{

};

/*
* Global state registry.
*/

std::array<Transform, MAX_ACTORS> sTransforms  = {};
std::map<std::string, Actor*>     sNameToActor = {};

/*
* Actor specific methods.
*/

export template<Actorable A, typename ... Args>
__forceinline A* Create(std::string const& name, Args&& ... args) noexcept
{
  auto& pActor = sNameToActor[name];
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
* Component specific methods.
*/

export template<Componentable C, typename ... Args>
__forceinline C* Attach(Actor* pActor, Args&& ... args) noexcept
{
  if (!pActor->mpComponents)
  {
    pActor->mpComponents = new Components;
  }
  Component* pComponent = &(*pActor->mpComponents)[typeid(C).hash_code()];
  if (!pComponent)
  {
    pComponent = new C{ std::forward<Args>(args) ... };
  }
  return (C*)pComponent;
}
export template<Componentable C>
__forceinline C* Detach() noexcept
{
  return {};
}

/*
* Dispatch specific methods.
*/

export template<Componentable ... Cs>
__forceinline void Dispatch(std::function<void(typename Identity<Cs>::Ptr ...)>&& predicate) noexcept
{
  for (auto const& [name, pActor] : sNameToActor)
  {
    predicate(((typename Identity<Cs>::Ptr)(*pActor->mpComponents)[typeid(typename Identity<Cs>::Type).hash_code()]) ...);
  }
}