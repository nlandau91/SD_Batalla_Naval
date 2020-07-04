#ifndef BATTLESHIP_H_
#define BATTLESHIP_H_

#define HORIZONTAL 0
#define VERTICAL 1

enum ship_types {FRIGATE, DESTROYER, BATTLESHIP, CARRIER};

#define MISS 0
#define HIT 1
#define SUNK 2


//estructura del barco
typedef struct Ship
{
    char nombre[16];
    int largo; //cantidad de posiciones que ocupa
    int x; //posision horizontal en el tablero
    int y; //posicion vertical en el tablero
    int orientacion; //vertical u horizontal
    int hitsremaining; //hits restantes para hundir al barco
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

//crea una nave
Ship* create_ship(int type);

//inicia el tablero, colocando los barcos
int setup();


#endif //BATTLESHIP_H_
