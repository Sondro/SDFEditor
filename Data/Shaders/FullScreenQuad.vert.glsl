#version 430

const vec2 sVertices[6] =   vec2[6](vec2( 1.0, -1.0),
                                    vec2(-1.0, -1.0),
                                    vec2( 1.0,  1.0),
                                    vec2( 1.0,  1.0),
                                    vec2(-1.0,  1.0),
                                    vec2(-1.0, -1.0));

out vec2 ex_TexCoord;

void main(void)
{
    ex_TexCoord.xy = (sVertices[gl_VertexID].xy + 1.0) * 0.5;
    gl_Position = sVertices[gl_VertexID];

}
