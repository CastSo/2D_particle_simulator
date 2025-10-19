#version 400 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in mat4 aTransform;
layout (location = 5) in vec4 aColor;

out vec4 vertexColor;


void main()
{
    gl_Position = aTransform * vec4(aPos, 1.0);
    vertexColor = aColor;
}