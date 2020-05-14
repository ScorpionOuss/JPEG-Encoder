#include <stdlib.h>
#include <stdio.h>
#include <assert.h>


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

float tab[8][8] = {{0,1,5,6,14,15,27,28},{2,4,7,13,16,26,29,42},{3,8,12,17,25,30,41,43},{9,11,18,24,31,40,44,54},{10,19,23,32,39,45,53,55},{20,22,33,38,46,52,56,61},{21,34,37,47,51,57,60,62},{35,36,49,50,58,59,63,64}};
void main(void)
{
    int i;
    float *vecteurRes = malloc(64*sizeof(float));
    vecteurRes = zigZag(tab);
    for (i=0; i<64; i++)
    {
        printf("valeur %f: \n", vecteurRes[i]);
    }
}

