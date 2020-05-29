#ifndef DCT_H
#define DCT_H

#include <math.h>


extern float** precalculcos(int32_t largeur, int32_t longueur):

extern int32_t* operations_dct_zig_zag_quantification(uint8_t** data, int cc, float** cosinus);
extern int32_t* operations_naives(uint8_t** data, int cc);


#endif
