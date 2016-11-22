#version 150

in vec3 pass_Normal;
in vec4 frag_pos; // fragment position in view space
in vec3 incidence_ray; // incidence vector of the sun
in vec2 texcoord; // planet texture coordinates

uniform sampler2D ColorTex;

out vec4 out_Color;

vec3 col_sun = vec3(1.0, 1.0, 0.0);

void main() {
/*   //Blinn-Phong reflection model:

  // ambient:
  // arbitrary factor for good looks
  vec3 light_amb = col_sun * col_planet;

  // diffuse:
  // we need to clamp negative values to avoid lighting the backside
  float lambert = max(dot(normalize(incidence_ray), normalize(pass_Normal)), 0.0);
  vec3 light_dif = col_sun * col_planet * vec3(lambert);
*/
  vec4 texcolor = texture(ColorTex, texcoord);

  // specular:
  vec3 halfway = normalize(incidence_ray + vec3(frag_pos));
  float product = max(dot(normalize(pass_Normal), halfway), 0.0);
  vec3 light_spec = vec3(pow(product, 4));

  out_Color = texcolor + vec4(light_spec, 1.0);


}
