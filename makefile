all: main
.phony: all

main : main.c battleship.c
	gcc -o main main.c battleship.c
