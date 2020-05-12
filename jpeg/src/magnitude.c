#include <stdlib.h>
#include <stdio.h>

int32_t calc_magnitude(int32_t entier)
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
    printf("%i \n", calc_magnitude(0));
    printf("%i \n", calc_magnitude(1));
    printf("%i \n", calc_magnitude(-2));
    printf("%i \n", calc_magnitude(3));
    printf("%i \n", calc_magnitude(-4));
    printf("%i \n", calc_magnitude(5));
    printf("%i \n", calc_magnitude(-6));
    printf("%i \n", calc_magnitude(7));
    printf("%i \n", calc_magnitude(-8));
    printf("%i \n", calc_magnitude(9));
    printf("%i \n", calc_magnitude(-10));
    printf("%i \n", calc_magnitude(16));

 return EXIT_SUCCESS;
}

