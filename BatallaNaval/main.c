#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>

void texto()
{
	printf("Desea crear una partida o unirse a una partida?\n");
    printf("1) Crear una partida\n");
    printf("2) Unirse a una partida\n\n");
}

int create_game(int port)
{
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
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                       (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        return -1;
    }

    return new_socket;
}

int join_game(int hostname, int port)
{

    return 0;
}

int play_game(int socket)
{
    int valread;
    char buffer[1024] = {0};
    char *hello = "Hello from your opponent";

    valread = read( socket , buffer, 1024);
    printf("%s\n",buffer );
    send(socket , hello , strlen(hello) , 0 );
    printf("Hello message sent\n");
}

int main()
{
    system("clear");
    printf("Batalla Naval 2020 GOTY Edition\n\n");
    texto();

    int opcion;
    fflush(stdin);
    scanf("%d,",&opcion);

    while(!(opcion == 1 || opcion ==2))
	{
        printf("%d no es una opcion valida.\n",opcion);
		printf("Elija una opcion valida.\n\n");
		texto();
		fflush(stdin);
		scanf("%d",&opcion);
	}

    int new_socket;
	if(opcion == 1)
	{
        int port;
        new_socket = create_game(port);
	}
	else{
        if(opcion == 2)
        {
            int hostname;
            int port;
            new_socket = join_game(hostname,port);
        }
	}

	play_game(new_socket);

    return 0;
}
