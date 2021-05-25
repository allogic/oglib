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

import math;
import acs;

/*
* Components.
*/

export struct Transform : Component
{
  r32v3 mPosition;
  r32v3 mRotation;
  r32v3 mScale;

  Transform(r32v3 const& position, r32v3 const& rotation, r32v3 const& scale) : mPosition{ position }, mRotation{ rotation }, mScale{ mScale } {}
};
export struct Camera : Component
{
  
};
export struct Rigidbody : Component
{
  r32v3 mVelocity;
  r32   mGravity;

  Rigidbody(r32v3 const& velocity, r32 gravity) : mVelocity{ velocity }, mGravity{ gravity } {}
};