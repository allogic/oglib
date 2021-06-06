#ifndef VK_COMPONENTS
#define VK_COMPONENTS

/*
* Default components.
*/

#include "VkCore.h"

namespace acs
{
  struct Transform
  {
    r32v3 mPosition;
    r32v3 mRotationEuler;
    r32v3 mScale;

    Transform(r32v3 const& position, r32v3 const& rotationEuler, r32v3 const& scale)
      : mPosition{ position }
      , mRotationEuler{ rotationEuler }
      , mScale{ scale } {}
  };
  struct Camera
  {
    r32 mFov;
    r32 mNear;
    r32 mFar;

    Camera(r32 fov, r32 near, r32 far) : mFov{ fov }, mNear{ near }, mFar{ far } {}
  };
  struct Renderable
  {
    void* mpMeshLayout;
    void* mpShaderLayout;

    Renderable(void* meshLayout, void* shaderLayout) : mpMeshLayout{ meshLayout }, mpShaderLayout{ shaderLayout } {}
  };
  struct Rigidbody
  {
    r32v3 mVelocity;
    r32   mGravity;

    Rigidbody(r32v3 const& velocity, r32 gravity) : mVelocity{ velocity }, mGravity{ gravity } {}
  };
}

#endif