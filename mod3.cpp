// Group 1
//program: spaceshooter.cpp
//author:  Gordon Griesel
//date:    2014 - 2021
//mod spring 20151: added constructors
//This program is a game starting point for a 3350 project.

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

//defined types
typedef float Flt;
typedef float Vec[3];
typedef Flt Matrix[4][4];

//macros
#define rnd() (((Flt)rand())/(Flt)RAND_MAX)
#define random(a) (rand()%a)
#define VecZero(v) (v)[0]=0.0,(v)[1]=0.0,(v)[2]=0.0
#define MakeVector(x, y, z, v) (v)[0]=(x),(v)[1]=(y),(v)[2]=(z)
#define VecCopy(a,b) (b)[0]=(a)[0];(b)[1]=(a)[1];(b)[2]=(a)[2]
#define VecDot(a,b) ((a)[0]*(b)[0]+(a)[1]*(b)[1]+(a)[2]*(b)[2])
#define VecSub(a,b,c) (c)[0]=(a)[0]-(b)[0]; \
						(c)[1]=(a)[1]-(b)[1]; \
						(c)[2]=(a)[2]-(b)[2]
//constants
const float timeslice = 1.0f;
const float gravity = -0.2f;
#define PI 3.141592653589793
#define ALPHA 1
const int MAX_BULLETS = 1;
const Flt MINIMUM_ASTEROID_SIZE = 60.0;

//-------------------------------------------------------------------------
//Setup timers
const double physicsRate = 1.0 / 60.0;
const double oobillion = 1.0 / 1e9;
extern struct timespec timeStart, timeCurrent;
extern struct timespec timePause;
extern double physicsCountdown;
extern double timeSpan;
extern double timeDiff(struct timespec *start, struct timespec *end);
extern void timeCopy(struct timespec *dest, struct timespec *source);
//-------------------------------------------------------------------------

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
class Ship {
	public:
		Vec pos;
		Vec dir;
		Vec vel;
		Vec acc;
		float angle;
		float color[3];
	public:
		Ship() {
			pos[0] = (Flt)(gl.xres/2);
			pos[1] = (Flt)(gl.yres/8);
			pos[2] = 0.0f;
			VecZero(dir);
			VecZero(vel);
			VecZero(acc);
			angle = 0.0;
			color[0] = color[1] = color[2] = 1.0;
		}
};
 
class Bullet {
	public:
		Vec pos;
		Vec vel;
		float color[3];
		struct timespec time;
	public:
		Bullet() { }
};

class Game {
	public:
		Ship ship;
		Bullet *barr;
		int nasteroids;
		int nbullets;
		struct timespec bulletTimer;
		struct timespec mouseThrustTimer;
	public:
		Game() {
			barr = new Bullet[MAX_BULLETS];
			nasteroids = 0;
			nbullets = 0;
			clock_gettime(CLOCK_REALTIME, &bulletTimer);
		}
		~Game() {
			delete [] barr;
		}
} g;

