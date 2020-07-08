#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "utils.h"

#define ANSI_COLOR_RESET   "\x1b[0m"

static const char *fg_color_string[] = {
    "\x1b[30m", "\x1b[31m", "\x1b[32m", "\x1b[33m","\x1b[34m", "\x1b[35m", "\x1b[36m", "\x1b[37m"
};

static const char *bg_color_string[] = {
    "\x1b[40m", "\x1b[41m", "\x1b[42m", "\x1b[43m","\x1b[44m", "\x1b[45m", "\x1b[46m", "\x1b[47m"
};

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
    //input = getc(stdin);
    //clearstdin();
    while((input = get_input()) < 0);
    while(!(input >= '0' && input <='9'))
    {
        printf("Error: %c no es una coordenada valida.",input);
        printf("Ingrese la coordenada X [0-9]:\n");
        while((input = get_input()) < 0);
        //input = getc(stdin);
        //clearstdin();
    }
    coords[0] = charToInt(input);


    printf("Ingrese la coordenada Y [0-9]:\n");
    //input = getc(stdin);
    //clearstdin();
    while((input = get_input()) < 0);
    while(!(input >= '0' && input <= '9'))
    {
        printf("Error: %c no es una coordenada valida.",input);
        printf("Ingrese la coordenada Y [0-9]:\n");
        while((input = get_input()) < 0);
        //input = getc(stdin);
        //clearstdin();
    }
    coords[1] = charToInt(input);
    return EXIT_SUCCESS;
}

void set_bg_color(color bgColor)
{
    printf("%s",bg_color_string[bgColor]);
}

void set_fg_color(color fgColor)
{
    printf("%s",fg_color_string[fgColor]);
}

void color_reset()
{
    printf(ANSI_COLOR_RESET);
}

char get_input()
{
    char c ;
    c = getc(stdin);
    if(isprint(c))
    {
        clearstdin(0);
    }
    else
    {
        if(c != '\n')
        {
            clearstdin();
        }
        return -1;
    }
    return c;
}
