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
#include <GL/glut.h>
extern Global gl;

void welcomeDisplay()
{
	Rect r;
    glClear(GL_COLOR_BUFFER_BIT);
    r.bot = gl.yres - 250;
	r.left = 430;
	r.center = 0;
	ggprint8b(&r, 16, 0x00ffff44, "X - Play");
	ggprint8b(&r, 16, 0x00ffff44, "            ");
	ggprint8b(&r, 16, 0x00ffff44, "            ");
	ggprint8b(&r, 16, 0x00ffff44, "            ");
	ggprint8b(&r, 16, 0x00ffff44, "S - Instructions");
	ggprint8b(&r, 16, 0x00ffff44, "            ");
	ggprint8b(&r, 16, 0x00ffff44, "            ");
    ggprint8b(&r, 16, 0x00ffff44, "            ");
	ggprint8b(&r, 16, 0x00ffff44," C - Credits");
    ggprint8b(&r, 16, 0x00ffff44, "            ");
    ggprint8b(&r, 16, 0x00ffff44, "            ");
    ggprint8b(&r, 16, 0x00ffff44, "            ");
	ggprint8b(&r, 16, 0x00ffff44, "Q - Quit");	
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
