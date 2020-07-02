#include <stdlib.h>
#include "battleship.h"

//intenta colocar un barco en el tablero
int putship(Ship* board[10][10], Ship* ship)
{
	if(ship->orientacion == HORIZONTAL)
	{
		if((ship->x + ship->largo) > 10)
		{
			return -1;
		}
		for(int i = ship->x; i < ship->largo + ship->x; i++)
		{
			if(!board[i][ship->y])
			{
				board[i][ship->y] = ship;
			}
			else
			{
				return -1;
			}
		}
	}

	if(ship->orientacion == VERTICAL)
	{
		if((ship->y + ship->largo) > 10)
		{
			return -1;
		}
		for(int i = ship->y; i < ship->largo + ship->y; i++)
		{
			if(!board[ship->x][i])
			{
				board[ship->x][i] = ship;
			}
			else
			{
				return -1;
			}
		}
	}


	return 0;
}

//decide si un intento es un hit, miss o hundido
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
void print_gamestate(Gamestate* gamestate)
{


}

//inicia el tablero, colocando los barcos
int setup()
{

    return 0;
}
