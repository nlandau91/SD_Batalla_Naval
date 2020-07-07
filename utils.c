#include <stdlib.h>
#include <stdio.h>

int clearstdin()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
    return EXIT_SUCCESS;
}

int charToInt(char c)
{
    return c - '0';
}

char intToChar(int n)
{
    return n + '0';
}

int read_coords(int coords[2])
{
    char input;

    printf("Ingrese la coordenada X [0-9]:\n");
    input = getc(stdin);
    clearstdin();
    while(!(input >= '0' && input <='9'))
    {
        printf("Error: %c no es una coordenada valida.",input);
        printf("Ingrese la coordenada X [0-9]:\n");
        input = getc(stdin);
        clearstdin();
    }
    coords[0] = charToInt(input);


    printf("Ingrese la coordenada Y [0-9]:\n");
    input = getc(stdin);
    clearstdin();
    while(!(input >= '0' && input <= '9'))
    {
        printf("Error: %c no es una coordenada valida.",input);
        printf("Ingrese la coordenada Y [0-9]:\n");
        input = getc(stdin);
        clearstdin();
    }
    coords[1] = charToInt(input);
    return EXIT_SUCCESS;
}
