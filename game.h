#pragma once


class Global {
    public:
        int xres, yres;
        char keys[65536];
        int show_credits;
        Global() {
        }
} gl;

class Ship {
	public:
        Vec pos;
        Vec dir;
        Vec vel;
        Vec acc;
        float angle;
        float color[3]; //[3]
    public:
        Ship() {
        }
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
        Enemy() {
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
        Enemy enemy;
        Bullet *barr;
        int nbullets;
        int nenemy;
        struct timespec bulletTimer;
    public:
        Game();
        ~Game();
} g;