//#version 450

//#extension GL_ARB_shader_storage_buffer_object : require

layout(location = 0) in vec2 inFragUV;
layout(location = 1) in vec4 inNear;
layout(location = 2) in vec4 inFar;

layout(location = 0) out vec4 outColor;


struct ray_t
{
    vec3 pos;
    vec3 dir;
};

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


    vec3 origin = inNear.xyz / inNear.w;  //ray's origin
    vec3 far3 = inFar.xyz / inFar.w;
    vec3 dir = far3 - origin;
    dir = normalize(dir);        //ray's direction

    ray_t camRay;
    camRay.pos = origin; // inRayOrigin;
    camRay.dir = dir; // normalize(inRayDirection);
    
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
        //bgColor += (1.0 - distance(inFragUV, vec2(0.5, 0.5))) * vec3(0.0, 0.01, 0.01);
        outColor = vec4(bgColor, 1.0);
    }

    outColor.rgb = LinearToSRGB(outColor.rgb);
    
    {
        // Vignette
        vec2 uv2 = inFragUV * (vec2(1.0) - inFragUV.yx);   //vec2(1.0)- uv.yx; -> 1.-u.yx; Thanks FabriceNeyret !
        float vig = uv2.x * uv2.y * 13.0; // multiply with sth for intensity
        vig = pow(vig, 0.35); // change pow for modifying the extend of the  vignette
        vig = mix(0.35, 1.0, vig);
        vig = smoothstep(0.0, 0.75, vig);
        outColor.rgb *= vig;
    }

    //outColor.rgb = abs(strokes[0].posb.xyz);
}
