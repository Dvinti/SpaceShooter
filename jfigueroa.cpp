//Jose Figueroa
 
#include <iostream>
#include <ctime>
#include <cmath>
#include "fonts.h"
#include "game.h"

extern Global gl;  

void show_time(int st)
 {
    Rect r;
    // ****
    r.bot = (gl.yres/2)+20;
    r.left = (gl.xres/2)+340;
    r.center = 0;
    ggprint13(&r, 16, 0xfbfbfa, "* * * * * * * * * * * *");

    // Time
    r.bot = gl.yres/2;
    r.left = (gl.xres/2)+355;
    r.center = 0;
    ggprint13(&r, 16, 0xfbfbfa, "TIME: ");

    r.bot = gl.yres/2;
    r.left = (gl.xres/2)+405;
    r.center = 0;
    
    if (st > 15)
	ggprint13(&r, 16, 0xfbfbfa, "%i", st);
    else
	ggprint13(&r, 16, 0xe92b36, "%i", st);
	
    // sec
    r.bot = gl.yres/2;
    r.left = (gl.xres/2)+425;
    r.center = 0;
    ggprint8b(&r, 16, 0xfbfbfa, "sec ");

    // *****
    r.bot = (gl.yres/2)-25;
    r.left = (gl.xres/2)+340;
    r.center = 0;
    ggprint13(&r, 16, 0xfbfbfa, "* * * * * * * * * * * *");
}

void show_timesup()
{

    Rect r;
    r.bot = gl.yres/2;
    r.left = (gl.xres/2)-50 ;
    r.center = 0;
    ggprint13(&r, 16, 0xe92b36, "Time is up!!!");
}

void show_ui()
{
	Rect r;

	// Name
    r.bot = gl.yres - 60;
    r.left = (gl.xres/2) - 65;
    r.center = 0;
    ggprint13(&r, 16, 0x978eff, "- - - SpaceShooter - - -");

	// SCORE
    r.bot = gl.yres - 665;
    r.left = (gl.xres/2) - 65;
    r.center = 0;
    ggprint13(&r, 16, 0xf3d172, "- - - SCORE - - -");

	// Line under title
    r.bot = gl.yres - 75;
    r.left = 0;
    r.center = 0;
    ggprint8b(&r, 16, 0xf82a27, "                                             "
            " _______________________________________________________________"
            "___________________________");

	// LINE
	r.bot = gl.yres - 635;
	r.left = 0;
	r.center = 0;
	ggprint8b(&r, 16, 0xf82a27, "                                             "
			" _______________________________________________________________"
			"___________________________");
	// left line
	r.bot = gl.yres - 85;
	r.left = 185;
	r.center = 0;

	for (int i = 0; i < 35; i++) {
		ggprint8b(&r, 16, 0xf82a27, "|");
	}

	// right line	
	r.bot = gl.yres - 85;
	r.left = 815;
	r.center = 0;
	for (int i = 0; i < 35; i++) {
		ggprint8b(&r, 16, 0xf82a27, "|");
	}

	// # LIVES
    r.bot = gl.yres - 675;
    r.left = 20;
    r.center = 0;
    ggprint8b(&r, 16, 0x00a1ee, "NUMBER OF LIVES: ");
}

void show_instructions()
{
	Rect r;
	// Quit
    r.bot = gl.yres - 30;
    r.left = 870;
    r.center = 0;
    ggprint8b(&r, 16, 0xb21f32, "! PRESS 'Q' TO QUIT!");

    // HOW TO PLAY
    r.bot = gl.yres - 30;
    r.left = 20;
    r.center = 0;
    ggprint8b(&r, 16, 0x4dbbbe, "HOW TO PLAY?");

    // Instructions
    r.bot = gl.yres - 50;
    r.left = 20;
    r.center = 0;
    ggprint8b(&r, 16, 0xfbfbfa, "Press 'A' to move left and 'D' to move right");
    r.bot = gl.yres - 65;
    r.left = 20;
    r.center = 0;
    ggprint8b(&r, 16, 0xfbfbfa, "Press 'SPACE BAR' to shoot");

	// CREDITS
    r.bot = gl.yres - 675;
    r.left = 830;
    r.center = 0;
    ggprint8b(&r, 16, 0xec8bc2, "Press c to see the credits");
}

void instruct_toggle()
{
	if (gl.show_instructions == 0) {
		gl.show_instructions = 1;
	} else {
		gl.show_instructions = 0;
	}
}

void show_jose_credits(int x, int y)
{
    Rect r;
    r.bot = y;
    r.left = x;
    r.center = 1;
    ggprint8b(&r, 16, 0x00ff0000, "Jose Figueroa");
}
