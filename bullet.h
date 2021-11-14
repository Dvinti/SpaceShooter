#ifndef SS_BULLET_H
#define SS_BULLET_H

#include "define.h"

class Bullet {
    public:
        Vec pos;
        Vec vel;
        float color[3];
        struct timespec time;
    public:
        Bullet();
};

#endif