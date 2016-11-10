#version 150
#extension GL_ARB_explicit_attrib_location : require

// vertex attributes of VAO
layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;

//Matrix Uniforms as specified with glUniformMatrix4fv
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 NormalMatrix;
uniform vec3 Color;
uniform vec3 SunViewPos; // position of sun in view space

out vec3 pass_Normal;
out vec4 frag_pos; // pass fragment position in view space
out vec3 incidence_ray; // pass sun position in view space
out vec3 col_planet;

void main(void)
{
	gl_Position = (ProjectionMatrix  * ViewMatrix * ModelMatrix) * vec4(in_Position, 1.0);
	pass_Normal = (NormalMatrix * vec4(in_Normal, 0.0)).xyz;
  frag_pos = ViewMatrix * ModelMatrix * vec4(in_Position, 1.0);
  // incidence vector from light source to fragment position:
  incidence_ray = SunViewPos - vec3(frag_pos);
  col_planet = Color;
}
