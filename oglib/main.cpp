#include <iostream>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

import type;
import glfw;
import acs;
import component;
import render;

u32 Width  = 720;
u32 Height = 360;

struct Box : Actor
{
  Transform*  mpTransform  = nullptr;
  Renderable* mpRenderable = nullptr;

  Box(r32v3 const& p)
  {
    mpTransform = Attach<Transform>(this, p, r32v3{ 0, 0, 0 }, r32v3{ 10, 10, 10 });
    mpRenderable = Attach<Renderable>(this, nullptr, nullptr);
  }
};
struct Player : Actor
{
  Transform*  mpTransform  = nullptr;
  Camera*     mpCamera     = nullptr;
  Renderable* mpRenderable = nullptr;
  Rigidbody*  mpRigidbody  = nullptr;

  Player(r32v3 const& p)
  {
    mpTransform = Attach<Transform>(this, p, r32v3{ 0, 0, 0 }, r32v3{ 1, 1, 1 });
    mpCamera = Attach<Camera>(this, glm::radians(45.f), 0.001f, 1000.f);
    mpRenderable = Attach<Renderable>(this, nullptr, nullptr);
    mpRigidbody = Attach<Rigidbody>(this, r32v3{ 0, 0, 0 }, 0.f);
  }
};

struct Demo : Sandbox
{
  Demo()
  {
    std::cout << "Demo created\n";

    Create<Box>("Box0", r32v3{ 0, 0, 0 });
    Create<Box>("Box1", r32v3{ 0, 0, 0 });
    Create<Player>("Player", r32v3{ 0, 0, -10 });

    int i = 0;
    Dispatch<Transform>([&i](Transform* pTransform)
      {
        std::cout << i++ << ":Transform:" << pTransform << std::endl;
      });
    Dispatch<Transform, Renderable>([&i](Transform* pTransform, Renderable* pRenderable)
      {
        std::cout << i++ << ":Transform:" << pTransform << ":Renderable:" << pRenderable << std::endl;
      });
    Dispatch<Transform, Rigidbody>([&i](Transform* pTransform, Rigidbody* pRigidbody)
      {
        std::cout << i++ << ":Transform:" << pTransform << ":Rigidbody:" << pRigidbody << std::endl;
      });
    Dispatch<Transform, Renderable, Rigidbody>([&i](Transform* pTransform, Renderable* pRenderable, Rigidbody* pRigidbody)
      {
        std::cout << i++ << ":Transform:" << pTransform << ":Renderable:" << pRenderable << ":Rigidbody:" << pRigidbody << std::endl;
      });
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
  void OnDebug(r32 time) const override
  {
    // TODO: test gizmo rendering & queues
    DebugRenderLine(r32v3{ 0, 0, 0 }, r32v3{ 10, 0, 0 }, r32v4{ 1, 0, 0, 1 }, glm::identity<r32m4>());
    DebugRenderLine(r32v3{ 0, 0, 0 }, r32v3{ 0, 10, 0 }, r32v4{ 0, 1, 0, 1 }, glm::identity<r32m4>());
    DebugRenderLine(r32v3{ 0, 0, 0 }, r32v3{ 0, 0, 10 }, r32v4{ 0, 0, 1, 1 }, glm::identity<r32m4>());
  }
};

int main()
{
  return Start<Demo>(1280, 720, 4, 6, "Sandbox");
}