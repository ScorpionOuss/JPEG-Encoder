#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <qtables.h>

#define pi 3.1415927

void parcoursCroissant(int** tab, int *vecteur, int x, int y, int *adresse)
{
    if (y==0)
    {
        int absc=x, ordo=0;
        while (absc >= 0)
        {
            vecteur[*adresse] = *(*(tab+absc)+ordo);
            absc -=1;
            ordo += 1;
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




void parcoursDecroissant(int** tab, int *vecteur, int x, int y, int *adresse)
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



int *zigZag(int** tab)
{
    int x = 0;
    int y = 0;
    int i;
    int j;
    int *indiceCour = malloc(sizeof(int));
    int *vecteurRes = malloc(64*sizeof(int));
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

    // Test affichage
    for (int i=0; i<8; i++)
    {
        free(tab[i]);
    }
    free(tab);
    free(indiceCour);
    return vecteurRes;
}



float fonctionC(int32_t indice)
// Retourne de l'image de la fonction qui a tout entier associe 1
// sauf en 0 o?? elle vaut (racine(2))^(-1)
{
    if (indice == 0)
    {
        return (float) 1/sqrt(2);
    }
    return 1;
}



float transformation(uint8_t** s, int i, int j, float** cosinus)
// R??alise le calcul de DCT proprement dit. La double boucle calcule la somme.
// Le r??sultat total est fourni par return
{
    // D??claration des variables d'indices de sommes
    int x;
    int y;

    //Calcul de la somme
    float somme=0;
    for (x=0; x<8; x++)
    {
        for (y=0; y<8; y++)
        {
            somme += ((uint8_t) *(*(s+x)+y)-128) *((float)cosinus[x][i]) *((float) cosinus[y][j]);
        }
    }


    //printf("Aux coord i=%i j=%i, %f \n", i, j, (0.25)*fonctionC(i)*fonctionC(j)*somme);

    return (0.25)*fonctionC(i)*fonctionC(j)*somme;
}

int** dtc(uint8_t** t, float** cosinus)
{
    //printf(" %d \n", *(*(t+1)+2));
    int** nouveau = malloc(8*sizeof(int*));
    for (size_t i = 0; i < 8; i++) 
    {
      nouveau[i] = malloc(8*sizeof(int));
    }

    int i;
    int j;

    for (i=0; i < 8; i++)
    {
        for (j=0; j < 8; j++)
        {
            int entier = (int) transformation(t, i, j, cosinus);
            float flottant = transformation(t, i, j, cosinus);
        
            if (flottant > 0 && (int) (flottant + 0.0001) != entier)
            {
                nouveau[i][j] = entier + 1;
                printf("ICI C EST SPECIAL VPLUS !!!!!, %f, %f, %i", flottant, flottant + 0.0001, entier);
                printf("Nouvelle valeur %i", nouveau[i][j]);
            }
            else
            {
                nouveau[i][j] = entier;
            }
        }
    }
    //affichage_dct(nouveau);
    for (int i=0; i<8; i++)
    {
    free(t[i]);
    }
    free(t);   
    return nouveau;
}


float** precalculcos(int32_t largeur, int32_t longueur){
// On factorise le calcul des cosinus
    int32_t maxi = 0;
    if (longueur > largeur)
    {
        maxi = longueur;
    }
    else
    {
        maxi = largeur;
    }

    float **cosinus = malloc(maxi*sizeof(float*));
    for (int i = 0; i < maxi; i++) 
    {
        cosinus[i] = malloc(8*sizeof(float));
    }
    for (int i = 0; i < maxi; i++)
    {
        for (int x = 0; i < 8; i++)
        {
            cosinus[i][x] = cos((2 *(float) x + 1) * ((float) i ) *(pi/((float) 16)));
        }
    }

    return cosinus;
}



float transformation_naive(uint8_t** s, int i, int j)
{
    int x;
    int y;
    float somme=0;
    for (x=0; x<8; x++)
    {
        for (y=0; y<8; y++)
        {
            somme += ((uint8_t) *(*(s+x)+y)-128) * cosf((2 *(float) x + 1) * ((float)i ) *(pi/((float) 16))) * cosf((2 *(float) y + 1) * ((float)j ) *(pi/((float) 16)));
        }
    }

    
    
    return (0.25)*fonctionC(i)*fonctionC(j)*somme;
}


//int t[8][8] = {0};
int** dtc_naif(uint8_t** t)
{
    int** nouveau = malloc(8*sizeof(int*));
    for (size_t i = 0; i < 8; i++) {
      nouveau[i] = malloc(8*sizeof(int));
    }
    int i;
    int j;
    for (i=0; i < 8; i++)
    {
        for (j=0; j < 8; j++)
        {
            int entier = (int) transformation_naive(t, i, j);
            nouveau[i][j] = entier;
        }
    }
    //affichage_dct(nouveau);
    for (int i=0; i<8; i++)
    {
    free(t[i]);
    }
    free(t);   

    return nouveau;
}


int32_t* quantification(int* t, int cc)
{
    // On alloue de la m??moire pour un tableau de 64 cases d'entier
    int32_t *nouveau = malloc(64*sizeof(int32_t*));

    int32_t j;

    // Dans le cas de la composante Y
    if (cc==0)
    {
        for (j=0; j < 64; j++)
        {
            // On r??cup??re le resultat de la quantification ie valeur pr??c??dente/valeur de quantification
            // On r??alise une troncature du resultat obtenu
            nouveau[j] = (int16_t) ((*(t+j))/(quantification_table_Y[j]));
        }
    }
    // Dans le cas des composantes Cb et Cr
    else
    {
        for (j=0; j < 64; j++)
        {
            nouveau[j] = (int16_t) ((*(t+j))/(quantification_table_CbCr[j]));
        }
    }

    //Affichage pour test
    //affichage_quantification(nouveau);
    free(t);
    return nouveau;
}





/* FONCTIONS D'AFFICHAGE*/



void affichage_quantification(int* t)
// Affichage des valeurs du bloc apr??s quantification
{
    printf("Affichage quantification \n");
    for (int j=0; j < 64; j++)
    {
        printf("%d ->", *(t+j));
    }
    printf("\n");
}


/*
void affichage_zigzag(float* t)
// Affichage des valeurs du bloc apr??s zig-zag
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

*/

void affichage_dct(int** t)
// Affichage des valeurs du bloc apr??s DCT
{
    printf(" Affichage DCT: \n");
    for (int j=0; j < 8; j++)
    {
        for (int i=0; i < 8; i++)
        {
            printf("%d ->", (int) *(*(t+j)+i));
        }

        printf("\n");
    }

    printf("\n");
}



void affichage_data(uint8_t** t)
// Affichage des valeurs du bloc initial
{
    printf(" Affichage donn??es d'entr??es de dct: \n");
    for (int j=0; j < 8; j++)
    {
        for (int i=0; i < 8; i++)
        {
            printf("%d ->", *(*(t+j)+i));
        }

        printf("\n");
    }

    printf("\n");
}





/*FONCTION PRINCIPALE : GESTION ORDRE OPERATOIRE */


int32_t* operations_naives(uint8_t** data, int cc) //NOM DE LA FONCTION A CHANGER
{

    return quantification(zigZag(dtc_naif(data)), cc);
}
int32_t* operations_dct_zig_zag_quantification(uint8_t** data, int cc, float** cosinus) //NOM DE LA FONCTION A CHANGER
//Fonction qui a partir d'un bloc 8x8 r??alise successivement les op??rations :
//dct puis zig-zag puis quantification
{
    // Affichage des donn??es d'entr??e au terme de la mise sous forme de MCU
    // ie traitement par bloc 8x8 ?? partir d'ici

    return quantification(zigZag(dtc(data, cosinus)), cc);
}
