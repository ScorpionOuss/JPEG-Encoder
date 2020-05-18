#ifndef RLE_H
#define RLE_H

#include "htables.h"
#include "huffman.h"
extern void gestion_compression(struct jpeg* image, int32_t* ptr_tab, int8_t taille_tab, int exDC);
extern void RLE(struct bitstream* stream, int32_t* ptr_sur_tab, uint8_t taille_tab, struct jpeg* image);
extern int nbr_bit_binaire(int nbr);
extern int32_t calc_magnitude(int32_t entier);
extern int32_t num_magnitude(int32_t entier, int32_t magnitude);
#endif
