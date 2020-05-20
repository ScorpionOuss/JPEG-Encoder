#ifndef _RECUP_H_
#define _RECUP_H_
struct Entete;

extern int32_t **recuper_data_gris(const char* file_pgm,uint32_t largeur,uint32_t longueur);
extern struct Entete *recupe_entete(const char* file_pgm);
extern int32_t ***recuper_data_couleur(const char* file_ppm,uint32_t largeur,uint32_t longueur);
#endif
