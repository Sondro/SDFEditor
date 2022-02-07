
layout(binding = 0, r8) uniform image3D uSdfAtlasImage;

layout(local_size_x = 8, local_size_y = 8, local_size_z = 8) in;

#define ATLAS_SIZE (ivec3(1024, 1024, 256))
#define ATLAS_SLOTS (ATLAS_SIZE / 8)

void main()
{
    uint slot = uint(gl_WorkGroupID.x);
    uint lutIdx = slot_list[slot];
    ivec3 lutCoord = IndexToCoord(lutIdx);
    
    // slot center world pos
    vec3 slotWorldPos = LutCoordToWorld(lutCoord); 
    
    // atlas voxel offset in world units, local to the 8x8x8 slot
    vec3 workItemOffset = (vec3(ivec3(gl_LocalInvocationID.xyz)) - vec3(ivec3(gl_WorkGroupSize.xyz)) * 0.5f + 0.5f) * uVoxelSide.z;
    //vec3 workItemOffset = vec3(0.0);
    
    // atlas voxel center world position
    vec3 worldPos = slotWorldPos + workItemOffset.xyz /*xzy*/;
    
    ivec3 atlasSlotCoord = GetCellCoordFromIndex(slot, ATLAS_SLOTS);
    ivec3 atlasVoxelCoord = (atlasSlotCoord * ivec3(gl_WorkGroupSize.xyz)) + ivec3(gl_LocalInvocationID.xyz);
    
    float dist = distToScene(worldPos) * uVoxelSide.y;
    //float normDist = ((dist * uVoxelSide.y * uVolumeExtent.y) + 1.0) * 0.5;

    imageStore(uSdfAtlasImage, atlasVoxelCoord.xyz, vec4(abs(dist)));
}