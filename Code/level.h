#ifndef LEVEL_H_INCLUDED
#define LEVEL_H_INCLUDED

#include "game.h"

int* levelman(char* felh, SDL_Surface *screen, Window ablak, int levelpcs);
particles* new_used_element(particles* part, Point which);
void part_free(particles *part);
particles* del(particles *part, particles *which);

#endif
