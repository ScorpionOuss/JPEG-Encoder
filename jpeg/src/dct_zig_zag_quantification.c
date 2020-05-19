#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <qtables.h>

#define pi 3.1415927

void parcoursCroissant(float** tab, float *vecteur, int x, int y, int *adresse)
{
    if (y==0)
    {
        int absc=x, ordo=0;
        while (absc >= 0)
        {
            vecteur[*adresse] = *(*(tab+absc)+ordo);
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
        vecteur[*adresse] = *(*(tab+absc)+ordo);
        absc -=1;
        ordo += 1;
        *adresse += 1;
        }
    }
}

void parcoursDecroissant(float** tab, float *vecteur, int x, int y, int *adresse)
{
    if (y==0)
    {
        int absc=0, ordo=x;
        while (ordo >= 0)
        {
        vecteur[*adresse] = *(*(tab+absc)+ordo);
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
        vecteur[*adresse] = *(*(tab+absc)+ordo);
            ordo -= 1;
            absc += 1;
            *adresse += 1;
        }
    }
}



float *zigZag(float** tab)
{
    int x = 0;
    int y = 0;
    int i;
    int j;
    int *indiceCour = malloc(sizeof(int));
    float *vecteurRes = malloc(64*sizeof(float));
    *indiceCour = 1;
    vecteurRes[0] = *(*(tab));
    for (i=0; i<3; i++)
    {
        x += 1;
        parcoursDecroissant(tab, vecteurRes, x, 0, indiceCour);
        x +=1;
        parcoursCroissant(tab, vecteurRes, x, 0, indiceCour);
    }
    parcoursDecroissant(tab, vecteurRes,7, 0, indiceCour);
    for (j=0; j<3; j++)
    {
        y += 1;
        parcoursCroissant(tab, vecteurRes, y, 7, indiceCour);
        y += 1;
        parcoursDecroissant(tab, vecteurRes, y, 7, indiceCour);
    }
    vecteurRes[63] = *(*(tab+7)+7);
    affichage(vecteurRes);
    return vecteurRes;
}



float fonctionC(int32_t indice)
{
    if (indice == 0)
    {
        return (float) 1/sqrt(2);
    }
    return 1;
}

float transformation(int32_t** s, int i, int j)
{
    int x;
    int y;
    float somme=0;
    for (x=0; x<8; x++)
    {
        for (y=0; y<8; y++)
        {
            somme += (*(*(s+x)+y)-128) * cosf((2 *(float) x + 1) * ((float)i ) *(pi/((float) 16))) * cosf((2 *(float) y + 1) * ((float)j ) *(pi/((float) 16)));
        }
    }
    return (0.25)*fonctionC(i)*fonctionC(j)*somme;
}


int t[8][8] = {0};

int32_t** dtc(int32_t** t)
{
    printf(" %d \n", *(*(t+1)+2));
    float** nouveau = malloc(8*sizeof(int16_t*));
    for (size_t i = 0; i < 8; i++) {
      nouveau[i] = malloc(8*sizeof(int16_t));
    }
    int i;
    int j;
    for (i=0; i < 8; i++)
    {
        for (j=0; j < 8; j++)
        {
            nouveau[i][j] = (int16_t) (transformation(t, i, j));
        }
    }
    affichage_dct(nouveau);
    return nouveau;
}

int32_t* quantification(float* t)
{
    int32_t *nouveau = malloc(64*sizeof(int32_t*));
    int32_t j;
    for (j=0; j < 64; j++)
    {
        nouveau[j] = (int16_t) ((int16_t) *(t+j))/(quantification_table_Y[j]);
    }
    affichage_quantification(nouveau);
    return nouveau;
}
void affichage(float* t)
{
    printf("affichage zigZag: \n");
    for (int j=0; j < 64; j++)
    {
        printf("%f ->", *(t+j));
    }
    printf("\n");
    printf("\n");
    printf("\n");
}
void affichage_quantification(int* t)
{
    printf("Affichage quantification \n");
    for (int j=0; j < 64; j++)
    {
        printf("%d ->", *(t+j));
    }
    printf("\n");
}
void affichage_data(int** t)
{
    printf(" Affichage données d'entrées de dct: \n");
    for (int j=0; j < 8; j++)
    {
    for (int i=0; i < 8; i++)
    {
       printf("%d ->", *(*(t+i)+j));
    }
    printf("\n");
    }
    printf("\n");
}
void affichage_dct(float** t)
{
    printf(" Affichage DCT: \n");
    for (int j=0; j < 8; j++)
    {
    for (int i=0; i < 8; i++)
    {
       printf("%f ->", (float) *(*(t+i)+j));
    }
    printf("\n");
    }
    printf("\n");
}
int32_t* operations_dct_quantification_puis_zig_zag(int32_t** data)
{
    affichage_data(data);
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
