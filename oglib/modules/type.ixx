/*
* Global type.
*/

module;

/*
* Includes.
*/

#include <glm/glm.hpp>

/*
* Begin module type.
*/

export module type;

/*
* Primitives.
*/

export using s8  = char;
export using s16 = short;
export using s32 = int;
export using s64 = int long long;

export using u8  = char unsigned;
export using u16 = short unsigned;
export using u32 = int unsigned;
export using u64 = int long long unsigned;

export using r32 = float;
export using r64 = double;

export using r32v2 = glm::fvec2;
export using r32v3 = glm::fvec3;
export using r32v4 = glm::fvec4;

export using r32m3 = glm::fmat3;
export using r32m4 = glm::fmat4;