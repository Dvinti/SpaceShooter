//Frankie Sanchez
#include "fonts.h"

void show_frankie_credits(int x, int y) {

    Rect r;
    r.bot = y;
    r.left = x;
    r.center = 1;
    ggprint8b(&r, 16, 0x00ff0000, "Frankie Sanchez");

}
