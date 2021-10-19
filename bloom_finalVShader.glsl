#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 TangentViewPos;
out vec3 TangentFragPos;
out vec3 viewDir1;

uniform mat3 TBN1;
uniform mat3 TBN2;
uniform vec3 viewPos;


void main()
{
    mat3 TBN;
    if((aPos.x * (-0.505) + 50) < aPos.y)
        TBN = TBN1;
    else
        TBN = TBN2;
    TangentViewPos  = TBN * viewPos;
    TangentFragPos  = TBN * vec3(vec4(aPos, 1.0));
    viewDir1 = normalize(viewPos - aPos);
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}