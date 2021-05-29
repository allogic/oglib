#version 460 core

/*
* Uniform layouts.
*/

layout (binding = 1) uniform ProjectionUniform
{
  mat4 uProjection;
  mat4 uView;
  mat4 uTransform;
};

/*
* Vertex input.
*/

layout (location = 0) in vec3 iPosition;
layout (location = 1) in vec4 iColor;

/*
* Fragment output.
*/

layout (location = 0) out VertOut
{
  vec4 color;
} vertOut;

/*
* Gizmo vertex routines.
*/

void main()
{
  vertOut.color = iColor;
  gl_Position = uProjection * uView * uTransform * vec4(iPosition, 1.f);
}