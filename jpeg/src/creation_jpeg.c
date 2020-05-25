#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "htables.h"
#include "qtables.h"
#include "huffman.h"
#include "ligne_de_cmd.h"
#define pi 3.1415927
/* partie recupération des datas */
// structure entete
struct Entete {
    int32_t nbre_octet;  // le nombre d'octets dans un entête
    int32_t type;        // ppm ou pgm (si type = 5 donc pgm sinon si type = 6 donc ppm)
    int32_t longueur;    // longuer de l'image
    int32_t largeur;     // largeur de l'imagei
    int32_t range_couleur;
};


int32_t ***bonne_taille_couleur_ssechantillonnage(int32_t ***data, int32_t* entete, int8_t echantillonage)
//Transforme l'image de sorte que ses dimensions soient un multiple de 8
{
    int8_t na = 8;
    int8_t no = 8;
    
    // On met les bons parametres de dimensions des MCU, 8 ou 16
    
    if (echantillonage == 1){
        na = 16;
    }
    else if (echantillonage == 2){
        na = 16;
        no = 16;
    }

    // On calcule les bonnes dimensions
    int bonne_abscisse = *entete;
    if (*entete % na != 0)
    {
    bonne_abscisse += na - *entete % na;
    }
    
    int bonne_ordonnee = *(entete+1);
    if (*(entete+1) % no != 0)
    {
    bonne_ordonnee += (no - *(entete+1) % no);
    }
        
    // On créé un nouveau tableau data aux bonnes dimensions
    int32_t*** data_new = malloc(bonne_ordonnee*sizeof(int32_t*));
    for (size_t i = 0; i < bonne_ordonnee; i++) {
      
      data_new[i] = malloc(bonne_abscisse*sizeof(int32_t*));
      for (size_t j = 0; j < bonne_abscisse; j++)
      {
        data_new[i][j] = malloc(3*sizeof(int32_t));
      }
    }
    
    // On le remplit avec les anciennes valeurs
    for (size_t y = 0; y < *(entete+1); y++) {
      for (size_t x = 0; x < *(entete); x++) {
          for (size_t z = 0; z<3; z++)
          {
          data_new[y][x][z] = data[y][x][z];
          }
      }
    }
        
    // On met les nouvelles valeurs
    // En bas d'abord
     for (size_t y = *(entete+1); y < bonne_ordonnee; y++) {
        for (size_t x = 0; x < *entete; x++) {
          for (size_t z = 0; z<3; z++)
          {
            data_new[y][x][z] = data[*(entete+1)-1][x][z];
          }
        }
    }
    // Puis à droite
     for (size_t y = 0; y < bonne_ordonnee; y++) {
        for (size_t x = *entete; x < bonne_abscisse; x++) {
          for (size_t z = 0; z<3; z++)
          {
            data_new[y][x][z] = data[*(entete+1)-1][*entete -1][z];
          }
        }
    }

    
    return data_new;

}


int max(int t[2])
{
    if (t[0]>t[1])
    {
    return t[0];
    }
    else
    {
    return t[1];
    }
}
int32_t ***Y_Cb_Cr(int32_t*** data, int32_t longueur, int32_t largeur)
{
  int32_t*** data_nouv = malloc(longueur*sizeof(int32_t**)); 
  for (size_t i = 0; i < longueur; i++) {
    data_nouv[i] = malloc(largeur*sizeof(int32_t*));
    for (size_t j = 0; j < largeur; j++) {
      data_nouv[i][j] = malloc(3*sizeof(int32_t));
    }
  }

  // On définit trois pointeurs pour les calculs, un sur chaque composante RGB
  
  uint8_t R;
  uint8_t G;
  uint8_t B;

  for (size_t lon = 0; lon < longueur; lon++) {
    for (size_t lar = 0; lar < largeur; lar++) {
        R = data[lon][lar][0];
        G = data[lon][lar][1];
        B = data[lon][lar][2];
        data_nouv[lon][lar][0] = (int) round(0.299 * (float) R + 0.587 * (float) G + 0.114 * (float) B);
        data_nouv[lon][lar][1] = (int) round(-0.1687 * (float) R - 0.3313 * (float) G + 0.5 * (float) B + 128);
        data_nouv[lon][lar][2] = (int) round(0.5 * (float) R - 0.4187* (float) G - 0.0813 * (float) B + 128);
        printf("R %d G %d B %d deviennent", R, G, B);

    }
  }

  return data_nouv;

} 

// récupération des pixel des fichiers pgm dans une matrice de taille "longueurXlargeur"
int32_t **recuper_data_gris(const char* file_pgm,uint32_t largeur,uint32_t longueur, int32_t taille)
{
  FILE* fichier = fopen(file_pgm, "r+");
  int32_t** data = malloc(longueur*sizeof(int32_t*));
  for (size_t i = 0; i < longueur; i++) {
    data[i] = malloc(largeur*sizeof(int32_t));
  }
  fseek(fichier, taille, SEEK_SET);
  for (size_t y = 0; y < longueur; y++) {
    for (size_t x = 0; x < largeur; x++) {
      data[y][x] = fgetc(fichier);
    }
  }
  fclose(fichier);
  return data;
}

