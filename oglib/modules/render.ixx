/*
* OpenGL renderer.
*/

module;

/*
* Includes.
*/

#include <string>
#include <cstdio>
#include <vector>
#include <fstream>

#include "glad.h"

/*
* Begin module OpenGL renderer.
*/

export module render;

/*
* Imports.
*/

import math;
import acs;
import component;

/*
* Vertex layouts.
*/

enum VertexLayoutType : u32
{
  VERTEX_LAYOUT_GIZMO = 0,
  VERTEX_LAYOUT_LAMBERT,
  VERTEX_LAYOUT_SCREEN,
};

export struct VertexGizmo
{
  constexpr static VertexLayoutType Type = VERTEX_LAYOUT_GIZMO;

  r32v3 mPosition;
  r32v4 mColor;
};
export struct VertexLambert
{
  constexpr static VertexLayoutType Type = VERTEX_LAYOUT_LAMBERT;

  r32v3 mPosition;
  r32v3 mNormal;
  r32v3 mUv;
  r32v4 mColor;
};
export struct VertexScreen
{
  constexpr static VertexLayoutType Type = VERTEX_LAYOUT_SCREEN;

  r32v3 mPosition;
  r32v4 mUv;
};

/*
* Mesh layouts.
*/

template<typename V, typename I>
struct MeshLayout
{
  u32              mNumSubMeshes;
  std::vector<u32> mVaos;
  std::vector<u32> mVbos;
  std::vector<u32> mEbos;
  std::vector<u32> mVertexBufferSizes;
  std::vector<u32> mIndexBufferSizes;
};

export using MeshGizmo   = MeshLayout<VertexGizmo, u32>;
export using MeshLambert = MeshLayout<VertexLambert, u32>;
export using MeshScreen  = MeshLayout<VertexScreen, u32>;

/*
* Mesh creational specific routines.
*/

export template<typename V, typename I>
void        MeshLayoutCreate(MeshLayout<V, I>& meshLayout, u32 numSubMeshes, u32* pVertexBufferSizes, u32* pIndexBufferSizes)
{
  meshLayout.mNumSubMeshes = numSubMeshes;
  meshLayout.mVaos.resize(numSubMeshes);
  meshLayout.mVbos.resize(numSubMeshes);
  meshLayout.mEbos.resize(numSubMeshes);
  meshLayout.mVertexBufferSizes = { pVertexBufferSizes, pVertexBufferSizes + numSubMeshes };
  meshLayout.mIndexBufferSizes = { pIndexBufferSizes, pIndexBufferSizes + numSubMeshes };
  glGenVertexArrays(numSubMeshes, meshLayout.mVaos.data());
  glGenBuffers(numSubMeshes, meshLayout.mVbos.data());
  glGenBuffers(numSubMeshes, meshLayout.mEbos.data());
  for (u32 i = 0; i < numSubMeshes; i++)
  {
    glBindVertexArray(meshLayout.mVaos[i]);
    glBindBuffer(GL_ARRAY_BUFFER, meshLayout.mVbos[i]);
    glBufferStorage(GL_ARRAY_BUFFER, meshLayout.mVertexBufferSizes[i] * sizeof(V), nullptr, GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
    switch (V::Type)
    {
      case VERTEX_LAYOUT_GIZMO:
      {
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(V), (void*)(0));
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(V), (void*)(sizeof(r32v3)));
        break;
      }
      case VERTEX_LAYOUT_LAMBERT:
      {
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(V), (void*)(0));
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(V), (void*)(sizeof(r32v3)));
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(V), (void*)(sizeof(r32v3) + sizeof(r32v3)));
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(V), (void*)(sizeof(r32v3) + sizeof(r32v3) + sizeof(r32v2)));
        break;
      }
      case VERTEX_LAYOUT_SCREEN:
      {
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(V), (void*)(0));
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(V), (void*)(sizeof(r32v3)));
        break;
      }
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshLayout.mEbos[i]);
    glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, meshLayout.mIndexBufferSizes[i] * sizeof(I), nullptr, GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
  }
  glBindVertexArray(0);
}
export template<typename V, typename I>
void        MeshLayoutBind(MeshLayout<V, I>& meshLayout, u32 subMeshIndex)
{
  glBindVertexArray(meshLayout.mVaos[subMeshIndex]);
}
export void MeshLayoutUnBind()
{
  glBindVertexArray(0);
}
export template<typename V, typename I>
void        MeshLayoutData(MeshLayout<V, I> const& meshLayout, u32 subMeshIndex, void* pVertexData, void* pIndexData, u32 vertexBufferOffset, u32 indexBufferOffset, u32 vertexBufferSizeSub, u32 indexBufferSizeSub)
{
  glBindBuffer(GL_ARRAY_BUFFER, meshLayout.mVbos[subMeshIndex]);
  glBufferSubData(GL_ARRAY_BUFFER, vertexBufferOffset * sizeof(V), vertexBufferSizeSub * sizeof(V), pVertexData);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshLayout.mEbos[subMeshIndex]);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, indexBufferOffset * sizeof(I), indexBufferSizeSub * sizeof(I), pIndexData);
}
export template<typename V, typename I>
void        MeshLayoutClear(MeshLayout<V, I> const& meshLayout, u32 subMeshIndex)
{
  glBindBuffer(GL_ARRAY_BUFFER, meshLayout.mVbos[subMeshIndex]);
  glBufferSubData(GL_ARRAY_BUFFER, 0, meshLayout.mVertexBufferSizes[subMeshIndex] * sizeof(V), nullptr);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshLayout.mEbos[subMeshIndex]);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, meshLayout.mIndexBufferSizes[subMeshIndex] * sizeof(I), nullptr);
}

