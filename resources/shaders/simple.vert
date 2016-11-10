#version 150
#extension GL_ARB_explicit_attrib_location : require
#extension GL_ARB_separate_shader_objects : enable
// vertex attributes of VAO
layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;

//Matrix Uniforms as specified with glUniformMatrix4fv
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 NormalMatrix;
uniform vec3 SunViewPos; // position of sun in view space

layout(location = 0) out vec3 pass_Normal;
layout(location = 1) out vec4 pass_FragPos; // pass fragment position in view space
layout(location = 2) out vec3 pass_LightPos; // pass sun position in view space

void main(void)
{
	gl_Position = (ProjectionMatrix  * ViewMatrix * ModelMatrix) * vec4(in_Position, 1.0);
	pass_Normal = (NormalMatrix * vec4(in_Normal, 0.0)).xyz;
  pass_FragPos = ViewMatrix * ModelMatrix * vec4(in_Position, 1.0);
  pass_LightPos = SunViewPos;
}
