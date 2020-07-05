#include <stdlib.h>
#include <stdio.h>
#include "battleship.h"
#include <time.h>

//ship default configurations
Ship default_ships[4] =
{
    {"frigate",1,0,0,0,1},
    {"destroyer",2,0,0,0,2},
    {"battleship",3,0,0,0,3},
    {"carrier",4,0,0,0,4}
};

//intenta colocar un barco en el tablero
int putship(Ship *(*board)[][10], Ship* ship)
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
            if((*board)[i][ship->y])
            {
                return -1;
            }
        }
        //no hubo colision
        //colocamos la nave en el tablero
         for(int i = ship->x; i < ship->largo + ship->x; i++)
        {
            (*board)[i][ship->y] = ship;
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
            if((*board)[ship->x][i])
            {
                return -1;
            }
        }
        //no hubo colision
        //colocamos la nave en el tablero
        for(int i = ship->y; i < ship->largo + ship->y; i++)
        {
            //colocamos la nave en el tablero
            (*board)[ship->x][i] = ship;
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
int checkHit(Ship *(*board)[][10], int x, int y)
{
    printf("Checking hit on [%d,%d]\n\n",x,y);

    if((*board)[x][y])
    {
        (*board)[x][y]->hitsremaining--;
        if((*board)[x][y]->hitsremaining > 0)
        {
            return HIT;
        }
        else
        {
            return SUNK;
        }
    }
    else
    {
        return MISS;
    }
}

//imprime una representacion del gamestate por pantalla
void print_gamestate(Gamestate* gamestate)
{

    printf("Barcos de mi oponente: %d\n",gamestate->hisships);
    printf("Tablero del oponente:\n");
    for(int j = 0; j < 10; j++)
    {
        printf("| ");
        for(int i = 0; i < 10; i++)
        {
            char c;
            int act = gamestate->hisboard[i][j];
            if(act == UNKNOWN)
            {
                c = '?';
            }
            if(act == WATER)
            {
                c = 'W';
            }
            if(act == SHIP)
            {
                c = 'S';
            }
            if(act == DESTROYED)
            {
                c = 'D';
            }
            printf("%c ",c);
        }
        printf("|\n");
    }

    int cant = 0;
    for(int i = 0; i<9; i++)
    {
        if(gamestate->myships[i].hitsremaining > 0)
        {
            cant++;
        }
    }
    printf("Mis barcos: %d\n",cant);
    printf("Mi tablero:\n");
    for(int j = 0; j < 10; j++)
    {
        printf("| ");
        for(int i = 0; i < 10; i++)
        {
            char c;
            if(gamestate->myboard[i][j])
            {
                if(gamestate->myboard[i][j]->hitsremaining > 0)
                {
                    c = 'S';
                }
                else
                {
                    c = 'D';
                }
            }
            else{
                c = 'W';
            }
            printf("%c ",c);
        }
        printf("|\n");
    }
}

int autoaddships(Gamestate* gamestate)
{
    srand(time(0));
    for(int i = 0; i < 9; i++)
    {
        int valido = -1;
        while(valido<0)
        {
            gamestate->myships[i].x = rand() % 10;
            gamestate->myships[i].y = rand() % 10;
            if(rand() % 2)
            {
                gamestate->myships[i].orientacion = HORIZONTAL;
            }
            else
            {
                gamestate->myships[i].orientacion = VERTICAL;
            }
            valido = putship(&gamestate->myboard,&gamestate->myships[i]);
            if(valido==0)
            {
                printf("%s colocado en la posicion [%d,%d]\n",gamestate->myships[i].nombre,gamestate->myships[i].x,gamestate->myships[i].y);
            }
        }
    }
    return 0;
}