/*
* Mesh rendering specific routines.
*/

enum RenderModeType : u32
{
  RENDER_MODE_LINE = 0,
  RENDER_MODE_TRIANGLE,
};

export template<typename V, typename I>
void MeshLayoutDrawLines(MeshLayout<V, I> const& meshLayout, u32 subMeshIndex)
{
  switch (sizeof(I))
  {
    case sizeof(u32):
    {
      glDrawElements(GL_LINES, (s32)meshLayout.mIndexBufferSizes[subMeshIndex], GL_UNSIGNED_INT, nullptr);
      break;
    }
  }
}
export template<typename V, typename I>
void MeshLayoutDrawLinesInstanced(MeshLayout<V, I> const& meshLayout, u32 subMeshIndex, u32 numInstances)
{
  switch (sizeof(I))
  {
    case sizeof(u32):
    {
      glDrawElementsInstanced(GL_LINES, (s32)meshLayout.mIndexBufferSizes[subMeshIndex], GL_UNSIGNED_INT, nullptr, numInstances);
      break;
    }
  }
}
export template<typename V, typename I>
void MeshLayoutDrawTriangles(MeshLayout<V, I> const& meshLayout, u32 subMeshIndex)
{
  switch (sizeof(I))
  {
    case sizeof(u32):
    {
      glDrawElements(GL_TRIANGLES, (s32)meshLayout.mIndexBufferSizes[subMeshIndex], GL_UNSIGNED_INT, nullptr);
      break;
    }
  }
}
export template<typename V, typename I>
void MeshLayoutDrawTrianglesInstanced(MeshLayout<V, I> const& meshLayout, u32 subMeshIndex, u32 numInstances)
{
  switch (sizeof(I))
  {
    case sizeof(u32):
    {
      glDrawElementsInstanced(GL_TRIANGLES, (s32)meshLayout.mIndexBufferSizes[subMeshIndex], GL_UNSIGNED_INT, nullptr, numInstances);
      break;
    }
  }
}

