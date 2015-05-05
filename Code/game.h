#ifndef GAME_H
#define GAME_H

#include "menu.h"

typedef struct particles{
    Point element;
    struct particles *next;
}particles;

typedef struct Barrier{
    double x1, x2;
    double y1, y2;
}Barrier;

typedef struct Barlist{
    Barrier element;
    struct Barlist *next;
}Barlist;

double point_distance(Point a, Point b);

void level_draw(Levels* level, Window win, double rad, Point goal, const char* user, particles *part, Point use[], Point f[], int fsize, int usesize, Barrier *bar, int barpcs, int shot, int levelnum, SDL_Surface *scr, Barlist *barl, int build_f);

int shot_an(Point* moving, Point f[], particles *part, int fsize, double *v_x, double *v_y, SDL_Surface *scr);

#endif
