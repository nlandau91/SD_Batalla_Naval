#ifndef BATTLESHIP_H_
#define BATTLESHIP_H_

/**
    Enum para representar la orientacion de una nave en el tablero
*/
typedef enum orientation
{
    HORIZONTAL,
    VERTICAL
} orientation;

/**
    Enum para representar los tipos de nave
*/
typedef enum ship_type
{
    FRIGATE,
    DESTROYER,
    BATTLESHIP,
    CARRIER
} ship_type;

/**
    Enum para representar los tiles que formaran el tablero del oponente
*/
typedef enum tile
{
    UNKNOWN, /**< representa una posicion desconocida del tablero */
    WATER, /**< representa una posicion donde se sabe que hay agua */
    SHIP, /**< representa una posicion donde se sabe que hay un barco sin hundirse */
    DESTROYED /**< para representar una posicion donde hay una barco hundido */
} tile;

/**
    Enum para representar los posibles estados de un jugador
*/
typedef enum state
{
    SHOOTING,
    WAITING,
    WON,
    LOST
} state;

/**
    Enum para representar el resultado de un disparo
*/
typedef enum result
{
    MISS=48, /**< '0' */
    HIT=49, /**< '1' */
    SUNK=50 /**< '2' */
} result;

/**
    Estructura de una nave
    @var Ship::nombre tablero de punteros a nave representando las naves del jugador
    @var Ship::cantidad de posiciones que ocupa
    @var Ship::x posision horizontal en el tablero
    @var Ship::y posicion vertical en el tablero
    @var Ship::orientacion orientacion horizontal o vertical
    @var Ship::hitsremaining hits restantes para hundir al nave
*/
typedef struct Ship
{
    char nombre[16];
    int largo;
    int x;
    int y;
    orientation orientacion;
    int hitsRemaining;
} Ship;

/**
    Estructura del gamestate
    @var Gamestate::myBoard tablero de punteros a nave representando las naves del jugador
    @var Gamestate::hisBoard tablero de tiles representando el tablero del oponente
    @var Gamestate::myShips un arreglo conteniendo las naves del jugador
    @var Gamestate::hiShips un entero representando cuantas naves le quedan al oponente
    @var Gamestate::myState representa el estado actual del jugador
*/
typedef struct Gamestate
{
    Ship* myBoard[10][10];
    tile hisBoard[10][10];
    Ship myShips[9];
    int hisShips;
    state myState;
} Gamestate;

/**
    Intenta colocar una nave en el tablero de naves
    La posicion de la nave ya esta ingresada en la nave
    @param board puntero al tablero de naves donde se intentara colocar la nave
    @param ship puntero a nave que se intentara colocar
    @return un codigo indicando si tuvo exito la funcion
*/
int put_ship(Ship * (*board)[][10], Ship* ship);

/**
    Decide si un disparo es hit, miss, o hundido
    Si es hit, reduce hits_remaining de la nave en 1
    @param board puntero al tablero de naves donde se hara el checkeo
    @param x coordenada x del disparo
    @param y coordenada y del disparo
    @return un codigo indicando si tuvo exito la funcion
*/
int check_hit(Ship *(*board)[][10], int x, int y);

/**
    Imprime una representacion del gamestate por pantalla
    @param gamestate es el gamestate que se representara por pantalla
    @return un codigo indicando si tuvo exito la funcion
*/
int print_gamestate(Gamestate* gamestate);

/**
    Crear una nave de tipo type
    @param ship puntero a la nave que se creara
    @param type tipo de nave que se creara
    @return un codigo indicando si tuvo exito la funcion
*/
int create_ship(Ship* ship, ship_type type);

/**
    //Imprime un tablero de tiles por pantalla
    @param tileboard es el puntero al tablero de tiles que se imprimira
    @return un codigo indicando si tuvo exito la funcion
*/
int print_tileboard(tile (*tileboard)[][10]);

/**
    //Imprime un tablero de naves por pantalla
    @param shipboard es el puntero al tablero de nave que se imprimira
    @return un codigo indicando si tuvo exito la funcion
*/
int print_shipboard(Ship *(*shipboard)[][10]);

#endif //BATTLESHIP_H_
