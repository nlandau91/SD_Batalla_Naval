//#include "main.c"
/*A partir de aca vienen metodos para comunicar con la GUI*/

int GUI_create_game(int port)
{
    int new_socket = create_game(port);
    return new_socket;
}


int GUI_join_game(char* hostname, int port)
{
    int new_socket = join_game(hostname,port);
    return new_socket;
}







