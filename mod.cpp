// Group #1
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
#include "game.h"
using namespace std;

const float timeslice = 1.0f;
const float gravity = -0.2f;
const int MAX_BULLETS = 100;
const int MAX_ENEMIES = 0;
#define PI 3.141592653589793
#define ALPHA 1
const Flt MINIMUM_ASTEROID_SIZE = 20.0;

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

//probably not necessary \/\/\/
Display* disp = XOpenDisplay(NULL);
Screen*  scrn = DefaultScreenOfDisplay(disp);
int height = scrn->height;
int width  = scrn->width;

Global::Global()
{
	xres = 1000;
	yres = 700;
	memset(keys, 0, 65536);
	show_credits = 0;
	show_instructions = 0;
	startUpDisplay = 0;
	paused = false;
	Background1 = 0;
	Background2 = 0;
	BackgroundTitle = 1;
	Highscore = 0;
};

Global gl;

Ship::Ship()
{
	pos[0] = (Flt)(gl.xres/2);
	pos[1] = (Flt)(gl.yres/7);
	pos[2] = 0.0f;
	VecZero(dir);
	VecZero(vel);
	VecZero(acc);
	angle = 0.0;
	color[0] = color[1] = color[2] = 1.0;
};

Bullet::Bullet() {};

Asteroid::Asteroid()
{
	prev = NULL;
	next = NULL;
};

Game::Game()
{
	ahead = NULL;
	barr = new Bullet[MAX_BULLETS];
	nasteroids = 0;
	nbullets = 0;
	srand (time(NULL));
	//build Asteroids...
	for (int j = 0; j < 5; j++) {
		Asteroid *a = new Asteroid;
		a->nverts = 3;  //number of vertices
		a->radius = 30.0; //size of asteroid
		//a->radius = rnd()*80.0 + 40.0;
		Flt r2 = a->radius / 2.0;
		Flt angle = 0.0f;   //starting angle
		Flt inc = (PI * 2.0) / (Flt)a->nverts;  //angle increment
		for (int i=0; i<a->nverts; i++) {
			a->vert[i][0] = sin(angle) * (r2 + rnd() * a->radius);
			a->vert[i][1] = cos(angle) * (50);
			angle += inc;
		}
		a->pos[0] = (Flt)(rand() % (800 - 200 + 1) + 200); //x position
		a->pos[1] = (Flt)(gl.yres/1.1589362); //y position
		a->pos[2] = 0.0f; //z position
		a->angle = 0.0;
		a->rotate = rnd() * 4.0 - 2.0;
		a->color[0] = 0.0;
		a->color[1] = 1.0;
		a->color[2] = 1.0;
		a->vel[0] = (Flt)(rnd()*2.0-1.0); //velocity in x direction
		a->vel[1] = (Flt)(rnd()*2.0-1.0); //velocity in y direction
		//std::cout << "asteroid" << std::endl;
		//add to front of linked list
		a->next = ahead;
		if (ahead != NULL)
			ahead->prev = a;
		ahead = a;
		++nasteroids;
	}
	clock_gettime(CLOCK_REALTIME, &bulletTimer);
};

Game::~Game() {
	delete [] barr;
};

Game g;

/* Prototype */
extern void init();

