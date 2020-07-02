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

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt)))
    {
        perror("setsockopt");
        return -1;
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( port );

    // Forcefully attaching socket to the port 8080
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
    //para cada barco a ingresar
    //preguntarle al usuario coordenadas y orientacion
    //luego llamar a putship() y agregarlo a la lista de barcos

    //el tablero del oponente se inicializa vacio
    return 0;
}

int play_game(int socket, int mode)
{
    int valread;
    char receive_buffer[1024] = {0};
    char send_buffer[1024] = {0};

    Gamestate* gamestate;
    init_gamestate(gamestate);


    if(mode == 1) //soy el host, comienzo la partida
    {
        printf("Vos: ");
        fgets(send_buffer, sizeof(send_buffer), stdin);
        send(socket, send_buffer, strlen(send_buffer), 0);
    }
    while(1)
    {
        valread = read( socket, receive_buffer, 1024);
        printf("El: %s\n",receive_buffer);
        printf("Vos: ");
        fgets(send_buffer, sizeof(send_buffer), stdin);
        send(socket, send_buffer, strlen(send_buffer), 0);
    }

    return 0;
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
    play_game(new_socket, opcion);

    return 0;
}
