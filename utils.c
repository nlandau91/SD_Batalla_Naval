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
