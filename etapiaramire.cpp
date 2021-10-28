//Enrique Tapia

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


void show_enrique_credits(int x, int y)
{
	Rect r;
	r.bot = y;
	r.left = x;
	r.center = 1;
	ggprint8b(&r, 16, 0x00ff0000, "Enrique Tapia");
}

void credit_toggle()
{
	if (gl.show_credits == 0) {
		gl.show_credits = 1;
	}
	else {
		gl.show_credits = 0;
	}
}