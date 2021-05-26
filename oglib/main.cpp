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

  Box(r32v3 const& p)
  {
    //pTransform = Attach<Transform>(this, p, r32v3{ 0, 0, 0 }, r32v3{ 10, 10, 10 });
  }
};

struct Demo : Sandbox
{
  Demo()
  {
    std::cout << "Demo created\n";

    Create<Box>("Box", r32v3{ 0, 0, 0 });

    // TODO: test component selects
    // TODO: test gizmo rendering & queues

    r32v2 v2 = { 1.f, 2.f };
    r32v3 v3 = { v2, 3.f };
    r32v4 v4 = { v3, 4.f };

    Print(v2);
    Print(v3);
    Print(v4);

    r32m2 m2 = { 0.f, 1.f, 2.f, 3.f };
    r32m3 m3 = { 0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f };
    r32m4 m4 = { 0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 11.f, 12.f, 13.f, 14.f, 15.f };

    Print(m2);
    Print(m3);
    Print(m4);
  }
  virtual ~Demo()
  {
    std::cout << "Demo destroyed\n";
  }

  void OnUpdate(r32 time) override
  {

  }
  void OnPhysic(r32 time) override
  {

  }
};

int main()
{
  return Start<Demo>(1280, 720, 4, 6, "Sandbox");
}