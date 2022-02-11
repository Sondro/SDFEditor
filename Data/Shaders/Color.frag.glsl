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

float CalcAO(in vec3 pos, in vec3 nor)
{
    float occ = 0.0;
    float sca = 1.0, dd;
    for (int i = 0; i < 5; i++)
    {
        float hr = 0.01 + 0.23   * float(i) / 4.0;
        vec3 aopos = nor * hr + pos;
        dd = distToScene(aopos);
        occ += -(dd - hr) * sca;
        sca *= 0.95;
    }
    return clamp(1.0 - 1.6 * occ, 0.0, 1.0);
}


float CalcAOLut(in vec3 pos, in vec3 nor)
{
    float occ = 0.0;
    float sca = 1.0, dd;
    for (int i = 0; i < 5; i++)
    {
        float hr = 0.01 + 0.23   * float(i) / 4.0;
        vec3 aopos = nor * hr + pos;
        dd = distToSceneLut(aopos);
        occ += -(dd - hr) * sca;
        sca *= 0.95;
    }
    return clamp(1.0 - 1.6 * occ, 0.0, 1.0);
}

float CalcAOAtlas(in vec3 pos, in vec3 nor)
{
    float occ = 0.0;
    float sca = 1.0, dd;
    for (int i = 0; i < 5; i++)
    {
        float hr = 0.01 + 0.23 * float(i) / 4.0;
        vec3 aopos = nor * hr + pos;
        dd = distToSceneAtlas(aopos);
        occ += -(dd - hr) * sca;
        sca *= 0.95;
    }
    return clamp(1.0 - 1.6 * occ, 0.0, 1.0);
}

vec3 ApplyMaterial(vec3 pos, vec3 rayDir, vec3 normal, float ao)
{
    vec3 lightDir = normalize(vec3(1.0, 1.0, 0.0));

    float dotSN = dot(normal, lightDir);
    dotSN = (dotSN + 1.0) * 0.5;
    dotSN = mix(0.4, 1.0, dotSN);

    float dotCam = 1.0 - abs(dot(rayDir, normal));
    dotCam = pow(dotCam, 2.0);

    //vec3 color = vec3(0.5 + 0.5 * normal) * dotSN * mix(0.5, 1.0, ao);
    //color = mix(color, vec3(0.5), dotCam);
    //float ao = CalcAO(pos, normal);
    
    vec3 color = vec3(0); 
    color = mix(vec3(0.09, 0.008, 0.00), vec3(0.14, 0.045, 0.025), dotCam);
    color = mix(vec3(0.036, 0.008, 0.009), color, ao) * dotSN;// *dotSN;
    
    return color;
}

vec3 RaymarchStrokes(in ray_t camRay)
{
    float totalDist = 0.0;
    float finalDist = distToScene(camRay.pos);
    int iters = 0;
    int maxIters = 70;
    float limit = 0.02f;

    vec3 color = vec3(0.07, 0.08, 0.19) * 0.8;

    for (iters = 0; iters < maxIters && finalDist > limit; iters++)
    {
        camRay.pos += finalDist * camRay.dir;
        totalDist += finalDist;
        finalDist = distToScene(camRay.pos);
    }

    if (finalDist <= limit)
    {
        vec3 normal = estimateNormal(camRay.pos);
        //vec3 normal = estimateNormalAtlas(camRay.pos);
        color = ApplyMaterial(camRay.pos, camRay.dir, normal, CalcAO(camRay.pos, normal));
    }

    return color;
}

vec2 opMinV2(in vec2 a, in vec2 b) { return (a.x < b.x) ? a : b; }

