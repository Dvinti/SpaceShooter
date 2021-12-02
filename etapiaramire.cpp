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
extern Asteroid a;


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
        //cout << "<--" << g.ship.pos[0] << endl;
        if (g.ship.pos[0] < 220) {
                //cout << "|<-- out of bounds" << endl;
                g.ship.pos[0] = 230;
        }
        else if (g.ship.pos[0] > 780) {
                //cout << "out of bounds -->|" << endl;
                g.ship.pos[0] = 770;
        }
}

void move_ship_right() {
        g.ship.pos[0] += 8.0;
        //cout << "-->" << g.ship.pos[0] << endl;
        if (g.ship.pos[0] < 220) {
                //cout << "|<-- out of bounds" << endl;
                g.ship.pos[0] = 230;
        }
        else if (g.ship.pos[0] > 780) {
                //cout << "out of bounds -->|" << endl;
                g.ship.pos[0] = 770;
        }
}

//keeps enemy from going out of bounds
int enemy_boundary_check(Asteroid *a) {
        //reaches left edge
        if (a->pos[0] < 220.0) {
                a->vel[0] = -a->vel[0];
        }
        //reaches right edge
        else if (a->pos[0] > 780.0) {
                a->vel[0] = -a->vel[0];
        }
        //reaches bottom of screen
        else if (a->pos[1] < gl.yres-525) {
			a->vel[1] = -a->vel[1];
	//      a->pos[0] = (Flt)(rand() % (815 - 185 + 1) + 185);
	//      a->pos[1] = (Flt)(gl.yres/1.1589362);
	//      a->vel[0] = (Flt)(rnd()*2.0-1.0); //velocity in x direction
	//      a->vel[1] = (Flt)(rnd()*2.0-1.0); //velocity in y direction
			//cout << "Enemy reached the threshold, reducing lives" << endl;
			return true;
                /*
                     CAUSING SEGMENTATION FAULT
                     if (g.nasteroids == 1) {
                     cout << "Game Over!" << endl;
                     break;
                     }
                     else if (g.nasteroids == 2) {
                     cout << "Last one: " << g.nasteroids << endl;
                     deleteAsteroid(&g, a);
                     g.nasteroids--;
                     }
                     else {
                     cout << "Asteroid: " << g.nasteroids << endl;
                     Asteroid *savea = a->next;
                     deleteAsteroid(&g, a);
                     a = savea;
                     g.nasteroids--;
                     }
                     */
        }
        //reaches top of screen
        else if (a->pos[1] > (float)gl.yres - 95) {
                a->vel[1] = -a->vel[1];
        }
        return false;
}


//checks if bullet has reached the top bound of the screen
void bullet_bounds_check(Bullet *b, int i) {
	if (b->pos[1] > (float)gl.yres - 75.0) {
		//cout << "Bullet out of bounds TOP" << endl;
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
	ggprint40(&r, 16, 0x00ff0000, "Enrique Tapia");
}

void shooting_line() {
	// Minimum shooting distance
	Rect r;
	r.bot = gl.yres - 555;
	r.left = 0;
	r.center = 0;
	ggprint8b(&r, 16, 0x2a2a2a, "                                             "
		" _______________________________________________________________"
		"___________________________");
}

extern void deleteAsteroid(Game *g, Asteroid *node);
extern void buildAsteroidFragment(Asteroid *ta, Asteroid *a);

int ship_enemy_collision(Asteroid *a) {
	//int x, y, dist;
	int x1, x2, y1, y2;		//ship hitbox
	int Ex1, Ex2, Ey1, Ey2; //enemy hitbox

	//create a hitbox for the ship
	x1 = g.ship.pos[0] - 35;	//left
	x2 = g.ship.pos[0] + 35; //right
	y1 = g.ship.pos[1] - 35; //top
	y2 = g.ship.pos[1] + 35; //bottom

	//create a hitbox for the enemy
	Ex1 = a->pos[0] - 35; //left
	Ex2 = a->pos[0] + 35; //right
	Ey1 = a->pos[1] - 35; //top
	Ey2 = a->pos[1] + 35; //bottom

//	x = g.ship.pos[0] - a->pos[0];
//	y = g.ship.pos[1] - a->pos[1];
//	dist = (x*x + y*y);


	//check if the hitboxes intersect
	/*if (x1 < Ex2 &&
			x2 > Ex1 &&
			y1 < Ey2 &&
			y2 > Ey1) {
			cout << "Collision!" << endl;
			//if they do, then the enemy is dies
			a->color[0] = 1.0;
			a->color[1] = 0.1;
			a->color[2] = 0.1;
			//delete the asteroid
			Asteroid *savea = a->next;
			deleteAsteroid(&g, a);
			a = savea;
			g.nasteroids--;
		return true;
	}*/

	//check for collision
		if (x1 < Ex2 &&
				x2 > Ex1 &&
				y1 < Ey2 &&
				y2 > Ey1) {

//	if (dist < (30*30)) {
			cout << "Collision!" << endl;
			//if they do, then spawn baby enemies

			//credit to Gordon Grissel from his Asteroid framework
			if (a->radius >= 30) {
				//break it into pieces.
				Asteroid *ta = a;
				buildAsteroidFragment(ta, a);
				int r = rand()%7+3; //random number between 0 and 4
				for (int k=0; k<r; k++) {
					//get the next asteroid position in the array
					Asteroid *ta = new Asteroid;
					buildAsteroidFragment(ta, a);
					//add to front of asteroid linked list
					ta->next = g.ahead;
					if (g.ahead != NULL)
						g.ahead->prev = ta;
					g.ahead = ta;
					g.nasteroids++;
				}
			} 
			else {
				a->color[0] = 1.0;
				a->color[1] = 0.1;
				a->color[2] = 0.1;
				//asteroid is too small to break up
				//delete the asteroid and bullet
				Asteroid *savea = a->next;
				deleteAsteroid(&g, a);
				a = savea;
				g.nasteroids--;
			}
		return true;
	}
	return false;
}


int checkpoint = 300;
int score_new_lives(int score, float lives) {
	if (score >= checkpoint && lives < 7.0) {
		if (lives > 6.0){
			lives = 7.0;
		}
		else{
			lives += 1.0;
		}
		checkpoint += 300;
		//cout << "new lives added"<<endl<<endl;
	}
	return lives;
}
int score_new_lives_type2(int score) {
	if (score >= checkpoint) {
		//eligible for new lives
		checkpoint += 300;
		return true;
	}
	return false;
}
