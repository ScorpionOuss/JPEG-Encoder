#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#define pi 3.1415927


float fonctionC(int indice)
{
    if (indice == 0)
    {
        return 1/sqrt(2);
    }
    return 1;
}

float transformation(int s[8][8], int i, int j)
{
    int x;
    int y;
    float somme=0;
    for (x=0; x<8; x++)
    {
        for (y=0; y<8; y++)
        {
            somme += s[x][y] * cos((2 * x + 1) * i *(pi/2 * 8)) * cos((2 * y + 1) * j * pi/(2 *8));
        }
    }
    return (2/8)*fonctionC(i)*fonctionC(j)*somme;
}


int t[8][8] = {0};

float **dtc(int t[8][8])
{
    float **nouveau = malloc(8*sizeof(float*));
    int i;
    int j;
    for (i=0; i < 8; i++)
    {
        nouveau[i] = malloc(8*sizeof(float));
        for (j=0; j < 8; j++)
        {
            nouveau[i][j] = transformation(t, i, j);
        }
    }
    return nouveau;
}

/*
int main(void)
{
    float **p = dtc(t);
    int i;
    int j;
    for (i=0; i < 8; i++)
    {
        for (j=0; j < 8; j++)
        {
            printf("la valeur est:%f", p[i][j]);
        }
    }
    return EXIT_SUCCESS;
}*/
