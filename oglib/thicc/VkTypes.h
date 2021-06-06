#ifndef VK_TYPES
#define VK_TYPES

#include <glm/glm.hpp>

using s8  = char;
using s16 = short;
using s32 = int;
using s64 = int long long;

using u8  = char unsigned;
using u16 = short unsigned;
using u32 = int unsigned;
using u64 = int long long unsigned;

using r32 = float;
using r64 = double;

using r32v2 = glm::fvec2;
using r32v3 = glm::fvec3;
using r32v4 = glm::fvec4;

using r32m3 = glm::fmat3;
using r32m4 = glm::fmat4;

#endif