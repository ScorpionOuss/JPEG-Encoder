#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <qtables.h>
#define pi 3.1415927

void parcoursCroissant(float tab[8][8], float *vecteur, int x, int y, int *adresse)
{
    if (y==0)
    {
        int absc=x, ordo=0;
        while (absc >= 0)
        {
            vecteur[*adresse] = tab[absc][ordo];
            absc -=1;
            ordo += 1;
            //printf("abscisse %d, ordonnée %d\n", absc, ordo);
            *adresse += 1;
        }
    }
    else if (y == 7)
    {
        int absc=7, ordo=x;
        while (absc >= x)
        {
        vecteur[*adresse] = tab[absc][ordo];
        absc -=1;
        ordo += 1;
        *adresse += 1;
        }
    }
}

void parcoursDecroissant(float tab[8][8], float *vecteur, int x, int y, int *adresse)
{
    if (y==0)
    {
        int absc=0, ordo=x;
        while (ordo >= 0)
        {
            vecteur[*adresse] = tab[absc][ordo];
            ordo -= 1;
            absc += 1;
            *adresse += 1;
        }
    }
    else if (y == 7)
    {
        int absc=x, ordo=7;
        while (ordo >= x)
        {
            vecteur[*adresse] = tab[absc][ordo];
            ordo -= 1;
            absc += 1;
            *adresse += 1;
        }
    }
}





float *zigZag(float tab[8][8])
{
    int x = 0;
    int y = 0;
    int i;
    int j;
    int *indiceCour = malloc(sizeof(int));
    float *vecteurRes = malloc(64*sizeof(float));
    *indiceCour = 1;
    vecteurRes[0] = tab[0][0];

    for (i=0; i<3; i++)
    {
        x += 1;
        parcoursDecroissant(tab, vecteurRes, x, 0, indiceCour);
        x +=1;
        parcoursCroissant(tab, vecteurRes, x, 0, indiceCour);
    }
    printf("la valeur de l'adresse est:%d", *indiceCour);
    parcoursDecroissant(tab, vecteurRes,7, 0, indiceCour);
    printf("la valeur de l'adresse est:%d", *indiceCour);
    for (j=0; j<3; j++)
    {
        y += 1;
        parcoursCroissant(tab, vecteurRes, y, 7, indiceCour);
        y += 1;
        parcoursDecroissant(tab, vecteurRes, y, 7, indiceCour);
    }
    printf("la valeur de l'adresse est:%d", *indiceCour);

    vecteurRes[63] = tab[7][7];
    return vecteurRes;
}



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
int32_t **quantification(int t[64])
{
    float **nouveau = malloc(8*sizeof(float*));
    int i;
    int j;
    for (j=0; j < 64; j++)
    {
        nouveau[i][j] = floor(t[i]/quantification_table_Y[i]);
    }
    return nouveau;
}

int32_t **operations_dct_quantification_puis_zig_zag(int data[8][8])
{
    return quantification(zigZag(dtc(data)));
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
}
*/