export template<typename V, typename I>
void MeshLayoutRender(MeshLayout<V, I> const& meshLayout, RenderModeType renderMode)
{
  for (u32 i = 0; i < meshLayout.mNumSubMeshes; i++)
  {
    MeshLayoutBind(meshLayout, i);
    switch (renderMode)
    {
      case RENDER_MODE_LINE: MeshLayoutDrawLines(meshLayout, i); break;
      case RENDER_MODE_TRIANGLE: MeshLayoutDrawTriangles(meshLayout, i); break;
    }
  }
}
export template<typename V, typename I>
void MeshLayoutRenderPartial(MeshLayout<V, I> const& meshLayout, u32 subMeshIndex, RenderModeType renderMode)
{
  switch (renderMode)
  {
    case RENDER_MODE_LINE: MeshLayoutDrawLines(meshLayout, subMeshIndex); break;
    case RENDER_MODE_TRIANGLE: MeshLayoutDrawTriangles(meshLayout, subMeshIndex); break;
  }
}
export template<typename V, typename I>
void MeshLayoutRenderInstanced(MeshLayout<V, I> const& meshLayout, RenderModeType renderMode, u32 numInstances)
{
  for (u32 i = 0; i < meshLayout.mNumSubMeshes; i++)
  {
    MeshLayoutBind(meshLayout, i);
    switch (renderMode)
    {
      case RENDER_MODE_LINE: MeshLayoutDrawLinesInstanced(meshLayout, i, numInstances); break;
      case RENDER_MODE_TRIANGLE: MeshLayoutDrawTrianglesInstanced(meshLayout, i, numInstances); break;
    }
  }
}
export template<typename V, typename I>
void MeshLayoutRenderInstancedPartial(MeshLayout<V, I> const& meshLayout, u32 subMeshIndex, RenderModeType renderMode, u32 numInstances)
{
  switch (renderMode)
  {
    case RENDER_MODE_LINE: MeshLayoutDrawLinesInstanced(meshLayout, subMeshIndex, numInstances); break;
    case RENDER_MODE_TRIANGLE: MeshLayoutDrawTrianglesInstanced(meshLayout, subMeshIndex, numInstances); break;
  }
}

/*
* Shader layouts.
*/

struct ShaderPaths
{
  std::string mVertexShader;
  std::string mFragmentShader;
  std::string mComputeShader;
};

enum ShaderLayoutType : u32
{
  SHADER_LAYOUT_GIZMO = 0,
  SHADER_LAYOUT_LAMBERT,
  SHADER_LAYOUT_LAMBERT_INSTANCED,
  SHADER_LAYOUT_SCREEN,
  SHADER_LAYOUT_COMPUTE,
};

template<ShaderLayoutType L>
struct ShaderLayout
{
  u32 mProgId;
  u32 mVertId;
  u32 mFragId;
  u32 mCompId;
};

export using ShaderGizmo            = ShaderLayout<SHADER_LAYOUT_GIZMO>;
export using ShaderLambert          = ShaderLayout<SHADER_LAYOUT_LAMBERT>;
export using ShaderLambertInstanced = ShaderLayout<SHADER_LAYOUT_LAMBERT_INSTANCED>;
export using ShaderScreen           = ShaderLayout<SHADER_LAYOUT_SCREEN>;
export using ShaderCompute          = ShaderLayout<SHADER_LAYOUT_COMPUTE>;

/*
* Shader specific routines.
*/

enum ShaderResultType : u32
{
  SHADER_RESULT_ERROR = 0,
  SHADER_RESULT_OK    = 1,
};

static ShaderResultType ShaderLayoutLoadBinary(std::vector<s8>& shaderBytes, std::string const& fileName)
{
  std::ifstream file(fileName, std::ios::binary);
  if (file.is_open())
  {
    shaderBytes.clear();
    shaderBytes =
    {
      std::istreambuf_iterator<s8>{ file },
      std::istreambuf_iterator<s8>{}
    };
    file.close();
    return SHADER_RESULT_OK;
  }
  return SHADER_RESULT_ERROR;
}
static ShaderResultType ShaderLayoutCheckCompileStatus(u32 shaderId, std::string& log)
{
  s32 compileStatus;
  s32 infoLogLength;
  glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileStatus);
  if (!compileStatus)
  {
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
    log.clear();
    log.resize(infoLogLength);
    glGetShaderInfoLog(shaderId, infoLogLength, &infoLogLength, log.data());
    return SHADER_RESULT_ERROR;
  }
  return SHADER_RESULT_OK;
}
static ShaderResultType ShaderLayoutCheckLinkStatus(u32 programId, std::string& log)
{
  s32 linkStatus;
  s32 infoLogLength;
  glGetProgramiv(programId, GL_LINK_STATUS, &linkStatus);
  if (!linkStatus)
  {
    glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLogLength);
    log.clear();
    log.resize(infoLogLength);
    glGetProgramInfoLog(programId, infoLogLength, &infoLogLength, log.data());
    return SHADER_RESULT_ERROR;
  }
  return SHADER_RESULT_OK;
}

