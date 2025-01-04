#version 460 core

#define M_PI 3.1415926535897932384626433832795

// ==================================
in VS_OUT
{
    vec2 SquarePos; /* All interpolated values are inside the square [-1, 1] x [-1, 1] */
} i;

layout(location = 0) out vec4 FragColor;
// ==================================

uniform vec3 colorDiffuse;  /* Color of the inner and outer circles */
uniform vec3 colorSpecular; /* Color of the dot inside the billboard */

// [BEGIN] PARAMETERS
const float c_r = 0.35; /* Radius of the dot inside the billboard */
const float c_R1 = 0.60; /* Radius of the inner circle, where its saturation is maximum */
const float c_R2 = 0.85; /* Radius of the outer circle, where its saturation is maximum */
const float c_dR1 = 0.15; /* Radius delta of the inner circle. */
const float c_dR2 = 0.10; /* Radius delta of the outer circle. */
const float c_N = 14; /* Number of segments on which the billboard will be devided. Only even segments of the inner circle are drawn*/
const float c_earlySatA = 2.0f;  /* See genSegment function */
// [END] PARAMETERS

float genCircle(float R, float dR, float l)
{
    float t = 0.0f;
    t += (1 - smoothstep(R, R + dR, l)) * step(R, l);
    t += smoothstep(R - dR, R, l) * step(l, R);
    return t;
}

float genSegment(float A, float dA, float alpha)
{
    float earlySaturate = dA / c_earlySatA;  // must be between [0.0f, dA)
    float t = 0.0f;
    t += (1 - smoothstep(A + earlySaturate, A + dA, alpha)) * step(A, alpha);
    t += smoothstep(A - dA, A - earlySaturate, alpha) * step(alpha, A);
    return t;    
}

vec4 lightColorer()
{
    // Compute separate colors for bilboard elements
    // ==============================================

    vec4 spec = vec4(colorSpecular, 1.0f);
    vec4 diffInner = vec4(colorDiffuse, 1.0f);
    vec4 diffOuter = vec4(colorDiffuse, 1.0f);
    float l = length(i.SquarePos);

    // central point color
    float ts = 1 - smoothstep(0, c_r, l);
    spec.a *= ts;

    float td1 = genCircle(c_R1, c_dR1, l); // inner circle color
    float td2 = genCircle(c_R2, c_dR2, l); // outer circle color

    // delta alpha - angle of one segment. 
    float da = (2.0f * M_PI) / c_N;  
    // alpha - angle from Up Y axis to Right X axis of a fragment
    float a = atan(-i.SquarePos.x, -i.SquarePos.y); 
    a += M_PI;  // from [-PI, +PI] to [0, 2 * PI]
    a = mod(a + da / 2, 2 * M_PI); // Rotate da/2 left

    // betta - angle which defines an enlargement angle of a drawn segment
    float b = da / 2.0f;  

    float begin = floor(a / da);
    float beginR = floor((a + b) / da); // If we go right (clockwise order) are we in drawn segment
    float beginL = floor((a - b) / da); // If we go left (counterclockwise order) are we in drawn segment
    
    bool isEven = mod(begin, 2.0f) == 0.0f; 
    bool isEvenR = mod(beginR, 2.0f) == 0.0f;
    bool isEvenL = mod(beginL, 2.0f) == 0.0f;
    
    float end = ceil(a / da); 
    float center_a = ((begin + end) * da) / 2.0f;
    
    float ta1 = isEven  ? genSegment(center_a, da / 2.0f + b, a) : 
                isEvenR ? genSegment(center_a + da, da / 2.0f + b, a) : 
                isEvenL ? genSegment(center_a - da, da / 2.0f + b, a) : 0.0f;

    diffInner.a *= ta1 * td1; // inner circle is devided into segments
    diffOuter.a *= td2; // outer circle is drawn in full

    // Alpha blending 
    // ==============

    vec4 color = vec4(0.0f);
    color.a = spec.a + diffInner.a + diffOuter.a; 
    color.rgb += step(diffInner.a, 0.0f) * step(diffOuter.a, 0.0f) * spec.rgb; 
    color.rgb += step(spec.a, 0.0f) * step(diffOuter.a, 0.0f) * diffInner.rgb; 
    color.rgb += step(spec.a, 0.0f) * step(diffInner.a, 0.0f) * diffOuter.rgb;

    return color;
}

void main()
{
    FragColor = lightColorer();
}