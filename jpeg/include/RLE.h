#ifndef _RLE_H_
#define _RLE_H_


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


extern void gestion_compression(struct jpeg* image, int32_t* ptr_tab, int8_t taille_tab, int exDC, int cc);
extern void RLE(struct bitstream* stream, int32_t* ptr_sur_tab, uint8_t taille_tab, struct jpeg* image, int cc);
extern int nbr_bit_binaire(int nbr);
extern int32_t calc_magnitude(int32_t entier);
extern int32_t num_magnitude(int32_t entier, int32_t magnitude);
#endif
