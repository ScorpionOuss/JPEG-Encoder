#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "jpeg_writer.h"
#include "qtables.h"
#include "huffman.h"
#include "htables.h"
#include "bitstream.h"
/*
struct jpeg *jpeg_create(void)
{
    struct jpeg *nouvStruct = malloc(sizeof(struct jpeg));
    nouvStruct -> ppm_filename = malloc(20 * sizeof(char));
    nouvStruct -> jpeg_filename = malloc(20 * sizeof(char));
    nouvStruct -> huffman_table = malloc(NB_SAMPLE_TYPES * sizeof(struct huff_table**));
    int i=0;
    for (i; i<NB_SAMPLE_TYPES; i++)
    {
       nouvStruct -> huffman_table[i] = malloc(NB_COLOR_COMPONENTS*sizeof(struct huff_table*));
    }
    nouvStruct -> sampling_factor = malloc(NB_COLOR_COMPONENTS * sizeof(uint8_t[NB_DIRECTIONS]));
    for (i = 0; i < NB_COLOR_COMPONENTS; i++)
    {
        nouvStruct -> sampling_factor[i] = malloc(NB_DIRECTIONS * sizeof(uint8_t));
    }
    nouvStruct -> quantization_table = malloc(NB_COLOR_COMPONENTS*sizeof(uint8_t*));
    return nouvStruct;
}
*/
struct jpeg
{
    char *ppm_filename;
    char *jpeg_filename;
    uint32_t image_height;
    uint32_t image_width;
    uint32_t nb_components;
    struct huff_table*** huffman_table;
    uint8_t** sampling_factor;
    struct bitstream* bitstream;
    uint8_t** quantization_table;
};

struct jpeg *jpeg_create(void)
{
    struct jpeg *nouvStruct = malloc(sizeof(struct jpeg));
    nouvStruct -> huffman_table = NULL;
    nouvStruct -> sampling_factor = NULL;
    nouvStruct -> quantization_table = NULL;
    return nouvStruct;
}

void jpeg_destroy(struct jpeg *jpg)
// On libère la mémoire
{
//    printf("PPM_FILENAME : %s\n", jpg->ppm_filename);
    if (jpg->ppm_filename != NULL)
    {
//        printf("On est passé par là\n");
        free(jpg->ppm_filename);
    }
//    printf("JPEG_FILENAME : %s\n", jpg->jpeg_filename);
    if (jpg->jpeg_filename != NULL)
    {
//        printf("On est passé par là aussi\n");
        free(jpg->jpeg_filename);
    }

    // On libere hauffman
    for (int ACDC =0; ACDC<2; ACDC++)
    {
        for (int cc=0; cc<jpg->nb_components;cc++)
        {
        //free(huffman_table_get_symbols(jpg->huffman_table[ACDC][cc]));
        huffman_table_destroy(jpg->huffman_table[ACDC][cc]);
        }
        
        free(jpg->huffman_table[ACDC]);
    }
    
    free(jpg->huffman_table);
    // On libère bitstream : gere par le footer
    //On libère quantization_table
    free(jpg->quantization_table);
    // On libere le sampling_factor
    for(int32_t i=0; i>jpg->nb_components; i++)
    {
        free(jpg->sampling_factor[i]);
    }
    free(jpg->sampling_factor);
    // Pour finir on libère la struct jpeg
    free(jpg);
}

void ecriture_SOI(struct jpeg *jpg)
{
    printf("entréSOI\n");
    // Marqueur SOI ie ff d8
    bitstream_write_bits(jpg -> bitstream, 65496, 16, true);
    printf("Fin SOI\n");
}

void ecriture_APPx(struct jpeg *jpg)
{
    printf("entré APPx\n");
    // Marqueur APP0 ie ff e0
    bitstream_write_bits(jpg -> bitstream, 65504, 16, true);
    // Ecriture longueur 10
    bitstream_write_bits(jpg -> bitstream, 16, 16, false);
    // Ecriture JF
    bitstream_write_bits(jpg -> bitstream, 19014, 16, false);
    // Ecriture IF\0
    bitstream_write_bits(jpg -> bitstream, 4802048, 24, false);
    // Ecriture version JFIF 1
    bitstream_write_bits(jpg -> bitstream, 1, 8, false);
    // Ecriture version JFIF .1
    bitstream_write_bits(jpg -> bitstream, 1, 8, false);
    // Données spécifiques JFIF ( 0 sur 7 octets)
    bitstream_write_bits(jpg -> bitstream, 0, 32, false);
    bitstream_write_bits(jpg -> bitstream, 0, 24, false);
}


