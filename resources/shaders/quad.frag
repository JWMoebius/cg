#version 150

in vec2 pass_Texcoord;
out vec4 pass_Color;
uniform sampler2D ColorTex;

void main()
{
    pass_Color = texture(ColorTex, pass_Texcoord);
}