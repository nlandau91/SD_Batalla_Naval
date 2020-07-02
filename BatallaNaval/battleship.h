#ifndef BATTLESHIP_H_
#define BATTLESHIP_H_

#define HORIZONTAL 0
#define VERTICAL 1

#define CARRIER 5
#define BATTLESHIP 4
#define CRUISER 3
#define SUBMARINE 2
#define DESTROYER 1
#define WATER 0

#define MISS 0
#define HIT 1
#define SUNK 2


//estructura del barco
typedef struct Ship
{
	int x; //posision horizontal en el tablero
	int y; //posicion vertical en el tablero
	int largo; //cantidad de posiciones que ocupa
	int orientacion; //vertical u horizontal
	int hitsremaining; //hits necesarios para hundir al barco
} Ship;

//estructura que almacena el gamestate
typedef struct Gamestate
{
	Ship* myboard[10][10];
	int hisboard[10][10];
	Ship ships[9];
} Gamestate;

//intenta colocar un barco en el tablero
int putship(Ship* board[10][10], Ship* ship);

//decide si un intento es un hit, miss o hundido
int checkHit(Ship* board[10][10], int x, int y);

//imprime una representacion del gamestate por pantalla
void print_gamestate(Gamestate* gamestate);

//inicia el tablero, colocando los barcos
int setup();


#endif //BATTLESHIP_H_
