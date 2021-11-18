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
#include "game.h"

using namespace std;

//#include "header.h"

const float timeslice = 1.0f;
const float gravity = -0.2f;
const int MAX_BULLETS = 100;
#define PI 3.141592653589793
#define ALPHA 1
//const Flt MINIMUM_ASTEROID_SIZE = 60.0;

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

Global::Global() {
	xres = 1000;
	yres = 700;
	memset(keys, 0, 65536);
	show_credits = 0;
    show_instructions = 0;
};

Global gl;

Ship::Ship() {
    pos[0] = (Flt)(gl.xres/2);
    pos[1] = (Flt)(gl.yres/8);
    pos[2] = 0.0f;
    VecZero(dir);
    VecZero(vel);
    VecZero(acc);
    angle = 0.0;
    color[0] = color[1] = color[2] = 1.0;
};

Enemy::Enemy() {
    //seed random time
    srand (time(NULL));
    //rand()%(max-min + 1) + min;
    pos[0] = (Flt)(rand() % (815 - 185 + 1) + 185);
    //pos[0] = (Flt)(gl.xres/2);
    pos[1] = (Flt)(gl.yres/1.1589362);
    pos[2] = 0.0f;
    VecZero(dir);
    VecZero(vel);
    VecZero(acc);
    angle = 180;
    color[0] = color[1] = color[2] = 1.0;
};

Bullet::Bullet() {};

Game::Game() {
    barr = new Bullet[MAX_BULLETS];
    nbullets = 0;
    nenemy = 7;
    clock_gettime(CLOCK_REALTIME, &bulletTimer);
}

Game::~Game() {
    delete [] barr;
}

Game g;

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
//  XStoreName(dpy, win, "Asteroids template");     // Title
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
int check_keys(XEvent *e);
void physics();
void render();
int score = 0;

//==========================================================================
// M A I N
//==========================================================================
int main() {
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

void init_opengl(void) {
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

void normalize2d(Vec v) {
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

int check_keys(XEvent *e) {
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
            extern void move_ship_left();
            move_ship_left();
            break;
        case XK_d:
            extern void move_ship_right();
            move_ship_right();
            break;
        case XK_c:
            extern void credit_toggle();
            credit_toggle();
            break;
        case XK_i:
            extern void instruct_toggle();
            instruct_toggle();
            break;
        case XK_q:
            return 1;
            break;
        case XK_space:
            extern void shoot_bullets();
            shoot_bullets();
            break;
	}
    return 0;
}

void physics() {
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
        extern void bullet_bounds_check(Bullet *b, int i);
        bullet_bounds_check(b, i);

        //move the bullet
        b->pos[0] += b->vel[0];
        b->pos[1] += b->vel[1];

        //cout << "bullet POS " << b->pos[0] << " " << b->pos[1] << endl;
        //Check for collision with window edges

//********************************WE CAN DELETE THIS***********************************************************
        if (b->pos[0] < 0.0) {
            b->pos[0] += (float)gl.xres;
        }
        else if (b->pos[0] > (float)gl.xres) {
            b->pos[0] -= (float)gl.xres;
        }
        else if (b->pos[1] < 0.0) {
            cout << "out of bounds bottom" << endl;
            //b->pos[1] += (float)gl.yres;
        }
        else if (b->pos[1] > (float)gl.yres) {
            //b->pos[1] -= (float)gl.yres;
            //break; //We can kill the bullets this way but its laggy
        }
//***********************************************************************************************************        
        extern int check_bullet_collision(int i, Bullet *b, int score);
        score = check_bullet_collision(i, b, score);
        ++i;
    }

// bullet hits enemy
}

// Credit Screen
extern void show_Daniels_credits(int, int);
extern void show_frankie_credits(int, int);
extern void show_enrique_credits(int, int);
extern void show_jennipher_credits(int, int);
extern void show_jose_credits(int, int);

void render() {
    Rect r;
    glClear(GL_COLOR_BUFFER_BIT);

    // Show UI
    extern void show_ui();
    show_ui();

    // Calculate Score
    extern void show_scores(int);
    show_scores(score);

    // Show Instructions
    if (gl.show_instructions) {
        extern void show_instructions();
        show_instructions();
    }

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

    //--------------------------------------------------------------------

    //Draw the enemy ship, rows = 3, col = 6
    glColor3fv(g.enemy.color);
    glPushMatrix();
    glTranslatef(g.enemy.pos[0], g.enemy.pos[1], g.enemy.pos[2]);
    glRotatef(g.enemy.angle, 0.0f, 0.0f, 1.0f);
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
    glVertex2f(5.0f, 0.0f);
    glEnd();
    glPopMatrix();
    glEnd();

    //-------------------------------------------------------------------------

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
        glEnd();
    }
}
