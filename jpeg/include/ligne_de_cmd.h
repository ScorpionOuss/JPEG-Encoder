#ifndef _LIGNE_DE_CMD_H_
#define _LIGNE_DE_CMD_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

struct ligne_cmd {
    int8_t validite; // 1 si valide, 0 sinon
    char* nom;        // nom du fichier
    int8_t sample;    // version sous echantillonnage 0, 1, 2
    int8_t dtc;
};


extern struct ligne_cmd *lecture_ligne_cmd(int argc, char const *argv[]);

#endif
