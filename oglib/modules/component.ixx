/*
* Default ACS components.
*/

module;

/*
* Begin module component.
*/

export module component;

/*
* Imports.
*/

import type;
import acs;

/*
* Components.
*/

export struct Transform : Component
{
  r32v3 mPosition;
  r32v3 mRotationEuler;
  r32v3 mScale;

  Transform(r32v3 const& position, r32v3 const& rotationEuler, r32v3 const& scale)
    : mPosition{ position }
    , mRotationEuler{ rotationEuler }
    , mScale{ scale } {}
}; 
export struct Camera : Component
{
  r32 mFov;
  r32 mNear;
  r32 mFar;

  Camera(r32 fov, r32 near, r32 far) : mFov{ fov }, mNear{ near }, mFar{ far } {}
};
export struct Renderable : Component
{
  void* mpMeshLayout;
  void* mpShaderLayout;

  Renderable(void* meshLayout, void* shaderLayout) : mpMeshLayout{ meshLayout }, mpShaderLayout{ shaderLayout } {}
};
export struct Rigidbody : Component
{
  r32v3 mVelocity;
  r32   mGravity;

  Rigidbody(r32v3 const& velocity, r32 gravity) : mVelocity{ velocity }, mGravity{ gravity } {}
};