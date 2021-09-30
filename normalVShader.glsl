#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;  

out vec2 TexCoords;
out vec3 v_texCoord3D;


out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
    mat3 TBN;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;

uniform vec3 viewPos;
uniform float time;
uniform float tremor;
uniform float frequency;


void main()
{
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
    vs_out.TexCoords = aTexCoords;
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
    vec3 T = normalize(vec3(model * vec4(aTangent,   0.0)));
    vec3 B = normalize(vec3(model * vec4(aBitangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(aNormal,    0.0)));
    mat3 TBN = mat3(T, B, N);
    vs_out.TBN = transpose(mat3(T, B, N));
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    v_texCoord3D = aPos;
    vec4 offset = sin(time + gl_Position * frequency) * tremor;
    vec3 viewDir = normalize(viewPos - vs_out.FragPos);
    vec3 normal = normalize(vs_out.Normal);
    gl_Position = gl_Position + offset * tremor *(dot(normal, viewDir));
}


