#version 460 core 

/* Uncomment it if you want to test how fragment shader works */
//#define TEST_FRAGMENT_SHADER

// =========================================
layout(location = 0) in vec3 iSquarePos;

out VS_OUT
{
    vec2 SquarePos;
} o;
// =========================================

layout(std140, binding = 0) uniform MatricesBlock {
    mat4 view;
    mat4 projection;
};

//uniform mat4 view;
//uniform mat4 projection;

uniform vec3 billboardPos;  /* billboard's world position. */
uniform ivec2 screenSize;   /* screen size in pixels */

#ifndef TEST_FRAGMENT_SHADER
    const ivec2 billboardSize = ivec2(25, 25); /* billboard's size in pixels */
#else
    const ivec2 billboardSize = ivec2(400, 400);
#endif

void main()
{
    // compute billboard's center in screen space 
    vec4 projBillboardPos = projection * view * vec4(billboardPos, 1.0f);
    projBillboardPos /= projBillboardPos.w;   

    // move billboard's center torwards the point iSquarePos
    // also take into accound the size of the billboard in screen space  
    projBillboardPos.xy += iSquarePos.xy * billboardSize / vec2(screenSize);     
    gl_Position = projBillboardPos;

    // Interpolate squarePos for fragment shader computations
    o.SquarePos = iSquarePos.xy;
}