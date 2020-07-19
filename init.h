#ifndef INIT_H_
#define INIT_H_
#include "battleship.h"

/**
    Inicializa el gamestate
    @param gamestate puntero al gamestate que sera inicializado
    @return un codigo indicando si tuvo exito la funcion
*/
int init_gamestate(Gamestate* gamestate);
int init_gamestate2(Gamestate* gamestate);
int autoaddships(Gamestate* gamestate);
int manualAddShipGUI(Gamestate* gamestate, int ship, int x, int y, int orientacion);
#endif
