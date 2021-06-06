#ifndef VK_VERTICES
#define VK_VERTICES

#include "VkCore.h"

// TODO: Create cross compilable versions

#pragma pack(push, 1)
struct VertexLambert
{
  r32 mPosition[3];
  r32 mNormal[3];
  r32 mUv[2];
  r32 mColor[4];
};
#pragma pack(pop)

#endif