//X Windows variables
class X11_wrapper {
	private:
		Display *dpy;
		Window win;
		GLXContext glc;
public:
	X11_wrapper() { }
	X11_wrapper(int w, int h) {
		GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
		//GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, None };
		XSetWindowAttributes swa;
		setup_screen_res(gl.xres, gl.yres);
		dpy = XOpenDisplay(NULL);
		if (dpy == NULL) {
			std::cout << "\n\tcannot connect to X server" << std::endl;
			exit(EXIT_FAILURE);
		}
		
		Window root = DefaultRootWindow(dpy);
		XWindowAttributes getWinAttr;
		XGetWindowAttributes(dpy, root, &getWinAttr);
		int fullscreen = 0;
		gl.xres = w;
		gl.yres = h;
		if (!w && !h) {
			//Go to fullscreen.
			gl.xres = getWinAttr.width;
			gl.yres = getWinAttr.height;
			//When window is fullscreen, there is no client window
			//so keystrokes are linked to the root window.
			XGrabKeyboard(dpy, root, False,
					GrabModeAsync, GrabModeAsync, CurrentTime);
			fullscreen=1;
		}
		XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
		if (vi == NULL) {
			std::cout << "\n\tno appropriate visual found\n" << std::endl;
			exit(EXIT_FAILURE);
		}
		Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
		swa.colormap = cmap;
		swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
			PointerMotionMask | MotionNotify | ButtonPress | ButtonRelease |
			StructureNotifyMask | SubstructureNotifyMask;
		unsigned int winops = CWBorderPixel|CWColormap|CWEventMask;
		if (fullscreen) {
			winops |= CWOverrideRedirect;
			swa.override_redirect = True;
		}
		win = XCreateWindow(dpy, root, 0, 0, gl.xres, gl.yres, 0,
				vi->depth, InputOutput, vi->visual, winops, &swa);
		//win = XCreateWindow(dpy, root, 0, 0, gl.xres, gl.yres, 0,
		//vi->depth, InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
		set_title();
		glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
		glXMakeCurrent(dpy, win, glc);
		//show_mouse_cursor(0);
	}
	~X11_wrapper() {
		XDestroyWindow(dpy, win);
		XCloseDisplay(dpy);
	}
	void set_title() {
		//Set the window title bar.
		XMapWindow(dpy, win);
		XStoreName(dpy, win, "Asteroids template");     // Title
	}
	void check_resize(XEvent *e) {
		//The ConfigureNotify is sent by the
		//server if the window is resized.
		if (e->type != ConfigureNotify)
			return;
		XConfigureEvent xce = e->xconfigure;
		if (xce.width != gl.xres || xce.height != gl.yres) {
			//Window size did change.
		reshape_window(xce.width, xce.height);
		}
	}
	void reshape_window(int width, int height) {
		//window has been resized.
		setup_screen_res(width, height);
		glViewport(0, 0, (GLint)width, (GLint)height);
		glMatrixMode(GL_PROJECTION); glLoadIdentity();
		glMatrixMode(GL_MODELVIEW); glLoadIdentity();
		glOrtho(0, gl.xres, 0, gl.yres, -1, 1);
		set_title();
	}
	void setup_screen_res(const int w, const int h) {
		gl.xres = w;
		gl.yres = h;
	}
	void swapBuffers() {
		glXSwapBuffers(dpy, win);
	}
	bool getXPending() {
		return XPending(dpy);
	}
	XEvent getXNextEvent() {
		XEvent e;
		XNextEvent(dpy, &e);
		return e;
	}
} x11(gl.xres, gl.yres);
 // ---> for fullscreen x11(0, 0);

 //function prototypes
 void init_opengl(void);
 //void check_mouse(XEvent *e);        // Might remove
 int check_keys(XEvent *e);
 void physics();
 void render();
 void credit_toggle();

 //==========================================================================
 // M A I N
 //==========================================================================
int main()
{
	logOpen();
	init_opengl();
	srand(time(NULL));
	clock_gettime(CLOCK_REALTIME, &timePause);
	clock_gettime(CLOCK_REALTIME, &timeStart);
	//x11.set_mouse_position(100,100);        // Might remove
	int done=0;
	while (!done) {
		while (x11.getXPending()) {
			XEvent e = x11.getXNextEvent();
			x11.check_resize(&e);
			//check_mouse(&e);
			done = check_keys(&e);
		}
		clock_gettime(CLOCK_REALTIME, &timeCurrent);
		timeSpan = timeDiff(&timeStart, &timeCurrent);
		timeCopy(&timeStart, &timeCurrent);
		physicsCountdown += timeSpan;
		while (physicsCountdown >= physicsRate) {
			physics();
			physicsCountdown -= physicsRate;
		}
		render();
		x11.swapBuffers();
	}
	cleanup_fonts();
	logClose();
	return 0;
}

void init_opengl(void)
{
	//OpenGL initialization
	glViewport(0, 0, gl.xres, gl.yres);
	//Initialize matrices
	glMatrixMode(GL_PROJECTION); glLoadIdentity();
	glMatrixMode(GL_MODELVIEW); glLoadIdentity();
	//This sets 2D mode (no perspective)
	glOrtho(0, gl.xres, 0, gl.yres, -1, 1);
	//
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_FOG);
	glDisable(GL_CULL_FACE);
	//
	//Clear the screen to black
	glClearColor(0.0, 0.0, 0.0, 1.0);
	//Do this to allow fonts
	glEnable(GL_TEXTURE_2D);
	initialize_fonts();
}

