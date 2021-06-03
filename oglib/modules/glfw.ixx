/*
* GLFW OpenGL context.
*/

module;

/*
* Includes.
*/

#include <string>
#include <vector>

#include <glfw3.h>

#include <vulkan/vulkan.h>

/*
* Begin module GLFW.
*/

export module glfw;

/*
* Imports.
*/

import type;
import acs;
import render;

/*
* Forward decls.
*/

export struct Sandbox;

/*
* Type deduction utilities.
*/

template<typename T>
concept Sandboxable = std::is_base_of_v<Sandbox, T>;

/*
* Interface for custom demos.
*/

export struct Sandbox
{
  virtual void OnUpdate(r32 time) {};
  virtual void OnPhysic(r32 time) {};
  virtual void OnDebug(r32 time) const {};
};

/*
* Window routines.
*/

export template<Sandboxable S>
__forceinline s32 Start(u32 width, u32 height, std::string const& title, u32 fps = 60, u32 debug = 0) noexcept
{
  // Initialize GLFW
  glfwInit();
  glfwWindowHint(GLFW_RESIZABLE, 1);
  glfwWindowHint(GLFW_VISIBLE, 1);
  glfwWindowHint(GLFW_DECORATED, 1);
  glfwWindowHint(GLFW_FOCUSED, 1);
  glfwWindowHint(GLFW_SAMPLES, 0);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  GLFWwindow* pWindow{ glfwCreateWindow((s32)width, (s32)height, title.c_str(), nullptr, nullptr) };
  if (!pWindow)
  {
    return -1;
  }
  glfwMakeContextCurrent(pWindow);
  glfwSwapInterval(0);
  // Gather instance extensions
  u32 glfwExtensionSize{};
  s8 const** ppGlfwExtensions{ glfwGetRequiredInstanceExtensions(&glfwExtensionSize) };
  std::vector<s8 const*> glfwExtensions{};
  for (u32 i{}; i < glfwExtensionSize; ++i)
  {
    glfwExtensions.emplace_back(ppGlfwExtensions[i]);
  }
  if (debug)
  {
    glfwExtensions.emplace_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
  }
  // Engine core
  u32 running{ 1 };
  r32 time{};
  r32 timePrev{};
  r32 timeDelta{};
  r32 timeFixed{ 1.f / fps };
  r32 timeFixedPrev{};
  Sandbox* pSandbox{ new S };
  if (!pSandbox)
  {
    glfwDestroyWindow(pWindow);
    glfwTerminate();
    return -1;
  }
  CreateRenderer(pWindow, glfwExtensions, debug);
  while (running)
  {
    glfwPollEvents();
    time = (r32)glfwGetTime();
    timeDelta = time - timePrev;
    pSandbox->OnUpdate(time);
    if ((time - timeFixedPrev) >= timeFixed)
    {
      pSandbox->OnPhysic(time);
      //RenderBegin(width, height, time);
      //DeferredRenderBegin();
      //DeferredRender();
      //DeferredRenderEnd();
      //DebugRenderBegin();
      //pSandbox->OnDebug(time);
      //DebugRender();
      //DebugRenderEnd();
      //RenderEnd();
      glfwSwapBuffers(pWindow);
      timeFixedPrev = time;
    }
    timePrev = time;
  }
  DestroyRenderer();
  delete pSandbox;
  glfwDestroyWindow(pWindow);
  glfwTerminate();
  return 0;
}