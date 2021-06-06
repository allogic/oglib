#version 460 core

/*
* Uniform layouts.
*/

layout (binding = 0) uniform ProjectionUniform
{
  mat4 uProjection;
  mat4 uView;
  mat4 uModel;
};

/*
* Vertex input.
*/

layout (location = 0) in vec3 iPosition;
layout (location = 1) in vec3 iNormal;
layout (location = 2) in vec2 iUv;
layout (location = 3) in vec4 iColor;

/*
* Fragment output.
*/

layout (location = 0) out VertOut
{
  vec4 color;
} vertOut;

/*
* Instanced lambert vertex routines.
*/

void main()
{
  vertOut.color = iColor;
  gl_Position = uProjection * uView * uModel * vec4(iPosition, 1.f);
}