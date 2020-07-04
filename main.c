#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include "battleship.h"

void texto()
{
    printf("Desea crear una partida o unirse a una partida?\n");
    printf("1) Crear una partida\n");
    printf("2) Unirse a una partida\n\n");
}

void print_addresses()
{
    struct ifaddrs *ifap, *ifa;
    struct sockaddr_in *sa;
    char *addr;

    getifaddrs (&ifap);
    for (ifa = ifap; ifa; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr && ifa->ifa_addr->sa_family==AF_INET)
        {
            sa = (struct sockaddr_in *) ifa->ifa_addr;
            addr = inet_ntoa(sa->sin_addr);
            printf("Interface: %s\tAddress: %s\n", ifa->ifa_name, addr);
        }
    }

    freeifaddrs(ifap);
}

int create_game(int port)
{
    printf("Creando partida...\n");
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        return -1;
    }

    // Forcefully attaching socket to the port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt)))
    {
        perror("setsockopt");
        return -1;
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( port );

    // Forcefully attaching socket to the port
    if (bind(server_fd, (struct sockaddr *)&address,
             sizeof(address))<0)
    {
        perror("bind failed");
        return -1;
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        return -1;
    }
    printf("Partida creada!\n");
    printf("Esperando oponente...\n\n");
    printf("PORT: %d\n",port);
    printf("Addresses:\n");
    print_addresses();
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                             (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        return -1;
    }
    printf("\nOponente conectado, comenzando partida...\n\n");

    return new_socket;
}

int join_game(char* hostname, int port)
{
    printf("Intentando conectarse a %s : %d\n\n",hostname,port);
    int sock = 0;
    struct sockaddr_in serv_addr;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, hostname, &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    printf("Conectado! comenzando partida...\n\n");
    return sock;
}



//inicializa el gamestate
int init_gamestate(Gamestate* gamestate)
{
    //el tablero del oponente se inicializa con posiciones desconocidas
    for(int i = 0; i < 10; i++)
    {
        for(int j = 0; j < 10; j++)
        {
            gamestate->hisboard[i][j] = UNKNOWN;
        }
    }
    return 0;

    //el oponente inicia con sus nueve naves
    gamestate->hisships = 9;

    //creamos las nueve naves que luego ubicaremos en nuestro tablero
    Ship* ship;
    ship = create_ship(CARRIER);
    gamestate->myships[0] = *ship;
    ship = create_ship(BATTLESHIP);
    gamestate->myships[1] = *ship;
    ship = create_ship(BATTLESHIP);
    gamestate->myships[2] = *ship;
    ship = create_ship(BATTLESHIP);
    gamestate->myships[3] = *ship;
    ship = create_ship(DESTROYER);
    gamestate->myships[4] = *ship;
    ship = create_ship(DESTROYER);
    gamestate->myships[5] = *ship;
    ship = create_ship(DESTROYER);
    gamestate->myships[6] = *ship;
    ship = create_ship(FRIGATE);
    gamestate->myships[7] = *ship;
    ship = create_ship(FRIGATE);
    gamestate->myships[8] = *ship;

    //agregamos nuestras naves a nuestro tablero
    for(int i = 0; i < 9; i++)
    {
        int valido = -1;
        while(valido<0)
        {
            printf("Colocando nave de tipo '%s'...\n",gamestate->myships[i].nombre);
            printf("Ingrese la coordenada X entre 0 y 9, de izquierda a derecha:\n");
            scanf("%d",&gamestate->myships[i].x);
            printf("Ingrese la coordenada Y entre 0 y 9, de arriba a abajo:\n");
            scanf("%d",&gamestate->myships[i].y);

            printf("Ingrese la orientacion 'h' para horizontal o 'v' para vertical\n");
            //guardar input en barco
            char orientacion;
            scanf("%c",&orientacion);
            if(orientacion == 'h')
            {
                gamestate->myships[i].orientacion = HORIZONTAL;
            }
            if(orientacion == 'v')
            {
                gamestate->myships[i].orientacion = VERTICAL;
            }

            //intengamos poner el barco en mi tablero
            valido = putship(gamestate->myboard,&gamestate->myships[i]);
        }
    }
    gamestate->state = PLAYING;
}