vec3 RaymarchLut(in ray_t camRay) // Debug only
{
    float totalDist = 0.0;
    float finalDist = 1000000.0f;
    int iters = 0;
    int maxIters = 150;
    //float limit = sqrt(pow(uVoxelSide.x * 0.5, 2.0) * 2.0f);
    float limit = uVoxelSide.x * 0.5;
    float limitSubVoxel = 0.02;
    vec3 color = vec3(0.07, 0.08, 0.19) * 0.8;

    vec3 testNormal = vec3(0, 0, 0);
    vec2 testDistance = vec2(0, 0);
    bool intersectsBox = rayboxintersect(camRay.pos, camRay.dir, vec3(uVoxelSide.x * uVolumeExtent.x * -0.5), vec3(uVoxelSide.x * uVolumeExtent.x * 0.5), testNormal, testDistance);
    float minZeroDist = clamp(testDistance.x, 0.0, abs(testDistance.x));
    vec3 enterPoint = camRay.pos + camRay.dir * minZeroDist;

    if (intersectsBox)
    {
        camRay.pos = enterPoint;
        testDistance.y -= minZeroDist;
        
        finalDist = distToSceneLut(camRay.pos);

        for (iters = 0; iters < maxIters && (finalDist > limit && totalDist < testDistance.y); iters++)
        {
            camRay.pos += finalDist * camRay.dir;
            totalDist += finalDist;
            finalDist = distToSceneLut(camRay.pos);

            if (finalDist <= limit && totalDist < testDistance.y)
            {
                // this should be done inside the raymarching loop
                // when finalDist < limit ...
                // fetch the atlas cell coords for camRay.pos
                // do a box intersection to know the distance with edges and calculate 4 points along the ray inside the box
                // convert the four points to voxel coords and ensure edge ones are at least + 0.5 of the border (clamp offset?)
                // sample the four points in this atlas cell
                // get the smallest distance of four
                // **** limit calculation ?? ******
                // if smalles distance is < threshold ( 0.02? ) is a hit
                // if not, here comes the tricky part, advance to camRay.pos + td.y + threshold to move to the next box and ...
                // reset some values to avoid raymarch loop stop (maybe exit conditions should be changed)
                // continue raymarching

                vec3 tn = vec3(0, 0, 0);
                vec2 td = vec2(0, 0);
                vec3 bmin = (floor(camRay.pos * uVoxelSide.y)) * uVoxelSide.x;
                vec3 bmax = bmin + uVoxelSide.x;
                if (rayboxintersect(camRay.pos, camRay.dir, bmin, bmax, tn, td))
                {
                    ivec3 lutcoord = WorldToLutCoord(camRay.pos);
                    vec3 fetchedIndex = texelFetch(uSdfLutTexture, clamp(lutcoord, ivec3(0), ivec3(uVolumeExtent.x - 1)), 0).rgb;
                   
                    vec2 minDist = vec2(1.0, 0.0);

                    if (ivec3(fetchedIndex + 0.5) != ivec3(1))
                    {
                        uint slot = NormCoordToIndex(fetchedIndex);
                        ivec3 cellCoord = GetCellCoordFromIndex(slot, ATLAS_SLOTS) * 8;

                        // TODO: calculate the exact offset to avoid filtering with adjadcent Atlas cubes
                        float At = td.x + 0.01;
                        float Bt = 0.0f;
                        float Ct = td.y * 0.5;
                        float Dt = td.y - 0.01;

                        vec3 Ap = camRay.pos + At * camRay.dir;
                        vec3 Bp = camRay.pos + Bt * camRay.dir;
                        vec3 Cp = camRay.pos + Ct * camRay.dir;
                        vec3 Dp = camRay.pos + Dt * camRay.dir;

                        vec3 offsetA = vec3(fract(Ap * uVoxelSide.y) * 8);
                        vec3 offsetB = vec3(fract(Bp * uVoxelSide.y) * 8);
                        vec3 offsetC = vec3(fract(Cp * uVoxelSide.y) * 8);
                        vec3 offsetD = vec3(fract(Dp * uVoxelSide.y) * 8);

                        offsetA = clamp(offsetA, 0.5f, 7.5f);
                        offsetB = clamp(offsetB, 0.5f, 7.5f);
                        offsetC = clamp(offsetC, 0.5f, 7.5f);
                        offsetD = clamp(offsetD, 0.5f, 7.5f);

                        vec2 A = vec2(sampleAtlasDist((cellCoord + offsetA) / vec3(ATLAS_SIZE)).r, At);
                        vec2 B = vec2(sampleAtlasDist((cellCoord + offsetB) / vec3(ATLAS_SIZE)).r, Bt);
                        vec2 C = vec2(sampleAtlasDist((cellCoord + offsetC) / vec3(ATLAS_SIZE)).r, Ct);
                        vec2 D = vec2(sampleAtlasDist((cellCoord + offsetD) / vec3(ATLAS_SIZE)).r, Dt);
                        minDist = opMinV2(opMinV2(A, B), opMinV2(C, D));
                    }

                    if (minDist.x < limitSubVoxel)
                    {
                        finalDist = minDist.x;
                        camRay.pos = camRay.pos + minDist.y * camRay.dir;
                    }
                    else
                    {
                        finalDist = limit * 2.0;
                        camRay.pos = camRay.pos + camRay.dir * (td.y + 0.0001);                    
                    }
                    //color = vec3(minDist);
                    /*if (minDist < 0.05)
                    {
                        //color = vec3(minDist * uVoxelSide.y);
                        vec3 normal = estimateNormalLut(camRay.pos);
                        color = ApplyMaterial(camRay.pos, camRay.dir, normal, CalcAOLut(camRay.pos, normal));
                    }*/

                    //color = vec3(float(slot) / 50000.0f);


                }
            }
        }

        if (finalDist < limitSubVoxel)
        {
            //break;
            vec3 normal = estimateNormalAtlas(camRay.pos);
            color = ApplyMaterial(camRay.pos, camRay.dir, normal, CalcAOAtlas(camRay.pos, normal));
        }
        else
        {
            //color = vec3(0.0, 1.0, 0.0);
        }

        /*
        if (finalDist <= limit && totalDist < testDistance.y)
        {
            
            vec3 tn = vec3(0, 0, 0);
            vec2 td = vec2(0, 0);
            vec3 bmin = (floor(camRay.pos * uVoxelSide.y)) * uVoxelSide.x;
            vec3 bmax = bmin + uVoxelSide.x;
            if (rayboxintersect(camRay.pos, camRay.dir, bmin, bmax, tn, td))
            {


                //color = 0.5 + 0.5 * tn;

                //camRay.pos += (td.x - uVoxelSide.x * 0.01) * camRay.dir;
                //bmin = (floor(camRay.pos * uVoxelSide.y)) * uVoxelSide.x;
                //bmax = bmin + uVoxelSide.x;
                //if (rayboxintersect(camRay.pos - totalDist * camRay.dir, camRay.dir, bmin, bmax, tn, td))
                //{
                //    color = 0.5 + 0.5 * tn;
                //}


                vec3 Ap = camRay.pos + td.x * camRay.dir;
                vec3 Bp = camRay.pos;
                vec3 Cp = camRay.pos + td.y * 0.5 * camRay.dir;
                vec3 Dp = camRay.pos + td.y * camRay.dir;

                ivec3 lutcoord = WorldToLutCoord(camRay.pos);
                vec3 fetchedIndex = texelFetch(uSdfLutTexture, clamp(lutcoord, ivec3(0), ivec3(uVolumeExtent.x - 1)), 0).rgb;
                uint slot = NormCoordToIndex(fetchedIndex);
                ivec3 cellCoord = GetCellCoordFromIndex(slot, ATLAS_SLOTS) * 8;

                ivec3 offsetA = ivec3(fract(Ap * uVoxelSide.y) * 8);
                ivec3 offsetB = ivec3(fract(Bp * uVoxelSide.y) * 8);
                ivec3 offsetC = ivec3(fract(Cp * uVoxelSide.y) * 8);
                ivec3 offsetD = ivec3(fract(Dp * uVoxelSide.y) * 8);

                float A = sampleAtlasDist((cellCoord + offsetA + 0.5) / vec3(ATLAS_SIZE)).r;
                float B = sampleAtlasDist((cellCoord + offsetB + 0.5) / vec3(ATLAS_SIZE)).r;
                float C = sampleAtlasDist((cellCoord + offsetC + 0.5) / vec3(ATLAS_SIZE)).r;
                float D = sampleAtlasDist((cellCoord + offsetD + 0.5) / vec3(ATLAS_SIZE)).r;
                float minDist = min(min(A, B), min(C, D));

                if (minDist < 0.02)
                {
                   //break;
                    vec3 normal = estimateNormalLut(camRay.pos);
                    color = ApplyMaterial(camRay.pos, camRay.dir, normal, CalcAOLut(camRay.pos, normal));
                }
                else
                {
                    color = vec3(0.0, 1.0, 0.0);
                }
                //color = vec3(minDist);
                //if (minDist < 0.05)
                //{
                //    //color = vec3(minDist * uVoxelSide.y);
                //    vec3 normal = estimateNormalLut(camRay.pos);
                //    color = ApplyMaterial(camRay.pos, camRay.dir, normal, CalcAOLut(camRay.pos, normal));
                //}

                //color = vec3(float(slot) / 50000.0f);

                
            }            
        }*/

        // Debug box
        //color = mix(color, vec3(0.0, 0.5, 0.0), 0.1);
    }

    return color;
}

