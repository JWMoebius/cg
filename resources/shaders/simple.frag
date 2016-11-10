#version 150

in vec3 pass_Normal;
in vec4 frag_pos; // fragment position in view space
in vec3 incidence_ray; // sun position in view space
in vec3 col_planet; // planet color

out vec4 out_Color;

vec3 col_sun = vec3(1.0, 1.0, 0.0);
// vec3 col_planet = vec3(0.0, 0.5, 1.0);

void main() {
  // we need to clamp negative values to avoid lighting the backside
  float lambert = max(dot(normalize(incidence_ray), normalize(pass_Normal)), 0.0);
  vec3 light_dif = col_sun * col_planet * vec3(lambert);

  out_Color = vec4(abs(normalize(light_dif)), 1.0);
}
