#include "thicc/VkApi.h"

struct Box : acs::Actor
{
  //Transform*  mpTransform  = nullptr;
  //Renderable* mpRenderable = nullptr;
  //
  //Box(r32v3 const& p)
  //{
  //  mpTransform = Attach<Transform>(this, p, r32v3{ 0, 0, 0 }, r32v3{ 10, 10, 10 });
  //  mpRenderable = Attach<Renderable>(this, nullptr, nullptr);
  //}
};
struct Player : acs::Actor
{
  //Transform*  mpTransform  = nullptr;
  //Renderable* mpRenderable = nullptr;
  //Rigidbody*  mpRigidbody  = nullptr;
  //
  //Player(r32v3 const& p)
  //{
  //  mpTransform = Attach<Transform>(this, p, r32v3{ 0, 0, 0 }, r32v3{ 10, 10, 10 });
  //  mpRenderable = Attach<Renderable>(this, nullptr, nullptr);
  //  mpRigidbody = Attach<Rigidbody>(this, r32v3{ 0, 0, 0 }, 0.f);
  //}
};

struct Demo : Sandbox
{
  Demo()
  {
    //At<ShaderLambert>("shaderBox", { ... }, { ... });
    //At<MeshLambert>("meshBox", { ... }, { ... });
  }
  virtual ~Demo()
  {

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
    //DebugRenderLine(r32v3{ 0, 0, 0 }, r32v3{ 10, 0, 0 }, r32v4{ 1, 0, 0, 1 }, glm::identity<r32m4>());
    //DebugRenderLine(r32v3{ 0, 0, 0 }, r32v3{ 0, 10, 0 }, r32v4{ 0, 1, 0, 1 }, glm::identity<r32m4>());
    //DebugRenderLine(r32v3{ 0, 0, 0 }, r32v3{ 0, 0, 10 }, r32v4{ 0, 0, 1, 1 }, glm::identity<r32m4>());
  }
};

int main()
{
  VkWindow<Demo>{ 1280, 720, "Sandbox", 60, 1 };
  return 0;
}