#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 normal;

out vec2 uv;
out vec3 Normal;

uniform mat4 MVP;
uniform mat4 model;

uniform

void main()
{
    gl_Position = MVP * vec4(aPos, 1.0);
    uv = aTexCoord;
}