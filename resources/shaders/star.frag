#version 150

in vec4 pass_position;
out vec3 out_Color;

void main()
{
  out_Color = vec3(pass_position);
}