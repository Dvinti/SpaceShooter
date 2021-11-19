//Enrique Tapia
//Software Engineering

#include <ctime>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include "fonts.h"
#include "game.h"

using namespace std;
extern Global gl;
extern Game g;
extern Bullet b;


void credit_toggle() {
    if (gl.show_credits == 0) {
        gl.show_credits = 1;
    }
    else {
        gl.show_credits = 0;
    }
}

void move_ship_left() {
	g.ship.pos[0] += -8.0;
	cout << "<--" << g.ship.pos[0] << endl;
	if (g.ship.pos[0] < 200) {
		cout << "|<-- out of bounds" << endl;
		g.ship.pos[0] = 205;
	}
	else if (g.ship.pos[0] > 800) {
		cout << "out of bounds -->|" << endl;
		g.ship.pos[0] = 795;
	}
}

void move_ship_right() {
	g.ship.pos[0] += 8.0;
		cout << "-->" << g.ship.pos[0] << endl;
		if (g.ship.pos[0] < 200) {
			cout << "|<-- out of bounds" << endl;
			g.ship.pos[0] = 205;
		}
		else if (g.ship.pos[0] > 800) {
			cout << "out of bounds -->|" << endl;
			g.ship.pos[0] = 795;
		}
}

int check_bullet_collision(int i, Bullet *b, int score, int MAX_ENEMIES) {
	//hitbox for the enemy ship
	for(int j = 0; j < MAX_ENEMIES ; j++) {
		int x1 = g.enemy[j].xpos[j] - 15.0;
		int x2 = g.enemy[j].xpos[j] + 15.0;
		int y1 = g.enemy[j].ypos[j] - 15.0;
		int y2 = g.enemy[j].ypos[j] + 15.0;

		//cout << "hitbox "<< j << ": pos: " << x1 << " " <<y1 << endl;

		//check for collision with enemy ship
		if(b->pos[0] > x1 && b->pos[0] < x2 && b->pos[1] > y1 && b->pos[1] < y2) {
			score += 1;
			cout << "hit! Score: " << score << endl;
			//time to delete the bullet.
			memcpy(&g.barr[i], &g.barr[g.nbullets-1], sizeof(Bullet));
			g.nbullets--;
			//do not increment i.
		}
	}
	return score;
}

//checks if bullet has reached the top bound of the screen
void bullet_bounds_check(Bullet *b, int i) {
	if (b->pos[1] > (float)gl.yres - 75.0) {
		cout << "Bullet out of bounds TOP" << endl;
		//time to delete the bullet.
		memcpy(&g.barr[i], &g.barr[g.nbullets-1],
				sizeof(Bullet));
		g.nbullets--;
		//do not increment i.
	}
}

void show_enrique_credits(int x, int y)
{
	Rect r;
	r.bot = y;
	r.left = x;
	r.center = 1;
	ggprint8b(&r, 16, 0x00ff0000, "Enrique Tapia");
}