void ecrire_QuantificationTable(struct jpeg *jpg, uint8_t i)
// Cette fonction ecrit dans la bitstream les valeurs de la table de quantification i
{
    int compteur = 0;
    // On parcourt les 64 valeurs de la table
    for (compteur = 0; compteur < 64; compteur++)
    {
        bitstream_write_bits(jpg -> bitstream, *(jpg -> quantization_table[i] + compteur), 8, false);
    }
}

void ecriture_DQT(struct jpeg *jpg, uint8_t i)
{
    // Marqueur ff db
    bitstream_write_bits(jpg -> bitstream, 65499, 16, true);
    // Longueur de la section 67 octets
    bitstream_write_bits(jpg -> bitstream, 67, 16, false);
    // Precision 0 = 8 bits, 1 = 16 bits
    bitstream_write_bits(jpg -> bitstream, 0, 4, false); 
    // Indice I_Q
    bitstream_write_bits(jpg -> bitstream, i, 4, false);
    // Ecriture des valeurs
    ecrire_QuantificationTable(jpg, i);
}

void ecriture_SOFX(struct jpeg *jpg)
{
    // Marquage ff c0
    bitstream_write_bits(jpg -> bitstream, 65472, 16, true);
    // Longueur section : 8 fixes + 3*nb_composants
    bitstream_write_bits(jpg -> bitstream, 8 + 3*(jpg -> nb_components), 16, false);
    // Precision en 8 : toujours 8
    bitstream_write_bits(jpg -> bitstream, 8, 8, false);
    // Hauteur en pixel image
    bitstream_write_bits(jpg -> bitstream, jpg -> image_height, 16, false);
    // Largeur en pixel image
    bitstream_write_bits(jpg -> bitstream, jpg -> image_width, 16, false);
    // Nombre de composantes (3 en couleurs, 1 sinon)
    bitstream_write_bits(jpg -> bitstream, jpg -> nb_components, 8, false);
    
    // Puis on ecrit les informations pour chaque composante
    int i;
    for (i=0; i<jpg -> nb_components; i++)
    {
        // Valeur de i_c en 0 à 255
        bitstream_write_bits(jpg -> bitstream, i+1, 8, false);
        // Facteur d'échantillonage horizontal
        bitstream_write_bits(jpg -> bitstream, jpg -> sampling_factor[i][0], 4, false);
        // Facteur d'échantillonage vertical                                                 A VERIFIER (ordre)
        bitstream_write_bits(jpg -> bitstream, jpg -> sampling_factor[i][1], 4, false);
        // Table i_Q associee (de 1 à 3)
        int i_q;
        if ( i == 0)
        {
            i_q = 0;
        }
        else
        {
            i_q = 1;
        }
        bitstream_write_bits(jpg -> bitstream, i_q, 8, false);
    }
}

void ecriture_vecteur_lengths(struct jpeg *jpg, int i, int a)
// Cette fonction écrit dans la bitstream le nombre de symboles par profondeur d'une table de Huffman
{
    int compteur;
    // On parcourt les 16 niveaux de profondeur
    for (compteur=0; compteur < 16; compteur++)
    {
        printf("On est pour i = %i, ACDC = %i, et on traite la longueur %i\n", i, a, compteur);
        bitstream_write_bits(jpg -> bitstream, *((jpg -> huffman_table[a][i])-> nb_symb_per_lengths + compteur), 8, false);
        printf("Et c'est un succes\n");
    }
}

void ecriture_symbols(struct jpeg *jpg, int i, int j)
// Cette fonction écrit dans la bitstream les symboles de la table
{
    int compteur;
    printf("Debut ecriture symboles\n");
    for (compteur=0; compteur < (jpg ->huffman_table[j][i])->nb_symbols; compteur++)
    {
        bitstream_write_bits(jpg -> bitstream, *((jpg ->huffman_table[j][i])->symbols +compteur), 8, false);
    }
    printf("Fin ecriture symboles\n");
}

