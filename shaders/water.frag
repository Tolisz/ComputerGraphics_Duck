#version 460 core 

#define MAX_NUM_OF_LIGHTS 5

// =========================================
in FS_IN
{
    vec3 worldPos;
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
uniform vec3 objectColor;

layout(binding = 0) uniform sampler2D normalTex;
layout(binding = 1) uniform samplerCube cubeMap;

vec3 Phong(vec3 worldPos, vec3 norm, vec3 view)
{
    vec3 N = normalize(norm);
    vec3 V = normalize(view);

    // ambient 
    vec3 intensity = material.ka * ambientColor.rgb;

    for (int i = 0; i < min(numberOfLights, MAX_NUM_OF_LIGHTS); i++) {
        // diffuse 
        vec3 L = normalize(light[i].position.xyz - worldPos);
        intensity += material.kd * light[i].diffuseColor.rgb * max(dot(N, L), 0.0f);

        // specular
        vec3 R = reflect(-L, N);
        intensity += material.ks * light[i].specularColor.rgb * pow(max(dot(R, V), 0.0f), material.shininess);
    }

    return intensity;
}

float fresnel(vec3 N, vec3 V, float n1, float n2)
{
    float n12 = (n2 - n1) / (n2 + n1);
    float F0 = n12 * n12;
    float cosTheta = max(dot(N, V), 0.0f);
    
    return F0 + (1 - F0) * pow(1.0f - cosTheta, 5.0f);
}

vec3 intersectRay(vec3 p, vec3 d)
{
    vec3 one = vec3(1.0f, 1.0f, 1.0f);

    vec3 t1 = (one - p) / d;
    vec3 t2 = (-one - p) / d;
    
    vec3 tm = max(t1, t2);
    float t = min(min(tm.x, tm.y), tm.z);
    
    return p + t * d;
}

void main()
{   
    //FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    //return;
    // Compute color of water based on reflectance and refraction
    // ==========================================================
    vec3 norm = normalize(texture(normalTex, i.texCoords).xyz);
    vec3 view = normalize(cameraPos - i.worldPos);

    float n1 = 1.0f; // air
    float n2 = 4.0f / 3.0f; // water

    bool isUnderWater = (dot(norm, view) < 0);  
    float n = !isUnderWater ? n1 / n2 : n2 / n1;
    if (isUnderWater) norm *= -1.0f;

    // compute reflect and refract vector
    vec3 reflectVec = reflect(-view, norm);
    vec3 refractVec = refract(-view, norm, n);

    vec3 texReflectCoord = intersectRay(i.worldPos, reflectVec);
    vec3 texRefractCoord = intersectRay(i.worldPos, refractVec);
    vec3 reflectColor = vec3(texture(cubeMap, texReflectCoord));
    vec3 refractColor = vec3(texture(cubeMap, texRefractCoord));

    float f = fresnel(norm, view, n1, n2);
    vec3 color = refractVec != vec3(0.0) ? (1 - f) * refractColor + f * reflectColor : reflectColor;
    
    // Compute Phong's color of water 
    // =======================================================
    vec3 intensity = Phong(i.worldPos, norm * (isUnderWater ? -1.0f : 1.0f), view);
    vec3 waterColor = intensity * objectColor;

    // FragColor = vec4(refractColor + waterColor * 0.1f, 1.0f);
    FragColor = vec4(color + waterColor * 0.1f, 1.0f);
}