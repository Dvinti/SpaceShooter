//Frankie Sanchez
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <ctime>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "log.h"
#include "fonts.h"
#include "game.h"

extern Global gl;
/*
 *
 *
 *
 * Code pulled from the rainforest framework
 * Put into my individual file since I set it up to work with our framework
 * and most importanly cleans the main code file
 */


////////////////////////////     Rainforest Framework         ///////////////////////////////////////////
//////////////////////////// Code provided by Gordon Griesel  ///////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////// 

Image::~Image() { delete [] data; }


Image::Image(const char *fname) {
        if (fname[0] == '\0')
            return;
        //printf("fname **%s**\n", fname);
        int ppmFlag = 0;
        char name[40];
        strcpy(name, fname);
        int slen = strlen(name);
        char ppmname[80];
        if (strncmp(name+(slen-4), ".ppm", 4) == 0)
            ppmFlag = 1;
        if (ppmFlag) {
            strcpy(ppmname, name);
        } else {
            name[slen-4] = '\0';
            //printf("name **%s**\n", name);
            sprintf(ppmname,"%s.ppm", name);
            //printf("ppmname **%s**\n", ppmname);
            char ts[100];
            //system("convert eball.jpg eball.ppm");
            sprintf(ts, "convert %s %s", fname, ppmname);
            system(ts);
        }
        //sprintf(ts, "%s", name); 
        FILE *fpi = fopen(ppmname, "r");
        if (fpi) {
            char line[200];
            fgets(line, 200, fpi);
            fgets(line, 200, fpi);
            //skip comments and blank lines
            while (line[0] == '#' || strlen(line) < 2)
                fgets(line, 200, fpi);
            sscanf(line, "%i %i", &width, &height);
            fgets(line, 200, fpi);
            //get pixel data
            int n = width * height * 3; 
            data = new unsigned char[n];
            for (int i=0; i<n; i++)
                data[i] = fgetc(fpi);
            fclose(fpi);
        } else {
            printf("ERROR opening image: %s\n",ppmname);
            exit(0);
        }
        if (!ppmFlag)
            unlink(ppmname);
}



unsigned char *buildAlphaData(Image *img)
{
    //Add 4th component to an RGB stream...
    //RGBA
    //When you do this, OpenGL is able to use the A component to determine
    //transparency information.
    //It is used in this application to erase parts of a texture-map from view.
    int i;
    int a,b,c;
    unsigned char *newdata, *ptr;
    unsigned char *data = (unsigned char *)img->data;
    newdata = (unsigned char *)malloc(img->width * img->height * 4);
    ptr = newdata;
    for (i=0; i<img->width * img->height * 3; i+=3) {
        a = *(data+0);
        b = *(data+1);
        c = *(data+2);
        *(ptr+0) = a;
        *(ptr+1) = b;
        *(ptr+2) = c;
        //-----------------------------------------------
        //get largest color component...
        //*(ptr+3) = (unsigned char)((
        //      (int)*(ptr+0) +
        //      (int)*(ptr+1) +
        //      (int)*(ptr+2)) / 3);
        //d = a;
        //if (b >= a && b >= c) d = b;
        //if (c >= a && c >= b) d = c;
        //*(ptr+3) = d;
        //-----------------------------------------------
        //this code optimizes the commented code above.
        *(ptr+3) = (a|b|c);
        //-----------------------------------------------
        ptr += 4;
        data += 3;
    }
    return newdata;
}

////////////////////////////     Rainforest Framework         ///////////////////////////////////////////
//////////////////////////// Code provided by Gordon Griesel  ///////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void build_imageTexture(GLuint texid /*,Image img[], int imageNumber*/) {

    glBindTexture(GL_TEXTURE_2D, texid);
    //
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    //
} 


void show_frankie_credits(int x, int y) {

    Rect r;
    r.bot = y;
    r.left = x;
    r.center = 1;
    ggprint8b(&r, 16, 0x00ff0000, "Frankie Sanchez");

}

void show_background(int x, int y, GLuint texid) {
    
    glBindTexture(GL_TEXTURE_2D, texid);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex2i(0, 0);
    glTexCoord2f(0.0f, 0.0f); glVertex2i(0, y);
    glTexCoord2f(1.0f, 0.0f); glVertex2i(x,y);
    glTexCoord2f(1.0f, 1.0f); glVertex2i(x, 0);
    glEnd();

}
