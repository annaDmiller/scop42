#version 330 core

in vec3 vFragPos;
in vec3 vNormal;

in vec2 vTexCoord;

in vec3 vAmb;
in vec3 vDif;
in vec3 vSpec;
in float vShine;

uniform vec3 uLightDir;
uniform vec3 uLightColor;
uniform vec3 uViewPos;
uniform vec3 uAmbLightColor;

out vec4 fragColor;

void main()
{
    vec3 normal = normalize(vNormal);
    vec3 light = normalize(vLightDir);
    vec3 view = normalize(uViewPos - vFragPos);

    vec3 ambient = vAmb * uAmbLightColor;

    float difFactor = max(dot(normal, light), 0.0);
    vec3 diffuse = vDif * uLightColor * difFactor;

    vec3 ref = reflect(-light, normal);
    float specFactor = 0.0;
    if (difFactor > 0.0)
    {
        specFactor = pow(max(dot(ref, view), 0.0), vShine);
    }

    vec3 specular = vSpec * uLightColor * specFactor;

    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result, 1.0);
}