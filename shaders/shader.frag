#version 140

in vec2 uv;
in vec4 particlecolor;

out vec4 fragColor;

uniform sampler2D sampler;

void main()
{
    vec4 texCol = texture(sampler, uv);
    texCol.a *= particlecolor.a;
    fragColor = texCol;
}
