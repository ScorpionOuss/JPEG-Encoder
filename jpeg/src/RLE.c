#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "jpeg_writer.h"
#include "huffman.h"
#include "htables.h"
#include  "bitstream.h"

/* RLE - GESTION_COMPRESSION  */

// Ce fichier effectue les opérations d'écriture dans la bitstream des donnes de façon encodée à partir du resultat de zig zag

int32_t puissance(int32_t a, int32_t b)
// renvoie a**b si b >= 0 | Version récursive
{
    if (b == 0)
    {
        return 1;
    }
    else
    {
        return a*puissance(a, b-1);
    }
}


int32_t calc_magnitude(int32_t entier)
// renvoie la magnitude d'un entier
{
    int32_t p = 0;
    int32_t n = 1;
    while(n <= abs(entier))
    {
        p += 1;
        n*= 2;
    }

    return p;
}



int32_t num_magnitude(int32_t entier, int32_t magnitude)
// renvoie l'indice d'un entier dans sa magnitude
{
    if (entier >= 0)
    {
        return entier;
    }
    else
    {
        return puissance(2, magnitude) - abs(entier) -1;
    }
}

void RLE (struct bitstream* stream, int32_t* ptr_sur_tab, uint8_t taille_tab, struct jpeg* image, int cc)
// Cette fonction s'occupe de l'écriture dans la bitstream des coefficients AC
{
    // Pointeur qui s'occupe de la longueur de l'encodage
	uint8_t *ptr_nbr_bit;
	ptr_nbr_bit = malloc(sizeof(uint8_t));

    uint8_t nbr_zero_prec = 0; 
    uint32_t suite_bit_premier, suite_bit_suivant;
    uint8_t value;

    uint8_t sample_type = 1; 
    // On récupère la table de Huffman associée aux coefficients AC
	struct huff_table *pointeur_sur_htable;
    pointeur_sur_htable = jpeg_get_huffman_table(image, sample_type,cc);


    // On parcourt tous les coefficients AC qu'on écrit successivement dans la bitstream
    for (int indice_tab = 1; indice_tab < taille_tab; indice_tab++)
    {
        // Cas nul : on s'intéresse aux suites de 0
        if (*(ptr_sur_tab + indice_tab) == 0)
        {
            nbr_zero_prec++;
        }
        // Cas non nul
        else
        {
            while (nbr_zero_prec >= 16)
            // On s'occupe de l'encodage des blocs de 16 zeros consécutifs situés en amont s'ils existent
            {
                // On code 0xF0 = -16 qui est le marqueur en calculant son huffman d'abord
                suite_bit_premier = huffman_table_get_path(pointeur_sur_htable,-16, ptr_nbr_bit);
                bitstream_write_bits(stream, suite_bit_premier, *ptr_nbr_bit, false);
                nbr_zero_prec -= 16;
            }
            // On encode la magnitude via Huffman (subtilité et il reste des 0 precedents dont on additionne le nombre)
            value = 16*nbr_zero_prec + calc_magnitude(*(ptr_sur_tab + indice_tab));
            suite_bit_premier = huffman_table_get_path(pointeur_sur_htable,value, ptr_nbr_bit);
            // On écrit la magnitude encodée dans la bitstream
            bitstream_write_bits(stream, suite_bit_premier, *ptr_nbr_bit, false);

            // Puis on écrit l'indice dans la magnitude dans la bitstream
            suite_bit_suivant = num_magnitude(*(ptr_sur_tab+indice_tab),calc_magnitude(*(ptr_sur_tab+indice_tab)));
            bitstream_write_bits(stream, suite_bit_suivant, calc_magnitude(*(ptr_sur_tab+indice_tab)), false);

            nbr_zero_prec=0;
        }
    }

    // Cas où on finit les AC par des 0
    if (!(nbr_zero_prec==0))
    {
        // On écrit dans la bitstream le marqueur 0x00 = 0
        suite_bit_premier = huffman_table_get_path(pointeur_sur_htable,0, ptr_nbr_bit);
        bitstream_write_bits(stream, suite_bit_premier, *ptr_nbr_bit, true);
    }

    // On désalloue la mémoire utilisée dans la fonction
    free(ptr_nbr_bit);
}





/* FONCTION PRINCIPALE : GERE LA COMPRESSION DES DONNES */



void gestion_compression(struct jpeg* image, int32_t* ptr_tab, int8_t taille_tab, int exDC, int cc)
// Cette fonction gère la suite d'opération qui mène la sortie de zig-zag à l'écriture dans la bitstream
// Pour un bloc donné, elle gère DC mais délègue à RLE les coefficients AC et les calculs de magnitude
// et d'indice dans magnitude à des fonctions auxiliaires
{    //  << Codage de DC >>

    // Pointeur qui s'occupe de la longueur de l'encodage
    uint8_t* ptr_nbr_bit;
    ptr_nbr_bit = malloc(sizeof(uint8_t));

    // On récupère la bitstream associée au jpeg pour pouvoir écrire avec
    struct bitstream* stream;
    stream = jpeg_get_bitstream(image);

    uint8_t sample_type = 0;

    // On récupère la table de Huffman utile à l'encodage de DC
    struct huff_table* pointeur_sur_htable;
    pointeur_sur_htable = jpeg_get_huffman_table(image, sample_type,cc);

    uint32_t suite_bit_premier;
    uint32_t suite_bit_suivant;
    uint8_t value;

    // On calcule la value a écrire qui correspond à la magnitude du coefficient - le DC précédent s'il existe
    value = calc_magnitude(*ptr_tab-exDC);

    // On calcule le code de Hauffman de la magnitude
    suite_bit_premier = huffman_table_get_path(pointeur_sur_htable, value, ptr_nbr_bit);

    // On regarde l'indice de la valeur de DC dans la classe
    suite_bit_suivant = num_magnitude(*ptr_tab-exDC, calc_magnitude(*ptr_tab-exDC));

    // On écrit ces deux valeurs dans la bitstream
    bitstream_write_bits(stream, suite_bit_premier,*ptr_nbr_bit, false);
    bitstream_write_bits(stream, suite_bit_suivant, value, false);

    // << Codage des AC >>
    RLE(stream, ptr_tab, taille_tab, image, cc);

    // On libère la mémoire allouée dans la fonction
    free(ptr_nbr_bit);

}


