#include "init.h"
#include "battleship.h"
#include <stdlib.h>
#include <stdio.h>
#include "utils.h"
#include <sys/time.h>

/**
    Inserta las naves automaticamente en el tablero
    @param gamestate el gamestate que aloja las naves y el tablero
    @return un codigo indicando si tuvo exito la funcion
*/
int autoaddships(Gamestate* gamestate)
{
    char* orientaciones[2] = {"horizontal","vertical"};
    //seteamos la semilla de srand
    //srand(time(0));
    struct timeval time; 
    gettimeofday(&time,NULL);
    srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
    for(int i = 0; i < 9; i++)
    {
        int valido = EXIT_FAILURE;
        while(valido!=EXIT_SUCCESS)
        {
            gamestate->myShips[i].x = rand() % 10;
            gamestate->myShips[i].y = rand() % 10;
            if(rand() % 2)
            {
                gamestate->myShips[i].orientacion = HORIZONTAL;
            }
            else
            {
                gamestate->myShips[i].orientacion = VERTICAL;
            }
            valido = put_ship(&gamestate->myBoard,&gamestate->myShips[i]);
            if(valido == EXIT_SUCCESS)
            {
                printf("%s colocado en la posicion [%d,%d] con orientacion %s\n",gamestate->myShips[i].nombre,gamestate->myShips[i].x,gamestate->myShips[i].y,orientaciones[gamestate->myShips[i].orientacion]);
            }
        }
    }
    return EXIT_SUCCESS;
}

/**
    Inserta las naves de forma manual en el tablero
    Le pide input al usuario para ubicarlas
    @param gamestate el gamestate que aloja las naves y el tablero
    @return un codigo indicando si tuvo exito la funcion
*/
int manualaddships(Gamestate* gamestate)
{
    for(int i = 0; i < 9; i++)
    {
        print_shipboard(&gamestate->myBoard);
        int valido = EXIT_FAILURE;
        while(valido!=EXIT_SUCCESS)
        {
            printf("Colocando nave de tipo '%s'...\n",gamestate->myShips[i].nombre);
            int coords[2];
            read_coords(coords);
            gamestate->myShips[i].x = coords[0];
            gamestate->myShips[i].y = coords[1];

            printf("Ingrese la orientacion 'h' para horizontal o 'v' para vertical\n");
            //guardar input en nave
            char input;
            //input = getc(stdin);
            //clearstdin();
            while((input = get_input()) < 0);
            while(input != 'h' && input !='v')
            {
                printf("Error: %c no es una orientacion valida.\n",input);
                printf("Ingrese la orientacion 'h' para horizontal o 'v' para vertical\n");
                //input = getc(stdin);
                //clearstdin();
                while((input = get_input()) < 0);
            }
            if(input == 'h')
            {
                gamestate->myShips[i].orientacion = HORIZONTAL;
            }
            if(input == 'v')
            {
                gamestate->myShips[i].orientacion = VERTICAL;
            }

            //intengamos poner la nave en mi tablero
            valido = put_ship(&gamestate->myBoard,&gamestate->myShips[i]);
            if(valido == EXIT_SUCCESS)
            {
                printf("%s colocado en la posicion [%d,%d]\n",gamestate->myShips[i].nombre,gamestate->myShips[i].x,gamestate->myShips[i].y);
            }
            if(valido != EXIT_SUCCESS)
            {
                printf("No se pudo colocar la nave. Intente nuevamente...\n\n");
            }
        }
    }
    return EXIT_SUCCESS;
}

/**
    Inserta una nave en el tablero en la posicion y orientacion indicada
    @param gamestate el gamestate que aloja las naves y el tablero
    @param ship nave a ubicar
    @param x coordenada x
    @param y coordenada y
    @param orientacion orientacion del barco
    @return un codigo indicando si tuvo exito la funcion
*/
int manualAddShipGUI(Gamestate* gamestate, int ship, int x, int y, int orientacion)
{
    int valido = EXIT_FAILURE;
    printf("Colocando nave de tipo '%s'...\n",gamestate->myShips[ship].nombre);
    gamestate->myShips[ship].x = x;
    gamestate->myShips[ship].y = y;
    if(orientacion == 0)
    {
        gamestate->myShips[ship].orientacion = HORIZONTAL;
    }
    if(orientacion == 1)
    {
        gamestate->myShips[ship].orientacion = VERTICAL;
    }
    //intengamos poner la nave en mi tablero
    valido = put_ship(&gamestate->myBoard,&gamestate->myShips[ship]);
    if(valido == EXIT_SUCCESS)
    {
        printf("%s colocado en la posicion [%d,%d]\n",gamestate->myShips[ship].nombre,gamestate->myShips[ship].x,gamestate->myShips[ship].y);
        return 0;
    }
    if(valido != EXIT_SUCCESS)
    {
        printf("No se pudo colocar la nave. Intente nuevamente...\n\n");
        return -1;
    }
    
}



