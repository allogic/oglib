#ifndef VK_UNIFORMS
#define VK_UNIFORMS

#include "VkCore.h"

// TODO: Create cross compilable versions

#pragma pack(push, 1)
struct UniformMvp
{
  r32 mProjection[16];
  r32 mView[16];
  r32 mModel[16];
};
#pragma pack(pop)

#endif