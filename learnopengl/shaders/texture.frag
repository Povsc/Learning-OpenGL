#version 460 core
out vec4 FragColor;

in vec2 uv;

uniform sampler2D diffuse1;
uniform sampler2D diffuse2;

void main()
{
    FragColor = mix(texture(diffuse1, uv), texture(diffuse2, uv), 0.6);
}