// Daniel Josep
// djosep.cpp
// My Own Work

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <ctime>
#include <cmath>
#include <GL/glut.h>
#include "fonts.h"
#include "game.h"

using namespace std;

extern Global gl;
extern Game g;
extern Bullet b;

void show_Daniels_credits(int x, int y)
{
    Rect r;
    r.bot = y;
    r.left = x;
    r.center = 1;
    ggprint8b(&r, 16, 0x00ff0000, "Daniel Josep");
}

void instructions()
{
    Rect r;
    r.bot = gl.yres - 30;
    r.left = 20;
    r.center = 0;
    ggprint8b(&r, 16, 0xfbfbfa, "Press s to show instructions");
}

void show_scores(int score)
{
    Rect r;
    r.bot = gl.yres - 690;
    r.left = (gl.xres/2) - 65;
    r.center = 0;
    ggprint13(&r, 16, 0xfbfbfa, "      <%0.4i>      ", score);
}
/*
void highscore(int score, string scorename)
{
    int highscore[3] = {0000};
    string highscorename[3];
    string scorename[
    string scorename;
    string score2name;
    glClear(GL_COLOR_BUFFER_BIT);
    // If Round is finished

    // Show Top_score Image

    // Show list of Highscore
    if (score > highscore[0]) {
        score = highscore[0];
        scorename = highscorename[0];
        highscore[0] = score;
        highscorename[0] = score;

        for (i = 0; i < 3; i++) {
            int score2 = highscore[i];
            int score2name = highscorename[i];
            highscore[i] = score;
            highscorename[i] = scorename;
            score = score2;
            scorename = score1name;
        }
    }
}*/