#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "jpeg_writer.h"
#include "huffman.h"
#include "htables.h"
#include  "bitstream.h"
/*#include "magnitude.h"*/
void RLE (struct bitstream* stream, int32_t* ptr_sur_tab, uint8_t taille_tab, struct jpeg* image, int cc)
{
	struct huff_table *pointeur_sur_htable;
    uint8_t i=1;
	uint8_t nbr_zero_prec = 0; 
    uint32_t suite_bit_premier, suite_bit_suivant;
    uint8_t value = 7;
	uint8_t *ptr_nbr_bit;
	ptr_nbr_bit = malloc(sizeof(uint8_t));
    uint8_t sample_type = 1;
    pointeur_sur_htable = jpeg_get_huffman_table(image, 1,cc);
    for (int i = 1; i < taille_tab; i++)
    {
    if (*(ptr_sur_tab + i) == 0)
    {
    nbr_zero_prec++;
    }
    else
    {
    while (nbr_zero_prec >= 16)
    {   
    suite_bit_premier = huffman_table_get_path(pointeur_sur_htable,-16, ptr_nbr_bit);
    bitstream_write_bits(stream, suite_bit_premier, *ptr_nbr_bit, false);
    nbr_zero_prec -= 16;
    }
    value = 16*nbr_zero_prec + calc_magnitude(*(ptr_sur_tab + i));
    suite_bit_premier = huffman_table_get_path(pointeur_sur_htable,value, ptr_nbr_bit);
    bitstream_write_bits(stream, suite_bit_premier, *ptr_nbr_bit, false);
    suite_bit_suivant = num_magnitude(*(ptr_sur_tab+i),calc_magnitude(*(ptr_sur_tab+i)));
    bitstream_write_bits(stream, suite_bit_suivant, calc_magnitude(*(ptr_sur_tab+i)), false);
    nbr_zero_prec=0; 
    }
    }
    if (!(nbr_zero_prec==0))
    {
    suite_bit_premier = huffman_table_get_path(pointeur_sur_htable,0, ptr_nbr_bit);
    bitstream_write_bits(stream, suite_bit_premier, *ptr_nbr_bit, true);
    }
}


void gestion_compression(struct jpeg* image, int32_t* ptr_tab, int8_t taille_tab, int exDC, int cc)
{
    /* Codage de DC*/
   uint8_t* ptr_nbr_bit;
   struct bitstream* stream;
   stream = jpeg_get_bitstream(image);
   ptr_nbr_bit = malloc(sizeof(uint8_t));
   uint8_t sample_type = 0;
   struct huff_table* pointeur_sur_htable;
   uint32_t suite_bit_premier;
   uint32_t suite_bit_suivant;
   uint8_t value = 7;
   //bitstream_write_bits(stream,55551, 16, true);
   pointeur_sur_htable = jpeg_get_huffman_table(image, 0,cc);
   value = calc_magnitude(*ptr_tab-exDC);
   suite_bit_premier = huffman_table_get_path(pointeur_sur_htable, value, ptr_nbr_bit);
   suite_bit_suivant = num_magnitude(*ptr_tab-exDC, calc_magnitude(*ptr_tab-exDC));
    
    
   bitstream_write_bits(stream, suite_bit_premier,*ptr_nbr_bit, false);
   bitstream_write_bits(stream, suite_bit_suivant, value, false);
   /* Codage des AC*/
   RLE(stream, ptr_tab, taille_tab, image, cc);
   /* Ecriture fin de l'image */
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