// récupération d'un tableau de trois matrices des couleurs de taille "longueurXlargeur"
int32_t ***recuper_data_couleur(const char* file_ppm,uint32_t largeur,uint32_t longueur, int32_t taille)
{
  FILE* fichier = fopen(file_ppm, "r+");
  int32_t*** data; // 3 pointeurs vers les trois matrices des différentes couleurs
    data = malloc(longueur*sizeof(int32_t*));
    for (size_t j = 0; j < longueur; j++) {
      data[j] = malloc(largeur*sizeof(int32_t*));
      for (size_t i = 0; i < largeur; i++) {
        data[j][i] = malloc(3*sizeof(int32_t));
        }
    }
  



  fseek(fichier, taille, SEEK_SET);// on enleve l'entete 11 octets
  // on parcours les données
  for (size_t y = 0; y < longueur; y++) {
    for (size_t x = 0; x < largeur; x++) {
      for (size_t i = 0; i < 3; i++) {
        data[y][x][i] = fgetc(fichier);
      }
    }
  }
  fclose(fichier);
  return data;
}

// nombre de chiffre dans un nombre pour savoir le nombre d'octets dans l'entête
int nombre_chiffre(int32_t nombre) {
    int s = 0, i = 1;
    if (nombre != 0) {
        s = (int)nombre / 10;
        while (s != 0) {
            s = (int)(s / 10);
            i = i + 1;
        }
    }
    return i;
}

// récupération de l'entête des deux fichiers pgm ou ppm
struct Entete* recupe_entete(const char* file_pgm) {
    FILE* fichier = fopen(file_pgm, "r+");
    struct Entete* entete = malloc(sizeof(struct Entete*));
    int32_t longueur = 0;
    int32_t largeur = 0;
    int32_t range_couleur = 0;
    fseek(fichier, 1, SEEK_SET);
    entete->type = fgetc(fichier);
    fseek(fichier, 3, SEEK_SET);
    largeur = fgetc(fichier) - 48;
    while (fgetc(fichier) != 32) {
        fseek(fichier, -1, SEEK_CUR);
        largeur = 10 * largeur + fgetc(fichier) - 48;
    }
    entete->largeur = largeur;
    longueur = fgetc(fichier) - 48;
    while (fgetc(fichier) != 10) {
        fseek(fichier, -1, SEEK_CUR);
        longueur = 10 * longueur + fgetc(fichier) - 48;
    }
    entete->longueur = longueur;

    range_couleur = fgetc(fichier)-48;
    while (fgetc(fichier) != 10) {
        fseek(fichier, -1, SEEK_CUR);
        range_couleur = 10 * range_couleur + fgetc(fichier) - 48;
    }
    entete->range_couleur = range_couleur;
    fclose(fichier);
    entete->nbre_octet = 6+nombre_chiffre(range_couleur) + nombre_chiffre(longueur) + nombre_chiffre(largeur);
    return entete;
}
int32_t** recuper_data(const char* file_pgm, int32_t* entete)
{
    FILE* fichier = fopen(file_pgm, "r+");
    int32_t** data = malloc(*(entete+1)*sizeof(int32_t*));
    for (size_t i = 0; i < *(entete+1); i++) {
      data[i] = malloc(*(entete)*sizeof(int32_t));
    }
    fseek(fichier, *(entete+3), SEEK_SET);
    for (size_t y = 0; y < *(entete+1); y++) {
      for (size_t x = 0; x < *(entete); x++) {
        data[y][x] = fgetc(fichier);
      }
}
fclose(fichier);
return data;
}
int32_t **bonne_taille(int32_t **data, int32_t* entete)
//Transforme l'image de sorte que ses dimensions soient un multiple de 8
{
    // On calcule les bonnes dimensions
    int bonne_abscisse = *entete;
    if (*entete % 8 != 0)
    {
    bonne_abscisse += 8 - *entete % 8;
    }
    
    int bonne_ordonnee = *(entete+1);
    if (*(entete+1) % 8 != 0)
    {
    bonne_ordonnee += (8 - *(entete+1) % 8);
    }
        
    // On créé un nouveau tableau data aux bonnes dimensions
    int32_t** data_new = malloc(bonne_ordonnee*sizeof(int32_t*));
    for (size_t i = 0; i < bonne_ordonnee; i++) {
      
      data_new[i] = malloc(bonne_abscisse*sizeof(int32_t));
    }
    printf("hello avant premieère boucle \n");    
    // On le remplit avec les anciennes valeurs
    for (size_t y = 0; y < *(entete+1); y++) {
      for (size_t x = 0; x < *(entete); x++) {
        data_new[y][x] = data[y][x];
      }
    }
        
    printf("hello2 \n"); 
    // On met les nouvelles valeurs
    // En bas d'abord
     for (size_t y = *(entete+1); y < bonne_ordonnee; y++) {
        for (size_t x = 0; x < *entete; x++) {
            data_new[y][x] = data[*(entete+1)-1][x];
        }
    }
    // Puis à droite
    printf("hello2 \n"); 
     for (size_t y = 0; y < bonne_ordonnee; y++) {
        for (size_t x = *entete; x < bonne_abscisse; x++) {
            data_new[y][x] = data[*(entete+1)-1][*entete -1];
        }
    }

    
    return data_new;

}
int32_t ***bonne_taille_couleur(int32_t ***data, int32_t* entete)
//Transforme l'image de sorte que ses dimensions soient un multiple de 8
{
    // On calcule les bonnes dimensions
    int bonne_abscisse = *entete;
    if (*entete % 8 != 0)
    {
    bonne_abscisse += 8 - *entete % 8;
    }
    
    int bonne_ordonnee = *(entete+1);
    if (*(entete+1) % 8 != 0)
    {
    bonne_ordonnee += (8 - *(entete+1) % 8);
    }
        
    printf("hello avant premieère boucle \n");    
    // On créé un nouveau tableau data aux bonnes dimensions
    int32_t*** data_new = malloc(bonne_ordonnee*sizeof(int32_t*));
    for (size_t i = 0; i < bonne_ordonnee; i++) {
      
      data_new[i] = malloc(bonne_abscisse*sizeof(int32_t*));
      for (size_t j = 0; j < bonne_abscisse; j++)
      {
        data_new[i][j] = malloc(3*sizeof(int32_t));
      }
    }
    
    printf("hello apres premieère boucle \n");    
    // On le remplit avec les anciennes valeurs
    for (size_t y = 0; y < *(entete+1); y++) {
      for (size_t x = 0; x < *(entete); x++) {
          for (size_t z = 0; z<3; z++)
          {
          data_new[y][x][z] = data[y][x][z];
          }
      }
    }
        
    printf("hello avant premieère boucle \n");    
    printf("hello2 \n"); 
    // On met les nouvelles valeurs
    // En bas d'abord
     for (size_t y = *(entete+1); y < bonne_ordonnee; y++) {
        for (size_t x = 0; x < *entete; x++) {
          for (size_t z = 0; z<3; z++)
          {
            data_new[y][x][z] = data[*(entete+1)-1][x][z];
          }
        }
    }
    // Puis à droite
    printf("hello2 \n"); 
     for (size_t y = 0; y < bonne_ordonnee; y++) {
        for (size_t x = *entete; x < bonne_abscisse; x++) {
          for (size_t z = 0; z<3; z++)
          {
            data_new[y][x][z] = data[*(entete+1)-1][*entete -1][z];
          }
        }
    }

    
    return data_new;

}