void ecriture_DHT(struct jpeg *jpg)
{
    int indice, ACDC;
    int limite;
    if (jpg -> nb_components == 1)
    {
        limite = 1;
    }
    else
    {
        limite = 2;
    }
    for (indice=0; indice<limite; indice++)  // ATTENTION !!!!
    {
        for (ACDC=0; ACDC <2; ACDC++)
        {
            // Marqueur ff c4
            bitstream_write_bits(jpg ->bitstream, 65476, 16, true);
            // Longueur 19 de bases + nb octets contenant la table
            uint8_t nombre = jpg -> huffman_table[ACDC][indice] -> nb_symbols;
            bitstream_write_bits(jpg -> bitstream, 19 + nombre, 16, false);
            // Info sur hauffman 3 bits à 0
            bitstream_write_bits(jpg -> bitstream, 0, 3, false);
            // Info sur hauffman 1 bit pour DC/AC (vaut 1 si AC)
            bitstream_write_bits(jpg -> bitstream, ACDC, 1, false);
            // 4 bits pour l'indice
            bitstream_write_bits(jpg -> bitstream, indice, 4, false);
            // Nombre de symbole d'une longueur donnees entre 1 et 16
            ecriture_vecteur_lengths(jpg, indice, ACDC);
            // Table avec les donnees
            ecriture_symbols(jpg, indice, ACDC);
        }
    }
}

void ecriture_SOS(struct jpeg* jpg)
{
    // Marqueur ff da
    bitstream_write_bits(jpg ->bitstream, 65498, 16, true);
    // Longueur = 6 octets + 2*N
    bitstream_write_bits(jpg ->bitstream, 6+2*jpg->nb_components, 16, false);
    //Nombre N de composante
    bitstream_write_bits(jpg ->bitstream, jpg ->nb_components, 8, false);
    // Pour toutes les composantes :
    for (int composante=1; composante<= jpg->nb_components;composante++)
    {
        //On écrit i_c sur un octet
        bitstream_write_bits(jpg ->bitstream, composante, 8, false);
        int8_t indice_h;
        if (composante == 1)
        {
            indice_h = 0;
        }
        else
        {
            indice_h = 1;
        }
        //Indice de I_h pour les coeff DC
        bitstream_write_bits(jpg ->bitstream, indice_h, 4, false);
        //Indice de I_h pour les coeff AC
        bitstream_write_bits(jpg ->bitstream, indice_h, 4, false);
    }
    // Premier indice selection spectrale = 0
    bitstream_write_bits(jpg ->bitstream, 0, 8, false);
    // Dernier indice slection spectrale = 63
    bitstream_write_bits(jpg ->bitstream, 63, 8, false);
    // Approximation successive
    // Poids forts
    bitstream_write_bits(jpg ->bitstream, 0, 4, false);
    // Poids faibles
    bitstream_write_bits(jpg ->bitstream, 0, 4, false);
}

/*
void ecriture_COM(struct jpeg *jpg)
{
    // Marqueur ff fe
    bitstream_write_bits(jpg ->bitstream, 65534, 16, true);
    // longueur 2 fixe + donnees
    bitstream_write_bits(jpg ->bitstream, jpg -> longueur_com, 2, false);
    // On écrit le commentaire sans doute via une boucle
    for (int indice=0; indice < jpg->longueur_com;indice++)
    {
        bitstream_write_bits(jpg->bitstream, jpg->commentaire[indice], 4, false);
    }
}
*/

