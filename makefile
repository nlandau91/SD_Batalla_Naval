all: main
.phony: all

main : main.c battleship.c connection.c init.c utils.c
	gcc -o main main.c battleship.c connection.c init.c utils.c
