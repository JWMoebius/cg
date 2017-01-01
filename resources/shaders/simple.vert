#version 150
#extension GL_ARB_explicit_attrib_location : require

// vertex attributes of VAO
layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec2 in_Texcoord;

//Matrix Uniforms as specified with glUniformMatrix4fv
uniform mat4 ModelMatrix;
uniform mat4 NormalMatrix;
uniform int NumPlanet;
uniform vec3 SunViewPos; // position of sun in view space
layout (std140) uniform CameraBlock {
  mat4 ViewMatrix;
  mat4 ProjectionMatrix;
} ;


out vec3 pass_Normal;
out vec4 frag_pos; // pass fragment position in view space
out vec3 incidence_ray; // pass sun position in view space
out vec2 pass_Texcoord;
flat out int pass_NumPlanet;

void main(void)
{
  gl_Position = (ProjectionMatrix  * ViewMatrix * ModelMatrix) * vec4(in_Position, 1.0);
  pass_Normal = (NormalMatrix * vec4(in_Normal, 0.0)).xyz;
  frag_pos = ViewMatrix * ModelMatrix * vec4(in_Position, 1.0);
  // incidence vector from light source to fragment position:
  incidence_ray = SunViewPos - vec3(frag_pos);
  pass_Texcoord = in_Texcoord;
  pass_NumPlanet = NumPlanet;

  //Skybox Exemption
  if(NumPlanet == 11) {
    pass_Normal = -pass_Normal;
  }
}
