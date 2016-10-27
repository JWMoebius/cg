#version 150
#extension GL_ARB_explicit_attrib_location : require

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;

uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

out vec3 pass_color;

void main(void)
{
  //Simply convert to homogeneous coordinates
  gl_Position = (ProjectionMatrix * ViewMatrix) * vec4(in_position, 1.0);
  gl_PointSize = 1.0; //Our stars are all the same size.
  pass_color = in_color;
}
