#version 150

in vec3 pass_Normal;
in vec4 frag_pos; // fragment position in view space
in vec3 incidence_ray; // sun position in view space

out vec4 out_Color;

vec3 sun_col = vec3(1.0, 1.0, 0.0);
vec3 planet_col = vec3(0.0, 0.5, 1.0);

void main() {
  // we need to clamp negative values to avoid lighting the backside
  float lambert = max(dot(normalize(incidence_ray), normalize(pass_Normal)), 0.0);
  vec3 diff_light = sun_col * planet_col * vec3(lambert);

  out_Color = vec4(abs(normalize(diff_light)), 1.0);
}
