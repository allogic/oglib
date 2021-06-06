#ifndef VK_WINDOW
#define VK_WINDOW

#include "VkCore.h"
#include "VkRenderer.h"

struct Sandbox
{
  virtual void OnUpdate(r32 time) {};
  virtual void OnPhysic(r32 time) {};
  virtual void OnDebug(r32 time) const {};
};

template<typename T>
concept Sandboxable = std::is_base_of_v<Sandbox, T>;

template<Sandboxable S>
class VkWindow
{
public:
  VkWindow(u32 width, u32 height, std::string const& title, u32 fps = 60, u32 debug = 0)
  {
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_RESIZABLE, 1);
    glfwWindowHint(GLFW_VISIBLE, 1);
    glfwWindowHint(GLFW_DECORATED, 1);
    glfwWindowHint(GLFW_FOCUSED, 1);
    glfwWindowHint(GLFW_SAMPLES, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    mpGlfwWindow = glfwCreateWindow((s32)width, (s32)height, title.c_str(), nullptr, nullptr);
    if (!mpGlfwWindow)
    {
      std::exit(1);
    }
    glfwMakeContextCurrent(mpGlfwWindow);
    glfwSwapInterval(0);
    // Engine core
    u32 running{ 1 };
    r32 time{};
    r32 timePrev{};
    r32 timeDelta{};
    r32 timeFixed{ 1.f / fps };
    r32 timeFixedPrev{};
    mpVkRenderer = new VkRenderer{ width, height, mpGlfwWindow, debug };
    mpSandbox = new S;
    while (running)
    {
      glfwPollEvents();
      time = (r32)glfwGetTime();
      timeDelta = time - timePrev;
      mpSandbox->OnUpdate(time);
      if ((time - timeFixedPrev) >= timeFixed)
      {
        mpSandbox->OnPhysic(time);
        //RenderBegin(width, height, time);
        //DeferredRenderBegin();
        //DeferredRender();
        //DeferredRenderEnd();
        //DebugRenderBegin();
        //pSandbox->OnDebug(time);
        //DebugRender();
        //DebugRenderEnd();
        //RenderEnd();
        glfwSwapBuffers(mpGlfwWindow);
        timeFixedPrev = time;
      }
      timePrev = time;
    }
  }
  virtual ~VkWindow()
  {
    delete mpSandbox;
    delete mpVkRenderer;
    glfwDestroyWindow(mpGlfwWindow);
    glfwTerminate();
  }

private:
  GLFWwindow* mpGlfwWindow{};
  Sandbox*    mpSandbox   {};
  VkRenderer* mpVkRenderer{};
};

#endif