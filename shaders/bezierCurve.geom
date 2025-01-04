#version 460 core 

layout(lines_adjacency) in;
layout(line_strip, max_vertices = 256) out;

layout(std140, binding = 0) uniform MatricesBlock {
    mat4 view;
    mat4 projection;
};

const float NumberOfPoints = 128.0f;

vec4 DeCastiljau(vec4 controlPoints[4], float t) {
    float onet = 1.0f - t;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3 - i; j++) {
            controlPoints[j] = onet * controlPoints[j] + t * controlPoints[j + 1];  
        }
    }
    return controlPoints[0];
}

void GenerateBezierCurve(vec4 controlPoints[4]) {
    float t = 0;
    float dt = 1.0f / (NumberOfPoints);

    while (t <= 1.0f) 
    {
        gl_Position = DeCastiljau(controlPoints, t);
        EmitVertex();

        t += dt; 
    } 

    EndPrimitive();
}

void main() 
{
    // vec4 p0 = gl_in[0].gl_Position;
    // vec4 p1 = gl_in[1].gl_Position;
    // vec4 p2 = gl_in[2].gl_Position;
    // vec4 p3 = gl_in[3].gl_Position;

    GenerateBezierCurve(vec4[4](
        gl_in[0].gl_Position, 
        gl_in[1].gl_Position, 
        gl_in[2].gl_Position, 
        gl_in[3].gl_Position));
}