int32_t *recuper_entete(const char* file_pgm)
{
    FILE* fichier = fopen(file_pgm, "r+");
    int taille = 0;
    int32_t *parametre = malloc(4*sizeof(int32_t));
    int compteur = 0;
    int nbr=0;
    int current = 0;
    int str[4];
    str[0] = fgetc(fichier);
    str[1] = fgetc(fichier);
    str[2] = fgetc(fichier);
    taille++;
    taille++;
    taille++;
    printf("P5 OK");
    current = fgetc(fichier);
    taille++;
    while (!(current == 32))
    {
        nbr = nbr*10+current-3*16;
        current = fgetc(fichier);
        taille++;
    }
    *(parametre+compteur) = nbr;
    nbr = 0;
    compteur++;
    current = fgetc(fichier);
    taille++;
    while (!(current == 10))
    {
        nbr = nbr*10+current-3*16;
        current = fgetc(fichier);
        taille++;
    }
    *(parametre+compteur) = nbr;
    nbr = 0;
    compteur++;
    current = fgetc(fichier);
    taille++;
    while (!(current == 10))
    {
        nbr = nbr*10+current-3*16;
        current = fgetc(fichier);
        taille++;
    }
    nbr = 0;
    compteur++;
    fclose(fichier);
    compteur++;
    *(parametre+3) = taille;
    return parametre;
    /*
    if (*(vect2)+*(vect2+1) == "5035")
    {
        current = (vect2+4)
        while !(*current = "20")
        {
          nbr = 10*nbr+
        }
    }
    */
}


