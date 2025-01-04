#version 460 core 

// =========================================
in FS_IN
{
    vec3 texCoords;
} i;

layout(location = 0) out vec4 FragColor;
// =========================================

layout(binding = 0) uniform samplerCube skyBox;

void main() 
{
    FragColor = texture(skyBox, i.texCoords);
    // FragColor = vec4(1.0f, 0.0f, 1.0f, 1.0f);
}