void jpeg_write_header(struct jpeg *jpg)
// Cette fonction gère l'écriture de l'en-tête
{
    struct bitstream* bs;
    bs = bitstream_create(jpg -> jpeg_filename);
    jpg -> bitstream = bs;
    ecriture_SOI(jpg);
    ecriture_APPx(jpg);
    uint8_t compteur;
    uint8_t maxi = 1;
    if (jpg->nb_components == 3)
    {
        maxi = 2;
    }
    for (compteur=0; compteur<maxi; compteur++)
    {
        ecriture_DQT(jpg, compteur);
    }
    ecriture_SOFX(jpg);
    ecriture_DHT(jpg);
    ecriture_SOS(jpg);
    printf("FIN SOS\n");
    //ecriture_DHT(jpg);
    /*
     à implémenter
     */
    /* ouverture du fichier binaire (création éventuelle)*/
    /*
    FILE* fp;
    if ((fp = fopen(jpg -> jpeg_filename, "wb")) == NULL)
    {
        fprintf(stderr, "problème lors de l'ouverture du fichier");
    }
    else
    */
        /******* écritue du SOI ***********/
        /*short int buffer[1] = {0xd8ff};
        fwrite(buffer, sizeof(short int), 1, fp);*/
        /******* écriture APPx**********/
    /*
        short int buffer2[9] ={0xe0ff, 0x1000, 0x464a, 0x4649, 0x0100, 0x0001, 0x0000, 0x0000, 0x0000};
        fwrite(buffer2, sizeof(short int), 9, fp);
    */
    /******Commentaire *******/
        // à ajouter s'il y a un commentaire
        /* DCT*/
    /*
        uint8_t i;
        for (i=0; i<jpg -> nb_components; i++)
        {
            uint8_t buffer3[5] = {0xff, 0xdb, 0x00, 0x43, 0x00 + i};
            fwrite(buffer3, sizeof(uint8_t), 5, fp);
            fwrite(jpg -> quantization_table[i], sizeof(uint8_t), 64, fp);
        }*/
        /* SOFx */
    /*
        short int buffer4[2] = {0xc0ff, 0x0800 + 264*3*(jpg -> nb_components) - 0x48};
        uint8_t Pre= 0x08;
        short int buffer5[2] = {0x0000 + 16*(jpg -> image_height), 0x0000 + 16*(jpg -> image_width)};
        fwrite(buffer4, sizeof(short int), 2, fp);
        fwrite(&Pre, sizeof(uint8_t), 1, fp);
        fwrite(buffer5, sizeof(short int), 2, fp);
        fwrite(& jpg-> nb_components, sizeof(uint8_t), 1, fp);
        for (i=0; i<jpg -> nb_components; i++)
        {
            uint8_t buffer6[3] = {0x00 + i, 0x00 + 16*(jpg -> sampling_factor[i][0]) + jpg -> sampling_factor[i][1], 0x00 + i};
            fwrite(buffer6, sizeof(uint8_t), 3, fp);
        }
        /************/
        /* DHT   */
        /********
        int a = 0;
        for (i=0; i<jpg -> nb_components; i++)
        {
            for (a=0; a <2; a++)
            {
                short int buffer7[2] = {0xc4ff, 0x1400 + 264*((jpg -> huffman_table[i][a]) -> nb_symbols) + 264*16};
                uint8_t Nuti = 0x00 + a*8 + i*16;
                fwrite(buffer7, sizeof(short int), 2, fp);
                fwrite(&Nuti, sizeof(uint8_t), 1, fp);
                fwrite((jpg -> huffman_table[i][a]) -> nb_symb_per_lengths, sizeof(uint8_t), 16, fp);
                fwrite((jpg -> huffman_table[i][a]) -> symbols, sizeof(uint8_t), (jpg -> huffman_table[i][a]) -> nb_symbols, fp);
            }
        }*/
   /* }
    fclose(fp);
    */
}

void jpeg_write_footer(struct jpeg *jpg)
// Cette fonction gère l'écriture de la fin de fichier
{
    // On purge le buffer s'il est non vide
    bitstream_flush(jpg->bitstream); 
    // On écrit le marqueur de fin de fichier ff d9
    bitstream_write_bits(jpg -> bitstream, 65497, 16, true);
    // On libere la bitsream
    bitstream_destroy(jpg->bitstream);
}


struct bitstream *jpeg_get_bitstream(struct jpeg *jpg)
{
   return jpg->bitstream;
}

void jpeg_set_ppm_filename(struct jpeg *jpg, const char *ppm_filename)
{
    int longueur = strlen(ppm_filename);
    printf("La longueur vaut %i\n", longueur);
    (jpg->ppm_filename) = malloc(longueur + 1);
    strcpy(jpg -> ppm_filename, ppm_filename);
}

char *jpeg_get_ppm_filename(struct jpeg *jpg)
{
    return jpg->ppm_filename;
}

void jpeg_set_jpeg_filename(struct jpeg *jpg, const char *jpeg_filename)
{
    int longueur = strlen(jpeg_filename);
    printf("La longueur JPEG vaut %i\n", longueur);
    (jpg->jpeg_filename) = malloc(longueur + 1);
    strcpy(jpg -> jpeg_filename, jpeg_filename);
}

char *jpeg_get_jpeg_filename(struct jpeg *jpg)
{
    return jpg -> jpeg_filename;
}

void jpeg_set_image_height(struct jpeg *jpg, uint32_t image_height)
{
    jpg -> image_height = image_height;
}

uint32_t jpeg_get_image_height(struct jpeg *jpg)
{
    return jpg -> image_height;
}

void jpeg_set_image_width(struct jpeg *jpg, uint32_t image_width)
{
    jpg -> image_width = image_width;
}

uint32_t jpeg_get_image_width(struct jpeg *jpg)
{
    return jpg -> image_width;
}

void jpeg_set_nb_components(struct jpeg *jpg, uint8_t nb_components)
{
    jpg -> nb_components = nb_components;
}

