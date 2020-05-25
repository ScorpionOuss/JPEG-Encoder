#ifndef DCT_H
#define DCT_H

#include <math.h>


extern float** precalculcos(int32_t largeur, int32_t longueur):

extern int32_t* operations_dct_quantification_puis_zig_zag(int32_t** data, int cc, float** cosinus);
extern int32_t* operations_naives(int32_t** data, int cc);


#endif
