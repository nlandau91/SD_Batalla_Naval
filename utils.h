#ifndef UTILS_H_
#define UTILS_H

/**
    Limpia stdin de los caracteres que pueden
    quedar restantes luego de un funcion como scanf
*/
int clearstdin();

/**
    Convierte un char a un int
    @param c el char que sera convertido, debe estar entre '0' y '9'
    @return el entero correspondiente
*/
int charToInt(char c);

/**
    Convierte un int a un char
    @param n el int que sera convertido, debe estar entre 0 y 9
    @return el char correspondiente
*/
char intToChar(int n);

#endif
