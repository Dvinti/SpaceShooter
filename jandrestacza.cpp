/***************************************
Author: Jennipher Andres Tacza
File: jandrestacza.cpp
Date: September 30, 2021
***************************************/
#include <iostream>
#include <ctime>
#include <cmath>
#include "fonts.h"
#include "game.h"

extern Global gl;

void welcomeDisplay()
{
	Rect r;
    unsigned int c = 0x00ffff44;
	r.bot = gl.yres - 180;
	r.left = 300;
	r.center = 0;
	ggprint8b(&r, 16, c, "x - Play");
	ggprint8b(&r, 16, c, "            ");
	ggprint8b(&r, 16, c, "            ");
	ggprint8b(&r, 16, c, "            ");
	//ggprint8b(&r, 16, c, "h - How to play");
	ggprint8b(&r, 16, c, "            ");
	ggprint8b(&r, 24, c, "            ");
	//ggprint8b(&r, 60, c, "c - Credits");
	//ggprint8b(&r, 16, c, "e - Exit");	
}
void startWindow()
{
    if (gl.startUpDisplay == 0){
        gl.startUpDisplay =1;
    }else{
        gl.startUpDisplay = 0;
    }
}
void show_jennipher_credits(int x, int y)
{
    Rect r;
    r.bot = y;
    r.left = x;
    r.center = 1;
    ggprint8b(&r, 16, 0X00ff0000, "Jennipher Andres");
}
