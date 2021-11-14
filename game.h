#ifndef SS_GAME_H
#define SS_GAME_H

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

#endif