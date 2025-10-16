#version 400 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aOffset;
layout (location = 2) in vec4 aColor;

out vec4 vertexColor;


void main()
{
    vec3 pos = vec3(aPos.xy + aOffset, aPos.z);
    gl_Position = vec4(pos, 1.0);
    vertexColor = aColor;
}