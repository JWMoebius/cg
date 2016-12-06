#version 150

in vec3 pass_Normal;
in vec4 frag_pos; // fragment position in view space
in vec3 incidence_ray; // incidence vector of the sun
in vec2 pass_Texcoord; // planet texture coordinates
flat in int pass_NumPlanet;

uniform sampler2D ColorTex; // index of texture unit

out vec4 out_Color;
vec4 col_sun = vec4(1.0, 1.0, 1.0, 1.0);

void main() {
  vec4 texcolor = texture(ColorTex, pass_Texcoord);

  // ambient:
  vec4 light_amb = col_sun * texcolor;

  // diffuse:
  // we need to clamp negative values to avoid lighting the backside
  float lambert = max(dot(normalize(incidence_ray), normalize(pass_Normal)), 0.0);
  vec4 light_dif = col_sun * texcolor * vec4(lambert);

  // specular:
  vec3 halfway = normalize(incidence_ray + vec3(frag_pos));
  float product = max(dot(normalize(pass_Normal), halfway), 0.0);
  vec4 light_spec = vec4(pow(product, 4));

  out_Color = light_amb + light_dif + light_spec;
}
