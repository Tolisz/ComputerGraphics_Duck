#version 460 core

#define MAX_NUM_OF_LIGHTS 5

// =========================================
in FS_IN
{
    vec3 worldPos;
    vec3 norm;
    vec2 texCoords;
} i;

layout(location = 0) out vec4 FragColor;
// =========================================

struct Light
{
    vec4 position;
    vec4 diffuseColor;
    vec4 specularColor;
};

struct Material
{
    vec3 ka, kd, ks;
    float shininess;
};

layout(std140, binding = 1) uniform LightsBlock {
    vec4 ambientColor;
    Light light[MAX_NUM_OF_LIGHTS];
};
uniform int numberOfLights;

uniform Material material;
uniform vec3 cameraPos;

uniform float anisoShininess;
uniform float anisoA;
uniform float anisoB;

layout(binding = 2) uniform sampler2D duckTex; 

subroutine vec3 LightFunction(vec3 worldPos, vec3 norm, vec3 tang, vec3 bitang, vec3 view, vec3 surfaceColor);
subroutine uniform LightFunction lightFun;

subroutine (LightFunction)
vec3 Phong(vec3 worldPos, vec3 norm, vec3 tang, vec3 bitang, vec3 view, vec3 surfaceColor)
{
    vec3 N = normalize(norm);
    vec3 V = normalize(view);

    // ambient 
    vec3 color = material.ka * ambientColor.rgb;

    for (int i = 0; i < min(numberOfLights, MAX_NUM_OF_LIGHTS); i++) {
        // diffuse 
        vec3 L = normalize(light[i].position.xyz - worldPos);
        color += material.kd * light[i].diffuseColor.rgb * surfaceColor * max(dot(N, L), 0.0f);

        // specular
        vec3 R = reflect(-L, N);
        color += material.ks * light[i].specularColor.rgb * pow(max(dot(R, V), 0.0f), material.shininess);
    }

    return color;
}

subroutine (LightFunction) 
vec3 PhongAnisotropic(vec3 worldPos, vec3 norm, vec3 tang, vec3 bitang, vec3 view, vec3 surfaceColor)
{
    vec3 V = normalize(view);
    vec3 N = normalize(norm);
    vec3 T = normalize(tang);
    vec3 B = normalize(bitang);
    mat3 TBN = mat3(T, B, N);
    vec3 dir = normalize(vec3(anisoA, 0.0f, anisoB));

    // ambient 
    vec3 color = material.ka * ambientColor.rgb;

    for (int i = 0; i < min(numberOfLights, MAX_NUM_OF_LIGHTS); i++) {
        // diffuse 
        vec3 L = normalize(light[i].position.xyz - worldPos);
        color += material.kd * light[i].diffuseColor.rgb * surfaceColor * max(dot(N, L), 0.0f);

        // specular
        vec3 H = normalize(L + V);
        vec3 HT = normalize(TBN * H);

        float cos_b = sqrt(1 - pow(dot(TBN * dir, HT), 2.0f));
        color += material.ks * light[i].specularColor.rgb * pow(cos_b, anisoShininess);
    }

    return color;
}

void main()
{
    // Compute T, B, N
    vec3 N = normalize(i.norm);
    vec3 dPdx = dFdxFine(i.worldPos);
    vec3 dPdy = dFdyFine(i.worldPos);
    vec2 dtdx = dFdxFine(i.texCoords);
    vec2 dtdy = dFdyFine(i.texCoords);
    vec3 T = normalize(-dPdx * dtdy.y + dPdy * dtdx.y);
    vec3 B = normalize(cross(N, T));
    T = cross(B, N);

    vec3 view = normalize(cameraPos - i.worldPos);
    vec3 duckColor = texture(duckTex, i.texCoords).xyz;

    vec3 color = lightFun(i.worldPos, N, T, B, view, duckColor);

    FragColor = vec4(color, 1.0f);
}