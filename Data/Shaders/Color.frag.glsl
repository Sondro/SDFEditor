#version 450

#extension GL_ARB_shader_storage_buffer_object : require

layout(location = 0) in vec2 inFragUV;
layout(location = 1) in vec3 inRayOrigin;
layout(location = 2) in vec3 inRayDirection;

layout(location = 0) out vec4 outColor;

struct stroke_t
{
    vec4 param0; // position.xyz, blend.a
    vec4 param1; // size.xyz, radius.x, 
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
float sminCubic(float a, float b, float k)
{
    float h = max(k - abs(a - b), 0.0) / k;
    return min(a, b) - h * h * h * k * (1.0 / 6.0);
}

void evalStroke(in out float d, vec3 p, stroke_t stroke)
{
    d = sminCubic(d, length(p - stroke.param0.xyz) - stroke.param1.x, stroke.param0.w);
}

//Distance to scene at point
float distToScene(vec3 p) 
{
    //float a = length(p - vec3(0.0, 0.0, -1.0)) - 0.3;
    //float b = length(p - vec3(0.35, 0.0, -1.0)) - 0.3;
    float d = 1000000.0;

    //d = sminCubic(d, length(p - vec3(0.0, 0.0, -1.0)) - 0.3, 0.2);
    //d = sminCubic(d, length(p - vec3(0.35, 0.0, -1.0)) - 0.3, 0.2);

    for (uint i = 0; i < uStrokesNum; i++)
    {
        evalStroke(d, p, strokes[i]);
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
    return pow(clamp(m2 * (a / b), 0.0, 1.0), vec3(1.0 / 2.4));
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
    int maxIters = 150;

    vec3 color = vec3(0.0, 0.0, 0.0);

    for (iters = 0; iters < maxIters && finalDist>0.01; iters++) {
        camRay.pos += finalDist * camRay.dir;
        totalDist += finalDist;
        finalDist = distToScene(camRay.pos);
    }

    if (finalDist < 0.01)
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
        outColor = vec4(0.05, 0.05, 0.1, 1.0);
    }

    outColor.rgb = LinearToSRGB(outColor.rgb);
    
    //outColor.rgb = abs(strokes[0].param0.xyz);
}
