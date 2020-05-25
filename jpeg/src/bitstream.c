#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
/* Module BITSREAM */

// Type opaque représentant le flux d'octets à écrire dans
//    le fichier JPEG de sortie (appelé bitstream dans le sujet). 
struct bitstream {
    char* nom; // nom du fichier dans lequel on écrit
    int8_t limite; // nombre de bit restant dans le buffer (au plus 8)
    int8_t data[40]; // données
};


// Retourne un nouveau bitstream prêt à écrire dans le fichier filename. 
struct bitstream *bitstream_create(const char *filename)
{
    struct bitstream* stream = malloc(sizeof(struct bitstream*));
    stream->nom = filename;
    for(int i=0; i<40; i++)
    {
        stream->data[i] = -1;
    }
    return stream;
}


 
int32_t puissance_bitstream(int32_t a, int32_t b)
// renvoie a**b si b >= 0
{
    if (b == 0)
    {
        return 1;
    }
    else
    {
        return a*puissance_bitstream(a, b-1);
    }
}



int nbr_bit_binaire_bitstream(int nbr)
// Renvoie le nombre de bit nécéssaires à l'écriture du nombre nbr en base 2
{
    if ((nbr == 1)||(nbr==0))
    // Cas de base
    {
        return (1);
    }
    else
    // On calcule la puissance de 2 strictement supérieure à nbr
    {
        int t = 1;
        int i = 0;
        while (t <= nbr)
        {
            t = t*2;
            i++;
        }
        return (i);
    }
}

void  affichage_bitstream(struct bitstream*stream)
//affichage pour tests
{
    for(int i=0; i<40; i++)
    {
        printf("%i", stream->data[i]);
    }
    printf("\n");
}

// Ecrit nb_bits bits dans le bitstream. La valeur portée par cet ensemble 
//de bits est value. Le paramètre is_marker permet d'indiquer qu'on est en train 
//d'écrire un marqueur de section dans l'en-tête JPEG ou non (voir section 
//"Ecriture dans le flux JPEG -> Byte stuffing" du sujet). 
void bitstream_write_bits(struct bitstream *stream, uint32_t value, uint8_t nb_bits, bool is_marker)
{
    // On les mets dans la bitstream
    // D'abord on s'occupe des 0
    int8_t nb_bit_reel = nbr_bit_binaire_bitstream(value);

    if (nb_bits != nb_bit_reel)
    {
        int8_t difference = nb_bits - nb_bit_reel;
        for (int i=0; i<difference; i++)
        {
            stream->data[stream->limite + i] = 0;
        }
        stream->limite += difference;
    }
    // Decomposition en binaire
    int32_t reste = value;
    for(int j=nb_bit_reel-1; j>=0; j--)
    {
        int32_t power = puissance_bitstream(2, j);
        if (power <= reste)
        {
            stream->data[stream->limite + abs(j - (nb_bit_reel - 1))] = 1;
            reste -= power;
        }
        else
        {
            stream->data[stream->limite + abs(j - (nb_bit_reel - 1))] = 0;
        }
    }
    stream->limite += nb_bit_reel;
    printf("\nLa limite est à %i\n", stream->limite);
    affichage_bitstream(stream);

    //if (stream->limite == 0){}
//    FILE* fichier = fopen("test.jpg", "a+");
//    fputc(0, fichier);
//    fputc(0, fichier);
//    fputc(10, fichier);
//    fputc(255, fichier);
//    fclose(fichier);
}

// Force l'exécution des écritures en attente sur le bitstream, s'il en existe.
void bitstream_flush(struct bitstream *stream);



// Détruit le bitstream passé en paramètre, en libérant la mémoire qui lui est associée. 
void bitstream_destroy(struct bitstream *stream);
/*
int main(void)
{
    struct bitstream* source = bitstream_create("maxime.jpg");
    bitstream_write_bits(source, 17, 10, false);
    bitstream_write_bits(source, 5, 7, false);
    bitstream_write_bits(source, 11, 10, false);
    return EXIT_SUCCESS;
}
*/
