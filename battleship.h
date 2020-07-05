#ifndef BATTLESHIP_H_
#define BATTLESHIP_H_

#define HORIZONTAL 0
#define VERTICAL 1

enum ship_types {FRIGATE, DESTROYER, BATTLESHIP, CARRIER};

//definiciones para la respuesta a un disparo
#define MISS 48 //0
#define HIT 49 //1
#define SUNK 50 //2

//definiciones para lo que vamos a representar en pantalla
#define UNKNOWN 0 //una posicion donde no se sabe que hay
#define WATER 1 //una posicion donde ya disparamos y sabemos que hay agua
#define SHIP 2 //una posicion donde disparamos y golpeamos un barco
#define DESTROYED 3 //una posicion donde hay un barco hundido

#define SHOOTING 0
#define WAITING 1
#define WON 2
#define LOST 3

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
    Ship myships[9];
    int hisships;
    int state;

} Gamestate;

//intenta colocar un barco en el tablero
int putship(Ship * (*board)[][10], Ship* ship);

//decide si un intento es un hit, miss o hundido
int checkHit(Ship *(*board)[][10], int x, int y);

//imprime una representacion del gamestate por pantalla
int print_gamestate(Gamestate* gamestate);

//crea una nave
int create_ship(Ship* ship, int type);

//imprime un tablero de naves que representa el tablero del jugador
int print_intboard(int (*intboard)[][10]);

//imprime un tablero de ints que representa el tablero del oponente
int print_shipboard(Ship *(*shipboard)[][10]);

#endif //BATTLESHIP_H_
