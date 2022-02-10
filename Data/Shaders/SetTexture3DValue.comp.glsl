
layout(local_size_x = 8, local_size_y = 8, local_size_z = 8) in;

layout(location = 0) uniform vec4 uValue;

layout(binding = 0) uniform writeonly image3D uOutTexture;

void main()
{
    ivec3 outPos = ivec3(gl_GlobalInvocationID.xyz);

    imageStore(uOutTexture, outPos.xyz, uValue);
}
