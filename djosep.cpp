// Daniel Josep
// djosep.cpp
// My Own Work

#include <iostream>
#include <ctime>
#include <cmath>
#include "fonts.h"
#include "game.h"

using namespace std;

#define PI 3.141592653589793

const int MAX_BULLETS = 100;

extern double timeDiff(struct timespec *start, struct timespec *end);
extern void timeCopy(struct timespec *dest, struct timespec *source);

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

void show_scores(int score)
{
	Rect r;

	r.bot = gl.yres - 690;
    r.left = (gl.xres/2) - 65;
    r.center = 0;
    ggprint13(&r, 16, 0xfbfbfa, "      <%0.4i>      ", score);
}

void shoot_bullets() 
{
	//move shoot
	cout << "space bar" << endl;
	struct timespec bt;
	clock_gettime(CLOCK_REALTIME, &bt);
	double ts = timeDiff(&g.bulletTimer, &bt);
	Bullet *b = &g.barr[g.nbullets];
	if (ts > 0.1) {
		timeCopy(&g.bulletTimer, &bt);
		if (g.nbullets < MAX_BULLETS) {
			timeCopy(&b->time, &bt);
			b->pos[0] = g.ship.pos[0];
			b->pos[1] = g.ship.pos[1];
			b->vel[0] = g.ship.vel[0];
			b->vel[1] = g.ship.vel[1];
			Flt rad = ((g.ship.angle+90.0)/ 360.0f) * PI * 2.0;
			Flt xdir = cos(rad);
			Flt ydir = sin(rad);
			b->pos[0] += xdir*20.0f;
			b->pos[1] += ydir*20.0f;
			b->vel[0] += xdir*6.0f + rnd()* 0.1;
			b->vel[1] += ydir*6.0f + rnd()* 0.1;
			b->color[0] = 1.0f;
			b->color[1] = 1.0f;
			b->color[2] = 1.0f;
			g.nbullets++;
		}
	}
}