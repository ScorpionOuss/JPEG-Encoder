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
int32_t** recuper_data(const char* file_pgm)
{
    FILE* fichier = fopen(file_pgm, "r+");
    int32_t** data = malloc(8*sizeof(int32_t*));
    for (size_t i = 0; i < 8; i++) {
      data[i] = malloc(8*sizeof(int32_t));
    }
    fseek(fichier, 11, SEEK_SET);
    for (size_t y = 0; y < 8; y++) {
      for (size_t x = 0; x < 8; x++) {
        data[y][x] = fgetc(fichier);
      }
}
fclose(fichier);
return data;
}


int32_t *recuper_entete(const char* file_pgm)
{
    FILE* fichier = fopen(file_pgm, "r+");
    int32_t *vect = malloc(11*sizeof(int32_t));
    char *vect2 = malloc(11*sizeof(char));
    int compteur = 0;
    int nbr=0;
    while (compteur < 11)
    {
      *(vect + compteur) = fgetc(fichier);
      compteur++;
    }
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
    fclose(fichier);
    return vect;
}


struct jpeg *creation_jpeg(int32_t parametres[2])
//fonction qui cree un fichier jpeg : parametres est le tableau qui contient les infos de recup_data
{
    struct jpeg *image = jpeg_create();
    //Premiere ligne mystere
    jpeg_set_ppm_filename(image, "chabanas.ppm");
    jpeg_set_jpeg_filename(image, "resultat.jpg");
    jpeg_set_image_width(image, 8);
    jpeg_set_image_height(image, 8);
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
int main(int argc, char const *argv[])
{
    int32_t* entete = NULL; // entête du fichier pgm
    int32_t** data = NULL;   // le contenu, les pixels
    int32_t* tab[2]= {8, 8};
    struct jpeg *image;
    struct bitstream *stream;
    /* récupération de l'image et création des données */
    entete = recuper_entete(argv[1]);
    data = recuper_data(argv[1]);
    image = creation_jpeg(tab);
        
    stream = jpeg_get_bitstream(image);
    /*DCT + ZigZagi*/
    int32_t* ptr_tab_data; 
    int8_t taille = 64;
    ptr_tab_data = operations_dct_quantification_puis_zig_zag(data); 

    /* Huffman  */
    gestion_compression(image, ptr_tab_data, taille);
    
    /* fin*/
    return 0;
}