export template<ShaderLayoutType L>
void        ShaderLayoutCreate(ShaderLayout<L>& shaderLayout, ShaderPaths const& shaderPaths)
{
  std::string log;
  switch (L)
  {
    case SHADER_LAYOUT_LAMBERT:
    case SHADER_LAYOUT_LAMBERT_INSTANCED:
    case SHADER_LAYOUT_GIZMO:
    case SHADER_LAYOUT_SCREEN:
    {
      shaderLayout.mVertId = glCreateShader(GL_VERTEX_SHADER);
      shaderLayout.mFragId = glCreateShader(GL_FRAGMENT_SHADER);
      break;
    }
    case SHADER_LAYOUT_COMPUTE:
    {
      shaderLayout.mCompId = glCreateShader(GL_COMPUTE_SHADER);
      break;
    }
  }
  std::vector<s8> shaderBytes;
  switch (L)
  {
    case SHADER_LAYOUT_LAMBERT:
    case SHADER_LAYOUT_LAMBERT_INSTANCED:
    case SHADER_LAYOUT_GIZMO:
    case SHADER_LAYOUT_SCREEN:
    {
      ShaderLayoutLoadBinary(shaderBytes, shaderPaths.mVertexShader);
      glShaderBinary(1, &shaderLayout.mVertId, GL_SHADER_BINARY_FORMAT_SPIR_V, shaderBytes.data(), (s32)shaderBytes.size());
      glSpecializeShader(shaderLayout.mVertId, "main", 0, nullptr, nullptr);
      if (!ShaderLayoutCheckCompileStatus(shaderLayout.mVertId, log)) std::printf("%s\n", log.data());
      ShaderLayoutLoadBinary(shaderBytes, shaderPaths.mFragmentShader);
      glShaderBinary(1, &shaderLayout.mFragId, GL_SHADER_BINARY_FORMAT_SPIR_V, shaderBytes.data(), (s32)shaderBytes.size());
      glSpecializeShader(shaderLayout.mFragId, "main", 0, nullptr, nullptr);
      if (!ShaderLayoutCheckCompileStatus(shaderLayout.mFragId, log)) std::printf("%s\n", log.data());
      break;
    }
    case SHADER_LAYOUT_COMPUTE:
    {
      ShaderLayoutLoadBinary(shaderBytes, shaderPaths.mComputeShader);
      glShaderBinary(1, &shaderLayout.mCompId, GL_SHADER_BINARY_FORMAT_SPIR_V, shaderBytes.data(), (s32)shaderBytes.size());
      glSpecializeShader(shaderLayout.mCompId, "main", 0, nullptr, nullptr);
      if (!ShaderLayoutCheckCompileStatus(shaderLayout.mCompId, log)) std::printf("%s\n", log.data());
      break;
    }
  }
  shaderLayout.mProgId = glCreateProgram();
  switch (L)
  {
    case SHADER_LAYOUT_LAMBERT:
    case SHADER_LAYOUT_LAMBERT_INSTANCED:
    case SHADER_LAYOUT_GIZMO:
    case SHADER_LAYOUT_SCREEN:
    {
      glAttachShader(shaderLayout.mProgId, shaderLayout.mVertId);
      glAttachShader(shaderLayout.mProgId, shaderLayout.mFragId);
      break;
    }
    case SHADER_LAYOUT_COMPUTE:
    {
      glAttachShader(shaderLayout.mProgId, shaderLayout.mCompId);
      break;
    }
  }
  glLinkProgram(shaderLayout.mProgId);
  if (!ShaderLayoutCheckLinkStatus(shaderLayout.mProgId, log)) std::printf("%s\n", log.data());
  switch (L)
  {
    case SHADER_LAYOUT_LAMBERT:
    case SHADER_LAYOUT_LAMBERT_INSTANCED:
    case SHADER_LAYOUT_GIZMO:
    case SHADER_LAYOUT_SCREEN:
    {
      glDetachShader(shaderLayout.mProgId, shaderLayout.mVertId);
      glDetachShader(shaderLayout.mProgId, shaderLayout.mFragId);
      break;
    }
    case SHADER_LAYOUT_COMPUTE:
    {
      glDetachShader(shaderLayout.mProgId, shaderLayout.mCompId);
      break;
    }
  }
}
export template<ShaderLayoutType L>
void        ShaderLayoutBind(ShaderLayout<L> const& shaderLayout)
{
  glUseProgram(shaderLayout.mProgId);
}
export void ShaderLayoutUnBind()
{
  glUseProgram(0);
}
export template<ShaderLayoutType L>
void        ShaderLayoutCompute(ShaderLayout<L> const& shaderLayout, u32 numThreadsX, u32 numThreadsY, u32 numThreadsZ)
{
  glDispatchCompute(numThreadsX, numThreadsY, numThreadsZ);
}

