#ifndef BATTLESHIP_H_
#define BATTLESHIP_H_

typedef enum orientation {HORIZONTAL, VERTICAL} orientation;

typedef enum ship_type {FRIGATE, DESTROYER, BATTLESHIP, CARRIER} ship_type;

typedef enum tile {UNKNOWN, WATER, SHIP, DESTROYED} tile;

typedef enum state {SHOOTING, WAITING, WON, LOST} state;

typedef enum result {MISS=48,HIT=49,SUNK=50} result;

//estructura del barco
typedef struct Ship
{
    char nombre[16];
    int largo; //cantidad de posiciones que ocupa
    int x; //posision horizontal en el tablero
    int y; //posicion vertical en el tablero
    orientation orientacion; //vertical u horizontal
    int hitsremaining; //hits restantes para hundir al barco
} Ship;

//estructura que almacena el gamestate
typedef struct Gamestate
{
    Ship* myBoard[10][10];
    tile hisBoard[10][10];
    Ship myShips[9];
    int hisShips;
    state myState;

} Gamestate;

//intenta colocar un barco en el tablero
int putship(Ship * (*board)[][10], Ship* ship);

//decide si un intento es un hit, miss o hundido
int checkHit(Ship *(*board)[][10], int x, int y);

//imprime una representacion del gamestate por pantalla
int print_gamestate(Gamestate* gamestate);

//crea una nave
int create_ship(Ship* ship, ship_type type);

//imprime un tablero de naves que representa el tablero del jugador
int print_intboard(enum tile (*intboard)[][10]);

//imprime un tablero de ints que representa el tablero del oponente
int print_shipboard(Ship *(*shipboard)[][10]);

#endif //BATTLESHIP_H_
