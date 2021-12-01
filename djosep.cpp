// Daniel Josep
// djosep.cpp
// My Own Work

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <ctime>
#include <cmath>
#include <cstring>
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

void show_lives(float lives)
{
    Rect r;
    r.bot = gl.yres - 675;
    r.left = 125;
    r.center = 0;
    ggprint8b(&r, 16, 0xfbfbfa, "%0.2f", lives);
}

// pause game on start up, resume when user clicks play
// pause game on instruction screen & credit screen
// ONLY resume when both instructions and credits are closed
void pause_game()
{
    if (!gl.paused) {
        gl.paused = true;
    } else if (!gl.show_instructions && !gl.show_credits) {
        gl.paused = false;
    }
}

/* Modified for easier use
void init(int nbuttons)
{
    for (int i = 1; i < nbuttons; i++) {
        button[i].r.width = gl.xres/gl.xres + 199;
        button[0].r.height = gl.yres/gl.yres + 50;
        button[1].r.height = gl.yres/gl.yres + 80;
        button[i].r.left = gl.xres - 210;
        button[0].r.bot = gl.yres - 60;
        button[1].r.bot = gl.yres/700 + 10;
        button[i].r.right =
            button[i].r.left + button[i].r.width;
        button[i].r.top = button[i].r.bot + button[i].r.height;
        button[i].r.centerx =
            (button[i].r.left + button[i].r.right) / 2;
        button[i].r.centery =
            (button[i].r.bot + button[i].r.top) / 2;
        strcpy(button[0].text, "Quit");
        strcpy(button[1].text, "Play");
        button[i].down = 0;
        button[i].click = 0;
        button[i].color[0] = 0.4f;
        button[i].color[1] = 0.4f;
        button[i].color[2] = 0.7f;
        button[i].dcolor[0] = button[i].color[0] * 0.5f;
        button[i].dcolor[1] = button[i].color[1] * 0.5f;
        button[i].dcolor[2] = button[i].color[2] * 0.5f;
        button[i].text_color = 0x00ffffff;
    }
}*/

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