#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

layout (location = 3) in vec3 aColorAmb;
layout (location = 4) in vec3 aColorDif;
layout (location = 5) in vec3 aColorSpec;
layout (location = 6) in float aShine;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat4 uNormalMat;

out vec3 vFragPos;
out vec3 vNormal;

out vec2 vTexCoord;

out vec3 vAmb;
out vec3 vDif;
out vec3 vSpec;
out float vShine;

void main()
{
    vec4 worldPos = uModel * vec4(aPos, 1.0f);
    vFragPos = worldPos.xyz;
    vNormal = normalize(uNormalMat * aNormal);
    vTexCoord = aTexCoord;

    vAmb = aColorAmb;
    vDif = aColorDif;
    vSpec = aColorSpec;
    vShine = aShine;

    gl_Position = uProjection * uView * worldPos;
}