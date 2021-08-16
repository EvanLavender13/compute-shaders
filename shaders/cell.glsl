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
    // vec4 data = imageLoad(utexture, idx);
    // if (data.x == 1.0f) return;

    int pixel = idx.y * uwidth + idx.x;
    uint random = hash(pixel);
    vec4 noise  = vec4(random) / 4294967295.0;
    // vec4 data = vec4(pixel % 13);

    // imageStore(utexture, idx, data);
    if (noise.x > 0.5) imageStore(utexture, idx, noise - 0.5);
    // imageStore(utexture, idx, noise - 0.5);
    // imageStore(utexture, ivec2(ux1, uy1), vec4(1.0, 0.1, 0.1, 1.0));
    // imageStore(utexture, ivec2(ux2, uy2), vec4(1.0, 0.1, 0.1, 1.0));
}