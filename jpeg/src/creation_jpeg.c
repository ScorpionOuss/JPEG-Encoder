#include <stdlib.h>
#include <stdio.h>
#include <jpeg_writer.h>
#include <RLE.h>
/* partie recupération des datas */
int32_t **recuper_data(const char* file_pgm)
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
  while (compteur < 11) {
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


struct jpeg *creation_jpeg(int32_t *parametres)
//fonction qui cree un fichier jpeg : parametres est le tableau qui contient les infos de recup_data
{
    struct jpeg *image = jpeg_create();
    //Premiere ligne mystere
    jpeg_set_ppm_filename(image, "chabanas.ppm");
    jpeg_set_jpeg_filename(image, "resultat.jpg");
    jpeg_set_image_width(image, parametres[1]);
    jpeg_set_image_height(image, parametres[2]);
    jpeg_set_nb_components(image, parametres[1]*parametres[2]);

    int cc = 0;
    for (int acdc=0; acdc<2;acdc++)
    {
    /*
    jpeg_set_sampling_factor(image, enum color_component cc, enum direction dir, sampling_factor);
    */
    struct huff_table *htable;
    htable = jpeg_get_huffman_table(image, acdc, cc);
    jpeg_set_huffman_table(image, acdc, cc, htable);
    }
    uint8_t qtable;
    qtable = jpeg_get_quantization_table(image, cc);
    jpeg_set_quantization_table(image, cc, qtable);
    

    return image;
}



int main(int argc, char const *argv[])
{


    int32_t* entete = NULL; // entête du fichier pgm
    int32_t** data = NULL;   // le contenu, les pixels
    int32_t* tab[2]= {8, 8};
    struct jpeg *image;
    struct bitstream *stream;
    
    entete = recuper_entete(argv[1]);
    data = recuper_data(argv[1]);
    image = creation_jpeg(tab);
    jpeg_write_header(image);
        
    stream = jpeg_get_bitstream(image);
    
    /*DCT + ZigZag*/
  
  
    /* Huffman  */
    gestion_compression(stream, ptr_tab_data, taille);
    
    
    /* fin*/
    jpeg_write_footer(image);
    bitstream_flush(stream);
    bitstream_destroy(stream);
    
  return 0;
}
