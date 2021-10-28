//Jose Figueroa
 
#include "fonts.h"
#include <GL/glx.h>
#include <cstring>

class Global {
	public:
		int xres, yres;
		char keys[65536];
		int show_credits;
		int move_left;
		int move_right;
		Global() {
			xres = 1000;
			yres = 700;
			memset(keys, 0, 65536);
			show_credits = 0;
			move_left = 0;
			move_right = 0;
		}
} gl;

void show_game_layout()
{
Rect r;
	glClear(GL_COLOR_BUFFER_BIT);

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

	// SCORE #
	r.bot = gl.yres - 690;
	r.left = (gl.xres/2) - 65;
	r.center = 0;
	ggprint13(&r, 16, 0xfbfbfa, "      <0000>      ");

	// Quit
	r.bot = gl.yres - 30;
	r.left = 870;
	r.center = 0;
	ggprint8b(&r, 16, 0xb21f32, "! PRESS 'Q' TO QUIT!");

	// # LIVES
	r.bot = gl.yres - 675;
	r.left = 20;
	r.center = 0;
	ggprint8b(&r, 16, 0x00a1ee, "NUMBER OF LIVES: ");

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

	// LINE
	r.bot = gl.yres - 635;
	r.left = 0;
	r.center = 0;
	ggprint8b(&r, 16, 0xf82a27, "___________________________________________"
			"_______________________________________________________________"
			"_____________________________________");
}

void show_jose_credits(int x, int y)
{
    Rect r;
    r.bot = y;
    r.left = x;
    r.center = 1;
    ggprint8b(&r, 16, 0x00ff0000, "Jose Figueroa");
}
