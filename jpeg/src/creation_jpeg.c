#include <stdlib.h>
#include <stdio.h>
#include <jpeg_writer.h>
#include <RLE.h>
#include <math.h>
#include "htables.h"
#include "qtables.h"
#include "huffman.h"

#define pi 3.1415927
/* partie recupération des datas */
int32_t** recuper_data(const char* file_pgm, int32_t* entete)
{
    FILE* fichier = fopen(file_pgm, "r+");
    int32_t** data = malloc(*entete*sizeof(int32_t*));
    for (size_t i = 0; i < *(entete); i++) {
      data[i] = malloc(*(entete+1)*sizeof(int32_t));
    }
    fseek(fichier, 11, SEEK_SET);
    for (size_t y = 0; y < *entete; y++) {
      for (size_t x = 0; x < *(entete+1); x++) {
        data[y][x] = fgetc(fichier);
      }
}
fclose(fichier);
return data;
}


int32_t *recuper_entete(const char* file_pgm)
{
    FILE* fichier = fopen(file_pgm, "r+");
    int32_t *parametre = malloc(11*sizeof(int32_t));
    int compteur = 0;
    int nbr=0;
    int current = 0;
    int str[4];
    str[0] = fgetc(fichier);
    str[1] = fgetc(fichier);
    str[2] = fgetc(fichier);
    if (str[0]==5*16 && str[1]==3*16+5 && str[2]==10)
    {
    printf("P5 OK \n");
    current = fgetc(fichier);
    while (!(current == 32))
    {
        nbr = nbr*10+current-3*16;
        current = fgetc(fichier);
    }
    *(parametre+compteur) = nbr;
    nbr = 0;
    compteur++;
    current = fgetc(fichier);
    while (!(current == 10))
    {
        nbr = nbr*10+current-3*16;
        current = fgetc(fichier);
    }
    *(parametre+compteur) = nbr;
    nbr = 0;
    compteur++;
    current = fgetc(fichier);
    while (!(current == 10))
    {
        nbr = nbr*10+current-3*16;
        current = fgetc(fichier);
    }
    *(parametre+compteur) = nbr;
    nbr = 0;
    compteur++;
    }
    fclose(fichier);
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
    for (int i=0; i<3; i++)
    {
    jpeg_set_sampling_factor(image,cc, i, 4);
    }
    uint8_t *qtables;
    qtables = &quantification_table_Y;
    jpeg_set_quantization_table(image, cc, qtables);
    

    return image;
}


int32_t **bonne_taille(int32_t **data, int dimensions[2])
//Transforme l'image de sorte que ses dimensions soient un multiple de 8
{
    // On calcule les bonnes dimensions
    int bonne_abscisse = dimensions[0];
    if (dimensions[0] % 8 != 0)
    {
    bonne_abscisse += 8 - dimensions[0] % 8;
    }
    
    int bonne_ordonnee = dimensions[1];
    if (dimensions[1] % 8 != 0)
    {
    bonne_ordonnee += (8 - dimensions[1] % 8);
    }
        
    // On créé un nouveau tableau data aux bonnes dimensions
    int32_t** data_new = malloc(bonne_ordonnee*sizeof(int32_t*));
    for (size_t i = 0; i < bonne_ordonnee; i++) {
      
      data_new[i] = malloc(bonne_abscisse*sizeof(int32_t));
    }
    
    
    // On le remplit avec les anciennes valeurs
    for (size_t y = 0; y < dimensions[1]; y++) {
      for (size_t x = 0; x < dimensions[0]; x++) {
        data_new[y][x] = data[y][x];
      }
    }
        
    // On met les nouvelles valeurs
    // En bas d'abord
     for (size_t y = dimensions[1]; y < bonne_ordonnee; y++) {
        for (size_t x = 0; x < dimensions[0]; x++) {
            data_new[y][x] = data[dimensions[1]-1][x];
        }
    }
    // Puis à droite
     for (size_t y = 0; y < bonne_ordonnee; y++) {
        for (size_t x = dimensions[0]; x < bonne_abscisse; x++) {
            data_new[y][x] = data[y][dimensions[0] -1];
        }
    }

    
    return data_new;

}





int32_t ***zigzag_bloc(int32_t** ptr_sur_tab, int largeur, int hauteur)
{

    int32_t **ptr_sur_tab_retour = malloc(((int) hauteur / 8)* sizeof(int32_t*));
    for (size_t i3 = 0; i3 < 8; i3++) {
      ptr_sur_tab[i3] = malloc(((int) largeur/8)*sizeof(int32_t));
    } 
    for (int i = 0; i<hauteur;i=i+8)
    {
    for (int j=0; j<largeur; i=i+8)
    {
    int32_t** data = malloc(8*sizeof(int32_t*));
    for (size_t i3 = 0; i3 < 8; i3++) {
      data[i3] = malloc(8*sizeof(int32_t));
    }
    for (int i1 = 0; i1<8; i1++)
    {
    for (int j1 = 0; j1<8; j1++)
    {
        data[i+i1][j+j1] = ptr_sur_tab[i+i1][j+j1];
    }
    }
    ptr_sur_tab_retour[i][j] = data; 
    }
    }

    int32_t** ptr_sur_tab_retour_ordonnee = malloc(((int) largeur/8 )*((int) hauteur / 8)* sizeof(int32_t*));
}

int main(int argc, char const *argv[])
{
    int32_t* entete = NULL; // entête du fichier pgm
    int32_t** data = NULL;   // le contenu, les pixels
    int32_t** data_new = NULL;
    struct jpeg *image;
    struct bitstream *stream;
    /* récupération de l'image et création des données */
    entete = recuper_entete(argv[1]);
    data = recuper_data(argv[1], entete);
    image = creation_jpeg(entete);
    int32_t dim[2] = {585, 487};
    data_new = bonne_taille(data, dim);
    printf("%i \n", data_new[0][589]);
    printf("%i \n", data_new[0][585]);
    printf("%i \n", data_new[0][586]);
    /*
    stream = jpeg_get_bitstream(image);
    //DCT + ZigZagi
    int32_t* ptr_tab_data; 
    int8_t taille = 64;
    ptr_tab_data = operations_dct_quantification_puis_zig_zag(data); 

    // Huffman
    gestion_compression(image, ptr_tab_data, taille);
    
    // fin
    */
    return 0;
}