struct jpeg *creation_jpeg_couleur(int32_t *parametres, int echantillonage)
//fonction qui cree un fichier jpeg : parametres est le tableau qui contient les infos de recup_data
{
    struct jpeg *image = jpeg_create();
    //Premiere ligne mystere
    jpeg_set_ppm_filename(image, "chabanas.ppm");
    jpeg_set_jpeg_filename(image, "resultat.jpg");
    jpeg_set_image_width(image, *(parametres));
    jpeg_set_image_height(image, *(parametres+1));
    jpeg_set_nb_components(image, 3);

    for (int cc=0; cc<3; cc++)
    {
    for (int acdc=0; acdc<2;acdc++)
    {
    /*
     facteur d'echantillonage: 4:4:4
    jpeg_set_sampling_factor(image,cc, enum direction dir, sampling_factor);
    */
    struct huff_table *htable;

    htable = huffman_table_build(htables_nb_symb_per_lengths[acdc][cc], htables_symbols[acdc][cc], htables_nb_symbols[acdc][cc]);
    //htable = jpeg_get_huffman_table(image, acdc, cc);
    jpeg_set_huffman_table(image, acdc, cc, htable);
    }
    }
    int cc = 0;
    if (echantillonage == 2)
    {
    jpeg_set_sampling_factor(image,cc, 0, 2);
    jpeg_set_sampling_factor(image,cc, 1, 2);
    }
    if (echantillonage==1)
    {
    jpeg_set_sampling_factor(image,cc, 0, 2);
    jpeg_set_sampling_factor(image,cc, 1, 1);
    }
    if (echantillonage ==0)
    {
    jpeg_set_sampling_factor(image,cc, 0, 1);
    jpeg_set_sampling_factor(image,cc, 1, 1);
    }
    cc=1;
    jpeg_set_sampling_factor(image,cc, 0, 1);
    jpeg_set_sampling_factor(image,cc, 1, 1);
    cc=2;
    jpeg_set_sampling_factor(image,cc, 0, 1);
    jpeg_set_sampling_factor(image,cc, 1, 1);

    uint8_t *qtables;
    qtables = &quantification_table_Y;
    jpeg_set_quantization_table(image, 0, qtables);
    qtables = &quantification_table_CbCr;
    jpeg_set_quantization_table(image, 1, qtables);
    jpeg_set_quantization_table(image, 2, qtables);
    
    return image;
}

