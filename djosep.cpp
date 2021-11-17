// Daniel Josep
// djosep.cpp
// My Own Work

#include <ctime>
#include "fonts.h"
#include "game.h"

extern Global gl;

void show_Daniels_credits(int x, int y)
{
	Rect r;
	r.bot = y;
	r.left = x;
	r.center = 1;
	ggprint8b(&r, 16, 0x00ff0000, "Daniel Josep");
}