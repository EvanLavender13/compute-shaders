#version 460

layout(local_size_x = 16) in;
layout(rgba32f, binding = 0) uniform image2D utexture;

uniform float udelta;
uniform int uwidth;
uniform int uheight;

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

float
scale(uint state)
{
    return state / 4294967295.0;
}

struct Agent
{
    float x, y;
    float angle;
    float dump;
};

layout (std430, binding = 1) buffer Agents
{
    Agent agents[];
};

float
sense(Agent agent, ivec2 position, float off)
{
    int dst   = 5;
    int ssize = 2;
    float angle  = agent.angle + off;
    vec2 dir     = vec2(cos(angle), sin(angle));
    ivec2 center = ivec2(position + dir * dst);

    float sum = 0.0f;
    for (int offx = -ssize; offx <= ssize; offx++) {
        for (int offy = -ssize; offy <= ssize; offy++) {
            ivec2 pos = center + ivec2(offx, offy);
            if (pos.x >= 0 && pos.x < uwidth && pos.y >= 0 && pos.y < uheight) {
                sum += length(imageLoad(utexture, pos));
            }
        }
    }
    return sum;
}

void
main()
{
    uint idx = gl_GlobalInvocationID.x;

    Agent agent = agents[idx];
    uint random = hash(uint(agent.y * uwidth + agent.x * uheight));
    vec2 position = vec2(agent.x, agent.y);
    vec2 direction = vec2(cos(agent.angle), sin(agent.angle));
    vec4 data = imageLoad(utexture, ivec2(position));
    vec2 newposition = position + direction * 0.25f;

    float wfwd  = sense(agent, ivec2(position),  0.75f);
    float wleft = sense(agent, ivec2(position),  0.75f);
    float wrght = sense(agent, ivec2(position), -0.75f);    

    if (wfwd > wleft && wfwd > wrght) {
        agents[idx].angle += (scale(random) - 0.5f);
    } else if (wrght > wleft) {
        agents[idx].angle -= scale(random) * 0.1f;
    } else if (wleft > wrght) {
        agents[idx].angle += scale(random) * 0.1f;
    }

    if (newposition.x < 0 || newposition.x >= uwidth || newposition.y < 0 || newposition.y >= uheight) {
        newposition.x = min(uwidth - 1.0f, max(0.0f, newposition.x));
        newposition.y = min(uheight - 1.0f, max(0.0f, newposition.y));
        // newposition.x = 400;
        // newposition.y = 300;
        // agents[idx].angle -= 3.14f;
        agents[idx].angle = scale(random) * 2.0f * 3.1415926535f;
    }

    // vec4 newdata = imageLoad(utexture, ivec2(newposition));
    // if (normalize(newdata).x < 0.5) {
    //     agents[idx].angle = scale(random);// * 2.0f * 3.1415926535f;
    // } else {
    //     agents[idx].angle -= 0.001f;
    // }

    agents[idx].x = newposition.x;
    agents[idx].y = newposition.y;
    // imageStore(utexture, ivec2(newposition), vec4(0.1, 0.1, 0.9, 1.0));
    imageStore(utexture, ivec2(newposition), vec4(1.0));
}