struct jpeg *creation_jpeg(int32_t *parametres)
//fonction qui cree un fichier jpeg : parametres est le tableau qui contient les infos de recup_data
{
    struct jpeg *image = jpeg_create();
    //Premiere ligne mystere
    jpeg_set_ppm_filename(image, "chabanas.ppm");
    jpeg_set_jpeg_filename(image, "resultat.jpg");
    jpeg_set_image_width(image, *(parametres));
    jpeg_set_image_height(image, *(parametres+1));
    jpeg_set_nb_components(image, 1);

    int cc = 0;
    for (int acdc=0; acdc<2;acdc++)
    {
    /*
     facteur d'echantillonage: 4:4:4
    jpeg_set_sampling_factor(image,cc, enum direction dir, sampling_factor);
    */
    struct huff_table *htable;

    htable = huffman_table_build(htables_nb_symb_per_lengths[acdc][cc], htables_symbols[acdc][cc], htables_nb_symbols[acdc][cc]);
    //htable = jpeg_get_huffman_table(image, acdc, cc);
    jpeg_set_huffman_table(image, acdc, cc, htable);
    }
    cc=0;
    jpeg_set_sampling_factor(image,cc, 0, 1);
    jpeg_set_sampling_factor(image,cc, 1, 1);  
    cc=1;
    jpeg_set_sampling_factor(image,cc, 0, 1);
    jpeg_set_sampling_factor(image,cc, 1, 1);
    cc=2;
    jpeg_set_sampling_factor(image,cc, 0, 1);
    jpeg_set_sampling_factor(image,cc, 1, 1);    
    uint8_t *qtables;
    qtables = &quantification_table_Y;
    jpeg_set_quantization_table(image, cc, qtables);
    

    return image;
}
int32_t ***zigzag_bloc(int32_t** ptr_sur_tab, int* entete)
{
    // On calcule les bonnes dimensions
    int largeur = *entete;
    printf("hello \n");
    if (*entete % 8 != 0)
    {
    largeur += 8 - *entete % 8;
    }
    
    int hauteur = *(entete+1);
    if (*(entete+1) % 8 != 0)
    {
    hauteur += (8 - *(entete+1) % 8);
    }
 
    int32_t ****ptr_sur_tab_retour = malloc(((int) hauteur / 8)* sizeof(int32_t***));
    for (size_t i3 = 0; i3 < (int) hauteur/8; i3++) {
      ptr_sur_tab_retour[i3] = malloc(((int) largeur/8)*sizeof(int32_t**));
    } 
    for (int i = 0; i<hauteur;i=i+8)
    {
    for (int j=0; j<largeur; j=j+8)
    {
    
    int32_t** data = malloc(8*sizeof(int32_t*));
    for (size_t i3 = 0; i3 < 8; i3++) {
      data[i3] = malloc(8*sizeof(int32_t));
    }
    for (int i1 = 0; i1<8; i1++)
    {
    for (int j1 = 0; j1<8; j1++)
    {
        data[i1][j1] = ptr_sur_tab[i+i1][j+j1];//*(*(ptr_sur_tab+i+i1)+j+j1);
    }
    }
    ptr_sur_tab_retour[i/8][j/8] = data; 
    }
    }

    printf("hello fini\n");
    int32_t ***ptr_sur_tab_retour_ordonnee = malloc(((int) largeur/8 )*((int) hauteur / 8)* sizeof(int32_t**));
    for (int i=0; i<((int) largeur/8 )*((int) hauteur / 8);i++)
    {
        ptr_sur_tab_retour_ordonnee[i] = ptr_sur_tab_retour[i/(largeur/8)][(i)%(largeur/8)];
    }

    printf("hello fini creation vecteur\n");

    return ptr_sur_tab_retour_ordonnee;
}
int32_t ***zigzag_bloc_couleur(int32_t*** ptr_sur_tab, int* entete, int echantillonage)
{
    if (echantillonage == 0)
    {
    // On calcule les bonnes dimensions
    int largeur = *entete;
    if (*entete % 8 != 0)
    {
    largeur += 8 - *entete % 8;
    }
    
    int hauteur = *(entete+1);
    if (*(entete+1) % 8 != 0)
    {
    hauteur += (8 - *(entete+1) % 8);
    }
 
    uint8_t ****ptr_sur_tab_retour = malloc(((int) hauteur / 8)* sizeof(uint8_t***));
    for (size_t i3 = 0; i3 < (int) hauteur/8; i3++) {
      ptr_sur_tab_retour[i3] = malloc(((int) largeur*3/8)*sizeof(uint8_t**));
    } 
    for (int i = 0; i<hauteur;i=i+8)
    {
    for (int j=0; j<largeur; j=j+8)
    {
    

    
    for (int i2= 0; i2<3; i2++)
    {
     int8_t** data = malloc(8*sizeof(int8_t*));
    for (size_t i3 = 0; i3 < 8; i3++) {
      data[i3] = malloc(8*sizeof(int8_t*));
      } 
    for (int i1 = 0; i1<8; i1++)
    {
    for (int j1 = 0; j1<8; j1++)
    {
        data[i1][j1] = ptr_sur_tab[i+i1][j+j1][i2];//*(*(ptr_sur_tab+i+i1)+j+j1);
    }
    }
    ptr_sur_tab_retour[i/8][3*j/8+i2] = data; 
    }
    }
    }
    uint8_t ***ptr_sur_tab_retour_ordonnee = malloc(((int) largeur/8)*((int) hauteur / 8)*3*sizeof(int32_t**));
    for (int i=0; i<(3*(int) (largeur/8) )*((int) hauteur / 8);i++)
    {
        ptr_sur_tab_retour_ordonnee[i] = ptr_sur_tab_retour[i/(3*largeur/8)][(i)%(3*largeur/8)];
    }
    return ptr_sur_tab_retour_ordonnee;
    }

    if (echantillonage == 1)
    {
    // On calcule les bonnes dimensions avec un echantillonage
 
    int largeur = *entete;
    if (*entete % 16 != 0)
    {
    largeur += 16 - *entete % 16;
    }
    
    int hauteur = *(entete+1);
    if (*(entete+1) % 8 != 0)
    {
    hauteur += (8 - *(entete+1) % 8);
    }
    uint8_t ****ptr_sur_tab_retour = malloc(((int) hauteur / 8)* sizeof(uint8_t***));
    for (size_t i3 = 0; i3 < (int) hauteur/8; i3++) {
      ptr_sur_tab_retour[i3] = malloc(((int) largeur*3/8)*sizeof(uint8_t**));
    } 
    uint8_t R, G, B;
    uint8_t Rs, Gs, Bs;
    int compteur =0;
    for (int i = 0; i<hauteur;i=i+8)
    {
    for (int j=0; j<largeur; j=j+8)
    {
    

    
    for (int i2= 0; i2<3; i2++)
    {
    int32_t** data = malloc(8*sizeof(int32_t*));
    for (size_t i3 = 0; i3 < 8; i3++) {
      data[i3] = malloc(8*sizeof(int32_t));
      } 
    for (int i1 = 0; i1<8; i1++)
    {
    for (int j1 = 0; j1<8; j1++)
    {
        R = ptr_sur_tab[i1+i][j+j1][0];
        G = ptr_sur_tab[i1+i][j+j1][1];
        B = ptr_sur_tab[i1+i][j+j1][2];

        //printf("pour i: %d et j: %d  avec hauteur %d et largeur %d; \n",i, j, hauteur, largeur);
        if ((j+j1+8*3)<largeur-1)
        {
        Rs = ptr_sur_tab[i1+i][j+8*3+j1][0];
        Gs = ptr_sur_tab[i1+i][j+8*3+j1][1];
        Bs = ptr_sur_tab[i1+i][j+j1+8*3][2];
        if ((compteur)%3==0)
        {
        data[i1][j1] = (int) round(((float) G) *0.587+((float)B)*0.299+0.114*((float) R));
        printf("data %d", data[i1][j1]);
        }
        if (compteur%3==1)
        {
        data[i1][j1] = (int) round(0.5*((float) B+(float)Bs)/2+-0.3313*((float)G+(float)Gs)/2-((float) R+(float)Rs)*0.1687/2+128);//*(*(ptr_sur_tab+i+i1)+j+j1);
        }
        if (compteur%3==2)
        {
        data[i1][j1] = (int) round(-0.0813*((float) B+(float)Bs)/2+-0.4187*((float)G+(float)Gs)/2+((float) R+(float)Rs)*0.5/2+128);//*(*(ptr_sur_tab+i+i1)+j+j1);
        }
        }
        else
        {
        if ((compteur)%3==0)
        {
        data[i1][j1] = (int) round(((float) G) *0.587+((float)B)*0.299+0.114*((float) R));
        }
        if ((compteur)%3==1)
        {
        data[i1][j1] = (int) round(0.5*((float) B)/2+-0.3313*((float)G)/2-((float) R)*0.1687/2+128);//*(*(ptr_sur_tab+i+i1)+j+j1);
        }
        if ((compteur)%3==2)
        {
        data[i1][j1] = (int) round(-0.0813*((float) B)/2+-0.4187*((float)G)/2+((float) R)*0.5/2+128);//*(*(ptr_sur_tab+i+i1)+j+j1);
        }
        }
    }
    }
    ptr_sur_tab_retour[i/8][3*j/8+i2] = data;
    compteur++;
    //affichage_data2(data);
    affichage_data2(ptr_sur_tab[i][j]);
    }
    }

    compteur=0;
    }
    printf("suite ZigZag \n");    
    int j = 0;
    int32_t ***ptr_sur_tab_retour_ordonnee = malloc(((int) largeur/16 )*((int) hauteur / 8)*4 *sizeof(uint8_t**));
    for (int i=0; i<4*((int) (largeur/16) )*((int) hauteur / 8);i++)
    {
        if (i%4==0)
        {
        ptr_sur_tab_retour_ordonnee[i] = ptr_sur_tab_retour[j/(3*largeur/8)][(j)%(3*largeur/8)];
        j=j+3;

        }
        if (i%4==1)
        {
        ptr_sur_tab_retour_ordonnee[i] = ptr_sur_tab_retour[j/(3*largeur/8)][(j)%(3*largeur/8)];
        j=j-2;
        }
        if (i%4==2)
        {
        ptr_sur_tab_retour_ordonnee[i] = ptr_sur_tab_retour[j/(3*largeur/8)][(j)%(3*largeur/8)];
        j++;
        }
        if (i%4==3)
        {
        ptr_sur_tab_retour_ordonnee[i] = ptr_sur_tab_retour[j/(3*largeur/8)][(j)%(3*largeur/8)];
        j=j+4;
        }
    }

    printf(" on fini avec j=%d et %d \n", j, (largeur/16)*(hauteur/8)*4);

    return ptr_sur_tab_retour_ordonnee;
    }
    
    
    if (echantillonage==2)
    {
    // On calcule les bonnes dimensions avec un echantillonage 2, horizontale et verticale
 
    int largeur = *entete;
    if (*entete % 16 != 0)
    {
    largeur += 16 - *entete % 16;
    }
    
    int hauteur = *(entete+1);
    if (*(entete+1) % 16 != 0)
    {
    hauteur += (16 - *(entete+1) % 16);
    }
    uint8_t ****ptr_sur_tab_retour = malloc(((int) hauteur / 8)* sizeof(uint8_t***));
    for (size_t i3 = 0; i3 < (int) hauteur/8; i3++) {
      ptr_sur_tab_retour[i3] = malloc(((int) largeur*3/8)*sizeof(uint8_t**));
    } 
    uint8_t R, G, B;
    uint8_t Rs1, Gs1, Bs1;
    uint8_t Rs2, Gs2, Bs2;
    uint8_t Rs3, Gs3, Bs3;
    int compteur =0;
    for (int i = 0; i<hauteur;i=i+8)
    {
    for (int j=0; j<largeur; j=j+8)
    {
    

    
    for (int i2= 0; i2<3; i2++)
    {
    int32_t** data = malloc(8*sizeof(int32_t*));
    for (size_t i3 = 0; i3 < 8; i3++) {
      data[i3] = malloc(8*sizeof(int32_t));
      } 
    for (int i1 = 0; i1<8; i1++)
    {
    for (int j1 = 0; j1<8; j1++)
    {
        R = ptr_sur_tab[i1+i][j+j1][0];
        G = ptr_sur_tab[i1+i][j+j1][1];
        B = ptr_sur_tab[i1+i][j+j1][2];
        //printf("pour i: %d et j: %d  avec hauteur %d et largeur %d; \n",i, j, hauteur, largeur);
        if (((j+j1+8*3)<largeur-1)&&((i+i1+8)<hauteur-1))
        {
        Rs1 = ptr_sur_tab[i1+i][j+8*3+j1][0];
        Gs1 = ptr_sur_tab[i1+i][j+8*3+j1][1];
        Bs1 = ptr_sur_tab[i1+i][j+j1+8*3][2];
        Rs2 = ptr_sur_tab[i1+i+8][j+j1][0];
        Gs2 = ptr_sur_tab[i1+i+8][j+j1][1];
        Bs2 = ptr_sur_tab[i1+i+8][j+j1][2];
        Rs3 = ptr_sur_tab[i1+i+8][j+8*3+j1][0];
        Gs3 = ptr_sur_tab[i1+i+8][j+8*3+j1][1];
        Bs3 = ptr_sur_tab[i1+i+8][j+j1+8*3][2];
        if ((i2)%3==0)
        {
        data[i1][j1] = (int) round(((float) G) *0.587+((float)B)*0.299+0.114*((float) R));
        }
        R = (uint8_t) R/4+Rs1/4+Rs2/4+Rs3/4;
        G = (uint8_t) G/4+Gs1/4+Gs2/4+Gs3/4;
        B =(uint8_t) B/4+Bs1/4+Bs2/4+Bs3/4;

        if (i2%3==1)
        {
        data[i1][j1] = (int) round(0.5*((float) B)-0.3313*((float)G)-((float) R)*0.1687+128);//*(*(ptr_sur_tab+i+i1)+j+j1);
        }
        if (i2%3==2)
        {
        data[i1][j1] = (int) round(-0.0813*((float) B)+-0.4187*((float)G)+((float) R)*0.5+128);//*(*(ptr_sur_tab+i+i1)+j+j1);
        }
        }
        else
        {
        if ((i2)%3==0)
        {
        data[i1][j1] = (int) round(((float) G) *0.587+((float)B)*0.299+0.114*((float) R));
        }
        if ((i2)%3==1)
        {
        data[i1][j1] = (int) round(0.5*((float) B)+-0.3313*((float)G)-((float) R)*0.1687+128);//*(*(ptr_sur_tab+i+i1)+j+j1);
        }
        if ((i2)%3==2)
        {
        data[i1][j1] = (int) round(-0.0813*((float) B)+-0.4187*((float)G)+((float) R)*0.5+128);//*(*(ptr_sur_tab+i+i1)+j+j1);
        }
        }
    }
    }
    ptr_sur_tab_retour[i/8][3*j/8+i2] = data;
    compteur++;
    affichage_data2(ptr_sur_tab_retour[i/8][3*j/8+i2]);
    }
    }
    compteur = 0;
    }
    int j = 0;
    uint8_t ***ptr_sur_tab_retour_ordonnee = malloc(((int) largeur/16 )*((int) hauteur / 16)*6*sizeof(uint8_t**));
    for (int i=0; i<6*((int) (largeur/16) )*((int) hauteur / 16);i++)
    {
        if (((j/(3*largeur/8)))%2==1)
        {
        j+=3*largeur/8;
        }
        if (i%6==0)
        {
        ptr_sur_tab_retour_ordonnee[i] = ptr_sur_tab_retour[j/(3*largeur/8)][(j)%(3*largeur/8)];
        j=j+3;
        }
        if (i%6==1)
        {
        ptr_sur_tab_retour_ordonnee[i] = ptr_sur_tab_retour[j/(3*largeur/8)][(j)%(3*largeur/8)];
        j=j-3;
        }
        if (i%6==2)
        {
        ptr_sur_tab_retour_ordonnee[i] = ptr_sur_tab_retour[j/(3*largeur/8)+1][(j)%(3*largeur/8)];
        j=j+3;
        }
        if (i%6==3)
        {
        ptr_sur_tab_retour_ordonnee[i] = ptr_sur_tab_retour[j/(3*largeur/8)+1][(j)%(3*largeur/8)];
        j=j-2;
        }
        if (i%6==4)
        {
        ptr_sur_tab_retour_ordonnee[i] = ptr_sur_tab_retour[j/(3*largeur/8)][(j)%(3*largeur/8)];
        j++;
        }
        if (i%6==5)
        {
        ptr_sur_tab_retour_ordonnee[i] = ptr_sur_tab_retour[j/(3*largeur/8)][(j)%(3*largeur/8)];
        j=j+4;
        }
     affichage_data2(ptr_sur_tab_retour_ordonnee[i]);   
     printf("fin echantillonage horizontale et verticald %d \n", i);
    }
  /*
    for (int i=0; i<largeur; i=i+8)
    {
    for (int j=0; j< hauteur; j=j+8)
    {
    free(ptr_sur_tab_retour[i][j]);
    }
    free(ptr_sur_tab_retour[i]);
    }
*/
    return ptr_sur_tab_retour_ordonnee;
    }
    }