int play_game(int socket, int mode)
{
    char receive_buffer[8] = {0};
    char send_buffer[8] = {0};

    Gamestate gamestate;
    init_gamestate(&gamestate);

    //gameloop
    while(gamestate.state == PLAYING)
    {
        //imprimo el estado del juego
        print_gamestate(&gamestate);

        char x = '0';
        char y = '0';
        int res;
        //modo 1 me toca disparar
        if(mode == 1)
        {
            fflush(stdin);

            printf("Tienes que disparar.\n");
            printf("Ingrese la coordenada X [0-9]:\n");
            scanf("%c",&x);
            send_buffer[0] = x;

            printf("Ingrese la coordenada X [0-9]:\n");
            scanf("%c",&y);
            send_buffer[1] = y;

            //envio el disparo y espero la respuesta
            printf("Disparando...\n");
            send(socket, send_buffer, 2, 0);
            read(socket, receive_buffer, 1);

            //revisamos el resultado del disparo
            if(receive_buffer[0] == MISS)
            {
                printf("AGUA! Ahora le toca al oponente...\n\n");
                mode = 2;
                res = WATER;
            }
            if(receive_buffer[0] == HIT)
            {
                printf("TOCADO! Vuelve a disparar...\n\n");
                res = SHIP;
            }
            if(receive_buffer[0] == SUNK)
            {

                gamestate.hisships--;
                if(gamestate.hisships>0)
                {
                printf("HUNDIDO! Vuelve a disparar...\n\n");
                }
                else
                {
                    printf("HUNDIDO! Has ganado!\n\n");
                    gamestate.state = WON;
                }
                res = DESTROYED;
            }
            gamestate.hisboard[x - '0'][y - '0'] = res;

        }
        //modo 2 me toca recibir los disparos del oponente
        if(mode == 2)
        {
            //recibo las coordenadas del disparo del oponente
            read(socket, receive_buffer, 1);
            x = receive_buffer[0];
            y = receive_buffer[1];

            //evaluo el disparo
            res = checkHit(gamestate.myboard, x, y);
            if(res == MISS)
            {
                mode = 1;
            }
            send_buffer[0] = res;
            send(socket, send_buffer, 1, 0);
            //me fijo si era mi ultimo barco
            if(res == SUNK)
            {
                int i = 0;
                while(i<9)
                {
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

    int opcion;
    fflush(stdin);
    scanf("%d",&opcion);
    getchar();

    while(!(opcion == 1 || opcion ==2))
    {
        printf("%d no es una opcion valida.\n",opcion);
        printf("Elija una opcion valida.\n\n");
        texto();
        scanf("%d",&opcion);
        getchar();
    }

    int new_socket = -1;;
    if(opcion == 1)
    {
        printf("Ingrese el puerto\n");
        int port;
        scanf("%d",&port);
        getchar();

        new_socket = create_game(port);
    }
    else
    {
        if(opcion == 2)
        {
            printf("Ingrese el hostname\n");
            char hostname[16] = {0};
            fgets(hostname,sizeof(hostname),stdin);
            hostname[strcspn(hostname, "\n")] = 0;
            //char* hostname = "127.0.0.1";

            printf("Ingrese el puerto\n");
            int port;
            scanf("%d",&port);
            getchar();

            new_socket = join_game(hostname,port);
        }
    }
    if(new_socket < 0 )
    {
        printf("Error al crear o unirse a la partida...\n");
    }

   //a jugar!
    int res = play_game(new_socket, opcion);
    if(res == WON)
    {

    }
    if(res == LOST)
    {

    }

    return 0;
}
