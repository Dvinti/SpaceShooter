#ifndef SS_ENEMY_H
#define SS_ENEMY_H

#include "define.h"

class Enemy {
    public:
        Vec pos;
        Vec dir;
        Vec vel;
        Vec acc;
        float angle;
        float color[1];
    public:
        Enemy();
};

#endif