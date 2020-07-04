#include <stdlib.h>
#include <stdio.h>
#include "battleship.h"

//ship default configurations
Ship default_ships[4] =
{
    {"frigate",1,0,0,0,1},
    {"destroyer",2,0,0,0,2},
    {"battleship",3,0,0,0,3},
    {"carrier",4,0,0,0,4}
};

//intenta colocar un barco en el tablero
int putship(Ship* board[10][10], Ship* ship)
{
    if(ship->orientacion == HORIZONTAL)
    {
        //la nave no entra en el tablero
        if((ship->x + ship->largo) > 10)
        {
            return -1;
        }
        //veo si hay colision con otra nave
        for(int i = ship->x; i < ship->largo + ship->x; i++)
        {

            //si hay colision con otra nave
            if(board[i][ship->y])
            {
                return -1;
            }
            //colocamos la nave en el tablero
            board[i][ship->y] = ship;
        }
    }

    if(ship->orientacion == VERTICAL)
    {
        //la nave no entra en el tablero
        if((ship->y + ship->largo) > 10)
        {
            return -1;
        }
        //veo si hay colision con otra nave
        for(int i = ship->y; i < ship->largo + ship->y; i++)
        {
            //si hay colision con otra nave
            if(board[ship->x][i])
            {
                return -1;
            }
            //colocamos la nave en el tablero
            board[ship->x][i] = ship;
        }
    }
    return 0;
}


int create_ship(Ship* ship, int type)
{
    *ship = default_ships[type];
    return 0;
}

//decide si un intento es un hit, miss o hundido
//si es un hit, reduce los hitsremaining
int checkHit(Ship* board[10][10], int x, int y)
{
    if(!board[x][y])
    {
        return MISS;
    }
    else
    {
        Ship* ship = board[x][y];
        ship->hitsremaining--;
        if(ship->hitsremaining)
        {
            return HIT;
        }
        else
        {
            return SUNK;
        }
    }
}

//imprime una representacion del gamestate por pantalla
//idealmente imprime ambos tableros en modo ascii
//por el momento solo imprime cuantos barcos le queda a cada uno
void print_gamestate(Gamestate* gamestate)
{
    int cant = 0;
    for(int i = 0; i<9; i++)
    {
        if(gamestate->myships->hitsremaining > 0)
        {
            cant++;
        }
    }
    printf("Mis barcos: %d\n",cant);
    printf("Barcos de mi oponente: %d\n",gamestate->hisships);
    //  if(gamestate->)

}

