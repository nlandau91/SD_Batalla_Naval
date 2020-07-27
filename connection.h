#ifndef CONNECTION_H_
#define CONNECTION_H_

/**
    Imprime por pantalla las interfaces de red y sus direcciones
    Util para poder decirle a un posible oponente a que direccion conectarse
*/
void print_addresses();

/**
    Abre un socket y se queda esperando a que se conecte un oponente
    @param port el puerto correspondiente al socket abierto
    @return el socket creado en caso de exito, -1 en caso de falla
*/
int create_game(int port);

/**
    Abre un socket e intenta conectarse a una partida ya creada
    @param hostname el ip donde esta creada la partida
    @param el puerto por donde se estara escuchando
    @return un codigo indicando si tuvo exito la funcion
*/
int join_game(char* hostname, int port);

/**
    Envia un mensaje correspondiente a un disparo
    @param socket por el cual se enviara el mensaje
    @param x coordenada x del disparo
    @param y coordenada y del disparo
    @return un codigo indicando si tuvo exito la funcion
*/
int send_shot(int socket, int x, int y);

/**
    Envia un mensaje correspondiente a la respuesta a un disparo
    @param socket por el cual se enviara el mensaje
    @param argv contiene los argumentos de la respuesta
    @param argc indica la cantidad de argumentos de la respuesta
    @return un codigo indicando si tuvo exito la funcion
*/
int respond_shot(int socket,char argv[], int argc);

/**
    Espera por un mensaje de disparo
    @param socket en el cual se espera el mensaje
    @param buf arreglo donde se guardara el mensaje recibido
    @return un codigo indicando si tuvo exito la funcion
*/
int receive_shot(int socket, char buf[2]);

/**
    Espera por un mensaje de respuesta de disparo
    @param socket en el cual se espera el mensaje
    @param buf arreglo donde se guardara el mensaje recibido
    @return un codigo indicando si tuvo exito la funcion
*/
int wait_shot_resp(int socket, char buf[5]);

/**
    Envia un mensaje de desconexion al oponente
    @param socket el socket creado para la comunicacion
    @return un codigo indicando si tuvo exito la funcion
*/
int send_disconnect(int socket);

/**
    Revisa si llego un mensaje de desconexion.
    @param socket el socket creado para la comunicacion
    @return un codigo indicando si llego el mensaje o no
*/
int check_disconnect(int socket);
#endif //CONNECTION_H_