/*
* Shader uniform routines.
*/

export template<ShaderLayoutType L>
__forceinline void ShaderLayoutUniformS32(ShaderLayout<L> const& shaderLayout, std::string const& uniformName, s32 value)
{
  u32 uniformIndex = (u32)glGetUniformLocation(shaderLayout.mProgId, uniformName.data());
  glUniform1i(uniformIndex, value);
}
export template<ShaderLayoutType L>
__forceinline void ShaderLayoutUniformU32(ShaderLayout<L> const& shaderLayout, std::string const& uniformName, u32 value)
{
  u32 uniformIndex = (u32)glGetUniformLocation(shaderLayout.mProgId, uniformName.data());
  glUniform1ui(uniformIndex, value);
}
template<ShaderLayoutType L>
__forceinline void ShaderLayoutUniformR32(ShaderLayout<L> const& shaderLayout, std::string const& uniformName, r32 value)
{
  u32 uniformIndex = (u32)glGetUniformLocation(shaderLayout.mProgId, uniformName.data());
  glUniform1f(uniformIndex, value);
}
template<ShaderLayoutType L>
__forceinline void ShaderLayoutUniformR32V3(ShaderLayout<L> const& shaderLayout, std::string const& uniformName, r32v3 const& vector)
{
  u32 uniformIndex = (u32)glGetUniformLocation(shaderLayout.mProgId, uniformName.data());
  glUniform3fv(uniformIndex, 1, &vector[0]);
}
template<ShaderLayoutType L>
__forceinline void ShaderLayoutUniformR32M4(ShaderLayout<L> const& shaderLayout, std::string const& uniformName, r32m4 const& matrix)
{
  u32 uniformIndex = (u32)glGetUniformLocation(shaderLayout.mProgId, uniformName.data());
  glUniformMatrix4fv(uniformIndex, 1, GL_FALSE, &matrix[0]);
}

/*
* Uniform layouts.
*/

template<typename B>
struct UniformLayout
{
  u32 mUbo;
};

/*
* Uniform specific routines.
*/

