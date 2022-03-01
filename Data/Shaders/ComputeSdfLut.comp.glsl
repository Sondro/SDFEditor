// Copyright (c) 2022 David Gallardo and SDFEditor Project

layout(binding = 0, rgba8) uniform writeonly image3D uSdfLutImage;

layout(local_size_x = 8, local_size_y = 8, local_size_z = 8) in;

#define LUT_VOXEL_SIDE (uVoxelSide.x)
#define INV_LUT_VOXEL_SIDE (uVoxelSide.y)

void main()
{
    ivec3 outPos = ivec3(gl_GlobalInvocationID.xyz);
    ivec3 imgSize = ivec3(gl_NumWorkGroups * gl_WorkGroupSize);
    vec3 invImgSize = vec3(1.0f) / vec3(imgSize);

    // outPos + 0.5 to move to the center of the voxel
    // divide by image size to normalize the position from 0 to 1
    // -0.5 remap to -0.5 , 0.5
    // multiply by image size remaps to -64 , 64  (-63.5 , 63.5)
    // multiply by voxel size
    //vec3 worldPos = (((vec3(outPos) + 0.5) * invImgSize) - 0.5f) * imgSize * LUT_VOXEL_SIDE;

    // add 0.5 to move to the center voxel (0.5, 127.5)
    // subtract half othe image size (-63.5, 63.5)
    // multiply by voxel size in world 
    
    //vec3 worldPos = ((vec3(outPos.xyz/*xzy*/) + 0.5) - uVolumeExtent.x) * LUT_VOXEL_SIDE;

    vec3 worldPos = LutCoordToWorld(outPos);

    // multiply by inverse volxe side to get values in voxels
    // multiply by twice the inverse of image size to normalize the values
    float dist = distToScene(worldPos);
    // remap to (0.0, 1.0) to store in the texture 
    

    float normDist = ((dist * INV_LUT_VOXEL_SIDE * invImgSize.x) + 1.0) * 0.5;

    vec3 slotCoord = vec3(1.0);

    //if (abs(dist * uVoxelSide.y) < 1.0)
    if (abs(dist) < uVoxelSide.x * 1.5f)
    {
        uint slot = atomicAdd(slot_count, 1);
        slot_list[slot] = CoordToIndex(outPos.xyz);
        slotCoord = IndexToNormCoord(slot);
    }

    imageStore(uSdfLutImage, outPos.xyz, vec4(slotCoord, normDist));
}