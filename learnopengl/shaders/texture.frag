#version 460 core
out vec4 FragColor;

in vec2 UV;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D diffuse1;
uniform sampler2D diffuse2;

uniform vec3 ambientColor;
// uniform float ambientColorIntensity; -- could be useful in the future
uniform vec3 lightPos1;
uniform vec3 lightPos2;
uniform vec3 lightColor1;
uniform vec3 lightColor2;


void main()
{
    // ambient lighting
    // TODO: it's precomputed now, but do we want it to be calculated here?
    vec3 ambient = ambientColor * /*ambientColorIntensity*/ 0.01;

    // diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir1 = normalize(FragPos - lightPos1);
    vec3 lightDir2 = normalize(FragPos - lightPos2);

    vec3 diff1 = max(dot(norm, lightDir1), 0.) * lightColor1;
    vec3 diff2 = max(dot(norm, lightDir2), 0.) * lightColor2;

    vec3 light = ambient + diff1 + diff2;

    vec3 textureColor = mix(texture(diffuse1, UV).rgb, texture(diffuse2, UV).rgb, 0.6);
    FragColor = vec4((light * textureColor), 1.);
}