/**
    Inicializa el arreglo de naves pasado por parametro
    @param shiparray puntero al arreglo de naves que sera inicializado
    @return un codigo indicando si tuvo exito la funcion
*/
int init_shiparray(Ship (*shiparray)[9])
{
    create_ship(&(*shiparray)[0], CARRIER);
    create_ship(&(*shiparray)[1], BATTLESHIP);
    create_ship(&(*shiparray)[2], BATTLESHIP);
    create_ship(&(*shiparray)[3], BATTLESHIP);
    create_ship(&(*shiparray)[4], DESTROYER);
    create_ship(&(*shiparray)[5], DESTROYER);
    create_ship(&(*shiparray)[6], DESTROYER);;
    create_ship(&(*shiparray)[7], FRIGATE);
    create_ship(&(*shiparray)[8], FRIGATE);
    return EXIT_SUCCESS;
}

/**
    Inicializa tablero de tiles pasado por parametro
    @param tileboard puntero al tablero de tiles que sera inicializado
    @return un codigo indicando si tuvo exito la funcion
*/
int init_tileboard(tile (*tileboard)[][10])
{
    for(int i = 0; i < 10; i++)
    {
        for(int j = 0; j < 10; j++)
        {
            (*tileboard)[i][j] = UNKNOWN;

        }
    }
    return EXIT_SUCCESS;
}

/**
    Inicializa el tablero de naves pasado por parametro
    @param shipboard puntero al tablero de naves que sera inicializado
    @return un codigo indicando si tuvo exito la funcion
*/
int init_shipboard(Ship* (*shipboard)[][10])
{
    for(int i = 0; i < 10; i++)
    {
        for(int j = 0; j < 10; j++)
        {
            (*shipboard)[i][j] = NULL;

        }
    }
    return EXIT_SUCCESS;
}

int init_gamestate(Gamestate* gamestate)
{
    //el tablero del oponente se inicializa con posiciones desconocidas
    init_tileboard(&gamestate->hisBoard);

    //el oponente inicia con sus nueve naves
    gamestate->hisShips = 9;

    //creamos las nueve naves que luego ubicaremos en nuestro tablero
    init_shiparray(&gamestate->myShips);

    //vaciamos nuestro tablero para luego cargar los naves
    init_shipboard(&gamestate->myBoard);

    char input;
    printf("Si quieres que las naves se ubiquen automaticamente, pulse 's'\n");
    printf("De lo contrario, pulse 'n'...\n");
    //input = getc(stdin);
    while((input = get_input()) < 0);
    //if (input != '\n') clearstdin();
    while(input != 's' && input != 'n')
    {
        printf("Error: %c no es una opcion valida\n",input);
        printf("Si quieres que las naves se ubiquen automaticamente, pulse 's'\n");
        printf("De lo contrario, pulse 'n'...\n");
        while((input = get_input()) < 0);
        //input = getc(stdin);
        //clearstdin();
    }
    if(input == 's')
    {
        autoaddships(gamestate);
    }
    else
    {
        manualaddships(gamestate);
    }
    return EXIT_SUCCESS;
}

/**
    Inicia el estado del juego para la GUI
    @param gamestate el gamestate que aloja las naves y el tablero
    @return un codigo indicando si tuvo exito la funcion
*/
int init_gamestate2(Gamestate* gamestate)
{
    //el tablero del oponente se inicializa con posiciones desconocidas
    init_tileboard(&gamestate->hisBoard);

    //el oponente inicia con sus nueve naves
    gamestate->hisShips = 9;

    //creamos las nueve naves que luego ubicaremos en nuestro tablero
    init_shiparray(&gamestate->myShips);

    //vaciamos nuestro tablero para luego cargar los naves
    init_shipboard(&gamestate->myBoard);
}
