#include <stdlib.h>
#include <stdio.h>

int32_t calc_magnetude(int32_t entier)
// renvoie la magnetude d'un entier
{
    int32_t p = 0;
    int32_t n = 1;
    while(n <= abs(entier)){
        p += 1;
        n*= 2;
    }

    return p;
}

int main(void)
{
    printf("%i \n", calc_magnetude(0));
    printf("%i \n", calc_magnetude(1));
    printf("%i \n", calc_magnetude(-2));
    printf("%i \n", calc_magnetude(3));
    printf("%i \n", calc_magnetude(-4));
    printf("%i \n", calc_magnetude(5));
    printf("%i \n", calc_magnetude(-6));
    printf("%i \n", calc_magnetude(7));
    printf("%i \n", calc_magnetude(-8));
    printf("%i \n", calc_magnetude(9));
    printf("%i \n", calc_magnetude(-10));
    printf("%i \n", calc_magnetude(16));

 return EXIT_SUCCESS;
}

