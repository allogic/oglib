/*
* OpenGL renderer.
*/

module;

/*
* Includes.
*/

#include <iostream>
#include <string>
#include <cstdio>
#include <vector>
#include <fstream>
#include <deque>
#include <type_traits>

#include <glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/*
* Begin module OpenGL renderer.
*/

export module render;

/*
* Imports.
*/

import type;
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
export template<typename V, typename I>
void        MeshLayoutDestroy(MeshLayout<V, I> const& meshLayout)
{
  glDeleteBuffers(meshLayout.mNumSubMeshes, meshLayout.mVbos.data());
  glDeleteBuffers(meshLayout.mNumSubMeshes, meshLayout.mEbos.data());
  glDeleteVertexArrays(meshLayout.mNumSubMeshes, meshLayout.mVaos.data());
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
  std::string mVertexSource;
  std::string mFragmentSource;
  std::string mComputeSource;
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
      ShaderLayoutLoadBinary(shaderBytes, shaderPaths.mVertexSource);
      glShaderBinary(1, &shaderLayout.mVertId, GL_SHADER_BINARY_FORMAT_SPIR_V, shaderBytes.data(), (s32)shaderBytes.size());
      glSpecializeShader(shaderLayout.mVertId, "main", 0, nullptr, nullptr);
      if (!ShaderLayoutCheckCompileStatus(shaderLayout.mVertId, log)) std::printf("%s\n", log.data());
      ShaderLayoutLoadBinary(shaderBytes, shaderPaths.mFragmentSource);
      glShaderBinary(1, &shaderLayout.mFragId, GL_SHADER_BINARY_FORMAT_SPIR_V, shaderBytes.data(), (s32)shaderBytes.size());
      glSpecializeShader(shaderLayout.mFragId, "main", 0, nullptr, nullptr);
      if (!ShaderLayoutCheckCompileStatus(shaderLayout.mFragId, log)) std::printf("%s\n", log.data());
      break;
    }
    case SHADER_LAYOUT_COMPUTE:
    {
      ShaderLayoutLoadBinary(shaderBytes, shaderPaths.mComputeSource);
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
export template<ShaderLayoutType L>
void        ShaderLayoutDestroy(ShaderLayout<L> const& shaderLayout)
{
  if (shaderLayout.mVertId) glDeleteShader(shaderLayout.mVertId);
  if (shaderLayout.mFragId) glDeleteShader(shaderLayout.mFragId);
  if (shaderLayout.mCompId) glDeleteShader(shaderLayout.mCompId);
  if (shaderLayout.mProgId) glDeleteProgram(shaderLayout.mProgId);
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
export template<typename B>
void        UniformLayoutDestroy(UniformLayout<B> const& uniformLayout)
{
  glDeleteBuffers(1, &uniformLayout.mUbo);
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

UniformBlockGlobal                    sUniformBlockGlobal     = {};
UniformBlockProjection                sUniformBlockProjection = {};
UniformBlockCamera                    sUniformBlockCamera     = {};

UniformLayout<UniformBlockGlobal>     sUniformGlobal          = {};
UniformLayout<UniformBlockProjection> sUniformProjection      = {};
UniformLayout<UniformBlockCamera>     sUniformCamera          = {};

/*
* Render task layouts.
*/

struct TaskDeferred
{
  Transform*     mpTransform;
  MeshLambert*   mpMeshLambert;
  ShaderLambert* mpShaderLambert;
};

/*
* Renderer layouts.
*/

struct DebugRenderer
{
  MeshGizmo   mMeshGizmo;
  ShaderGizmo mShaderGizmo;
  u32         mVertexOffsetGizmoLineBatch;
  u32         mIndexOffsetGizmoLineBatch;
};
struct DeferredRenderer
{
  std::deque<TaskDeferred> mRenderQueue;
  ShaderLambert            mShaderLambert;
  ShaderLambertInstanced   mShaderLambertInstanced;
};

DebugRenderer    sDebugRenderer    = {};
DeferredRenderer sDeferredRenderer = {};

/*
* Renderer specific routines.
*/

void DeferredRendererCreate()
{
  ShaderLayoutCreate(sDeferredRenderer.mShaderLambert, ShaderPaths
    {
      .mVertexSource{ "C:\\Users\\Michael\\Downloads\\oglib\\spirv\\compiled\\lambert.vert" },
      .mFragmentSource{ "C:\\Users\\Michael\\Downloads\\oglib\\spirv\\compiled\\lambert.frag" }
    });
  ShaderLayoutCreate(sDeferredRenderer.mShaderLambertInstanced, ShaderPaths
    {
      .mVertexSource{ "C:\\Users\\Michael\\Downloads\\oglib\\spirv\\compiled\\lambert_instanced.vert" },
      .mFragmentSource{ "C:\\Users\\Michael\\Downloads\\oglib\\spirv\\compiled\\lambert_instanced.frag" }
    });
}
void DeferredRendererDestroy()
{
  ShaderLayoutDestroy(sDeferredRenderer.mShaderLambert);
  ShaderLayoutDestroy(sDeferredRenderer.mShaderLambertInstanced);
}
void DebugRendererCreate()
{
  std::vector<u32> gizmoVertexBufferSizes{ 131070 };
  std::vector<u32> gizmoIndexBufferSizes{ 131070 * 2 };
  MeshLayoutCreate(sDebugRenderer.mMeshGizmo, 1, gizmoVertexBufferSizes.data(), gizmoIndexBufferSizes.data());
  ShaderLayoutCreate(sDebugRenderer.mShaderGizmo, ShaderPaths
    {
      .mVertexSource{ "C:\\Users\\Michael\\Downloads\\oglib\\spirv\\compiled\\gizmo.vert" },
      .mFragmentSource{ "C:\\Users\\Michael\\Downloads\\oglib\\spirv\\compiled\\gizmo.frag" }
    });
}
void DebugRendererDestroy()
{
  MeshLayoutDestroy(sDebugRenderer.mMeshGizmo);
  ShaderLayoutDestroy(sDebugRenderer.mShaderGizmo);
}

/*
* Deferred specific routines.
*/

export void DeferredRenderBegin()
{
  Dispatch<Transform, Renderable>([&](Transform* pTransform, Renderable* pRenderable)
    {
      sDeferredRenderer.mRenderQueue.push_front(TaskDeferred{ pTransform, (MeshLambert*)pRenderable->mpMeshLayout, (ShaderLambert*)pRenderable->mpShaderLayout });
    });
}
export void DeferredRender()
{
  // Deferred pass
  while (!sDeferredRenderer.mRenderQueue.empty())
  {
    TaskDeferred& task = sDeferredRenderer.mRenderQueue.front();

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

    sDeferredRenderer.mRenderQueue.pop_front();
  }
}
export void DeferredRenderEnd()
{
  MeshLayoutUnBind();
  ShaderLayoutUnBind();
}

/*
* Debug specific routines.
*/

export void DebugRenderBegin()
{
  MeshLayoutBind(sDebugRenderer.mMeshGizmo, 0);
  MeshLayoutClear(sDebugRenderer.mMeshGizmo, 0);
}
export void DebugRender()
{
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glEnable(GL_DEPTH_TEST);
  ShaderLayoutBind(sDebugRenderer.mShaderGizmo);
  MeshLayoutRenderPartial(sDebugRenderer.mMeshGizmo, 0, RENDER_MODE_LINE);
  glDisable(GL_DEPTH_TEST);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
export void DebugRenderEnd()
{
  MeshLayoutUnBind();
  ShaderLayoutUnBind();
  sDebugRenderer.mVertexOffsetGizmoLineBatch = 0;
  sDebugRenderer.mIndexOffsetGizmoLineBatch = 0;
}

/*
* Global render routines.
*/

// TODO: refactor *End() routines - don't need them anymore

export void CreateRenderer()
{
  // Create global UBO's
  UniformLayoutCreate(sUniformGlobal);
  UniformLayoutCreate(sUniformProjection);
  UniformLayoutCreate(sUniformCamera);
  // Create renderer
  DebugRendererCreate();
  DeferredRendererCreate();
}
export void RenderBegin(u32 width, u32 height, r32 time)
{
  // Update global UBO's
  sUniformBlockGlobal = { time, r32v2{ width, height } };
  Dispatch<Transform, Camera>([&](Transform* pTransform, Camera* pCamera)
    {
      r32m4 rotation = glm::identity<r32m4>();
      rotation = glm::rotate(rotation, pTransform->mRotationEuler.x, r32v3{ 1, 0, 0 });
      rotation = glm::rotate(rotation, pTransform->mRotationEuler.y, r32v3{ 0, 1, 0 });
      rotation = glm::rotate(rotation, pTransform->mRotationEuler.z, r32v3{ 0, 0, 1 });
      r32v3 const localForward = rotation * r32v4{ 0, 0, 1, 1 };
      r32v3 const localUp = rotation * r32v4{ 0, 1, 0, 1 };
      sUniformBlockProjection.mProjection = glm::perspective(pCamera->mFov, (r32)width / height, pCamera->mNear, pCamera->mFar);
      sUniformBlockProjection.mView = glm::lookAt(pTransform->mPosition, pTransform->mPosition + localForward, localUp);
      sUniformBlockCamera.mPosition = pTransform->mPosition;
    });
  UniformLayoutBind(sUniformGlobal);
  UniformLayoutData(sUniformGlobal, 1, &sUniformBlockGlobal);
  UniformLayoutMap(sUniformGlobal, 0);
  UniformLayoutBind(sUniformProjection);
  UniformLayoutData(sUniformProjection, 1, &sUniformBlockProjection);
  UniformLayoutMap(sUniformProjection, 1);
  UniformLayoutBind(sUniformCamera);
  UniformLayoutData(sUniformCamera, 1, &sUniformBlockCamera);
  UniformLayoutMap(sUniformCamera, 2);
  UniformLayoutUnBind();
}
export void RenderEnd()
{
  UniformLayoutUnBind();
}
export void DestroyRenderer()
{
  // Destroy renderer
  DebugRendererDestroy();
  DeferredRendererDestroy();
  // Destroy global UBO's
  UniformLayoutDestroy(sUniformGlobal);
  UniformLayoutDestroy(sUniformProjection);
  UniformLayoutDestroy(sUniformCamera);
}

export void DebugRenderLine(r32v3 const& p0, r32v3 const& p1, r32v4 const& color, r32m4 const& transform)
{
  VertexGizmo vertices[2]{ { p0, color }, { p1, color } };
  u32 indices[2]{ sDebugRenderer.mVertexOffsetGizmoLineBatch + 0, sDebugRenderer.mVertexOffsetGizmoLineBatch + 1 };
  MeshLayoutData(sDebugRenderer.mMeshGizmo, 0, vertices, indices, sDebugRenderer.mVertexOffsetGizmoLineBatch, sDebugRenderer.mIndexOffsetGizmoLineBatch, 2, 2);
  sDebugRenderer.mVertexOffsetGizmoLineBatch += 2;
  sDebugRenderer.mIndexOffsetGizmoLineBatch += 2;
}
export void DebugRenderBox(r32v3 const& position, r32v3 const& size, r32v4 const& color, r32m4 const& transform)
{
  r32v3 blf = r32v3{ -size.x, -size.y, -size.z };
  r32v3 brf = r32v3{  size.x, -size.y, -size.z };
  r32v3 tlf = r32v3{ -size.x,  size.y, -size.z };
  r32v3 trf = r32v3{  size.x,  size.y, -size.z };
  r32v3 blb = r32v3{ -size.x, -size.y,  size.z };
  r32v3 brb = r32v3{  size.x, -size.y,  size.z };
  r32v3 tlb = r32v3{ -size.x,  size.y,  size.z };
  r32v3 trb = r32v3{  size.x,  size.y,  size.z };
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
    sDebugRenderer.mVertexOffsetGizmoLineBatch + 0,
    sDebugRenderer.mVertexOffsetGizmoLineBatch + 1,
    sDebugRenderer.mVertexOffsetGizmoLineBatch + 0,
    sDebugRenderer.mVertexOffsetGizmoLineBatch + 2,
    sDebugRenderer.mVertexOffsetGizmoLineBatch + 2,
    sDebugRenderer.mVertexOffsetGizmoLineBatch + 3,
    sDebugRenderer.mVertexOffsetGizmoLineBatch + 3,
    sDebugRenderer.mVertexOffsetGizmoLineBatch + 1,
    // Back
    sDebugRenderer.mVertexOffsetGizmoLineBatch + 4,
    sDebugRenderer.mVertexOffsetGizmoLineBatch + 5,
    sDebugRenderer.mVertexOffsetGizmoLineBatch + 4,
    sDebugRenderer.mVertexOffsetGizmoLineBatch + 6,
    sDebugRenderer.mVertexOffsetGizmoLineBatch + 6,
    sDebugRenderer.mVertexOffsetGizmoLineBatch + 7,
    sDebugRenderer.mVertexOffsetGizmoLineBatch + 7,
    sDebugRenderer.mVertexOffsetGizmoLineBatch + 5,
    // Connections
    sDebugRenderer.mVertexOffsetGizmoLineBatch + 0,
    sDebugRenderer.mVertexOffsetGizmoLineBatch + 4,
    sDebugRenderer.mVertexOffsetGizmoLineBatch + 1,
    sDebugRenderer.mVertexOffsetGizmoLineBatch + 5,
    sDebugRenderer.mVertexOffsetGizmoLineBatch + 2,
    sDebugRenderer.mVertexOffsetGizmoLineBatch + 6,
    sDebugRenderer.mVertexOffsetGizmoLineBatch + 3,
    sDebugRenderer.mVertexOffsetGizmoLineBatch + 7,
  };
  MeshLayoutData(sDebugRenderer.mMeshGizmo, 0, vertices, indices, sDebugRenderer.mVertexOffsetGizmoLineBatch, sDebugRenderer.mIndexOffsetGizmoLineBatch, 8, 24);
  sDebugRenderer.mVertexOffsetGizmoLineBatch += 8;
  sDebugRenderer.mIndexOffsetGizmoLineBatch += 24;
}