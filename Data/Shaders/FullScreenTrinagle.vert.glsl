// Copyright (c) 2022 David Gallardo and SDFEditor Project

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
layout(location = 1) out vec4 outNear;
layout(location = 2) out vec4 outFar;

layout(location = 0) uniform mat4 uViewMatrix;
layout(location = 1) uniform mat4 uProjectionMatrix;

void main()
{
    vec2 clipPos = positions[gl_VertexID].xy;
    float near = 0.1f;
    float far = 100.0f;
    
    gl_Position = vec4(clipPos, 0.0, 1.0);
    outFragUV = uvs[gl_VertexID].xy;

    mat4 viewProj = uProjectionMatrix * uViewMatrix;
    mat4 invViewProj = inverse(viewProj);    

    outNear = invViewProj * vec4(clipPos, 0, 1);
    //nearPos /= nearPos.w;
    outFar = invViewProj * vec4(clipPos, 1, 1);
    //farPos /= farPos.w;
}




