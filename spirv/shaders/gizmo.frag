#version 460 core

/*
* Fragment input.
*/

layout (location = 0) in VertOut
{
  vec4 color;
} fragIn;

/*
* Screen output.
*/

layout (location = 0) out vec4 oColor;

/*
* Gizmo fragment routines.
*/

void main()
{
  oColor = fragIn.color;
}