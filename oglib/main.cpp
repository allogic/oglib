#include <iostream>
#include <string>

import glfw;
import math;
import acs;
import component;
import render;

u32 Width  = 720;
u32 Height = 360;

struct Box : Actor
{
  Transform*  pTransform  = nullptr;
  Renderable* pRenderable = nullptr;

  Box(r32v3 const& p)
  {
    //pTransform = Attach<Transform>(this, p, r32v3{ 0, 0, 0 }, r32v3{ 10, 10, 10 });
    //pRenderable = Attach<Renderable>(this);
  }

  static void Update(Transform* pTransform)
  {
    
  }
  static void Render(Transform* pTransform, Renderable* pRenderable)
  {

  }
};

struct Demo : Sandbox
{
  Demo()
  {
    std::cout << "Demo created\n";

    Create<Box>("Box", r32v3{ 0, 0, 0 });
  }
  ~Demo()
  {
    std::cout << "Demo destroyed\n";
  }

  void OnUpdate(r32 time) override
  {

  }
  void OnPhysic(r32 time) override
  {

  }
  void OnRender(r32 time) const override
  {

  }
};

int main()
{
  return Start<Demo>(1280, 720, 4, 6, "Sandbox");
}