void main()
{    
    vec3 origin = inNear.xyz / inNear.w;  //ray's origin
    vec3 far3 = inFar.xyz / inFar.w;
    vec3 dir = far3 - origin;
    dir = normalize(dir);        //ray's direction

    ray_t camRay;
    camRay.pos = origin;
    camRay.dir = dir;
    
    vec3 finalColor = (uVoxelPreview.x == 1) ? RaymarchLut(camRay) : RaymarchStrokes(camRay);

    finalColor = LinearToSRGB(finalColor.rgb);
    
    {
        // Vignette
        vec2 uv2 = inFragUV * (vec2(1.0) - inFragUV.yx);   //vec2(1.0)- uv.yx; -> 1.-u.yx; Thanks FabriceNeyret !
        float vig = uv2.x * uv2.y * 13.0; // multiply with sth for intensity
        vig = pow(vig, 0.35); // change pow for modifying the extend of the  vignette
        vig = mix(0.35, 1.0, vig);
        vig = smoothstep(0.0, 0.75, vig);
        finalColor *= vig;
    }

    outColor = vec4(finalColor, 1.0f);

    //outColor.rgb = abs(strokes[0].posb.xyz);
    if (uVoxelPreview.x == 1)
    {
        vec4 sdf = texelFetch(uSdfLutTexture, ivec3(inFragUV.x * 2.0 * (16.0f / 9.0f) * uVolumeExtent.x, float(uVoxelPreview.y), inFragUV.y * 2.0 * uVolumeExtent.x), 0).rgba;
        //vec4 sdfTop = texture(uSdfLutTexture, vec3(inFragUV.x * 2.0 * (16.0f / 9.0f), inFragUV.y * 2.0 - 1.0, float(uVoxelPreview.y) / 128.0f)).rgba;
        //sdf = mix(sdfTop, sdf, step(sdf.a, 1.0f / 128.0f));
        sdf.a = (sdf.a * 2.0) - 1.0;
        sdf.a = abs(sdf.a * uVolumeExtent.x * uVoxelSide.x);
        float debugLimit = sqrt(pow(uVoxelSide.x * 0.5, 2.0) * 2.0f) * 2.5f;
        outColor.rgb = mix(finalColor, sdf.rgb, sdf.a < debugLimit ? 1.0f : 0.0f);
    }
}
