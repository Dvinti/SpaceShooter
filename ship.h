#ifndef SS_SHIP_H
#define SS_SHIP_H

#include "define.h"

class Ship {
	public:
        Vec pos;
        Vec dir;
        Vec vel;
        Vec acc;
        float angle;
        float color[3]; //[3]
    public:
        Ship();
};

#endif