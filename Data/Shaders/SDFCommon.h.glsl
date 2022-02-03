
struct stroke_t
{
    vec4 posb;      // position.xyz, blend.a
    vec4 quat;      // rotation
    vec4 param0;    // size.xyz, radius.x, round.w
    vec4 param1;    // unused
    ivec4 id;       // shape.x, flags.y
};


layout(std430, binding = 0) readonly buffer strokes_buffer
{
    stroke_t strokes[];
};

layout(std430, binding = 1) buffer slot_list_buffer
{
    uint slot_list[];
};

layout(std430, binding = 2) buffer slot_count_buffer
{
    uint slot_count;
    uint padding[2];
};

layout(location = 20) uniform uint uStrokesCount;

// - MATHS -------------------------------
vec3 quatMultVec3(vec4 q, vec3 v)
{
    vec3 t = cross(q.xyz, cross(q.xyz, v) + q.w * v);
    return v + t + t;
}

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

// - SDF Primitives ---------------------
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

    // mirror goes here
    vec3 position = p - stroke.posb.xyz;
    position = quatMultVec3(stroke.quat, position);

    if (stroke.id.x == 0)
    {
        shape = sdEllipsoid(position, stroke.param0.xyz);
    }
    else if (stroke.id.x == 1)
    {
        float round = clamp(stroke.param0.w, 0.0, 1.0);
        float smaller = min(min(stroke.param0.x, stroke.param0.y), stroke.param0.z);
        round = mix(0.0, smaller, round);
        shape = sdRoundBox(position, stroke.param0.xyz - round, round);
    }
    else if (stroke.id.x == 2)
    {
        shape = sdTorus(position, stroke.param0.xy);
    }
    else if (stroke.id.x == 3)
    {
        vec2 params = max(stroke.param0.xy, vec2(0.0, 0.0));
        shape = sdVerticalCapsule(position - vec3(0.0, -params.x * 0.5, 0.0), params.y, params.x);
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

    for (uint i = 0; i < uStrokesCount; i++)
    {
        float shape = evalStroke(p, strokes[i]);

        float clampedBlend = max(0.0001, strokes[i].posb.w);

        //float m1 = ((stroke.id.y & 1) == 1) || ((stroke.id.y & 2) == 2) ? -1.0 : 1.0;
        //float m2 = ((stroke.id.y & 1) == 1) && ((stroke.id.y & 2) == 0) ? -1.0 : 1.0;
        //d = (index == 0) ? min(shape, d) : opSmoothAll(shape, d, max(0.0001, stroke.posb.w), m1, m2);

        // SMOOTH OPERATIONS

        if ((strokes[i].id.y & 3) == 0)
        {
            d = opSmoothUnion(shape, d, clampedBlend);
        }
        else if ((strokes[i].id.y & 1) == 1)
        {
            d = opSmoothSubtraction(shape + clampedBlend * 0.4, d, clampedBlend);
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