uint8_t jpeg_get_nb_components(struct jpeg *jpg)
{
    return jpg -> nb_components;
}

void jpeg_set_sampling_factor(struct jpeg *jpg, enum color_component cc, enum direction dir, uint8_t sampling_factor)
{
    if (jpg->sampling_factor == NULL)
    {
        printf("Nombre compo %i\n",jpg->nb_components);
        uint8_t** sampling_factor_tab = malloc((jpg->nb_components)*sizeof(uint8_t*));
        for (uint32_t i=0; i<jpg->nb_components; i++)
        {
            sampling_factor_tab[i]  = malloc(2*sizeof(uint8_t));
        }
        jpg->sampling_factor = sampling_factor_tab;
    }
    (jpg -> sampling_factor)[cc][dir] = sampling_factor;
}

uint8_t jpeg_get_sampling_factor(struct jpeg *jpg, enum color_component cc, enum direction dir)
{
    return jpg -> sampling_factor[cc][dir];
}

void jpeg_set_huffman_table(struct jpeg *jpg, enum sample_type acdc, enum color_component cc, struct huff_table *htable)
{
    if (jpg->huffman_table == NULL)
    {
        jpg -> huffman_table = malloc(2 * sizeof(struct huff_table**));
        int i=0;
        for (i; i<2; i++)
        {
            (jpg -> huffman_table)[i] = malloc(jpg->nb_components*sizeof(struct huff_table*));
        }
    }
    (jpg -> huffman_table)[acdc][cc] = htable;
}

struct huff_table *jpeg_get_huffman_table(struct jpeg *jpg, enum sample_type acdc, enum color_component cc)
{
    return (jpg -> huffman_table)[acdc][cc];
}

void jpeg_set_quantization_table(struct jpeg *jpg, enum color_component cc, uint8_t *qtable)
{
    if (jpg->quantization_table == NULL)
    {
        jpg -> quantization_table = malloc(jpg->nb_components * sizeof(uint8_t*));
    }
    jpg -> quantization_table[cc] = qtable;
}

uint8_t *jpeg_get_quantization_table(struct jpeg *jpg, enum color_component cc)
{
    return jpg -> quantization_table[cc];
}

/*
void main(void)
{
    struct jpeg* jpg;
    printf("hola\n");
    jpg = jpeg_create();
    printf("hola\n");
    jpeg_set_image_width(jpg, 10);
    printf("hola\n");
    jpeg_set_image_height(jpg, 20);
    printf("hola\n");
    jpeg_set_nb_components(jpg, 3);
    printf("hola\n");
    jpeg_set_ppm_filename(jpg, "chaba");
    printf("hola\n");
    jpeg_set_jpeg_filename(jpg, "monana.jpg");
    printf("hola\n");

    int x = 0;
    int y = 0;

    for (x=0; x<NB_COLOR_COMPONENTS; x++)
    {
        for (y=0; y < NB_SAMPLE_TYPES; y++)
        {
            struct huff_table* ht;
            uint8_t* hta = malloc(20*sizeof(uint8_t));
            int o;
            for (o=0; o<20; o++)
            {
                *(hta + o) = 1;
            }
            uint8_t* htables_nb_symb_per = malloc(16 * sizeof(uint8_t));
            for (o=0; o<16; o++)
            {
                *(htables_nb_symb_per + o) = 2;
            }
            strcpy(ht -> nb_symb_per_lengths , htables_nb_symb_per);
            strcpy(ht -> symbols , hta);
            ht -> nb_symbols = 16;
            ht = huffman_table_build(htables_nb_symb_per, hta, 20);
            jpeg_set_huffman_table(jpg, x, y, ht);
        }
    }
    struct bitstream* stream;
    stream = jpeg_get_bitstream(jpg);
    stream -> limite = 5;
    strcpy(stream -> nom, jpg -> jpeg_filename);
    printf("le nom est : %s\n, la limite est: %d", stream->nom, stream->limite);
    enum color_component i = Y;
    enum direction j =H;
    jpeg_set_sampling_factor(jpg, i, j, 8);
    enum color_component a=Cb;
    enum color_component b=Cr;
    jpeg_set_quantization_table(jpg, i, quantification_table_Y);
    jpeg_set_quantization_table(jpg, a, quantification_table_CbCr);
    jpeg_set_quantization_table(jpg, b, quantification_table_CbCr);
    printf("hola\n");
    jpeg_write_header(jpg);
    //jpeg_write_footer(jpg);

}
*/

