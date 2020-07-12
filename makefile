all: main ejecutable
.phony: all

main : main.c battleship.c connection.c init.c utils.c
	gcc -o main main.c battleship.c connection.c init.c utils.c

ejecutable : battleship.c connection.c init.c utils.c gui.c
		gcc -o ejecutable battleship.c connection.c init.c utils.c gui.c `pkg-config --cflags --libs gtk+-3.0` `pkg-config --cflags --libs x11` -rdynamic
