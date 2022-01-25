#version 450

#extension GL_ARB_shader_storage_buffer_object : require

layout(location = 0) in vec2 inFragUV;
layout(location = 1) in vec3 inRayOrigin;
layout(location = 2) in vec3 inRayDirection;

layout(location = 0) out vec4 outColor;

struct stroke_t
{
    vec4 param0;    // position.xyz, blend.a
    vec4 param1;    // size.xyz, radius.x, round.w
    ivec4 id;       // shape.x, flags.y
};

layout(std430, binding = 0) buffer strokes_buffer
{
    stroke_t strokes[];
};

layout(location = 1) uniform uint uStrokesNum;

struct ray_t 
{
    vec3 pos;
    vec3 dir;
};

// polynomial smooth min
//float sminCubic(float a, float b, float k)
//{
//    float h = max(k - abs(a - b), 0.0) / k;
//    return min(a, b) - h * h * h * k * (1.0 / 6.0);
//}

// - SMOOTH OPERATIONS --------------------------
// https://www.shadertoy.com/view/lt3BW2
float opSmoothUnion(float d1, float d2, float k)
{
    float h = max(k - abs(d1 - d2), 0.0);
    return min(d1, d2) - h * h * 0.25 / k;
    //float h = clamp( 0.5 + 0.5*(d2-d1)/k, 0.0, 1.0 );
    //return mix( d2, d1, h ) - k*h*(1.0-h);
}

float opSmoothSubtraction(float d1, float d2, float k)
{
    float h = max(k - abs(-d1 - d2), 0.0);
    return max(-d1, d2) + h * h * 0.25 / k;
    //float h = clamp( 0.5 - 0.5*(d2+d1)/k, 0.0, 1.0 );
    //return mix( d2, -d1, h ) + k*h*(1.0-h);
}

float opSmoothIntersection(float d1, float d2, float k)
{
    float h = max(k - abs(d1 - d2), 0.0);
    return max(d1, d2) + h * h * 0.25 / k;
    //float h = clamp( 0.5 - 0.5*(d2-d1)/k, 0.0, 1.0 );
    //return mix( d2, d1, h ) + k*h*(1.0-h);
}

// - SDFS -------------------------
float sdEllipsoid(vec3 p, vec3 r)
{
    float k0 = length(p / r);
    float k1 = length(p / (r * r));
    return k0 * (k0 - 1.0) / k1;
}

float sdRoundBox(vec3 p, vec3 b, float r)
{
    vec3 q = abs(p) - b;
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0) - r;
}

float sdTorus(vec3 p, vec2 t)
{
    vec2 q = vec2(length(p.xz) - t.x, p.y);
    return length(q) - t.y;
}

float sdVerticalCapsule(vec3 p, float h, float r)
{
    p.y -= clamp(p.y, 0.0, h);
    return length(p) - r;
}

// - STROKE EVALUATION --------------
float evalStroke(vec3 p, in stroke_t stroke)
{
    float shape = 1000000.0;
    vec3 position = p - stroke.param0.xyz;

    if (stroke.id.x == 0)
    {
        shape = sdEllipsoid(position, stroke.param1.xyz);
    }
    else if (stroke.id.x == 1)
    {
        float round = clamp(stroke.param1.w, 0.0, 1.0);
        float smaller = min(min(stroke.param1.x, stroke.param1.y), stroke.param1.z);
        round = mix(0.0, smaller, round);
        shape = sdRoundBox(position, stroke.param1.xyz - round, round);
    }
    else if (stroke.id.x == 2)
    {
        shape = sdTorus(position, stroke.param1.xy);
    }
    else if (stroke.id.x == 3)
    {
        vec2 params = max(stroke.param1.xy, vec2(0.0, 0.0));
        shape = sdVerticalCapsule(position - vec3(0.0, -params.x * 0.5, 0.0), params.x, params.y);
    }
    
    return shape;
}

//Distance to scene at point
float distToScene(vec3 p) 
{
    //float a = length(p - vec3(0.0, 0.0, -1.0)) - 0.3;
    //float b = length(p - vec3(0.35, 0.0, -1.0)) - 0.3;
    //float d = evalStroke(p, strokes[0]);
    float d = 100000.0;

    //d = sminCubic(d, length(p - vec3(0.0, 0.0, -1.0)) - 0.3, 0.2);
    //d = sminCubic(d, length(p - vec3(0.35, 0.0, -1.0)) - 0.3, 0.2);

    for (uint i = 0; i < uStrokesNum; i++)
    {
        float shape = evalStroke(p, strokes[i]);

        float clampedBlend = max(0.0001, strokes[i].param0.w);

        //float m1 = ((stroke.id.y & 1) == 1) || ((stroke.id.y & 2) == 2) ? -1.0 : 1.0;
        //float m2 = ((stroke.id.y & 1) == 1) && ((stroke.id.y & 2) == 0) ? -1.0 : 1.0;
        //d = (index == 0) ? min(shape, d) : opSmoothAll(shape, d, max(0.0001, stroke.param0.w), m1, m2);

        // SMOOTH OPERATIONS

        if ((strokes[i].id.y & 3) == 0)
        {
            d = opSmoothUnion(shape, d, clampedBlend);
        }
        else if ((strokes[i].id.y & 1) == 1)
        {
            d = opSmoothSubtraction(shape, d, clampedBlend);
        }
        else if ((strokes[i].id.y & 2) == 2)
        {
            d = opSmoothIntersection(shape, d, clampedBlend);
        }

        /*if ((strokes[i].id.y & 3) == 0)
        {
            d = sminCubic(d, shape, clampedBlend);
        }
        else if ((strokes[i].id.y & 1) == 1)
        {
            d = max(-shape, d);
        }
        else if ((strokes[i].id.y & 2) == 2)
        {
            d = max(shape, d);
        }*/
    }

    return d;
}

