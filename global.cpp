#include "global.h"
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include <iostream>

Global::Global() {
	memset(keys, 0, 65536);
	show_credits = 0;
}