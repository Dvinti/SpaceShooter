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

/*---------------------------------------------------------------------------*/
/*---------------------------Framework for Buttons---------------------------*/
/*---------------------------------------------------------------------------*/
int lbutton = 0;
int rbutton = 0;
#define nbuttons 3

typedef struct t_button {
    Rect r;
    char text[32];
    int over;
    int down;
    int click;
    float color[3];
    float dcolor[3];
    unsigned int text_color;
} Button;

Button button[nbuttons];

/* Modified for easier use with similar appearance */
void init(void)
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
        //strcpy(button[0].text, "Quit");
        if (gl.BackgroundTitle)
            strcpy(button[1].text, "Play");
        if (gl.Highscore)
            strcpy(button[2].text, "Credits");
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
    /*
    if (gl.BackgroundTitle) {
        button[1].r.width = gl.xres/gl.xres + 199;
        button[1].r.height = gl.yres/gl.yres + 80;
        button[1].r.left = gl.xres - 210;
        button[1].r.bot = gl.yres/700 + 10;
        button[1].r.right =
            button[1].r.left + button[1].r.width;
        button[1].r.top = button[1].r.bot + button[1].r.height;
        button[1].r.centerx =
            (button[1].r.left + button[1].r.right) / 2;
        button[1].r.centery =
            (button[1].r.bot + button[1].r.top) / 2;
        strcpy(button[1].text, "Play");
        button[1].down = 0;
        button[1].click = 0;
        button[1].color[0] = 0.4f;
        button[1].color[1] = 0.4f;
        button[1].color[2] = 0.7f;
        button[1].dcolor[0] = button[1].color[0] * 0.5f;
        button[1].dcolor[1] = button[1].color[1] * 0.5f;
        button[1].dcolor[2] = button[1].color[2] * 0.5f;
        button[1].text_color = 0x00ffffff;
    }

    if (gl.Highscore) {
        button[2].r.width = gl.xres/gl.xres + 199;
        button[2].r.height = gl.yres/gl.yres + 80;
        button[2].r.left = gl.xres - 210;
        button[2].r.bot = gl.yres/700 + 10;
        button[2].r.right =
            button[2].r.left + button[2].r.width;
        button[2].r.top = button[2].r.bot + button[2].r.height;
        button[2].r.centerx =
            (button[2].r.left + button[2].r.right) / 2;
        button[2].r.centery =
            (button[2].r.bot + button[2].r.top) / 2;
        strcpy(button[2].text, "Credits");
        button[2].down = 0;
        button[2].click = 0;
        button[2].color[0] = 0.4f;
        button[2].color[1] = 0.4f;
        button[2].color[2] = 0.7f;
        button[2].dcolor[0] = button[2].color[0] * 0.5f;
        button[2].dcolor[1] = button[2].color[1] * 0.5f;
        button[2].dcolor[2] = button[2].color[2] * 0.5f;
        button[2].text_color = 0x00ffffff;
    }
    */
}

void mouse_click(int action)
{
    int i;
    if (action == 1) {
        for (i = 0; i < nbuttons; i++) {
            if (button[i].over) {
                button[i].down = 1;
                button[i].click = 1;
                if (i == 0) {
                    //user clicked Quit
                    //done = 1;
                }
                if (i == 1) {
                    //user clicked Play
                    if (gl.BackgroundTitle == 1) {
                        gl.BackgroundTitle = 0;
                        gl.Background1 = 1;
                        extern void startWindow();
                        startWindow();
                    }
                }
                if (i == 2) {
                    if (gl.Highscore) {
                        extern void credit_toggle();
                        credit_toggle();
                    }
                }
            }
        }
    }
    if (action == 2) {
        for (i = 0; i < nbuttons; i++) {
            button[i].down = 0;
            button[i].click = 0;
        }
    }
}

void check_mouse(XEvent *e)
{
    static int savex = 0;
	static int savey = 0;
    //Log("check_mouse()...\n");
    int x,y;
    int i;
    lbutton = 0;
    rbutton = 0;
    if (e->type == ButtonRelease) {
		mouse_click(2);
		return;
	}
    if (e->type == ButtonPress) {
        if (e->xbutton.button == 1) {
            //Left button is down
            lbutton = 1;
        }
    }
    x = e->xbutton.x;
    y = e->xbutton.y;
    //reverse the y position
    y = gl.yres - y;
    if (savex != e->xbutton.x || savey != e->xbutton.y) {
        //Mouse moved
        savex = e->xbutton.x;
        savey = e->xbutton.y;
    }
    //Log("check over %i buttons\n", nbuttons);
    for (i = 0; i < nbuttons; i++) {
        button[i].over = 0;
        button[i].down = 0;
        //Log("xy: %i %i\n",x,y);
        if (x >= button[i].r.left &&
            x <= button[i].r.right &&
            y >= button[i].r.bot &&
            y <= button[i].r.top) {
            button[i].over = 1;
            //Log("over button: %i\n", i);
            break;
        }
    }
    if (lbutton)
		mouse_click(1);
}
/*---------------------------------------------------------------------------*/
/*---------------------------Framework for Buttons---------------------------*/
/*---------------------------------------------------------------------------*/

void PlayButton()
{
    Rect r;
    /* Button Use */
    for (int i = 0; i < 2; i++) {
        /* Draws a Border Highlight */
        if (button[i].over) {
            glColor3f(1.0f, 0.0f, 0.0f);
            glLineWidth(2);
            glBegin(GL_LINE_LOOP);
                glVertex2i(button[i].r.left-2,  button[i].r.bot-2);
                glVertex2i(button[i].r.left-2,  button[i].r.top+2);
                glVertex2i(button[i].r.right+2, button[i].r.top+2);
                glVertex2i(button[i].r.right+2, button[i].r.bot-2);
                glVertex2i(button[i].r.left-2,  button[i].r.bot-2);
            glEnd();
            glLineWidth(1);
        }
        if (button[i].down) {
            glColor3fv(button[i].dcolor);
        } else {
            glColor3fv(button[i].color);
        }
        glBegin(GL_QUADS);
            glVertex2i(button[i].r.left,  button[i].r.bot);
            glVertex2i(button[i].r.left,  button[i].r.top);
            glVertex2i(button[i].r.right, button[i].r.top);
            glVertex2i(button[i].r.right, button[i].r.bot);
        glEnd();
        r.left = button[i].r.centerx;
        r.bot  = button[i].r.centery-8;
        r.center = 1;
        if (button[i].down) {
            ggprint16(&r, 0, button[i].text_color, "Pressed!");
        } else {
            ggprint16(&r, 0, button[i].text_color, button[i].text);
        }
    }
}

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