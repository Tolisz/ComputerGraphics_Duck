#version 460 core

// =========================================
layout(location = 0) in vec3 iPos;

out FS_IN
{
    vec3 texCoords;
} o;
// =========================================

layout(std140, binding = 0) uniform MatricesBlock {
    mat4 view;
    mat4 projection;
};

void main()
{
    o.texCoords = iPos;
    gl_Position = projection * view * vec4(iPos, 1.0f);    
}