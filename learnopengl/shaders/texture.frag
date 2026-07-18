#version 460 core
out vec4 FragColor;

in vec2 uv;
in vec3 normal;

uniform sampler2D diffuse1;
uniform sampler2D diffuse2;

uniform vec3 ambientColor;
// uniform float ambientColorIntensity; -- could be useful in the future
uniform vec3 lightPos1;
uniform vec3 lightPost2;
uniform vec3 lightColor1;
uniform vec3 lightColor2;


void main()
{
    vec3 ambient = ambientColor * /*ambientColorIntensity*/ 0.2;
    vec3 textureColor = mix(texture(diffuse1, uv).rgb, texture(diffuse2, uv).rgb, 0.6);
    FragColor = vec4((ambient * textureColor), 1.);
}