#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include "battleship.h"
#include "connection.h"
#include "init.h"
#include "utils.h"

int texto()
{
    printf("Desea crear una partida o unirse a una partida?\n");
    printf("1) Crear una partida\n");
    printf("2) Unirse a una partida\n\n");
    return EXIT_SUCCESS;
}

int play_game(int socket, Gamestate* gamestate)
{
    if(init_gamestate(gamestate) == EXIT_FAILURE)
    {
        perror("Error: init_gamestate");
        return EXIT_FAILURE;
    }

    //gameloop
    while((gamestate->myState != WON) && (gamestate->myState != LOST))
    {
        //imprimo el estado del juego
        print_gamestate(gamestate);

        int x;
        int y;
        char receive_buffer[8] = {0};
        //modo 1 me toca disparar
        if(gamestate->myState == SHOOTING)
        {
            char input;
            printf("Tienes que disparar.\n");
            printf("Ingrese la coordenada X [0-9]:\n");
            input = getc(stdin);
            clearstdin();
            x = charToInt(input);

            printf("Ingrese la coordenada Y [0-9]:\n");
            input = getc(stdin);
            clearstdin();
            y = charToInt(input);

            //envio el disparo
            printf("Disparando...\n");
            send_shot(socket,x,y);
            //espero la respuesta
            wait_shot_resp(socket,receive_buffer);

            //revisamos el resultado del disparo
            tile new_tile;
            if(receive_buffer[0] == MISS)
            {
                printf("AGUA!\n\n");
                gamestate->myState = WAITING;
                new_tile = WATER;
            }
            if(receive_buffer[0] == HIT)
            {
                printf("TOCADO!\n\n");
                new_tile = SHIP;
            }
            if(receive_buffer[0] == SUNK)
            {
                printf("HUNDIDO!\n\n");
                new_tile = DESTROYED;
                gamestate->hisShips--;

                //informacion de la nave destruida
                //necesaria para actualizar el tablero
                int rcv_x = charToInt(receive_buffer[1]);
                int rcv_y = charToInt(receive_buffer[2]);
                int rcv_size = charToInt(receive_buffer[3]);
                orientation rcv_orientacion = charToInt(receive_buffer[4]);

                //actualizamos la informacion del tablero del oponente
                //hay que cambiar los "ship" por "destroyed"
                if(rcv_orientacion == VERTICAL)
                {
                    for(int i = 0; i < rcv_size; i++)
                    {
                        gamestate->hisBoard[rcv_x][rcv_y+i] = new_tile;
                    }
                }
                else
                {
                    for(int i = 0; i < rcv_size; i++)
                    {
                        gamestate->hisBoard[rcv_x+i][rcv_y] = new_tile;
                    }
                }
                //si no le quedan mas naves al oponente, ganamos
                if(gamestate->hisShips==0)
                {
                    gamestate->myState = WON;
                }
            }
            gamestate->hisBoard[x][y] = new_tile;
        }
        else
        {
            //modo 2 me toca recibir los disparos del oponente
            if(gamestate->myState == WAITING)
            {
                result res;
                char send_buffer[8] = {0};
                int argc = 0;

                printf("Esperando disparo del oponente...\n\n");
                receive_shot(socket,receive_buffer);
                x = charToInt(receive_buffer[0]);
                y = charToInt(receive_buffer[1]);
                printf("Recibido disparo en la posicion [%d,%d]\n\n",x,y);

                //evaluo el disparo
                res = check_hit(&gamestate->myBoard, x, y);
                send_buffer[0] = res;
                if(res == MISS)
                {
                    printf("AGUA!\n\n");
                    gamestate->myState = SHOOTING;
                    argc = 0;
                    //send(socket, send_buffer, 1, 0);
                }
                if(res == HIT)
                {
                    printf("TOCADO!\n\n");
                    argc = 0;
                    //send(socket, send_buffer, 1, 0);
                }
                if(res == SUNK)
                {
                    printf("HUNDIDO!\n\n");

                    int i = 0;
                    while(i<9)
                    {
                        //me fijo si era mi ultimo barco
                        if(gamestate->myShips[i].hitsremaining != 0)
                        {
                            break;
                        }
                        i++;
                    }
                    //te hundieron todos los barcos
                    if(i == 9)
                    {
                        gamestate->myState = LOST;
                    }

                    send_buffer[1] = intToChar(gamestate->myBoard[x][y]->x);
                    send_buffer[2] = intToChar(gamestate->myBoard[x][y]->y);
                    send_buffer[3] = intToChar(gamestate->myBoard[x][y]->largo);
                    send_buffer[4] = intToChar(gamestate->myBoard[x][y]->orientacion);
                    argc = 5;
                }
                respond_shot(socket, res, argc, send_buffer);
            }
        }
    }
    return EXIT_SUCCESS;
}

int main()
{

    system("clear");
    printf("|---------------------------v_1.0-|\n");
    printf("Batalla Naval 2020 GOTY Edition\n\n");
    texto();

    Gamestate gamestate;
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

        gamestate.myState = SHOOTING;
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

            gamestate.myState = WAITING;
        }
    }
    if(new_socket < 0 )
    {
        printf("Error al crear o unirse a la partida...\n");
    }

    //a jugar!
    int res = play_game(new_socket, &gamestate);
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
    close(new_socket);
    return 0;
}
