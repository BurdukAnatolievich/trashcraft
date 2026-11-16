#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform sampler2D texture_atlas;
uniform float shift;

void main()
{
    vec2 actual_uv = fs_in.TexCoords + vec2(shift, 0.0);
    actual_uv /= vec2(32.0);

    vec4 color = texture2D(texture_atlas, actual_uv).rgba;

    FragColor = color;
}
