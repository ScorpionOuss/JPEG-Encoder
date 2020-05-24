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
    affichage_zigzag(vecteurRes);

    return vecteurRes;
}



float fonctionC(int32_t indice)
// Retourne de l'image de la fonction qui a tout entier associe 1
// sauf en 0 où elle vaut (racine(2))^(-1)
{
    if (indice == 0)
    {
        return (float) 1/sqrt(2);
    }
    return 1;
}




float transformation(int32_t** s, int i, int j, float** cosinus)
// Réalise le calcul de DCT proprement dit. La double boucle calcule la somme.
// Le résultat total est fourni par return
{
    // Déclaration des variables d'indices de sommes
    int x;
    int y;

    //Calcul de la somme
    float somme=0;
    for (x=0; x<8; x++)
    {
        for (y=0; y<8; y++)
        {
            somme += (*(*(s+x)+y)-128) * cosinus[x][i] * cosinus[y][j];
        }
    }


    //printf("Aux coord i=%i j=%i, %f \n", i, j, (0.25)*fonctionC(i)*fonctionC(j)*somme);

    return (0.25)*fonctionC(i)*fonctionC(j)*somme;
}



int** dtc(int32_t** t, float** cosinus)
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
    affichage_dct(nouveau);
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
    for (size_t i = 0; i < maxi; i++) 
    {
        cosinus[i] = malloc(8*sizeof(float));
    }
    for (size_t i = 0; i < maxi; i++)
    {
        for (size_t x = 0; i < 8; i++)
        {
            cosinus[i][x] = cosf((2 *(float) x + 1) * ((float) i ) *(pi/((float) 16)));
        }
    }

    return cosinus;
}



/*
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

    
    //printf("Aux coord i=%i j=%i, %f \n", i, j, (0.25)*fonctionC(i)*fonctionC(j)*somme);
    
    return (0.25)*fonctionC(i)*fonctionC(j)*somme;
}


int t[8][8] = {0};

int** dtc(int32_t** t)
{
    //printf(" %d \n", *(*(t+1)+2));
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
            int entier = (int) transformation(t, i, j);
            float flottant = transformation(t, i, j);
            if (flottant > 0 && (int) (flottant + 0.0001) != entier){
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
    affichage_dct(nouveau);
    return nouveau;
}
*/



int32_t* quantification(int* t, int cc)
{
    // On alloue de la mémoire pour un tableau de 64 cases d'entier
    int32_t *nouveau = malloc(64*sizeof(int32_t*));

    int32_t j;

    // Dans le cas de la composante Y
    if (cc==0)
    {
        for (j=0; j < 64; j++)
        {
            // On récupère le resultat de la quantification ie valeur précédente/valeur de quantification
            // On réalise une troncature du resultat obtenu
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
    affichage_quantification(nouveau);

    return nouveau;
}





/* FONCTIONS D'AFFICHAGE*/



void affichage_quantification(int* t)
// Affichage des valeurs du bloc après quantification
{
    printf("Affichage quantification \n");
    for (int j=0; j < 64; j++)
    {
        printf("%d ->", *(t+j));
    }
    printf("\n");
}



void affichage_zigzag(float* t)
// Affichage des valeurs du bloc après zig-zag
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



void affichage_dct(int** t)
// Affichage des valeurs du bloc après DCT
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



void affichage_data(int** t)
// Affichage des valeurs du bloc initial
{
    printf(" Affichage données d'entrées de dct: \n");
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



int32_t* operations_dct_quantification_puis_zig_zag(int32_t** data, int cc, float** cosinus) //NOM DE LA FONCTION A CHANGER
//Fonction qui a partir d'un bloc 8x8 réalise successivement les opérations :
//dct puis zig-zag puis quantification
{
    // Affichage des données d'entrée au terme de la mise sous forme de MCU
    // ie traitement par bloc 8x8 à partir d'ici
    affichage_data(data);

    return quantification(zigZag(dtc(data, cosinus)), cc);
}
