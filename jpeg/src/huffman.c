#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "huffman.h"


/* Module Huffman */

// NB : La strcuture huff_table est créée dans huffman.h. Pour mémoire, voici sa définition :

/*
struct huff_table
{
    uint8_t *nb_symb_per_lengths;
    uint8_t *symbols;
    uint8_t nb_symbols;
};
*/
struct huff_table *huffman_table_build(uint8_t *nb_symb_per_lengths, uint8_t *symbols, uint8_t nb_symbols)
// Construit un arbre de Huffman à partir d'une table de symboles comme présenté en section
// "Compression d'un bloc fréquentiel -> Le codage de Huffman" du sujet.
// nb_symb_per_lengths est un tableau contenant le nombre de symboles pour chaque longueur de 1 à 16,
// symbols est le tableau  des symboles ordonnés, et nb_symbols représente la taille du tableau symbols.
{
    // On fait les allocations mémoires pour chacun des arguments
    struct huff_table *huffman_table = malloc(sizeof(struct huff_table));
    huffman_table -> nb_symb_per_lengths = malloc(16*sizeof(uint8_t));
    huffman_table -> symbols = malloc(nb_symbols*sizeof(uint8_t));

    // On définit les valeurs des arguments
    huffman_table -> nb_symb_per_lengths = nb_symb_per_lengths;
    huffman_table -> symbols = symbols;
    huffman_table -> nb_symbols = nb_symbols;

    return huffman_table;
}



int determiner(struct huff_table *ht, uint8_t value)
// Cette fonction permet de ...
{
    int compteur = 0;
    while (*(ht -> symbols + compteur) != value)
    {
        compteur += 1;
    }
    return compteur;
}

/*
uint32_t interpreterCommeEntier(char* chaine)
{
    int i=0, n=0;
    for (i=0; *(chaine + i) != "\0"; i++)
    {
        n = 10*n + chaine[i] - '0';
    }
    return n;
}
*/

uint32_t* update(uint32_t* table, int taille)
{
    if (table == NULL)
    {
        printf(" cas NULL %d \n",taille);
        uint32_t* tab_retour = malloc(2*sizeof(uint32_t));
        tab_retour[0] = 0;
        tab_retour[1] = 1;
        return tab_retour;
    }
    else
    {
        uint32_t* table_copie = malloc((taille/2)*sizeof(uint32_t));
        for (int ind=0; ind<taille/2; ind++)
        {
        *(table_copie+ind) = *(table+ind);
        }
        uint32_t* tab_retour = realloc(table, taille*sizeof(uint32_t));
        int ind;
        for (ind=0; ind<taille/2; ind++)
        {
            *(tab_retour + 2*ind) = *(table_copie + ind)*2;
            *(tab_retour + 2*ind + 1)= *(table_copie + ind)*2 + 1;
        }
        free(table_copie);
        return tab_retour;
    }
}


uint32_t* updateDeleting(uint32_t* tab, uint8_t longueur, int taille)
{
    uint32_t* tab_retour = malloc(taille*sizeof(uint32_t));
    for (int ind=0; ind <taille; ind++)
    {
        tab_retour[ind] =  tab[ind + longueur];
    }
    free(tab);
    return tab_retour;
}


uint32_t huffman_table_get_path(struct huff_table *ht, uint8_t value, uint8_t *nb_bits)
// Retourne le chemin dans l'arbre ht permettant d'atteindre la feuille de valeur value.
// nb_bits est un paramètre de sortie permettant de stocker la longueur du chemin retourné.
{
    int taille = 1;
    int valeur;
    uint8_t compteur;
    compteur = 0;
    valeur = determiner(ht, value) + 1;
    uint32_t* tableValeursCourante = NULL;
    //int compteurLongueur = 0;
    int compteurSommeValeurs = 0;
    while ( compteurSommeValeurs < valeur)
    {
        compteurSommeValeurs += ht -> nb_symb_per_lengths[compteur];
        compteur += 1;
        taille = taille*2;
        tableValeursCourante = update(tableValeursCourante, taille);
        if ( compteurSommeValeurs < valeur)
        {
            taille -= ht -> nb_symb_per_lengths[compteur - 1];
            tableValeursCourante = updateDeleting(tableValeursCourante, ht -> nb_symb_per_lengths[compteur - 1], taille);
        }
    }
    *nb_bits = compteur;
    //on enregiste la valeur de sortie dans une variable temporaire
    uint32_t temp = tableValeursCourante[valeur - compteurSommeValeurs + ht -> nb_symb_per_lengths[compteur - 1] - 1];
    free(tableValeursCourante);
    return temp;
}


uint8_t *huffman_table_get_symbols(struct huff_table *ht)
{
    return ht -> symbols;
}

uint8_t *huffman_table_get_length_vector(struct huff_table  *ht)
{
    return ht ->  nb_symb_per_lengths;
}



void huffman_table_destroy(struct huff_table *ht)   
// On libère la mémoire : d'abord les arguments puis la structure
{
    //free(ht -> nb_symb_per_lengths);
    //free(ht -> symbols);
    free(ht);
}


