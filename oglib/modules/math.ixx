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

enum : u8
{
  x = 0,
  y,
  z,
  t,
};
enum : u8
{
  w = 0,
  h,
  d,
};

/*
* Type deduction utilities.
*/

template<typename T>
concept Primitivable = std::is_integral_v<T> || std::is_floating_point_v<T>;

/*
* Abstract vector types.
*/

#pragma pack(push, 1)
template<typename T, u8 S>
struct tvn
{
  T mDims[S] = {};

  template<Primitivable ... Dims>
  __forceinline tvn(Dims&& ... dims)
  {
    u8 i = 0;
    ((mDims[i++] = (T)dims), ...);
  }
  template<u8 SS, Primitivable ... Dims>
  __forceinline tvn(tvn<T, SS> const& v, Dims&& ... dims)
  {
    switch (S)
    {
      case 2:
      {
        switch (SS)
        {
          case 2: { mDims[0] = v[0]; mDims[1] = v[1]; break; }
        }
        break;
      }
      case 3:
      {
        switch (SS)
        {
          case 2: { mDims[0] = v[0]; mDims[1] = v[1]; break; }
          case 3: { mDims[0] = v[0]; mDims[1] = v[1]; mDims[2] = v[2]; break; }
        }
        break;
      }
      case 4:
      {
        switch (SS)
        {
          case 2: { mDims[0] = v[0]; mDims[1] = v[1]; break; }
          case 3: { mDims[0] = v[0]; mDims[1] = v[1]; mDims[2] = v[2]; break; }
          case 4: { mDims[0] = v[0]; mDims[1] = v[1]; mDims[2] = v[2]; mDims[3] = v[3]; break; }
        }
        break;
      }
    }
    u8 i = SS;
    ((mDims[i++] = (T)dims), ...);
  }

  __forceinline T&       operator [] (u8 idx)       { return mDims[idx]; }
  __forceinline T const& operator [] (u8 idx) const { return mDims[idx]; }
};
#pragma pack(pop)

export using r32v2 = tvn<r32, 2>;
export using r32v3 = tvn<r32, 3>;
export using r32v4 = tvn<r32, 4>;

/*
* Vector routines.
*/

export template<typename T, u8 S>
__forceinline void Print(tvn<T, S> const& m) noexcept
{
  switch (S)
  {
    case 2: std::printf("[%.3f, %.3f]\n", m[x], m[y]); break;
    case 3: std::printf("[%.3f, %.3f, %.3f]\n", m[x], m[y], m[z]); break;
    case 4: std::printf("[%.3f, %.3f, %.3f, %.3f]\n", m[x], m[y], m[z], m[t]); break;
  }
}

/*
* Abstract matrix types.
*/

#pragma pack(push, 1)
template<typename T, u8 S>
struct tmn
{
  T mDims[S][S] = {};

  __forceinline T       (& operator [](u8 idx))[S]       { return (mDims[idx]); }
  __forceinline T const (& operator [](u8 idx) const)[S] { return (mDims[idx]); }
};
#pragma pack(pop)

export using r32m2 = tmn<r32, 2>;
export using r32m3 = tmn<r32, 3>;
export using r32m4 = tmn<r32, 4>;

/*
* Matrix routines.
*/

export template<typename T, u8 S>
__forceinline tmn<T, S>    Identity() noexcept
{
  return {};
}
export template<typename T, u8 S>
__forceinline void         Print(tmn<T, S> const& m) noexcept
{
  switch (S)
  {
    case 2:
    {
      std::printf("[%.3f, %.3f,\n%.3f, %.3f]\n",
        m[0][0], m[1][0],
        m[0][1], m[1][1]
      );
      break;
    }
    case 3:
    {
      std::printf("[%.3f, %.3f, %.3f,\n%.3f, %.3f, %.3f,\n%.3f, %.3f, %.3f]\n",
        m[0][0], m[1][0], m[2][0],
        m[0][1], m[1][1], m[2][1],
        m[0][2], m[1][2], m[2][2]
      );
      break;
    };
    case 4:
    {
      std::printf("[%.3f, %.3f, %.3f, %.3f,\n%.3f, %.3f, %.3f, %.3f,\n%.3f, %.3f, %.3f, %.3f,\n%.3f, %.3f, %.3f, %.3f]\n",
        m[0][0], m[1][0], m[2][0], m[3][0],
        m[0][1], m[1][1], m[2][1], m[3][1],
        m[0][2], m[1][2], m[2][2], m[3][2],
        m[0][3], m[1][3], m[2][3], m[3][3]
      );
      break;
    }
  }
}
export __forceinline r32m4 Perspective(r32 fov, r32 aspect, r32 near, r32 far) noexcept
{
  return {};
}
export __forceinline r32m4 View() noexcept
{
  return {};
}

/*
* Linear math routines.
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