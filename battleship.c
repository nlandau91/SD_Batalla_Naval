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

//intenta colocar una nave en el tablero
//la posicion ya esta ingresada en la nave
int put_ship(Ship *(*board)[][10], Ship* ship)
{
    if(ship->orientacion == HORIZONTAL)
    {
        //la nave no entra en el tablero
        if((ship->x + ship->largo) > 10)
        {
            return EXIT_FAILURE;
        }
        //veo si tengo una nave inmediatamente detras
        if(ship->x > 0)
        {
            if((*board)[ship->x - 1][ship->y])
            {
                return EXIT_FAILURE;
            }
        }
        //veo si tengo una nave inmediatamente adelante
        if((ship->x + ship->largo) < 10)
        {
            if((*board)[(ship->x + ship->largo)][ship->y])
            {
                return EXIT_FAILURE;
            }
        }
        //veo si hay colision con otra nave
        for(int i = ship->x; i < ship->largo + ship->x; i++)
        {
            //si hay colision con otra nave
            if((*board)[i][ship->y])
            {
                return EXIT_FAILURE;
            }

            //veo si hay una nave inmediatamente arriba
            if(ship->y > 0)
            {
                if((*board)[i][ship->y - 1])
                {
                    return EXIT_FAILURE;
                }
            }
            //veo si hay una nave inmediatamente abajo
            if(ship->y < 9)
            {
                if((*board)[i][ship->y + 1])
                {
                    return EXIT_FAILURE;
                }
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
            return EXIT_FAILURE;
        }
        //veo si tengo una nave inmediatamente arriba
        if(ship->y > 0)
        {
            if((*board)[ship->x][ship->y - 1])
            {
                return EXIT_FAILURE;
            }
        }
        //veo si tengo una nave inmediatamente abajo
        if((ship->y + ship->largo) < 10)
        {
            if((*board)[ship->x][(ship->y + ship->largo)])
            {
                return EXIT_FAILURE;
            }
        }
        //veo si hay colision con otra nave
        for(int i = ship->y; i < ship->largo + ship->y; i++)
        {
            //si hay colision con otra nave
            if((*board)[ship->x][i])
            {
                return EXIT_FAILURE;
            }
            //veo si hay una nave inmediatamente atras
            if(ship->x > 0)
            {
                if((*board)[ship->x - 1][i])
                {
                    return EXIT_FAILURE;
                }
            }
            //veo si hay una nave inmediatamente adelante
            if(ship->x < 9)
            {
                if((*board)[ship->x + 1][i])
                {
                    return EXIT_FAILURE;
                }
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
    return EXIT_SUCCESS;
}

//crea una nave de tipo type y la devuelve en la direccion pasada por parametro
int create_ship(Ship* ship, ship_type type)
{
    *ship = default_ships[type];
    return EXIT_SUCCESS;
}

//decide si un intento es un hit, miss o hundido
//si es un hit, reduce los hitsremaining
int check_hit(Ship *(*board)[][10], int x, int y)
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
int print_gamestate(Gamestate* gamestate)
{

    printf("Barcos de mi oponente: %d\n",gamestate->hisShips);
    printf("Tablero del oponente:\n");
    print_tileboard(&gamestate->hisBoard);

    //cuento cuantos barcos me quedan
    int cant = 0;
    for(int i = 0; i<9; i++)
    {
        if(gamestate->myShips[i].hitsremaining > 0)
        {
            cant++;
        }
    }
    printf("Mis barcos: %d\n",cant);
    printf("Mi tablero:\n");
    print_shipboard(&gamestate->myBoard);
    return EXIT_SUCCESS;
}

//imprime el tablero del jugador
int print_shipboard(Ship *(*shipboard)[][10])
{
    printf("   0 1 2 3 4 5 6 7 8 9\n");
    printf("  ---------------------\n");
    for(int j = 0; j < 10; j++)
    {
        printf("%d| ",j);
        for(int i = 0; i < 10; i++)
        {
            char c;
            if((*shipboard)[i][j])
            {
                if((*shipboard)[i][j]->hitsremaining > 0)
                {
                    c = 'S';
                }
                else
                {
                    c = 'D';
                }
            }
            else
            {
                c = 'W';
            }
            printf("%c ",c);
        }
        printf("|\n");
    }
    printf("  ---------------------\n");
    return EXIT_SUCCESS;
}


//imprime el tablero del oponente
int print_tileboard(tile (*tileboard)[][10])
{
    printf("   0 1 2 3 4 5 6 7 8 9\n");
    printf("  ---------------------\n");
    for(int j = 0; j < 10; j++)
    {
        printf("%d| ",j);
        for(int i = 0; i < 10; i++)
        {
            char c;
            int act = (*tileboard)[i][j];
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
    printf("  ---------------------\n");

    return EXIT_SUCCESS;
}
