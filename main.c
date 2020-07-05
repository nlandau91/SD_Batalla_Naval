#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <time.h>
#include "battleship.h"
#include "connection.h"

int texto()
{
    printf("Desea crear una partida o unirse a una partida?\n");
    printf("1) Crear una partida\n");
    printf("2) Unirse a una partida\n\n");
    return EXIT_SUCCESS;
}

int clearstdin()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
    return EXIT_SUCCESS;
}

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

int init_intboard(int (*intboard)[][10])
{
    for(int i = 0; i < 10; i++)
    {
        for(int j = 0; j < 10; j++)
        {
            (*intboard)[i][j] = UNKNOWN;

        }
    }
    return EXIT_SUCCESS;
}

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

//coloca las naves automaticamente en el tablero de forma aleatoria
int autoaddships(Gamestate* gamestate)
{
    char* orientaciones[2] = {"horizontal","vertical"};
    srand(time(0));
    for(int i = 0; i < 9; i++)
    {
        int valido = EXIT_FAILURE;
        while(valido!=EXIT_SUCCESS)
        {
            gamestate->myships[i].x = rand() % 10;
            gamestate->myships[i].y = rand() % 10;
            if(rand() % 2)
            {
                gamestate->myships[i].orientacion = HORIZONTAL;
            }
            else
            {
                gamestate->myships[i].orientacion = VERTICAL;
            }
            valido = putship(&gamestate->myboard,&gamestate->myships[i]);
            if(valido == EXIT_SUCCESS)
            {
                printf("%s colocado en la posicion [%d,%d] con orientacion %s\n",gamestate->myships[i].nombre,gamestate->myships[i].x,gamestate->myships[i].y,orientaciones[gamestate->myships[i].orientacion]);
            }
        }
    }
    return EXIT_SUCCESS;
}

//pide input al usuario para colocar las naves manualmente
int manualaddships(Gamestate* gamestate)
{
    for(int i = 0; i < 9; i++)
    {   print_shipboard(&gamestate->myboard);
        int valido = EXIT_FAILURE;
        while(valido!=EXIT_SUCCESS)
        {
            char input;
            printf("Colocando nave de tipo '%s'...\n",gamestate->myships[i].nombre);
            printf("Ingrese la coordenada X entre 0 y 9, de izquierda a derecha:\n");
            input = getc(stdin);
            clearstdin();
            gamestate->myships[i].x = input - '0';
            printf("Ingrese la coordenada Y entre 0 y 9, de arriba a abajo:\n");
            input = getc(stdin);
            clearstdin();
            gamestate->myships[i].y = input - '0';

            printf("Ingrese la orientacion 'h' para horizontal o 'v' para vertical\n");
            //guardar input en barco
            input = getc(stdin);
            clearstdin();
            if(input == 'h')
            {
                gamestate->myships[i].orientacion = HORIZONTAL;
            }
            if(input == 'v')
            {
                gamestate->myships[i].orientacion = VERTICAL;
            }

            //intengamos poner el barco en mi tablero
            valido = putship(&gamestate->myboard,&gamestate->myships[i]);
            if(valido == EXIT_SUCCESS)
            {
                printf("%s colocado en la posicion [%d,%d]\n",gamestate->myships[i].nombre,gamestate->myships[i].x,gamestate->myships[i].y);
            }
            if(valido != EXIT_SUCCESS)
            {
                printf("No se pudo colocar la nave. Intente nuevamente...\n\n");
            }
        }
    }
    return EXIT_SUCCESS;
}
//inicializa el gamestate
int init_gamestate(Gamestate* gamestate)
{
    //el tablero del oponente se inicializa con posiciones desconocidas
    init_intboard(&gamestate->hisboard);

    //el oponente inicia con sus nueve naves
    gamestate->hisships = 9;

    //creamos las nueve naves que luego ubicaremos en nuestro tablero
    init_shiparray(&gamestate->myships);

    //vaciamos nuestro tablero para luego cargar los barcos
    init_shipboard(&gamestate->myboard);

    char input;
    printf("Si quieres que las naves se ubiquen automaticamente, pulse 's'\n");
    printf("De lo contrario, pulse 'n'...\n");
    input = getc(stdin);
    clearstdin();
    if(input == 's')
    {
        autoaddships(gamestate);
    }
    else
    {
        manualaddships(gamestate);
    }
    gamestate->state = WAITING;
    return EXIT_SUCCESS;
}

