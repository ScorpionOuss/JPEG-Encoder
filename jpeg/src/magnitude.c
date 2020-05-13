#include <stdlib.h>
#include <stdio.h>

int32_t puissance(int32_t a, int32_t b)
// renvoie a**b si b >= 0
{
    if (b == 0){
        return 1;
    }
    else {
        return a*puissance(a, b-1);
    }
}


int32_t calc_magnitude(int32_t entier)
// renvoie la magnitude d'un entier
{
    int32_t p = 0;
    int32_t n = 1;
    while(n <= abs(entier)){
        p += 1;
        n*= 2;
    }

    return p;
}

int32_t num_magnitude(int32_t entier, int32_t magnitude)
// renvoie l'indice d'un entier dans sa magnitude
{
    if (entier >= 0){
        return entier;
    }
    else {
        return puissance(2, magnitude) - abs(entier) -1;
    }
}


int main(void)
{
    int32_t a;
    a = calc_magnitude(-1);
    printf("%i, %i\n", -1, a);
    printf("%i \n", num_magnitude(-1, a));

    a = calc_magnitude(-8);
    printf("%i, %i \n", -8, a);
    printf("%i \n", num_magnitude(-8, a));

 return EXIT_SUCCESS;
}

