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

#define DEFAULT_PORT 1914
#define DEFAULT_HOSTNAME "127.0.0.1"

void texto()
{
    printf("Desea crear una partida o unirse a una partida?\n");
    printf("1) Crear una partida\n");
    printf("2) Unirse a una partida\n\n");
}

//aqui se realizan las acciones correspondientes a un jugador que tiene que disparar
int shooting_state(Gamestate* gamestate, int socket)
{
    //obtenemos las coordenadas a donde disparar
    int coords[2];
    read_coords(coords);
    //me fijo si ya habia disparado en ese lugar
    while(gamestate->hisBoard[coords[0]][coords[1]] != UNKNOWN)
    {
        printf("Error: ya disparaste en las coordenadas [%d,%d]\n",coords[0],coords[1]);
        read_coords(coords);
    }
    int x = coords[0];
    int y = coords[1];

    //envio el disparo
    printf("Disparando...\n");
    send_shot(socket,x,y);

    //espero la respuesta
    char receive_buffer[8] = {0};
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

        //informacion de la nave destruida
        //necesaria para actualizar el tablero
        int rcv_x = charToInt(receive_buffer[1]);
        int rcv_y = charToInt(receive_buffer[2]);
        int rcv_size = charToInt(receive_buffer[3]);
        orientation rcv_orientacion = charToInt(receive_buffer[4]);

        //destruimos la nave y actualizamos el tablero
        destroy_enemy_ship(gamestate, rcv_x, rcv_y, rcv_size, rcv_orientacion);

        //vemos si ganamos
        gamestate->myState = check_win(gamestate);

    }
    //actualizamos el tablero
    gamestate->hisBoard[x][y] = new_tile;
    return 0;
}

//aqui se realizan las acciones correspondientes a un jugador que esta esperando un disparo
int waiting_state(Gamestate* gamestate, int socket)
{
    //me toca recibir los disparos del oponente
    if(gamestate->myState == WAITING)
    {
        result res;
        char send_buffer[8] = {0};
        int argc = 0;

        printf("Esperando disparo del oponente...\n\n");
        char receive_buffer[8] = {0};
        receive_shot(socket,receive_buffer);
        int x = charToInt(receive_buffer[0]);
        int y = charToInt(receive_buffer[1]);
        printf("Recibido disparo en la posicion [%d,%d]\n\n",x,y);

        //evaluo el disparo
        res = check_hit(&gamestate->myBoard, x, y);
        if(res == MISS)
        {
            printf("AGUA!\n\n");
            gamestate->myState = SHOOTING;
            argc = 1;
        }
        if(res == HIT)
        {
            printf("TOCADO!\n\n");
            argc = 1;
            gamestate->myBoard[x][y]->hitsRemaining--;
            if(gamestate->myBoard[x][y]->hitsRemaining == 0)
            {
                res = SUNK;
            }
        }
        if(res == SUNK)
        {
            printf("HUNDIDO!\n\n");

            //vemos si perdimos
            gamestate->myState = check_loss(gamestate);

            //guardamos en el buffer informacion de la nave hundida
            send_buffer[1] = intToChar(gamestate->myBoard[x][y]->x);
            send_buffer[2] = intToChar(gamestate->myBoard[x][y]->y);
            send_buffer[3] = intToChar(gamestate->myBoard[x][y]->largo);
            send_buffer[4] = intToChar(gamestate->myBoard[x][y]->orientacion);
            argc = 5;
        }
        send_buffer[0] = res;
        //enviamos la respuesta del disparo al oponente
        respond_shot(socket, send_buffer, argc);
    }
    return 0;
}

int play_game(int socket, Gamestate* gamestate)
{
    //gameloop
    //se juega hasta que ganes o pierdas
    while((gamestate->myState != WON) && (gamestate->myState != LOST))
    {
        //imprimo el estado del juego
        print_gamestate(gamestate);

        //me toca disparar
        if(gamestate->myState == SHOOTING)
        {
            shooting_state(gamestate, socket);
        }
        //me toca recibir un disparo
        else
        {
            waiting_state(gamestate, socket);
        }
    }
    return EXIT_SUCCESS;
}

int main()
{

    system("clear");
    printf("|---------------------------v_1.2-|\n");
    printf("Batalla Naval 2020 GOTY Edition\n\n");
    texto();

    Gamestate gamestate;

    //vamos a crear una partida o a unirnos a una?
    char opcion;
    //opcion = getc(stdin);
    //clearstdin();
    while((opcion = get_input()) < 0);
    while(!(opcion == '1' || opcion == '2'))
    {
        printf("%c no es una opcion valida.\n",opcion);
        printf("Elija una opcion valida.\n\n");
        texto();
        while((opcion = get_input()) < 0);
        //opcion = getc(stdin);
        //clearstdin();
    }

    int new_socket = -1;
    int port;
    char buf[16] = {0};
    if(opcion == '1')
    {
        printf("Ingrese el puerto [default: %d]\n",DEFAULT_PORT);
        fgets(buf,sizeof(buf),stdin);
        if(!(port = atoi(buf)))
        {
            port = DEFAULT_PORT;
        }
        else
        {
            clearstdin();
        }
        //abrimos un socket y escuchamos a que alguien se conecte
        new_socket = create_game(port);
        //como somos el que creo, empezamos disparando nosotros
        gamestate.myState = SHOOTING;
    }
    else
    {
        if(opcion == '2')
        {
            printf("Ingrese el hostname [default: %s]\n",DEFAULT_HOSTNAME);
            char* hostname = malloc(sizeof(16 * 'a'));
            fgets(hostname,sizeof(hostname),stdin);
            if(strlen(hostname) < 7)
            {
                hostname = DEFAULT_HOSTNAME;
            }
            else
            {
                clearstdin();
                hostname[strcspn(hostname, "\n")] = 0;
            }

            printf("Ingrese el puerto [default: %d]\n",DEFAULT_PORT);
            fgets(buf,sizeof(buf),stdin);
            if(!(port = atoi(buf)))
            {
                port = DEFAULT_PORT;
            }
            else
            {
                clearstdin();
            }

            //creamos un socket y nos conectamos a una partida ya creada
            new_socket = join_game(hostname,port);
            //como no creamos la partida nosotoros, nos toca esperar el primer disparo
            gamestate.myState = WAITING;
        }
    }
    //error creando o uniendose a una partida
    if(new_socket < 0 )
    {
        printf("Error al crear o unirse a la partida...\n");
    }
    else
    {
        //inicializamos el juego, colocamos las naves
        if(init_gamestate(&gamestate) == EXIT_FAILURE)
        {
            perror("Error: init_gamestate");
            return EXIT_FAILURE;
        }
        //avisamos al oponente que ya estamos listos
        char ready = 'r';
        send(new_socket, &ready, 1, 0);
        //esperamos a que el oponente este listo
        printf("Esperando a que el oponente coloque sus naves...\n\n");
        read(new_socket, &ready, 1);

        //a jugar!
        int res = play_game(new_socket, &gamestate);
        if(res == EXIT_FAILURE)
        {
            printf("Oops, hubo un problema...\n\n");
        }
        if(gamestate.myState == WON)
        {
            printf("Ganaste!\n\n");
        }
        if(gamestate.myState == LOST)
        {
            printf("Perdiste!\n\n");
        }
        close(new_socket);
    }


    return 0;
}
