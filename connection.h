#ifndef CONNECTION_H_
#define CONNECTION_H_

//imprime las direcciones por pantalla
//util para ver a que direcciones puede conectarse un oponente
void print_addresses();

//abre un socket y se queda esperando a que se conecte un oponente
int create_game(int port);

//intento conectarme a un oponente
int join_game(char* hostname, int port);

#endif //CONNECTION_H_
