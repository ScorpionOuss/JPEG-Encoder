#ifndef RLE_H
#define RLE_H

extern void gestion_compression(struct bitstream* stream, int32_t* ptr_tab, int8_t taille_tab);
extern void RLE (struct bitstream* stream, int32_t* ptr_sur_tab, uint8_t taille_tab);

#endif
