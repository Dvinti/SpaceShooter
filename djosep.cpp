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

/*---------------------------------------------------------------------------*/
/*---------------------------Framework for Buttons---------------------------*/
/*---------------------Code Provided by Gordon Griesel-----------------------*/
/*---------------------------------------------------------------------------*/
int lbutton = 0;
int rbutton = 0;
#define nbuttons 4

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
    for (int i = 1; i < 2; i++) {
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
}
void init_hs(void)
{
    for (int i = 3; i < 4; i++) {
        button[i].r.width = gl.xres/gl.xres + 199;
        button[0].r.height = gl.yres/gl.yres + 50;
        button[2].r.height = gl.yres/gl.yres + 80;
        button[i].r.left = gl.xres - 210;
        button[0].r.bot = gl.yres - 60;
        button[2].r.bot = gl.yres/700 + 10;
        button[2].r.right =
            button[i].r.left + button[i].r.width;
        button[i].r.top = button[i].r.bot + button[i].r.height;
        button[i].r.centerx =
            (button[i].r.left + button[i].r.right) / 2;
        button[i].r.centery =
            (button[i].r.bot + button[i].r.top) / 2;
        //strcpy(button[2].text, "Quit");
        if (gl.Highscore)
            strcpy(button[3].text, "Quit");
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
                if (i == 3) {
                    if (gl.Highscore) {
                        exit(0);
                    }
                }
            }
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
/*---------------------Code Provided by Gordon Griesel-----------------------*/
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

/*void Quit_Button()
{
    Rect r;
    if (button[3].over) {
        glColor3f(1.0f, 0.0f, 0.0f);
        glLineWidth(2);
        glBegin(GL_LINE_LOOP);
            glVertex2i(button[3].r.left-2,  button[3].r.bot-2);
            glVertex2i(button[3].r.left-2,  button[3].r.top+2);
            glVertex2i(button[3].r.right+2, button[3].r.top+2);
            glVertex2i(button[3].r.right+2, button[3].r.bot-2);
            glVertex2i(button[3].r.left-2,  button[3].r.bot-2);
        glEnd();
        glLineWidth(1);
    }
    if (button[3].down) {
        glColor3fv(button[3].dcolor);
    } else {
        glColor3fv(button[3].color);
    }

    glBegin(GL_QUADS);
        glVertex2i(button[3].r.left,  button[3].r.bot);
        glVertex2i(button[3].r.left,  button[3].r.top);
        glVertex2i(button[3].r.right, button[3].r.top);
        glVertex2i(button[3].r.right, button[3].r.bot);
    glEnd();

    r.left = button[3].r.centerx;
    r.bot  = button[3].r.centery-8;
    r.center = 1;

    if (button[3].down) {
        ggprint16(&r, 0, button[3].text_color, "Pressed!");
    } else {
        ggprint16(&r, 0, button[3].text_color, button[3].text);
    }
}*/

void show_Daniels_credits(int x, int y)
{
    Rect r;
    r.bot = y;
    r.left = x;
    r.center = 1;
    ggprint40(&r, 16, 0x00ff0000, "Daniel Josep");
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
    r.left = 175;
    r.center = 0;
    
    if (lives > 1.5) {
	    ggprint13(&r, 16, 0xfbfbfa, "%0.2f", lives);
    } else { 
	    ggprint13(&r, 16, 0xe92b36, "%0.2f", lives);
    }
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

void print_pause()
{
    if (gl.paused) {
        Rect r;
        r.bot = gl.yres/2;
        r.left = gl.xres/2 - 50;
        r.center = 0;
        ggprint40(&r, 16, 0xfbfbfa, "Paused");
    }
}

void high_score(int score, int highscore)
{
    Rect r;
    r.bot = gl.yres - 150;
    r.left = gl.xres/2 -100;
    r.center = 0;
    ggprint40(&r, 16, 0xfbfbfa, "Highscores");

    /* Names */
    r.bot = gl.yres - 285;
    r.left = 280;
    r.center = 0;
    ggprint40(&r, 16, 0xe92b36, "1.  Player 1");

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

    /* Scores */
    r.bot = gl.yres - 285;
    r.left = 600;
    r.center = 0;
    if (score > highscore) {
        ggprint40(&r, 16, 0xfbfbfa, "%0.5i", score); // Player 1
    } else {
        ggprint40(&r, 16, 0xfbfbfa, "%0.5i", highscore);
    }

    r.bot = gl.yres - 380;
    r.left = 600;
    r.center = 0;
    ggprint40(&r, 16, 0xfbfbfa, "00200"); // Player 2

    r.bot = gl.yres - 462;
    r.left = 600;
    r.center = 0;
    ggprint40(&r, 16, 0xfbfbfa, "00150"); // Player 3

    r.bot = gl.yres - 545;
    r.left = 600;
    r.center = 0;
    ggprint40(&r, 16, 0xfbfbfa, "00100"); // Player 4
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
