#ifndef MENU_H
#define MENU_H

#include <SDL.h>
#include <SDL_gfxPrimitives.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <math.h>
#include <string.h>
#include "debugmalloc.h"

typedef struct Menupoints{
    int x1, x2, y1, y2;
    char menuname[15];
    int namex;
}Menupoints;

typedef struct Point{
    double r, x, y, polarity, dist, click;
}Point;

typedef struct Levels{
    double x1, y1, x2, y2, x3, y3, x4 ,y4; /*Four points of the cannon's rectangle*/
    int radius, length;
    int up_zone, left_zone;
}Levels;

typedef struct Window{
    int high, width; /*A megjelenő ablak méretei*/
}Window;

typedef struct file{
    char us[11];
    int shot;
}file;

int menu(char *user, SDL_Surface *screen, Window window);
void menudraw(SDL_Surface *scr, Menupoints m[], int mpcs, Window win, int first, int delet);
void file_in(FILE *fp, int levnum, file *f);

#endif
