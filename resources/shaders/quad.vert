#version 150
#extension GL_ARB_explicit_attrib_location : require

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_texcoords;
out vec2 pass_Texcoord;

void main()
{
    pass_Texcoord = vec2(in_texcoords.x, in_texcoords.y);
    gl_Position = vec4(in_Position, 1.0);
}