void affichage_data2(int t[8][8])
{
    for (int i=0; i<8;i++)
    {
    for (int j=0; j<8; j++)
    {
        printf("%d ->",t[i][j]);
    }
    printf("\n");
    }
}

int main(int argc, char const *argv[])
{
        int32_t* entete = NULL; // entête du fichier pgm
        int echantillonage;
        int dct=0;
        int32_t*** ptr_sur_tab_MCU;
        int nbr_MCU;
        struct ligne_cmd *commandes;
        struct Entete *entete1;  // entête du fichier pgm
        entete1 = recupe_entete(argv[1]);
        struct jpeg *image;
        struct bitstream *stream;
        /* recupération des commandes */
        commandes = lecture_ligne_cmd(argc, argv); 
        if (commandes->validite ==0)
        {
        return 0;
        }
        echantillonage = commandes->sample; 
        dct = commandes->dtc;
        /* récupération de l'image et création des données */
        printf("Hello avant recup \n");
        entete = recuper_entete(argv[1]);
        //data = recuper_data(argv[1], entete);
        if (entete1->type == 53)
        {
        int32_t** data = NULL;   // le contenu, les pixels
        int32_t** data_new = NULL;
        int exDC = 0;
        data = recuper_data_gris(argv[1], entete1->largeur, entete1 -> longueur, entete1->nbre_octet); 
        image = creation_jpeg(entete);
        data_new = bonne_taille(data, entete);
        ptr_sur_tab_MCU = zigzag_bloc(data_new, entete); 
        int largeur = *entete;
        printf("hello \n");
        if (*entete % 8 != 0)
        {
        largeur += 8 - *entete % 8;
        }
        
        int hauteur = *(entete+1);
        if (*(entete+1) % 8 != 0)
        {
        hauteur += (8 - *(entete+1) % 8);
        }

        
        /*DCT + ZigZagi*/
        nbr_MCU = (largeur/8)*(hauteur/8);
        int32_t* ptr_tab_data; 
        int8_t taille = 64;
        jpeg_write_header(image);
        for (int i=0; i<nbr_MCU;i++)
        {
        ptr_tab_data = operations_dct_quantification_puis_zig_zag(*(ptr_sur_tab_MCU+i), 0); 
        /* Huffman  */
        gestion_compression(image, ptr_tab_data, taille, exDC, 0);
        exDC = *ptr_tab_data;

        printf("%d \n", hauteur);
        }
        jpeg_write_footer(image);
        jpeg_destroy(image);
        /* fin*/
        }
        else
        {
        /* Avec de la couleur !!!!! */
        int32_t*** data = NULL;   // le contenu, les pixels
        int32_t*** data_Y = NULL;   // le contenu, les pixels
        int32_t*** data_new = NULL;
        float** cosinus;
        int exDC[3] = {0, 0, 0};
        printf("hello1\n");
        data = recuper_data_couleur(argv[1], entete1->largeur, entete1 -> longueur, entete1->nbre_octet); 
        image = creation_jpeg_couleur(entete, echantillonage);
        if (echantillonage == 0)
        {
        data_Y = Y_Cb_Cr(data, entete1-> longueur, entete1->largeur);
        data_new = bonne_taille_couleur(data_Y, entete);
        printf("hello 3 \n");
        ptr_sur_tab_MCU = zigzag_bloc_couleur(data_new, entete, 0); 
        printf("hello 3 \n");
        int largeur = *entete;
        if (*entete % 8 != 0)
        {
        largeur += 8 - *entete % 8;
        }
        
        int hauteur = *(entete+1);
        if (*(entete+1) % 8 != 0)
        {
        hauteur += (8 - *(entete+1) % 8);
        }

        
        /*DCT + ZigZagi*/
        nbr_MCU = (largeur/8)*(hauteur/8)*3;
        int32_t* ptr_tab_data; 
        int8_t taille = 64;
        printf("hello 3 \n");
        jpeg_write_header(image);
        printf("hello 3 \n");
        for (int i=0; i<nbr_MCU;i++)
        {
        ptr_tab_data = operations_dct_quantification_puis_zig_zag(*(ptr_sur_tab_MCU+i), i%3); 
        /* Huffman  */
        gestion_compression(image, ptr_tab_data, taille, exDC[i%3],i%3);
        exDC[i%3] = *ptr_tab_data;
        printf("%d %d %d \n", exDC[0], exDC[1], exDC[2]);
        }
        jpeg_write_footer(image);
        jpeg_destroy(image);
        /* fin*/
        }
        if ((echantillonage == 2)||(echantillonage ==1))
        {
        //cas echantillonage 4:4:4 ou 4:2:0
        data_new = bonne_taille_couleur_ssechantillonnage(data, entete, echantillonage);
        ptr_sur_tab_MCU = zigzag_bloc_couleur(data_new, entete,echantillonage); 
        int na, no;
        if (echantillonage==1)
        {
        na = 16;
        no = 8;
        }
        if (echantillonage==2)
        {
        na = 16;
        no = 16;
        }
        int largeur = *entete;
        if (*entete % na != 0)
        {
        largeur += na - *entete % na;
        }
        
        int hauteur = *(entete+1);
        if (*(entete+1) % no != 0)
        {
        hauteur += (no - *(entete+1) % no);
        }

        
        /*DCT + ZigZagi*/
        if (echantillonage == 1)
        {
            nbr_MCU = (largeur/16)*(hauteur/8)*4;
        }
        if (echantillonage ==2)
        {
            nbr_MCU = (largeur/16)*(hauteur/16)*6;
        }
        int* ptr_tab_data; 
        int8_t taille = 64;
        int tab[2]={0, 0};
        jpeg_write_header(image);
        cosinus = precalculcos(largeur, hauteur);
        for (int i=0; i<nbr_MCU;i++)
        {
        /*
        if (echantillonage==1)
        {
        int tab[2]={i%4-1, 0};
        }
        if (echantillonage==2)
        {
        int tab[2]={i%6-3, 0};
        }
*/
        if (echantillonage==1)
        {
        tab[1]=i%4-1;
        }
        if (echantillonage==2)
        {
        tab[1]=i%6-3;
        }
        if (dct==1)
        {
        ptr_tab_data = operations_dct_quantification_puis_zig_zag(*(ptr_sur_tab_MCU+i), max(tab), cosinus);
        }
        if (dct==0)
        {
        ptr_tab_data = operations_naives(*(ptr_sur_tab_MCU+i), max(tab));
        }
        /* Huffman  */
        gestion_compression(image, ptr_tab_data, taille, exDC[max(tab)],max(tab));
        exDC[max(tab)] = *ptr_tab_data;
        }
        jpeg_write_footer(image);
        jpeg_destroy(image);
        /* fin*/
        }
        };
        return 0;
    }
