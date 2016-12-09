#version 150

in vec2 pass_Texcoord;
in vec4 gl_FragCoord;
out vec4 pass_Color;
uniform sampler2D ColorTex;
uniform float luminance;
uniform float vertical;
uniform float horizontal;
uniform float blur;
void main()
{
    float x_cord = pass_Texcoord.x;
    float y_cord = pass_Texcoord.y;
    bool lum = false;
    bool vert = false;
    bool hor = false;
    bool blu = false;

    pass_Color = texture(ColorTex, pass_Texcoord);
    //Mirror Vertical Modifies x-coordinate
    if (vertical == 1.0) {
        x_cord = 1 - x_cord; //reverses x_coordinate
        pass_Color = texture(ColorTex, vec2(x_cord, y_cord));
    }
    //Mirror Horizontal Modifies x-coordinate
    if (horizontal == 1.0) {
        y_cord = 1 - y_cord; //reverses x_coordinate
        pass_Color = texture(ColorTex, vec2(x_cord, y_cord));
    }
    //Blur using 3x3 cell calculation via texture look-ups from the exercise slides
    if (blur == 1.0) {
        float pixel_size = pass_Texcoord.x / gl_FragCoord.x;
        vec4 tex_1n1 = texture(ColorTex, vec2(x_cord - pixel_size, y_cord + pixel_size  )) * 1/16;
        vec4 tex_1n2 = texture(ColorTex, vec2(x_cord, y_cord + pixel_size               )) * 1/8;
        vec4 tex_1n3 = texture(ColorTex, vec2(x_cord + pixel_size, y_cord + pixel_size  )) * 1/16;
        vec4 tex_2n1 = texture(ColorTex, vec2(x_cord, y_cord - pixel_size               )) * 1/8;
        vec4 tex_2n2 = texture(ColorTex, vec2(x_cord, y_cord                            )) * 1/4;
        vec4 tex_2n3 = texture(ColorTex, vec2(x_cord + pixel_size, y_cord               )) * 1/8;
        vec4 tex_3n1 = texture(ColorTex, vec2(x_cord - pixel_size, y_cord - pixel_size  )) * 1/16;
        vec4 tex_3n2 = texture(ColorTex, vec2(x_cord, y_cord - pixel_size               )) * 1/8;
        vec4 tex_3n3 = texture(ColorTex, vec2(x_cord + pixel_size, y_cord - pixel_size  )) * 1/16;

        pass_Color = tex_1n1 + tex_1n2 + tex_1n3 + tex_2n1 + tex_2n2 + tex_2n3 + tex_3n1 + tex_3n2 + tex_3n3;
        blu = true;

    }
    if (luminance == 1.0) {

            float weight = (0.2126 * pass_Color.r + 0.7152 * pass_Color.g + 0.0722 * pass_Color.b) / 3.0;
            pass_Color = vec4(weight, weight, weight, 1.0);
    }
}