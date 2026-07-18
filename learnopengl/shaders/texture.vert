#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;


out vec2 UV;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 MVP;
uniform mat4 model;

void main()
{
    gl_Position = MVP * vec4(aPos, 1.0);
    UV = aTexCoord;

    // This is somethinng to do with how tFragPos is calculated
    // i.e. the scaling implicitly happening when multiplying by model
    // TODO: fully uderstand this
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    Normal = normalMatrix * -aNormal;
    FragPos = vec3(model * vec4(aPos, 1.));
}