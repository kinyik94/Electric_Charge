#ifndef BUILD_H_INCLUDED
#define BUILD_H_INCLUDED

#include "game.h"

typedef struct levelfile{
    Barrier *bar;
    int barpcs;
    Point *part;
    int partpcs;
    Point goal;
}levelfile;

void levelbuild(SDL_Surface *screen, Window win, const char *user, int levelnum);
void leveldel(SDL_Surface *screen, Window win, int levelnum);

#endif
