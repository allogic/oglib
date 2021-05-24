/*
* Math utility.
*/

module;

/*
* Includes.
*/

#include <utility>

/*
* Begin module math.
*/

export module math;

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

/*
* Dimensioal access token.
*/

enum : u32
{
  x = 0,
  y,
  z,
};
enum : u32
{
  w = 0,
  h,
  d,
};

/*
* Abstract vector types.
*/

template<typename T, u32 Size>
struct tvn
{
  T Dims[Size]{};

  __forceinline T&       operator [] (u32 idx) { return Dims[idx]; }
  __forceinline T const& operator [] (u32 idx) const { return Dims[idx]; }

  //template<typename T, 2>
  //__forceinline tvn operator + (tvn const& lhs, tvn const& rhs);
};

export using r32v2 = tvn<r32, 2>;
export using r32v3 = tvn<r32, 3>;
export using r32v4 = tvn<r32, 4>;

/*
* Abstract matrix types.
*/

template<typename T, u32 Size>
struct tmn
{
  T Dims[Size][Size]{};
};

export using r32m3 = tmn<r32, 3>;

/*
* Vector math methods.
*/

export __forceinline u32   AABB(r32v2 const& p0, r32v2 const& s0, r32v2 const& p1, r32v2 const& s1) noexcept
{
  if ((p0[x] + s0[w]) < p1[x] || p0[x] > (p1[x] + s1[w])) return 0;
  if ((p0[y] + s0[h]) < p1[y] || p0[y] > (p1[y] + s1[h])) return 0;
  return 1;
}
export __forceinline r32v2 RotateZ(r32v2 const& p, r32 theta) noexcept
{
  r32v2 r;
  r[x] = (p[x] * cosf(theta)) - (p[y] * sinf(theta));
  r[y] = (p[x] * sinf(theta)) + (p[y] * cosf(theta));
  return r;
}