export template<typename B>
void        UniformLayoutCreate(UniformLayout<B>& uniformLayout, u32 bufferSize = 1)
{
  glGenBuffers(1, &uniformLayout.mUbo);
  glBindBuffer(GL_UNIFORM_BUFFER, uniformLayout.mUbo);
  glBufferStorage(GL_UNIFORM_BUFFER, bufferSize * sizeof(B), nullptr, GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
export template<typename B>
void        UniformLayoutBind(UniformLayout<B> const& uniformLayout)
{
  glBindBuffer(GL_UNIFORM_BUFFER, uniformLayout.mUbo);
}
export void UniformLayoutUnBind()
{
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
export template<typename B>
void        UniformLayoutMap(UniformLayout<B> const& uniformLayout, u32 mappingIndex)
{
  glBindBufferBase(GL_UNIFORM_BUFFER, mappingIndex, uniformLayout.mUbo);
}
export template<typename B>
void        UniformLayoutData(UniformLayout<B> const& uniformLayout, u32 bufferSize, void* pBufferData)
{
  glBufferSubData(GL_UNIFORM_BUFFER, 0, bufferSize * sizeof(B), pBufferData);
}
export template<typename B>
void        UniformLayoutDataGet(UniformLayout<B> const& uniformLayout, u32 bufferSize, void* pBufferData)
{
  glGetBufferSubData(GL_UNIFORM_BUFFER, 0, bufferSize * sizeof(B), pBufferData);
}

/*
* SSBO layouts.
*/

template<typename B>
struct BufferLayout
{
  u32 mSsbo;
  u32 mBufferSize;
};

/*
* SSBO specific routines.
*/

export template<typename B>
void        BufferLayoutCreate(BufferLayout<B>& bufferLayout, u32 bufferSize)
{
  bufferLayout.mBufferSize = bufferSize;
  glGenBuffers(1, &bufferLayout.mSsbo);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferLayout.mSsbo);
  glBufferStorage(GL_SHADER_STORAGE_BUFFER, bufferLayout.mBufferSize * sizeof(B), nullptr, GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
export template<typename B>
void        BufferLayoutBind(BufferLayout<B> const& bufferLayout)
{
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferLayout.mSsbo);
}
export void BufferLayoutUnBind()
{
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
export template<typename B>
void        BufferLayoutMap(BufferLayout<B> const& bufferLayout, u32 mappingIndex)
{
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, mappingIndex, bufferLayout.mSsbo);
}
export template<typename B>
void        BufferLayoutData(BufferLayout<B> const& bufferLayout, u32 bufferSize, void* pBufferData)
{
  glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, bufferSize * sizeof(B), pBufferData);
}
export template<typename B>
void        BufferLayoutDataGet(BufferLayout<B> const& bufferLayout, u32 bufferSize, void* pBufferData)
{
  glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, bufferSize * sizeof(B), pBufferData);
}

/*
* Global render uniforms.
*/

#pragma pack(push, 1)
struct UniformBlockGlobal
{
  r32   mTime;
  r32v2 mScreenSize;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct UniformBlockProjection
{
  r32m4 mProjection;
  r32m4 mView;
  r32m4 mTransform;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct UniformBlockCamera
{
  r32v3 mPosition;
};
#pragma pack(pop)

/*
* Renderer layouts.
*/

export struct Renderer
{
  u32                                   mWidth;
  u32                                   mHeight;
  UniformLayout<UniformBlockGlobal>     mUniformGlobal;
  UniformLayout<UniformBlockProjection> mUniformProjection;
  UniformLayout<UniformBlockCamera>     mUniformCamera;
  ShaderGizmo                           mShaderGizmo;
  ShaderLambert                         mShaderLambert;
  ShaderLambertInstanced                mShaderLambertInstanced;
  r32m4                                 mTransformGizmo;
  MeshGizmo                             mMeshGizmoLines;
  u32                                   mVertexOffsetGizmoLineBatch;
  u32                                   mIndexOffsetGizmoLineBatch;
};

/*
* Render pass routines.
*/

void RenderPassGeometry(Renderer& renderer)
{

}
void RenderPassGeometryInstanced(Renderer& renderer)
{

}
void RenderPassLight(Renderer& renderer)
{

}
void RenderPassGizmo(Renderer& renderer)
{

}

/*
* Renderer specific routines.
*/

export void RendererCreate(Renderer& renderer, u32 width, u32 height)
{
  renderer.mWidth = width;
  renderer.mHeight = height;
  UniformLayoutCreate(renderer.mUniformGlobal);
  UniformLayoutCreate(renderer.mUniformProjection);
  UniformLayoutCreate(renderer.mUniformCamera);
  ShaderLayoutCreate(renderer.mShaderGizmo, ShaderPaths
    {
      .mVertexShader{ "C:\\Users\\Michael\\Downloads\\oglib\\oglib\\shaders\\spriv\\Gizmo.vert" },
      .mFragmentShader{ "C:\\Users\\Michael\\Downloads\\oglib\\oglib\\shaders\\spriv\\Gizmo.frag" }
    });
  ShaderLayoutCreate(renderer.mShaderLambert, ShaderPaths
    {
      .mVertexShader{ "C:\\Users\\Michael\\Downloads\\oglib\\oglib\\shaders\\spriv\\Lambert.vert" },
      .mFragmentShader{ "C:\\Users\\Michael\\Downloads\\oglib\\oglib\\shaders\\spriv\\Lambert.frag" }
    });
  ShaderLayoutCreate(renderer.mShaderLambertInstanced, ShaderPaths
    {
      .mVertexShader{ "C:\\Users\\Michael\\Downloads\\oglib\\oglib\\shaders\\spriv\\LambertInstanced.vert" },
      .mFragmentShader{ "C:\\Users\\Michael\\Downloads\\oglib\\oglib\\shaders\\spriv\\Lambert.frag" },
    });
  std::vector<u32> gizmoVertexBufferSizes = { 131070 };
  std::vector<u32> gizmoIndexBufferSizes = { 131070 * 2 };
  MeshLayoutCreate(renderer.mMeshGizmoLines, 1, gizmoVertexBufferSizes.data(), gizmoIndexBufferSizes.data());
}
export void RendererBegin(Renderer& renderer, r32 time)
{
  //Dispatch<Transform, Camera>([&](Transform* pTransform, Camera* pCamera)
  //  {
  //
  //  });
  UniformBlockGlobal uniformBlockGlobal = { time, r32v2{ (r32)renderer.mWidth, (r32)renderer.mHeight } };
  UniformBlockProjection uniformBlockProjection = { r32m4{}, r32m4{}, r32m4{} };
  UniformBlockCamera uniformBlockCamera = { r32v3{} };
  UniformLayoutBind(renderer.mUniformGlobal);
  UniformLayoutData(renderer.mUniformGlobal, 1, &uniformBlockGlobal);
  UniformLayoutBind(renderer.mUniformProjection);
  UniformLayoutData(renderer.mUniformProjection, 1, &uniformBlockProjection);
  UniformLayoutBind(renderer.mUniformCamera);
  UniformLayoutData(renderer.mUniformCamera, 1, &uniformBlockCamera);
  UniformLayoutUnBind();
  MeshLayoutBind(renderer.mMeshGizmoLines, 0);
  MeshLayoutClear(renderer.mMeshGizmoLines, 0);
  MeshLayoutUnBind();
}
export void Render(Renderer& renderer)
{
  glClearColor(0.f, 0.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Geometry pass
  //glBindFramebuffer(GL_DRAW_FRAMEBUFFER, renderer.mFrameBufferDeferred.mFbo);
  //glClearColor(1.f, 0.f, 0.f, 1.f);
  //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //glEnable(GL_DEPTH_TEST);
  //glEnable(GL_BLEND);
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  //RenderPassGeometry(renderer);
  //RenderPassGeometryInstanced(renderer);
  //glDisable(GL_BLEND);
  //glDisable(GL_DEPTH_TEST);
  //glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // Light pass
  //glBindFramebuffer(GL_READ_FRAMEBUFFER, renderer.mFrameBufferDeferred.mFbo);
  //RenderPassLight(renderer);
  //glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // Copy geometry depth into default framebuffer
  //glBindFramebuffer(GL_READ_FRAMEBUFFER, renderer.mFrameBufferDeferred.mFbo);
  //glBlitFramebuffer(0, 0, renderer.mWidth, renderer.mHeight, 0, 0, renderer.mWidth, renderer.mHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
  //glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // Gizmo pass
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glEnable(GL_DEPTH_TEST);
  RenderPassGizmo(renderer);
  glDisable(GL_DEPTH_TEST);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
export void RendererEnd(Renderer& renderer)
{
  MeshLayoutUnBind();
  ShaderLayoutUnBind();
  UniformLayoutUnBind();
  BufferLayoutUnBind();
  renderer.mVertexOffsetGizmoLineBatch = 0;
  renderer.mIndexOffsetGizmoLineBatch = 0;
}

/*
* Gizmo specific render routines.
*/

export void RendererLineBatchPushMatrix(Renderer& renderer, r32m4 const& transform)
{
  renderer.mTransformGizmo = transform;
}
export void RendererLineBatchPopMatrix(Renderer& renderer)
{
  renderer.mTransformGizmo = Identity<r32, 4>();
}
export void RendererLineBatchPushLine(Renderer& renderer, r32v3 const& p0, r32v3 const& p1, r32v4 const& color)
{
  //r32v3 v0 = renderer.mTransformGizmo * r32v4{ p0, 1.f };
  //r32v3 v1 = renderer.mTransformGizmo * r32v4{ p1, 1.f };
  //VertexGizmo vertices[2]{ { v0, color }, { v1, color } };
  //u32 indices[2]{ renderer.mVertexOffsetGizmoLineBatch + 0, renderer.mVertexOffsetGizmoLineBatch + 1 };
  //MeshLayoutDataSub(renderer.mMeshGizmoLineBatch, 0, vertices, indices, renderer.mVertexOffsetGizmoLineBatch, renderer.mIndexOffsetGizmoLineBatch, 2, 2);
  //renderer.mVertexOffsetGizmoLineBatch += 2;
  //renderer.mIndexOffsetGizmoLineBatch += 2;
}
export void RendererLineBatchPushBox(Renderer& renderer, r32v3 const& position, r32v3 const& size, r32v4 const& color)
{
  /*
  r32v3 blf = renderer.mTransformGizmo * r32v4{ -size.x, -size.y, -size.z, 1.f };
  r32v3 brf = renderer.mTransformGizmo * r32v4{ size.x, -size.y, -size.z, 1.f };
  r32v3 tlf = renderer.mTransformGizmo * r32v4{ -size.x,  size.y, -size.z, 1.f };
  r32v3 trf = renderer.mTransformGizmo * r32v4{ size.x,  size.y, -size.z, 1.f };
  r32v3 blb = renderer.mTransformGizmo * r32v4{ -size.x, -size.y,  size.z, 1.f };
  r32v3 brb = renderer.mTransformGizmo * r32v4{ size.x, -size.y,  size.z, 1.f };
  r32v3 tlb = renderer.mTransformGizmo * r32v4{ -size.x,  size.y,  size.z, 1.f };
  r32v3 trb = renderer.mTransformGizmo * r32v4{ size.x,  size.y,  size.z, 1.f };
  VertexGizmo vertices[8]
  {
    // front
    { position + blf, color },
    { position + brf, color },
    { position + tlf, color },
    { position + trf, color },

    // back
    { position + blb, color },
    { position + brb, color },
    { position + tlb, color },
    { position + trb, color },
  };
  u32 indices[24]
  {
    // Front
    renderer.mVertexOffsetGizmoLineBatch + 0,
    renderer.mVertexOffsetGizmoLineBatch + 1,
    renderer.mVertexOffsetGizmoLineBatch + 0,
    renderer.mVertexOffsetGizmoLineBatch + 2,
    renderer.mVertexOffsetGizmoLineBatch + 2,
    renderer.mVertexOffsetGizmoLineBatch + 3,
    renderer.mVertexOffsetGizmoLineBatch + 3,
    renderer.mVertexOffsetGizmoLineBatch + 1,
    // Back
    renderer.mVertexOffsetGizmoLineBatch + 4,
    renderer.mVertexOffsetGizmoLineBatch + 5,
    renderer.mVertexOffsetGizmoLineBatch + 4,
    renderer.mVertexOffsetGizmoLineBatch + 6,
    renderer.mVertexOffsetGizmoLineBatch + 6,
    renderer.mVertexOffsetGizmoLineBatch + 7,
    renderer.mVertexOffsetGizmoLineBatch + 7,
    renderer.mVertexOffsetGizmoLineBatch + 5,
    // Connections
    renderer.mVertexOffsetGizmoLineBatch + 0,
    renderer.mVertexOffsetGizmoLineBatch + 4,
    renderer.mVertexOffsetGizmoLineBatch + 1,
    renderer.mVertexOffsetGizmoLineBatch + 5,
    renderer.mVertexOffsetGizmoLineBatch + 2,
    renderer.mVertexOffsetGizmoLineBatch + 6,
    renderer.mVertexOffsetGizmoLineBatch + 3,
    renderer.mVertexOffsetGizmoLineBatch + 7,
  };
  MeshLayoutDataSub(renderer.mMeshGizmoLineBatch, 0, vertices, indices, renderer.mVertexOffsetGizmoLineBatch, renderer.mIndexOffsetGizmoLineBatch, 8, 24);
  renderer.mVertexOffsetGizmoLineBatch += 8;
  renderer.mIndexOffsetGizmoLineBatch += 24;
  */
}