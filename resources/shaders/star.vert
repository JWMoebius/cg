#version 150
#extension GL_ARB_explicit_attrib_location : require

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;
layout (std140) uniform CameraBlock {
  mat4 ViewMatrix;
  mat4 ProjectionMatrix;
} ;
uniform mat4 ModelMatrix;

out vec3 pass_color;

void main(void)
{
  // Convert position to homogeneous coordinates and apply projection and view.
  gl_Position = (ProjectionMatrix * ViewMatrix * ModelMatrix) * vec4(in_position, 1.0);
  gl_PointSize = 1.0; //Our stars are all the same size.

  pass_color = in_color;
}
