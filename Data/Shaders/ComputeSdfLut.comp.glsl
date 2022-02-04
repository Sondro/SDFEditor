
layout(binding = 0, rgba8) uniform image3D uSdfLut;


layout(local_size_x = 8, local_size_y = 8, local_size_z = 8) in;

#define LUT_VOXEL_SIDE (0.05f)
#define INV_LUT_VOXEL_SIDE (1.0f / LUT_VOXEL_SIDE)

void main()
{
    ivec3 outPos = ivec3(gl_GlobalInvocationID.xyz);
    vec3 imgSize = vec3(gl_NumWorkGroups * gl_WorkGroupSize);
    vec3 invImgSize = vec3(1.0f) / imgSize;

    vec3 worldPos = ((vec3(outPos) * invImgSize) - 0.5f) * 2.0f * imgSize * LUT_VOXEL_SIDE;

    float dist = abs(distToScene(worldPos)) * 0.5f * invImgSize.x * INV_LUT_VOXEL_SIDE;

    float col = dist < invImgSize.x ? 1.0f : 0.0f;
    //vec3 color = vec3(gl_GlobalInvocationID.xzy) * invImgSize;
    imageStore(uSdfLut, outPos.xzy, vec4(vec3(col), dist));
}