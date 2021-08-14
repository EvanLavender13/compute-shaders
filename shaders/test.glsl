#version 460

layout(local_size_x = 1, local_size_y = 1) in;
layout(rgba32f, binding = 0) uniform image2D utexture;

uniform float udelta;
uniform int uwidth;
uniform int uheight;
uniform int ux1, uy1;
uniform int ux2, uy2;

uint
hash(uint state)
{
    state ^= 2747636419u;
    state *= 2654435769u;
    state ^= state >> 16;
    state *= 2654435769u;
    state ^= state >> 16;
    state *= 2654435769u;
    return state;
}

void
main()
{
    ivec2 idx = ivec2(gl_GlobalInvocationID.xy) + ivec2(ux1, uy1);

    int pixel = idx.y * uwidth + idx.x;
    uint random = hash(pixel);
    //vec4 data = vec4(random) / 4294967295.0;
    vec4 data = vec4(pixel % 100);

    imageStore(utexture, idx, data);
}