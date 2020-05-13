#include <stdlib.h>
#include <stdio.h>
#include <jpeg_writer.h>

void creation_jpeg(int32_t *parametres)
//fonction qui cree un fichier jpeg : parametres est le tableau qui contient les infos de recup_data
{
    struct jpeg *image = jpeg_create();

    //Premiere ligne mystere
    jpeg_set_ppm_filename(image, "chabanas.ppm");
    jpeg_set_jpeg_filename(image, "resultat.jpg");
    jpeg_set_image_width(image, parametres[1]);
    jpeg_set_image_height(image, parametres[2]);
    jpeg_set_nb_components(image, parametres[1]*parametres[2]);


    //Nom remplis
    jpeg_set_sampling_factor(image, enum color_component cc, enum direction dir, sampling_factor);

    jpeg_set_huffman_table(struct jpeg *jpg, enum sample_type acdc, enum color_component cc, struct huff_table *htable);

    jpeg_set_quantization_table(image, enum color_component cc, uint8_t *qtable);

    jpeg_get_bitstream(struct jpeg *jpg);

    jpeg_write_header(image);
}


