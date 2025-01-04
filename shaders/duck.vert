#version 460 core

// ==================================
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNorm;
layout(location = 2) in vec2 inTexCoord;

out FS_IN
{
    vec3 worldPos;
    vec3 norm;
    vec2 texCoords;
} o;
// ==================================

layout(std140, binding = 0) uniform MatricesBlock {
    mat4 view;
    mat4 projection;
};

uniform mat4 model;

void main()
{
    o.norm =  mat3(transpose(inverse(model))) * inNorm;
    o.texCoords = inTexCoord;
    
    vec4 worldPos = model * vec4(inPos, 1.0f); 
    o.worldPos = worldPos.xyz;

    gl_Position = projection * view * worldPos;
}