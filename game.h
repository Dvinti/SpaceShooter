#ifndef SS_GAME_H
#define SS_GAME_H

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

class Global {
	public:
		int xres, yres;
		char keys[65536];
		int show_credits;
        int show_instructions;
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
        Vec pos;
        Vec dir;
        Vec vel;
        Vec acc;
        float angle;
        float color[1];
    public:
        Enemy();
};

class Game {
    public:
        Ship ship;
        Enemy enemy;
        Bullet *barr;
        int nbullets;
        int nenemy;
        struct timespec bulletTimer;
    public:
        Game();
        ~Game();
};

#endif
