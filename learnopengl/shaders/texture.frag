#version 460 core
out vec4 FragColor;

in vec2 UV;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 viewPos;

uniform sampler2D diffuse0;
uniform sampler2D diffuse1;

uniform vec3 ambientColor;
// uniform float ambientColorIntensity; -- could be useful in the future
uniform vec3 lightPos0;
uniform vec3 lightPos1;
uniform vec3 lightColor0;
uniform vec3 lightColor1;

// TODO: a lot of copy-paste code for multiple light sources
void main()
{
    float ambientColorIntensity = 0.01;
    float specularStrength = 0.5;
    int shininess = 32;

    // ambient lighting
    // TODO: it's precomputed now, but do we want it to be calculated here?
    vec3 ambient = ambientColor * ambientColorIntensity;

    // diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir0 = normalize(lightPos0 - FragPos);
    vec3 lightDir1 = normalize(lightPos1 - FragPos);

    vec3 diff0 = max(dot(norm, lightDir0), 0.) * lightColor0;
    vec3 diff1 = max(dot(norm, lightDir1), 0.) * lightColor1;

    // specular lighting (Blinn Phong)
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 halfwayDir0 = normalize(lightDir0 + viewDir);
    float spec0 = pow(max(dot(norm, halfwayDir0), 0.), shininess);
    vec3 specular0 = lightColor0 * spec0;

    vec3 halfwayDir1 = normalize(lightDir1 + viewDir);
    float spec1 = pow(max(dot(norm, halfwayDir1), 0.), shininess);
    vec3 specular1 = lightColor1 * spec1;

    vec3 light = ambient + diff0 + diff1 + specular0 + specular1;

    vec3 textureColor = mix(texture(diffuse1, UV).rgb, texture(diffuse0, UV).rgb, 0.6);
    FragColor = vec4((light * textureColor), 1.);
}