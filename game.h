#ifndef SS_GAME_H
#define SS_GAME_H

#include <GL/glx.h>

//defined types
typedef float Flt;
typedef float Vec[3];
typedef Flt Matrix[4][4];

/*
typedef struct t_button {
	Rect r;
	char text[32];
	int over;
	int down;
	int click;
	float color[3];
	float dcolor[3];
	unsigned int text_color;
};
*/

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


//const int MAX_ENEMIE = 4;

class Global {
    public:
        int xres, yres;
        char keys[65536];
        int show_credits;
        int show_instructions;
        int startUpDisplay;

        int Background1;
        int Background2;
        int BackgroundTitle;
        int Highscore;

        GLuint Background1Texture;
        GLuint Background2Texture;
        GLuint BackgroundTitleTexture;
        GLuint HighscoreTexture;
    public:
        // Constructer
        Global();
};

class Ship {
    public:
        Vec pos;
        Vec dir;
        Vec vel;
        Vec acc;
        float angle;
        float color[3]; //[3]
    public:
        Ship();
};

class Bullet {
    public:
        Vec pos;
        Vec vel;
        float color[3];
        struct timespec time;
    public:
        Bullet();
};

class Enemy {
    public:
        Vec xpos;
        Vec ypos;
        Vec pos;
        Vec dir;
        Vec vel;
        Vec acc;
        float angle;
        float color[1];
    public:
        Enemy();
};

class Asteroid {
public:
	Vec pos;
	Vec vel;
	int nverts;
	Flt radius;
	Vec vert[8];
	float angle;
	float rotate;
	float color[3];
	struct Asteroid *prev;
	struct Asteroid *next;
public:
	Asteroid();
};

class Game {
    public:
        Ship ship;
        Asteroid *ahead;
        Enemy enemy[3];
        Bullet *barr;
        int nbullets;
        int nasteroids;
        int nenemy;
        struct timespec bulletTimer;
    public:
        Game();
        ~Game();
};

class Image {
public:
    int width, height;
    unsigned char *data;
    ~Image();
    Image(const char *);
};

#endif
