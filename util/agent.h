#ifndef AGENT_H_
#define AGENT_H_

typedef struct Agent Agent;
struct Agent
{
    float x, y;
    float angle;
    float dump; // unused?
};

#endif