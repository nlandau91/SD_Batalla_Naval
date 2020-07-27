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
    SUNK=50, /**< '2' */
    DISCONNECTED=51
} result;

/**
    Estructura de una nave
*/
typedef struct Ship
{
    char nombre[16]; /**< nombre de la nave */
    int largo; /**< cantidad de posiciones que ocupa */
    int x; /**< posision horizontal en el tablero */
    int y; /**< posicion vertical en el tablero */
    orientation orientacion; /**< orientacion horizontal o vertical */
    int hitsRemaining; /**< hits restantes para hundir al nave */
} Ship;

/**
    Estructura del gamestate
*/
typedef struct Gamestate
{
    Ship* myBoard[10][10]; /**< tablero de punteros a nave representando las naves del jugador */
    tile hisBoard[10][10]; /**< tablero de tiles representando el tablero del oponente */
    Ship myShips[9]; /**< un arreglo conteniendo las naves del jugador */
    int hisShips; /**< cuantas naves le quedan al oponente */
    state myState; /**< representa el estado actual del jugador */
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
    Decide si un disparo es hit o miss
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
    Imprime un tablero de tiles por pantalla
    @param tileboard es el puntero al tablero de tiles que se imprimira
    @return un codigo indicando si tuvo exito la funcion
*/
int print_tileboard(tile (*tileboard)[][10]);
/**
    Imprime un tablero de naves por pantalla
    @param shipboard es el puntero al tablero de nave que se imprimira
    @return un codigo indicando si tuvo exito la funcion
*/
int print_shipboard(Ship *(*shipboard)[][10]);

/**
    Checkea si se perdio la partida
    @param gamestate el gamestate correspondiente a esta partida
    @returns LOST si se perdio la partida, el estado anterior en caso contrario
*/
state check_loss(Gamestate* gamestate);

/**
    Checkea si se gano la partida
    @param gamestate el gamestate correspondiente a esta partida
    @returns WON si se gano la partida, el estado anterior en caso contrario
*/
state check_win(Gamestate* gamestate);

/**
    Destruye la nave de un oponente
    @param gamestate gamestate de esta partida
    @param x coordenada x de la nave destruida
    @param y coordenada y de la nave destruida
    @param size tamaño de la nave destruida
    @param orientacion orientacion de la nave destruida
*/
int destroy_enemy_ship(Gamestate* gamestate, int x, int y, int size, orientation orientacion);

int** getGUIShips(Gamestate* gamestate);

int** getGUIAttacks(Gamestate* gamestate);

#endif //BATTLESHIP_H_