//X Windows variables
class X11_wrapper
{
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
			XStoreName(dpy, win, "SpaceShooter");     // Title
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
			init();
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
int check_keys(XEvent *e);
extern void check_mouse(XEvent *e);
void physics();
void render();

/* Initialize */
int score = 0;
int highscore = 0;
float lives = 10.0;
float st = 30.0;

/* Booleans */
bool done = false;

//==========================================================================
// M A I N
//==========================================================================
int main()
{
	logOpen();
	init_opengl();
	init();
	srand(time(NULL));
	clock_gettime(CLOCK_REALTIME, &timePause);
	clock_gettime(CLOCK_REALTIME, &timeStart);
	//x11.set_mouse_position(100,100);        // Might remove
	while (!done) {
		while (x11.getXPending()) {
			XEvent e = x11.getXNextEvent();
			x11.check_resize(&e);
			check_mouse(&e);
			check_keys(&e);
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

Image img[8] = {
	"./images/Background_game.png",
	"./images/Background_game2.png",
	"./images/SpaceShooter.png",
	"./images/MainShip.png",
	"./images/EnemyShip.png",
	"./images/Highscore.png",
	"./images/bullet.png",
	"./images/explosion.png"};

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

	glGenTextures(1, &gl.Background2Texture);
	glGenTextures(1, &gl.Background1Texture);
	glGenTextures(1, &gl.BackgroundTitleTexture);
	glGenTextures(1, &gl.HighscoreTexture);
	glGenTextures(1, &gl.MainShipTexture);
	glGenTextures(1, &gl.EnemyTexture);
	glGenTextures(1, &gl.LaserTexture);

	extern unsigned char *buildAlphaData(Image *image);
	extern void build_imageTexture(GLuint texid);

	build_imageTexture(gl.MainShipTexture);
	/*
	 *
	 * Must free data
	 *
	 */
	unsigned char *MainShipData = buildAlphaData(&img[3]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img[3].width, img[3].height, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, MainShipData);

	free(MainShipData);

	build_imageTexture(gl.EnemyTexture);
	/*
	 *
	 * Must free data
	 *
	 */
	unsigned char *EnemyData = buildAlphaData(&img[4]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img[4].width, img[4].height, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, EnemyData);

	free(EnemyData);

	build_imageTexture(gl.LaserTexture);
	/*
	 *
	 * Must free data
	 *
	 */
	unsigned char *LaserData = buildAlphaData(&img[6]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img[6].width, img[6].height, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, LaserData);

	free(LaserData);

	//Code for SpaceShooter title
	build_imageTexture(gl.BackgroundTitleTexture);

	/*
	 *
	 * Must free data
	 *
	 */
	unsigned char *BackgroundTitleData = buildAlphaData(&img[2]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img[2].width, img[2].height, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, BackgroundTitleData);

	free(BackgroundTitleData);

	//Code for Background1

	build_imageTexture(gl.Background1Texture);

	/*
	 *
	 * Must free data
	 *
	 */
	unsigned char *Background1Data = buildAlphaData(&img[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img[0].width, img[0].height, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, Background1Data);

	free(Background1Data);

	//Code for Background2

	build_imageTexture(gl.Background2Texture);

	/*
	 *
	 * Must free data
	 *
	 */
	unsigned char *Background2Data = buildAlphaData(&img[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img[1].width, img[1].height, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, Background2Data);

	free(Background2Data);

	//Code for Highscore

	build_imageTexture(gl.HighscoreTexture);

	/*
	 *
	 * Must free data
	 *
	 */
	unsigned char *Highscore = buildAlphaData(&img[5]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img[5].width, img[5].height, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, Highscore);

	free(Highscore);
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
	static int shift = 0;
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

	// Movement Controls
	extern void pause_game();
	if (!gl.paused && !gl.BackgroundTitle && !gl.Highscore) {
		switch (key) {
			case XK_Left:
				extern void move_ship_left();
				move_ship_left();
				break;
			case XK_Right:
				extern void move_ship_right();
				move_ship_right();
				break;
			case XK_a:
				move_ship_left();
				break;
			case XK_d:
				move_ship_right();
				break;
			case XK_space:
				void shoot_bullets();
				shoot_bullets();
				break;
		}
	}
	// Menu Controls
	switch (key) {
		case XK_Escape:
			done = 1;
			break;
		case XK_x:
			//extern void startWindow();
			//startWindow();
			break;
		case XK_s:
			extern void instruct_toggle();
			instruct_toggle();
			pause_game();
			break;
		case XK_c:
			extern void credit_toggle();
			credit_toggle();
			pause_game();
			break;
		case XK_q:
			done = 1;
			break;
		case XK_p:
			pause_game();
			break;
	}
	return 0;
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

void deleteAsteroid(Game *g, Asteroid *node)
{
	//Remove a node from doubly-linked list
	//Must look at 4 special cases below.
	if (node->prev == NULL) {
		if (node->next == NULL) {
			//only 1 item in list.
			g->ahead = NULL;
		} else {
			//at beginning of list.
			node->next->prev = NULL;
			g->ahead = node->next;
		}
	} else {
		if (node->next == NULL) {
			//at end of list.
			node->prev->next = NULL;
		} else {
			//in middle of list.
			node->prev->next = node->next;
			node->next->prev = node->prev;
		}
	}
	delete node;
	node = NULL;
}

void buildAsteroidFragment(Asteroid *ta, Asteroid *a)
{
	//build ta from a
	ta->nverts = 8;
	ta->radius = a->radius / 2.0;
	Flt r2 = ta->radius / 2.0;
	Flt angle = 0.0f;
	Flt inc = (PI * 2.0) / (Flt)ta->nverts;

	for (int i=0; i<ta->nverts; i++) {
		ta->vert[i][0] = sin(angle) * (r2 + rnd() * ta->radius);
		ta->vert[i][1] = cos(angle) * (r2 + rnd() * ta->radius);
		angle += inc;
	}

	ta->pos[0] = a->pos[0] + rnd()*10.0-5.0;
	ta->pos[1] = a->pos[1] + rnd()*10.0-5.0;
	ta->pos[2] = 0.0f;
	ta->angle = 0.0;
	ta->rotate = a->rotate + (rnd() * 4.0 - 2.0);
	ta->color[0] = 0.8;
	ta->color[1] = 0.8;
	ta->color[2] = 0.7;
	ta->vel[0] = a->vel[0] + (rnd()*2.0-1.0);
	ta->vel[1] = abs(a->vel[1] + (rnd()*2.0-1.0));
	//std::cout << "frag" << std::endl;
}

extern int ship_enemy_collision(Asteroid *a);
extern int enemy_boundary_check(Asteroid *a);
extern int score_new_lives(int score, float lives);
extern int score_new_lives_type2(int score);

void physics()
{
	Flt d0,d1,dist;
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

	if (!gl.paused && !gl.BackgroundTitle && !gl.Highscore) {
		//Update bullet positions
		struct timespec bt;
		clock_gettime(CLOCK_REALTIME, &bt);
		int i = 0;
		while (i < g.nbullets) {
			Bullet *b = &g.barr[i];
			extern void bullet_bounds_check(Bullet *b, int i);
			bullet_bounds_check(b, i);

			//move the bullet
			b->pos[0] += b->vel[0];
			b->pos[1] += b->vel[1];

			//cout << "bullet POS " << b->pos[0] << " " << b->pos[1] << endl;
			//Check for collision with window edges

			//********************************WE CAN DELETE THIS***********************************************************
			if (b->pos[0] < 0.0) {
				cout << "IF STATEMENT USED" << endl;
				b->pos[0] += (float)gl.xres;
			}
			else if (b->pos[0] > (float)gl.xres) {
				cout << "IF STATEMENT USED" << endl;
				b->pos[0] -= (float)gl.xres;
			}
			else if (b->pos[1] < 0.0) {
				cout << "IF STATEMENT USED" << endl;
				//b->pos[1] += (float)gl.yres;
			}
			else if (b->pos[1] > (float)gl.yres) {
				cout << "IF STATEMENT USED" << endl;
				//b->pos[1] -= (float)gl.yres;
				//break; //We can kill the bullets this way but its laggy
			}
			//***********************************************************************************************************
			++i;
		}
		//
		//Update asteroid positions
		Asteroid *a = g.ahead;
		while (a) {
			a->pos[0] += a->vel[0];
			a->pos[1] += a->vel[1];

			//if collision happened reduce lives
			if (ship_enemy_collision(a) == true) {
				cout << "collision true lives before: " << lives << endl;
				lives -= 0.5;
				cout << "collision true lives after deduction: " << lives << endl;
				score += 25;
				if (score_new_lives_type2(score) == true) {
					if (lives > 3.0)
						lives = 4.0;
					else
						lives += 1.0;
					cout << "type 2: new lives added"<<endl<<endl;
				}

				// cout << "type 2 lives after: " << lives << endl;
			}
			else {
				if (enemy_boundary_check(a) == true) {
					cout << "returned true lives before: " << lives << endl;
					lives -= 1.0;
					cout << "returned true lives after: " << lives << endl;
				}
			}
			//a->angle += a->rotate;
			a = a->next;
		}

		//cout << "lives: " << lives << endl;
		//
		//Asteroid collision with bullets?
		//If collision detected:
		//     1. delete the bullet
		//     2. break the asteroid into pieces
		//        if asteroid small, delete it
		a = g.ahead;
		while (a) {
			//is there a bullet within its radius?
			int i=0;
			while (i < g.nbullets) {
				Bullet *b = &g.barr[i];
				d0 = b->pos[0] - a->pos[0];
				d1 = b->pos[1] - a->pos[1];
				dist = (d0*d0 + d1*d1);
				if (dist < (a->radius*a->radius)) {
					//std::cout << "asteroid hit." << std::endl;
					//this asteroid is hit.
					if (a->radius > MINIMUM_ASTEROID_SIZE) {
						//break it into pieces.
						Asteroid *ta = a;
						buildAsteroidFragment(ta, a);
						int r = rand()%5; //random number between 0 and 4
						for (int k=0; k<r; k++) {
							//get the next asteroid position in the array
							Asteroid *ta = new Asteroid;
							buildAsteroidFragment(ta, a);
							//add to front of asteroid linked list
							ta->next = g.ahead;
							if (g.ahead != NULL)
								g.ahead->prev = ta;
							g.ahead = ta;
							g.nasteroids++;
						}
						score += 100;
						lives = score_new_lives(score, lives);
					}
					else {
						a->color[0] = 1.0;
						a->color[1] = 0.1;
						a->color[2] = 0.1;
						//asteroid is too small to break up
						//delete the asteroid and bullet
						Asteroid *savea = a->next;
						deleteAsteroid(&g, a);
						a = savea;
						g.nasteroids--;
						score += 50;
						lives = score_new_lives(score, lives);
					}
					//delete the bullet...
					memcpy(&g.barr[i], &g.barr[g.nbullets-1], sizeof(Bullet));
					g.nbullets--;
					if (a == NULL)
						break;
				}
				i++;
			}
			if (a == NULL)
				break;
			a = a->next;
		}
	}
	//---------------------------------------------------
}

// Background Images
extern void show_background(int x, int y, GLuint texid);

// Credit Screen
extern void show_Daniels_credits(int, int);
extern void show_frankie_credits(int, int);
extern void show_enrique_credits(int, int);
extern void show_jennipher_credits(int, int);
extern void show_jose_credits(int, int);

void render() {
	Rect r;

	//extern void welcomeDisplay();
	//welcomeDisplay();

	/* Title Screen */
	if (gl.BackgroundTitle) {
		glColor3f(1.0, 1.0, 1.0);
		show_background(gl.xres,gl.yres,gl.BackgroundTitleTexture);

		extern void PlayButton();
		PlayButton();
	}
	// working on the start window
	if (gl.startUpDisplay) {

		if (gl.Background1) {
			glColor3f(1.0, 1.0, 1.0);
			show_background(gl.xres,gl.yres,gl.Background1Texture);
		}

		extern void print_pause();
		print_pause();

		// Show UI
		extern void show_ui();
		extern void shooting_line();
		show_ui();
		shooting_line();

		//Draw the user's ship
		{
			glColor3fv(g.ship.color);
			glPushMatrix();
			glTranslatef(g.ship.pos[0], g.ship.pos[1], g.ship.pos[2]);
			//float angle = atan2(ship.dir[1], ship.dir[0]);
			glRotatef(g.ship.angle, 0.0f, 0.0f, 1.0f);

			extern void map_texture(GLuint texid, float w);
			map_texture(gl.MainShipTexture,25.0f);
		}

		//--------------------------------------------------------------------
		//Draw the enemy ships
		Asteroid *a = g.ahead;
		while (a)  {
			//Log("draw asteroid...\n");
			glColor3fv(a->color);
			glPushMatrix();
			glTranslatef(a->pos[0], a->pos[1], a->pos[2]);
			glRotatef(a->angle, 0.0f, 0.0f, 1.0f);

			/* Draws Triangles */
			/*glBegin(GL_LINE_LOOP);
			//Log("%i verts\n",a->nverts);
			for (int j=0; j<a->nverts; j++) {
				glVertex2f(a->vert[j][0], a->vert[j][1]);
			}
			glEnd();*/

			void map_texture(GLuint texid, float w);
			map_texture(gl.EnemyTexture,30.0f);

			glEnd();
			a = a->next;
		}

		//Draw the bullets
		for (int i = 0; i < g.nbullets; i++) {
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
		}

		// Show Instructions
		if (gl.show_instructions) {
			extern void show_instructions();
			show_instructions();
		} else {
			extern void instructions();
			instructions();
		}

		// Calculate Score
		extern void show_scores(int);
		show_scores(score);

		// Calculate Lives
		extern void show_lives(float);
		show_lives(lives);

		// Time: 30 sec
		if (!gl.paused) {
			if (st > 0) {
			extern void show_time(float);
			show_time(st);
			st -= 0.01;
		} else {
			extern void show_timesup();
			show_timesup();
			}
		}
		// Credit Screen
		if (gl.show_credits) {
			// Clears the Screen
			glClear(GL_COLOR_BUFFER_BIT);

			// Shows the student's credit
			r.bot = gl.yres - 80;
			r.left = (gl.xres/2) - 20;
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
	}

	if (lives <= 0) {
		gl.Highscore = 1;
		glColor3f(1.0, 1.0, 1.0);
		show_background(gl.xres,gl.yres,gl.HighscoreTexture);

		if (gl.Highscore) {
			/*
			   if (button[2].over) {
			   glColor3f(1.0f, 0.0f, 0.0f);
			   glLineWidth(2);
			   glBegin(GL_LINE_LOOP);
			   glVertex2i(button[2].r.left-2,  button[2].r.bot-2);
			   glVertex2i(button[2].r.left-2,  button[2].r.top+2);
			   glVertex2i(button[2].r.right+2, button[2].r.top+2);
			   glVertex2i(button[2].r.right+2, button[2].r.bot-2);
			   glVertex2i(button[2].r.left-2,  button[2].r.bot-2);
			   glEnd();
			   glLineWidth(1);
			   }
			   if (button[2].down) {
			   glColor3fv(button[2].dcolor);
			   } else {
			   glColor3fv(button[2].color);
			   }
			   glBegin(GL_QUADS);
			   glVertex2i(button[2].r.left,  button[2].r.bot);
			   glVertex2i(button[2].r.left,  button[2].r.top);
			   glVertex2i(button[2].r.right, button[2].r.top);
			   glVertex2i(button[2].r.right, button[2].r.bot);
			   glEnd();
			   r.left = button[2].r.centerx;
			   r.bot  = button[2].r.centery-8;
			   r.center = 1;
			   if (button[2].down) {
			   ggprint16(&r, 0, button[2].text_color, "Pressed!");
			   } else {
			   ggprint16(&r, 0, button[2].text_color, button[2].text);
			   }*/
		}

		r.bot = gl.yres - 150;
		r.left = gl.xres/2 -100;
		r.center = 0;
		ggprint40(&r, 16, 0xfbfbfa, "Highscores");

		r.bot = gl.yres - 285;
		r.left = 280;
		r.center = 0;
		ggprint40(&r, 16, 0xfbfbfa, "1.  Player 1");

		r.bot = gl.yres - 380;
		r.left = 280;
		r.center = 0;
		ggprint40(&r, 16, 0xfbfbfa, "2. Player 2");

		r.bot = gl.yres - 462;
		r.left = 280;
		r.center = 0;
		ggprint40(&r, 16, 0xfbfbfa, "3. Player 3");

		r.bot = gl.yres - 545;
		r.left = 280;
		r.center = 0;
		ggprint40(&r, 16, 0xfbfbfa, "4. Player 4");	

		r.bot = gl.yres - 285;
		r.left = 600;
		r.center = 0;
		if (score > highscore) {
			ggprint40(&r, 16, 0xfbfbfa, "%0.5i", score);
		} else {
			ggprint40(&r, 16, 0xfbfbfa, "%0.5i", highscore);
		}
		r.bot = gl.yres - 380;
		r.left = 600;
		r.center = 0;
		ggprint40(&r, 16, 0xfbfbfa, "00500");

		r.bot = gl.yres - 462;
		r.left = 600;
		r.center = 0;
		ggprint40(&r, 16, 0xfbfbfa, "00350");

		r.bot = gl.yres - 545;
		r.left = 600;
		r.center = 0;
		ggprint40(&r, 16, 0xfbfbfa, "00250");	
	}

	return;
}