int play_game(int socket, int mode)
{
    Gamestate gamestate;
    if(init_gamestate(&gamestate) == EXIT_FAILURE)
    {
        perror("Error: init_gamestate");
        return EXIT_FAILURE;
    }
    //esto es para que el jugador que creo la partida empiece
    //idealmente implementamos alguna forma aleatoria para elegir quien empieza
    if(mode == 1)
    {
        gamestate.state = SHOOTING;
    }
    if(mode == 2)
    {
        gamestate.state = WAITING;
    }
    char receive_buffer[8] = {0};
    char send_buffer[8] = {0};

    //gameloop
    while((gamestate.state != WON) && (gamestate.state != LOST))
    {
        //imprimo el estado del juego
        print_gamestate(&gamestate);

        char x = '0';
        char y = '0';
        int res;
        //modo 1 me toca disparar
        if(gamestate.state == SHOOTING)
        {
            printf("Tienes que disparar.\n");
            printf("Ingrese la coordenada X [0-9]:\n");
            x = getc(stdin);
            clearstdin();
            send_buffer[0] = x;

            printf("Ingrese la coordenada Y [0-9]:\n");
            y = getc(stdin);
            clearstdin();
            send_buffer[1] = y;

            //envio el disparo y espero la respuesta
            printf("Disparando...\n");
            send(socket, send_buffer, 2, 0);
            read(socket, receive_buffer, 5);

            //revisamos el resultado del disparo
            if(receive_buffer[0] == MISS)
            {
                printf("AGUA!\n\n");
                gamestate.state = WAITING;
                res = WATER;
            }
            if(receive_buffer[0] == HIT)
            {
                printf("TOCADO!\n\n");
                res = SHIP;
            }
            if(receive_buffer[0] == SUNK)
            {
                printf("HUNDIDO!\n\n");
                res = DESTROYED;
                gamestate.hisships--;

                //informacion de la nave destruida
                //necesaria para actualizar el tablero
                int rcv_x = receive_buffer[1] - '0';
                int rcv_y = receive_buffer[2] - '0';
                int rcv_size = receive_buffer[3] - '0';
                int rcv_orientacion = receive_buffer[4] - '0';

                //si no le quedan mas naves al oponente, ganamos
                if(gamestate.hisships==0)
                {
                    gamestate.state = WON;
                }

                //actualizamos la informacion del tablero del oponente
                //hay que cambiar los "ship" por "destroyed"
                if(rcv_orientacion == VERTICAL)
                {
                    for(int i = 0; i < rcv_size; i++)
                    {
                        gamestate.hisboard[rcv_x][rcv_y+i] = res;
                    }
                }
                else
                {
                    for(int i = 0; i < rcv_size; i++)
                    {
                        gamestate.hisboard[rcv_x+i][rcv_y] = res;
                    }
                }
            }
            gamestate.hisboard[x - '0'][y - '0'] = res;
        }
        else
        {
            //modo 2 me toca recibir los disparos del oponente
            if(gamestate.state == WAITING)
            {
                printf("Esperando disparo del oponente...\n\n");
                //recibo las coordenadas del disparo del oponente
                read(socket, receive_buffer, 2);
                x = receive_buffer[0];
                y = receive_buffer[1];
                printf("Recibido disparo en la posicion [%d,%d]\n\n",receive_buffer[0] - '0',receive_buffer[1] - '0');

                //evaluo el disparo
                res = checkHit(&gamestate.myboard, x - '0', y - '0');
                send_buffer[0] = res;
                if(res == MISS)
                {
                    printf("AGUA!\n\n");
                    gamestate.state = SHOOTING;
                    send(socket, send_buffer, 1, 0);
                }
                else
                {
                    if(res == HIT)
                    {
                        printf("TOCADO!\n\n");
                        send(socket, send_buffer, 1, 0);
                    }
                    else
                    {
                        if(res == SUNK)
                        {
                            printf("HUNDIDO!\n\n");
                            int i = 0;
                            while(i<9)
                            {
                                //me fijo si era mi ultimo barco
                                if(gamestate.myships[i].hitsremaining != 0)
                                {
                                    break;
                                }
                                i++;
                            }
                            //te hundieron todos los barcos
                            if(i == 9)
                            {
                                gamestate.state = LOST;
                            }

                            send_buffer[1] = gamestate.myboard[x - '0'][y - '0']->x + '0';
                            send_buffer[2] = gamestate.myboard[x - '0'][y - '0']->y + '0';
                            send_buffer[3] = gamestate.myboard[x - '0'][y - '0']->largo + '0';
                            send_buffer[4] = gamestate.myboard[x - '0'][y - '0']->orientacion + '0';
                            send(socket, send_buffer, 5, 0);
                        }
                    }
                }
            }
        }
    }
    return gamestate.state;
}

int main()
{

    system("clear");
    printf("Batalla Naval 2020 GOTY Edition\n\n");
    texto();

    char opcion;
    opcion = getc(stdin);
    clearstdin();

    while(!(opcion == '1' || opcion == '2'))
    {
        printf("%c no es una opcion valida.\n",opcion);
        printf("Elija una opcion valida.\n\n");
        texto();
        opcion = getc(stdin);
        clearstdin();
    }

    int new_socket = -1;
    int port;
    char buf[16] = {0};
    if(opcion == '1')
    {
        printf("Ingrese el puerto\n");
        fgets(buf,sizeof(buf),stdin);
        port = atoi(buf);

        new_socket = create_game(port);
    }
    else
    {
        if(opcion == '2')
        {
            printf("Ingrese el hostname\n");
            char hostname[16] = {0};
            fgets(hostname,sizeof(hostname),stdin);
            hostname[strcspn(hostname, "\n")] = 0;
            //char* hostname = "127.0.0.1";

            printf("Ingrese el puerto\n");
            fgets(buf,sizeof(buf),stdin);
            port = atoi(buf);

            new_socket = join_game(hostname,port);
        }
    }
    if(new_socket < 0 )
    {
        printf("Error al crear o unirse a la partida...\n");
    }

    //a jugar!
    int res = play_game(new_socket, opcion - '0');
    if(res == EXIT_FAILURE)
    {
        printf("Oops, hubo un problema...\n\n");
    }
    if(res == WON)
    {
        printf("Ganaste!\n\n");
    }
    if(res == LOST)
    {
        printf("Perdiste!\n\n");
    }

    return 0;
}
