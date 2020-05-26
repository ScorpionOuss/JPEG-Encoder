#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>



/* Module BITSREAM */




// Type opaque représentant le flux d'octets à écrire dans
//    le fichier JPEG de sortie (appelé bitstream dans le sujet). 
struct bitstream {
    char* nom; // nom du fichier dans lequel on écrit
    int8_t limite; // nombre de bits libres restant dans le buffer (au plus 8)
    uint8_t data; // données ou buffer
    FILE* fichier; // fichier de travail
};



// Retourne un nouveau bitstream prêt à écrire dans le fichier filename. 
struct bitstream *bitstream_create(const char *filename)
{
    //printf("creation\n");
    // Allocation mémoire de la source
    struct bitstream* stream = malloc(sizeof(struct bitstream*));
    //printf("intermediaire 0 \n");
    // On remplit ses attributs
    stream->nom = filename;
    //printf("intermediaire 1\n");
    stream->data = 0;
    //printf("intermediaire 2\n");
    stream->limite = 8;
    //printf("intermediaire 3\n");
    // S'il y a une erreur
    stream->fichier = fopen(stream->nom, "wb");
    //printf("fin creation");
    return stream;
}



uint32_t puissance_bitstream(uint32_t a, uint32_t b)
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



uint32_t nbr_bit_binaire_bitstream(uint32_t nbr)
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



// Force l'exécution des écritures en attente sur le bitstream, s'il en existe.
void bitstream_flush(struct bitstream *stream)
{
    if(stream->limite != 8)
    {
        // Ecriture de l'octet stream->data
        fputc(stream->data, stream->fichier);

        // On re-initialise les parametre pour le prochain appel
        stream->data = 0;
        stream->limite = 8;
    }
}



// Ecrit nb_bits bits dans le bitstream. La valeur portée par cet ensemble 
//de bits est value. Le paramètre is_marker permet d'indiquer qu'on est en train 
//d'écrire un marqueur de section dans l'en-tête JPEG ou non (voir section 
//"Ecriture dans le flux JPEG -> Byte stuffing" du sujet). 
void bitstream_write_bits(struct bitstream *stream, uint32_t value, uint8_t nb_bits, bool is_marker)
{
    // D'abord on s'occupe des 0 inutile devant s'il y en a
    // On regarde donc si on doit écrire value sur le nombre de bit qui sont necessaires
    //printf("Debut write");
    int8_t nb_bit_reel = nbr_bit_binaire_bitstream(value);
    int32_t difference = nb_bits - nb_bit_reel;

    while (stream->limite >0 && nb_bits != 0)
    // On procede étape par étape : on va charger un bit a la fois dans la bitstream
    // tant que le buffer n'est pas plein ou value est épuisé
    {
        //printf("Debut while\n");
        if (difference >0)
        // D'abord, les 0 inutiles devant
        {
            //printf("Dans le if\n");
            nb_bits -= 1;
            stream->limite -=1;
            difference -= 1;
        }
        else
        {
            //printf("Pas dans le if\n");
            // Désormais, on s'interesse à des bit significatifs
            // On veut savoir l'écriture en binaire de value
            //des bits de poids forts vers les bits de poids faibles
            //Donc on le compare à la puissance entière de 2 correspondant
            //au bit courant (déterminé par limite)
            int32_t power = puissance_bitstream(2, nb_bits-1);
            if(value >= power)
            {
                //printf("Cas 1");
                // Dans ce cas on met un 1
                value -= power;
                stream->limite -= 1;
                stream->data += 1 << stream->limite;
                nb_bits -= 1;
            }
            else
            {
                //printf("Cas 0");
                // Dans ce cas on met un 0 (valeur par défaut)
                stream->limite -= 1;
                nb_bits -= 1;
            }
        }
    }
    //printf("En sortie on a data = %i\n", stream->data);
    //printf("En sortie on a limite = %i\n", stream->limite);
    // Deux possibilités à ce stade : soit on a rempli le buffer et il faut le décharger (flush)
    // soit on a fini d'écrire dans le flux et on attend le suivant (fin de la fonction)

    if (stream->limite == 0)
    // Ici on va décharger car le buffer est plein
    {
        //printf("On va écrire\n");
        if (stream->data == 255 && is_marker == false)
        // Si on s'apprête à décharger ff et que ce n'est pas un marqueur,
        // il faut mettre le 00 (byte stuffing)
        {

            bitstream_flush(stream);
            bitstream_write_bits(stream, 0, 8, false);
        }
        else
        {
            bitstream_flush(stream);
        }
        // On finit alors ce que l'on devait écrire dans le buffer
        bitstream_write_bits(stream, value, nb_bits, false);
    }
}



// Détruit le bitstream passé en paramètre, en libérant la mémoire qui lui est associée. 
void bitstream_destroy(struct bitstream *stream)
{
    // On ferme le fichier
    fclose(stream->fichier);

    // On libère la mémoire
    free(stream);
}


int main(void)
{
    //printf("ALLEZ\n");
    struct bitstream* source = bitstream_create("./test/resultat-g8.jpg");
    if (source->fichier == NULL)
    {
        printf("ERREUR DE SAISI DU NOM DU FICHIER\n");
        return EXIT_SUCCESS;
    }

    bitstream_write_bits(source, 255, 8, false);
    bitstream_write_bits(source, 5, 4, false);
    bitstream_write_bits(source, 1, 4, false);
    //bitstream_write_bits(source, 1, 4, false);
    //bitstream_write_bits(source, 1, 4, false);
    //printf("Dans main");
    bitstream_write_bits(source, 1229324289 , 32, false);
    //bitstream_flush(source);
    return EXIT_SUCCESS;
}