void normalize2d(Vec v)
{
	Flt len = v[0]*v[0] + v[1]*v[1];
	if (len == 0.0f) {
		v[0] = 1.0;
		v[1] = 0.0;
		return;
}
	len = 1.0f / sqrt(len);
	v[0] *= len;
	v[1] *= len;
}

int check_keys(XEvent *e)
{
	static int shift=0;
	if (e->type != KeyRelease && e->type != KeyPress) {
		//not a keyboard event
		return 0;
	}
	int key = (XLookupKeysym(&e->xkey, 0) & 0x0000ffff);
	//Log("key: %i\n", key);
	if (e->type == KeyRelease) {
		gl.keys[key] = 0;
		if (key == XK_Shift_L || key == XK_Shift_R)
			shift = 0;
		return 0;
	}
	if (e->type == KeyPress) {
		//std::cout << "press" << std::endl;
		gl.keys[key]=1;
		if (key == XK_Shift_L || key == XK_Shift_R) {
			shift = 1;
			return 0;
		}
	}
	(void)shift;
	float speed, speedD = 0;
	switch (key) {
		case XK_Escape:
			return 1;
		case XK_f:
			break;
		case XK_s:
			break;
		case XK_Down:
			break;
		case XK_equal:
			break;
		case XK_minus:
			break;

		case XK_a:
			g.ship.pos[0] += -8.0;
			if (g.ship.pos[0] < 0.0) {
				g.ship.pos[0] = 0.0;
			}
			else if (g.ship.pos[0] > (float)gl.xres) {
				g.ship.pos[0] = (float)gl.xres - 2.0;
			}
			break;
		case XK_d:
			g.ship.pos[0] += 8.0;
			if (g.ship.pos[0] < 0.0) {
				g.ship.pos[0] = 0.0;
			}
			else if (g.ship.pos[0] > (float)gl.xres) {
				g.ship.pos[0] = (float)gl.xres - 2.0;
			}
			break;
		case XK_c:
			credit_toggle();
			break;
		case XK_q:
			exit(0);
			break;
		case XK_space:
			//move shoot
			struct timespec bt;
			clock_gettime(CLOCK_REALTIME, &bt);
			double ts = timeDiff(&g.bulletTimer, &bt);
			if (ts > 0.1) {
				timeCopy(&g.bulletTimer, &bt);
				if (g.nbullets < MAX_BULLETS) {
					Bullet *b = &g.barr[g.nbullets];
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
			break;
		
	}
	return 0;
}
/*
	void move_left()	
	{
	double mv = 0;
	gl.xres -= mv * 200;
	if (gl.xres < 1000)
	gl.xres = 1000;
	}

	void move_right()
	{
	double mv = 0;
	gl.xres += mv * 200;
	if (gl.xres > (gl.yres/12))
	gl.xres = (gl.yres/12);
	}
	*/

void credit_toggle()
{
	if (gl.show_credits == 0) {
		gl.show_credits = 1;
	}
	else {
		gl.show_credits = 0;
	}
}

void physics()
{
	//Update ship position
	g.ship.pos[0] += g.ship.vel[0];
	g.ship.pos[1] += g.ship.vel[1];
	
	//Check for collision with window edges
	if (g.ship.pos[0] < 0.0) {
		g.ship.pos[0] += (float)gl.xres;
	}
	else if (g.ship.pos[0] > (float)gl.xres) {
		g.ship.pos[0] -= (float)gl.xres;
	}
	else if (g.ship.pos[1] < 0.0) {
		g.ship.pos[1] += (float)gl.yres;
	}
	else if (g.ship.pos[1] > (float)gl.yres) {
		g.ship.pos[1] -= (float)gl.yres;
	}

	//Update bullet positions
	struct timespec bt;
	clock_gettime(CLOCK_REALTIME, &bt);
	int i = 0;
	while (i < g.nbullets) {
		Bullet *b = &g.barr[i];
		//How long has bullet been alive?
		double ts = timeDiff(&b->time, &bt);
		if (ts > 2.5) {
			//time to delete the bullet.
			memcpy(&g.barr[i], &g.barr[g.nbullets-1],
					sizeof(Bullet));
			g.nbullets--;
			//do not increment i.
			continue;
		}
		
		//move the bullet
		b->pos[0] += b->vel[0];
		b->pos[1] += b->vel[1];
		
		//Check for collision with window edges
		if (b->pos[0] < 0.0) {
			b->pos[0] += (float)gl.xres;
		}
		else if (b->pos[0] > (float)gl.xres) {
			b->pos[0] -= (float)gl.xres;
		}
		else if (b->pos[1] < 0.0) {
			b->pos[1] += (float)gl.yres;
		}
		else if (b->pos[1] > (float)gl.yres) {
			b->pos[1] -= (float)gl.yres;
		}
		++i;
	}
}

// Credit Screen
extern void show_Daniels_credits(int, int);
extern void show_frankie_credits(int, int);
extern void show_enrique_credits(int, int);
extern void show_jennipher_credits(int, int);
extern void show_jose_credits(int, int);

// Movement


void render()
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

	// CREDITS
	r.bot = gl.yres - 675;
	r.left = 830;
	r.center = 0;
	ggprint8b(&r, 16, 0xec8bc2, "Press c to see the credits");

	if (gl.show_credits) {
		// Clears the Screen
		glClear(GL_COLOR_BUFFER_BIT);

		// Shows the student's credit
		r.bot = gl.yres- 80 ;
		r.left = (gl.xres /2) - 20 ;
		r.center = 0;
		ggprint16(&r, 16, 0x00a1ee, "Credits");
		show_Daniels_credits(gl.xres/2, gl.yres/2);
		show_frankie_credits(gl.xres/2, (gl.yres - 20)/2);
		show_enrique_credits(gl.xres/2, (gl.yres - 40) /2);
		show_jennipher_credits(gl.xres/2, (gl.yres - 60) /2);
		show_jose_credits(gl.xres/2, (gl.yres - 80) /2);

		// Instructions
		r.bot = gl.yres - 20;
		r.left = 10;
		r.center = 0;
		ggprint8b(&r, 16, 0x00a1ee, "Press c to return to the main screen");
	}

	//--------------------------------------------------------------------
	//Draw the ship
	glColor3fv(g.ship.color);
	glPushMatrix();
	glTranslatef(g.ship.pos[0], g.ship.pos[1], g.ship.pos[2]);
	//float angle = atan2(ship.dir[1], ship.dir[0]);
	glRotatef(g.ship.angle, 0.0f, 0.0f, 1.0f);
	glBegin(GL_TRIANGLES);
	glVertex2f(-12.0f, -10.0f);
	glVertex2f(  0.0f,  20.0f);
	glVertex2f(  0.0f,  -6.0f);
	glVertex2f(  0.0f,  -6.0f);
	glVertex2f(  0.0f,  20.0f);
	glVertex2f( 12.0f, -10.0f);
	glEnd();
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POINTS);
	glVertex2f(0.0f, 0.0f);
	glEnd();
	glPopMatrix();
	glEnd();

	//-------------------------------------------------------------------------
	//Draw the bullets
	for (int i=0; i<g.nbullets; i++) {
		Bullet *b = &g.barr[i];
		//Log("draw bullet...\n");
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_POINTS);
		glVertex2f(b->pos[0],      b->pos[1]);
		glVertex2f(b->pos[0]-1.0f, b->pos[1]);
		glVertex2f(b->pos[0]+1.0f, b->pos[1]);
		glVertex2f(b->pos[0],      b->pos[1]-1.0f);
		glVertex2f(b->pos[0],      b->pos[1]+1.0f);
		glColor3f(0.8, 0.8, 0.8);
		glVertex2f(b->pos[0]-1.0f, b->pos[1]-1.0f);
		glVertex2f(b->pos[0]-1.0f, b->pos[1]+1.0f);
		glVertex2f(b->pos[0]+1.0f, b->pos[1]-1.0f);
		glVertex2f(b->pos[0]+1.0f, b->pos[1]+1.0f);
		glEnd();
	}
}                                            

