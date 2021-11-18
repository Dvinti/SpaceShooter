#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <ctime>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "log.h"
#include "fonts.h"
#include "game.h"

Global::Global() {
	xres = 1000;
	yres = 700;
	memset(keys, 0, 65536);
	show_credits = 0;
}

Ship::Ship() {
	pos[0] = (Flt)(gl.xres/2);
	pos[1] = (Flt)(gl.yres/8);
	pos[2] = 0.0f;
	VecZero(dir);
	VecZero(vel);
	VecZero(acc);
	angle = 0.0;
	color[0] = color[1] = color[2] = 1.0;
}


Enemy::Enemy() {
	pos[0] = (Flt)(gl.xres/2);
	pos[1] = (Flt)(gl.yres/1.191489362);
	pos[2] = 0.0f;
		VecZero(dir);
	VecZero(vel);
	VecZero(acc);
	angle = 180;
	color[0] = color[1] = color[2] = 1.0;
}


Game::Game() {
	barr = new Bullet[MAX_BULLETS];
	nbullets = 0;
	nenemy = 7;
	clock_gettime(CLOCK_REALTIME, &bulletTimer);
}
Game::~Game() {
	delete [] barr;
}