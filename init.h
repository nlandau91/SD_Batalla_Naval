#ifndef INIT_H_
#define INIT_H_
#include "battleship.h"

int init_shiparray(Ship (*shiparray)[9]);

int init_tileboard(enum tile (*intboard)[][10]);

int init_shipboard(Ship* (*shipboard)[][10]);

int init_gamestate(Gamestate* gamestate);

#endif
