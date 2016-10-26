#version 150
#extension GL_ARB_explicit_attrib_location : require

layout(location = 0) in vec3 in_position;

out vec3 pass_position;

void main(void)
{
  //Simply convert to homogeneous coordinates
  gl_Position = vec4(in_position, 1.0);
  gl_PointSize = 1.0; //Our stars are all the same size.
  pass_position = in_position;
}
