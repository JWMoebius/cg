#version 150
#extension GL_ARB_separate_shader_objects : enable
layout(location = 0) in vec3 pass_Normal;
layout(location = 1) in vec4 pass_FragPos; // fragment position in view space
layout(location = 2) in vec3 pass_LightPos; // sun position in view space

out vec4 out_Color;

void main() {
  out_Color = vec4(abs(normalize(pass_Normal)), 1.0);
}
