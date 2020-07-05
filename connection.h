#ifndef CONNECTION_H_
#define CONNECTION_H_

void print_addresses();

int create_game(int port);

int join_game(char* hostname, int port);

#endif //CONNECTION_H_