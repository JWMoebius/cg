#version 150

in vec3 pass_color;
out vec3 out_Color;

void main()
{
  // color is dependent on position of star
  out_Color = pass_color;
}