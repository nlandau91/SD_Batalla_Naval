#include <stdlib.h>
#include <stdio.h>
#include "battleship.h"

/**
    Configuraciones por default de las naves
*/
Ship default_ships[4] =
{
    {"frigate",1,0,0,0,1},
    {"destroyer",2,0,0,0,2},
    {"battleship",3,0,0,0,3},
    {"carrier",4,0,0,0,4}
};

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

int create_ship(Ship* ship, ship_type type)
{
    *ship = default_ships[type];
    return EXIT_SUCCESS;
}

int check_hit(Ship *(*board)[][10], int x, int y)
{
    printf("Checking hit on [%d,%d]\n\n",x,y);

    if((*board)[x][y])
    {
        return HIT;
    }
    else
    {
        return MISS;
    }
}

int print_gamestate(Gamestate* gamestate)
{

    printf("Naves de mi oponente: %d\n",gamestate->hisShips);
    printf("Tablero del oponente:\n");
    print_tileboard(&gamestate->hisBoard);

    //cuento cuantos naves me quedan
    int cant = 0;
    for(int i = 0; i<9; i++)
    {
        if(gamestate->myShips[i].hitsRemaining > 0)
        {
            cant++;
        }
    }
    printf("Mis naves: %d\n",cant);
    printf("Mi tablero:\n");
    print_shipboard(&gamestate->myBoard);
    return EXIT_SUCCESS;
}

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
                if((*shipboard)[i][j]->hitsRemaining > 0)
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

state check_loss(Gamestate* gamestate)
{
    state toReturn = gamestate->myState;
    int i = 0;
    while(i<9)
    {
        //me fijo si era mi ultima nave
        if(gamestate->myShips[i].hitsRemaining != 0)
        {
            break;
        }
        i++;
    }
    //te hundieron todas las naves
    if(i == 9)
    {
        toReturn = LOST;
    }
    return toReturn;
}

state check_win(Gamestate* gamestate)
{
    state toReturn = gamestate->myState;
    if(gamestate->hisShips==0)
    {
        toReturn = WON;
    }
    return toReturn;
}

int destroy_enemy_ship(Gamestate* gamestate, int x, int y, int size, orientation orientacion)
{
    //actualizamos la informacion del tablero del oponente
    //hay que cambiar los "ship" por "destroyed"
    if(orientacion == VERTICAL)
    {
        for(int i = 0; i < size; i++)
        {
            gamestate->hisBoard[x][y+i] = DESTROYED;
        }
    }

    if(orientacion == HORIZONTAL)
    {
        for(int i = 0; i < size; i++)
        {
            gamestate->hisBoard[x+i][y] = DESTROYED;
        }
    }
    gamestate->hisShips--;
    return EXIT_SUCCESS;
}