//Estimate normal based on distToScene function
const float EPS = 0.001;
vec3 estimateNormal(vec3 p) 
{
    float xPl = distToScene(vec3(p.x + EPS, p.y, p.z));
    float xMi = distToScene(vec3(p.x - EPS, p.y, p.z));
    float yPl = distToScene(vec3(p.x, p.y + EPS, p.z));
    float yMi = distToScene(vec3(p.x, p.y - EPS, p.z));
    float zPl = distToScene(vec3(p.x, p.y, p.z + EPS));
    float zMi = distToScene(vec3(p.x, p.y, p.z - EPS));
    float xDiff = xPl - xMi;
    float yDiff = yPl - yMi;
    float zDiff = zPl - zMi;
    return normalize(vec3(xDiff, yDiff, zDiff));
}


// - TONEMAPPING -----------------------
vec3 LessThan(vec3 f, float value)
{
    return vec3(
        (f.x < value) ? 1.0f : 0.0f,
        (f.y < value) ? 1.0f : 0.0f,
        (f.z < value) ? 1.0f : 0.0f);
}

vec3 LinearToSRGB(vec3 rgb)
{
    rgb = clamp(rgb, 0.0f, 1.0f);

    return mix(
        pow(rgb, vec3(1.0f / 2.4f)) * 1.055f - 0.055f,
        rgb * 12.92f,
        LessThan(rgb, 0.0031308f)
    );
}

vec3 SRGBToLinear(vec3 rgb)
{
    rgb = clamp(rgb, 0.0f, 1.0f);

    return mix(
        pow(((rgb + 0.055f) / 1.055f), vec3(2.4f)),
        rgb / 12.92f,
        LessThan(rgb, 0.04045f)
    );
}


vec3 aces_tonemap(vec3 color) {
    mat3 m1 = mat3(
        0.59719, 0.07600, 0.02840,
        0.35458, 0.90834, 0.13383,
        0.04823, 0.01566, 0.83777
    );
    mat3 m2 = mat3(
        1.60475, -0.10208, -0.00327,
        -0.53108, 1.10813, -0.07276,
        -0.07367, -0.00605, 1.07602
    );
    vec3 v = m1 * color;
    vec3 a = v * (v + 0.0245786) - 0.000090537;
    vec3 b = v * (0.983729 * v + 0.4329510) + 0.238081;
    return pow(clamp(m2 * (a / b), 0.0, 1.0), vec3(1.0 / 2.2));
}

// -----------------------

void main()
{
    //vec3 direction = normalize(inRayDirection);
    //outColor = vec4(abs(direction.xyz), 1.0);

    
    vec2 uv = inFragUV;
    uv -= vec2(0.5);//offset, so center of screen is origin
    uv *= 2.0;
    //uv.x *= iResolution.x / iResolution.y;//scale, so there is no rectangular distortion

    ray_t camRay;
    camRay.pos = inRayOrigin;
    camRay.dir = normalize(inRayDirection);

    float totalDist = 0.0;
    float finalDist = distToScene(camRay.pos);
    int iters = 0;
    int maxIters = 70;

    vec3 color = vec3(0.0, 0.0, 0.0);

    for (iters = 0; iters < maxIters && finalDist>0.02; iters++) 
    {
        camRay.pos += finalDist * camRay.dir;
        totalDist += finalDist;
        finalDist = distToScene(camRay.pos);
    }

    if (finalDist < 0.02)
    {
        color.r = 1.0;

        vec3 normal = estimateNormal(camRay.pos);

        vec3 lightDir = normalize(vec3(1.0, 1.0, 0.0));

        float dotSN = dot(normal, lightDir);
        dotSN = (dotSN + 1.0) * 0.5;
        dotSN = mix(0.1, 1.2, dotSN);

        //outColor = vec4(0.5 + 0.5 * normal, 1.0) * dotSN;
        outColor = vec4(0.5 + 0.5 * normal, 1.0) * dotSN;

    }

    else
    {
        // Background color
        vec3 bgColor = vec3(0.07, 0.08, 0.19) * 0.8;
        outColor = vec4(bgColor, 1.0);
    }

    outColor.rgb = LinearToSRGB(outColor.rgb);
    
    {
        // Vignette
        vec2 uv2 = inFragUV * (vec2(1.0) - inFragUV.yx);   //vec2(1.0)- uv.yx; -> 1.-u.yx; Thanks FabriceNeyret !
        float vig = uv2.x * uv2.y * 13.0; // multiply with sth for intensity
        vig = pow(vig, 0.35); // change pow for modifying the extend of the  vignette
        vig = mix(0.4, 1.0, vig);
        vig = smoothstep(0.0, 0.75, vig);
        outColor.rgb *= vig;
    }

    //outColor.rgb = abs(strokes[0].param0.xyz);
}
