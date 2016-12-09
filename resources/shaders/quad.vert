#version 150
#extension GL_ARB_explicit_attrib_location : require

layout(location = 0) in vec3 in_Position;
layout(location = 1) in int in_post_process;
out vec2 pass_Texcoord;
out int out_post_process;
void main()
{
    pass_Texcoord = vec2(in_Position.x, in_Position.y);
    gl_Position = vec4(in_Position, 1.0);
    out_post_process = in_post_process;
}