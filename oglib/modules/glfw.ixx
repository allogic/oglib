/*
* GLFW OpenGL context.
*/

module;

/*
* Includes.
*/

#include <string>

#include "glad.h"
#include "glfw3.h"

/*
* Begin module GLFW.
*/

export module glfw;

/*
* Imports.
*/

import math;
import acs;

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
  virtual void OnUpdate(r32 time) = 0;
  virtual void OnPhysic(r32 time) = 0;
  virtual void OnRender(r32 time) const = 0;
};

/*
* Window methods.
*/

export template<Sandboxable S>
__forceinline s32 Start(u32 width, u32 height, u32 major, u32 minor, std::string const& title, u32 fps = 60) noexcept
{
  glfwInit();
  glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_VISIBLE, GL_TRUE);
  glfwWindowHint(GLFW_DECORATED, GL_TRUE);
  glfwWindowHint(GLFW_FOCUSED, GL_TRUE);
  glfwWindowHint(GLFW_SAMPLES, 0);
  GLFWwindow* pWindow = glfwCreateWindow((s32)width, (s32)height, title.c_str(), nullptr, nullptr);
  if (!pWindow)
  {
    return -1;
  }
  gladLoadGL();
  glfwMakeContextCurrent(pWindow);
  glfwSwapInterval(0);
  u32 running = 1;
  r32 time = 0.f;
  r32 timePrev = 0.f;
  r32 timeDelta = 0.f;
  r32 timeRender = 1.f / fps;
  r32 timeRenderPrev = 0.f;
  Sandbox* pSandbox = new S;
  if (!pSandbox)
  {
    glfwDestroyWindow(pWindow);
    glfwTerminate();
    return -1;
  }
  while (running)
  {
    glfwPollEvents();
    time = (r32)glfwGetTime();
    timeDelta = time - timePrev;
    pSandbox->OnUpdate(time);
    if ((time - timeRenderPrev) >= timeRender)
    {
      pSandbox->OnPhysic(time);
      pSandbox->OnRender(time);
      glfwSwapBuffers(pWindow);
      timeRenderPrev = time;
    }
    timePrev = time;
  }
  delete pSandbox;
  glfwDestroyWindow(pWindow);
  glfwTerminate();
  return 0;
}