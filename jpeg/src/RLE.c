#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "jpeg_writer.h"
#include "huffman.h"
#include "htables.h"
#include  "bitstream.h"
/*#include "magnitude.h"*/
void RLE (struct bitstream* stream, int32_t* ptr_sur_tab, uint8_t taille_tab, struct jpeg* image)
{
	struct huff_table *pointeur_sur_htable;
    uint8_t i=1;
	uint8_t nbr_zero_prec = 0, suite_bit_premier, suite_bit_suivant;
    uint8_t value = 7;
	uint8_t *ptr_nbr_bit;
	ptr_nbr_bit = malloc(sizeof(uint8_t));
    uint8_t sample_type = 1;
    uint8_t color_component = 0; /* attention */
    pointeur_sur_htable = jpeg_get_huffman_table(image, 1,0);
    for (int i = 1; i < taille_tab; i++)
    {
    if (*(ptr_sur_tab + i) == 0)
    {
    nbr_zero_prec++; 
    }
    else
    {
    value = 16*nbr_zero_prec + calc_magnitude(*(ptr_sur_tab + i));
    suite_bit_premier = huffman_table_get_path(pointeur_sur_htable,value, ptr_nbr_bit);
    bitstream_write_bits(stream, suite_bit_premier, *ptr_nbr_bit, false);
    suite_bit_suivant = num_magnitude(*(ptr_sur_tab+i),calc_magnitude(*(ptr_sur_tab+i)));
    bitstream_write_bits(stream, suite_bit_suivant, nbr_bit_binaire(suite_bit_suivant), false);
    nbr_zero_prec=0; 
    }
    }

   /*
    entier_huff = huffman_table_get_path(pointeur_sur_htable,value, ptr_nbr_bit);
	printf("%d", entier_huff);*/
}


void gestion_compression(struct jpeg* image, int32_t* ptr_tab, int8_t taille_tab)
{
    /* Codage de DC*/
    jpeg_write_header(image);
    printf("\n %d \n \n \n",*ptr_tab);
	uint8_t* ptr_nbr_bit;
    struct bitstream* stream;
    stream = jpeg_get_bitstream(image);
    ptr_nbr_bit = malloc(sizeof(uint8_t));
    uint8_t sample_type = 0;
    uint8_t color_component = 0; /* attention */
    struct huff_table* pointeur_sur_htable;
    uint32_t suite_bit_premier;
    uint32_t suite_bit_suivant;
    uint8_t value = 7;
    //bitstream_write_bits(stream,55551, 16, true);
    pointeur_sur_htable = jpeg_get_huffman_table(image, 0,0);
    value = calc_magnitude(*ptr_tab);
    printf(" %d \n",value);
    suite_bit_premier = huffman_table_get_path(pointeur_sur_htable, value, ptr_nbr_bit);
    printf(" %d \n", suite_bit_premier);
    suite_bit_suivant = num_magnitude(*ptr_tab, calc_magnitude(*ptr_tab));
    printf(" %d \n", suite_bit_suivant);
    
    
    bitstream_write_bits(stream, suite_bit_premier,*ptr_nbr_bit, false);
    bitstream_write_bits(stream, suite_bit_suivant, nbr_bit_binaire(suite_bit_suivant), false);
    //bitstream_write_bits(stream, suite_bit_suivant+2^(nbr_bit_binaire(suite_bit_suivant))*suite_bit_premier,*ptr_nbr_bit+ nbr_bit_binaire(suite_bit_suivant), false);
    //ecrire_dans_l_autre_sens(stream, suite_bit_premier+2^(*ptr_nbr_bit)*suite_bit_suivant,*ptr_nbr_bit+ nbr_bit_binaire(suite_bit_suivant));
   /* Codage des AC*/
   RLE(stream, ptr_tab, taille_tab, image);
   
   //bitstream_write_bits(stream, 0, 8, true);
   //bitstream_flush(stream);
   jpeg_write_footer(image);
   jpeg_destroy(image);
//bitstream_write_bits(stream, 0, 8, false);
}
void ecrire_dans_l_autre_sens(struct bitstream* stream,int nombre, uint8_t nombre_de_bit)
{
    int nbr_de_bit_restant = nombre_de_bit;
    int nbr = nombre;
    while (!(nbr == 1))
    {
        bitstream_write_bits(stream, nbr % 2, 1, false);
        nbr = nbr / 2;
        nbr_de_bit_restant--;
    }
    bitstream_write_bits(stream, 1, 1, false);
    bitstream_write_bits(stream, 0, nbr_de_bit_restant, false);
    
}

int nbr_bit_binaire(int nbr)
{
    if ((nbr == 1)||(nbr==0))
    {
    return (1);
    }
    else
    {
    int t = 1;
    int i = 0;
    while (t<=nbr)
    {
        t = t*2;
        i++;
    }
    return (i);
    }
}
int32_t puissance(int32_t a, int32_t b)
// renvoie a**b si b >= 0
{
    if (b == 0){
        return 1;
    }
    else {
        return a*puissance(a, b-1);
    }
}


int32_t calc_magnitude(int32_t entier)
// renvoie la magnitude d'un entier
{
    int32_t p = 0;
    int32_t n = 1;
    while(n <= abs(entier)){
        p += 1;
        n*= 2;
    }

    return p;
}

int32_t num_magnitude(int32_t entier, int32_t magnitude)
// renvoie l'indice d'un entier dans sa magnitude
{
    if (entier >= 0){
        return entier;
    }
    else {
        return puissance(2, magnitude) - abs(entier) -1;
    }
}


/*
int main(void)
{
    int32_t t[7] = {14, -1, 1, 0,0,0,1};
	int32_t* ptr_tab;
    ptr_tab = &t;
    struct bitstream stream;
    stream = bitstream_create("test");
    RLE(stream, ptr_tab, 7);
    bitstream_flush(stream);
    bitstream_destroy(stream);
    return EXIT_SUCCESS;
}
*/
