#version 460

layout(local_size_x = 10, local_size_y = 10) in;
layout(rgba32f, binding = 0) uniform image2D utexture;

uniform float udelta;
uniform int uwidth;
uniform int uheight;

void
main()
{
    ivec2 idx = ivec2(gl_GlobalInvocationID.xy);

    vec4 data = imageLoad(utexture, idx);

    vec4 sum = vec4(0.0f);
    for (int offx = -1; offx <= 1; offx++) {
        for (int offy = -1; offy <= 1; offy++) {
            int samplex = idx.x + offx;
            int sampley = idx.y + offy;
            if (samplex >= 0 && samplex < uwidth && sampley >= 0 && sampley < uheight) {
                sum += imageLoad(utexture, ivec2(samplex, sampley));
            }
        }
    }
    
    sum /= 9.0f;
    data = mix(data, sum, 0.9f);
    data = max(vec4(0.0f), data - 0.001f);
    imageStore(utexture, idx, data);
}