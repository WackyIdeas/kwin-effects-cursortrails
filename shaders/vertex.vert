#version 330
#extension GL_ARB_explicit_attrib_location : enable
layout(location = 0) in vec4 vertex;
layout(location = 1) in vec4 instancePosAndSize;
layout(location = 2) in vec4 instanceColor;

uniform mat4 modelViewProjectionMatrix;

out vec2 uv;
out vec4 particlecolor;

void main(void)
{
    float scaleW = instancePosAndSize.z;
    float scaleH = instancePosAndSize.w;

    vec2 pos = vec2(vertex.x * scaleW, vertex.y * scaleH) + instancePosAndSize.xy;

    gl_Position = modelViewProjectionMatrix * vec4(pos, 0.0, 1.0);
    uv = vertex.zw;
    particlecolor = instanceColor;
}
