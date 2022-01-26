#version 450

out gl_PerVertex
{
  vec4 gl_Position;
  float gl_PointSize;
  float gl_ClipDistance[];
};

// Full screen triangle vertices
vec2 positions[3] = vec2[](
    vec2(3.0, -1.0),
    vec2(-1.0, -1.0),
    vec2(-1.0, 3.0)
);

/*vec2 uvs[3] = vec2[](
    vec2(2.0, 0.0),
    vec2(0.0, 0.0),
    vec2(0.0, 2.0)
    );*/

vec2 uvs[3] = vec2[](
    vec2(2.0, 0.0),
    vec2(0.0, 0.0),
    vec2(0.0, 2.0)
    );

layout(location = 0) out vec2 outFragUV;
layout(location = 1) out vec3 outRayOrigin;
layout(location = 2) out vec3 outRayDirection;
layout(location = 4) out vec4 outNear;
layout(location = 5) out vec4 outFar;

layout(location = 0) uniform mat4 uViewMatrix;
layout(location = 1) uniform mat4 uProjectionMatrix;

void main()
{
    vec2 clipPos = positions[gl_VertexID].xy;
    float near = 0.1f;
    float far = 100.0f;
    
    gl_Position = vec4(clipPos, 0.0, 1.0);
    outFragUV = uvs[gl_VertexID].xy;
    //outFragUV = (positions[gl_VertexID].xy + 1.0) * 0.5;

    mat4 viewProj = uProjectionMatrix * uViewMatrix;
    mat4 invViewProj = inverse(viewProj);
    outRayOrigin = (invViewProj * vec4(clipPos, -1.0, 1.0) * near).xyz;
    outRayDirection = (invViewProj * vec4(clipPos * (far - near), far + near, far - near)).xyz;
    

    //vec3 ray_origin_wc = (invViewProj * vec4(clipPos, -1.0, 1.0)).xyz;
    //vec3 eye_pos_wc = (inverse(uViewMatrix) * vec4(0, 0, 0, 1.0)).xyz;
    //vec3 ray_dir_wc = ray_origin_wc - eye_pos_wc;
    //outRayOrigin = ray_origin_wc;
    //outRayDirection = ray_dir_wc;

    outNear = invViewProj * vec4(clipPos, -1, 1);
    //nearPos /= nearPos.w;
    outFar = invViewProj * vec4(clipPos, 1, 1);
    //farPos /= farPos.w;

    //outRayOrigin = uCameraPos.xyz;
    //outRayDirection = farPos.xyz - nearPos.xyz;

    //outRayDirection = (uInvViewProjMatrix * vec4(clipPos, -1.0, 1.0)).xyz;
    //outRayOrigin = vec3(outRayDirection.xy, 0.0);

    // equivalent calculation:
    // ray = (invprojview * (vec4(pos, 1.0, 1.0) * far - vec4(pos, -1.0, 1.0) * near)).xyz
}




