// Daniel Josep
// djosep.cpp
// My Own Work

#include <iostream>
#include <ctime>
#include <cmath>
#include "fonts.h"
#include "game.h"

using namespace std;

extern Global gl;
extern Game g;
extern Bullet b;

void show_Daniels_credits(int x, int y)
{
    Rect r;
    r.bot = y;
    r.left = x;
    r.center = 1;
    ggprint8b(&r, 16, 0x00ff0000, "Daniel Josep");
}

void instructions()
{
    Rect r;
    r.bot = gl.yres - 30;
    r.left = 20;
    r.center = 0;
    ggprint8b(&r, 16, 0xfbfbfa, "Press s to show instructions");
}

void show_scores(int score)
{
    Rect r;
    r.bot = gl.yres - 690;
    r.left = (gl.xres/2) - 65;
    r.center = 0;
    ggprint13(&r, 16, 0xfbfbfa, "      <%0.4i>      ", score);
}

/*void credit_screen()
{
	
}*/