#include <stdlib.h>
#include <stdio.h>





//
// /*------- l'entête-----------*/
// // On peut lire et écrire dans le fichier
// // Boucle de lecture des caractères un à un
// do
// {
//     caractereActuel = fgetc(fichier); // On lit le caractère
//     printf("%c\n", caractereActuel); // On l'affiche
//     compteur++;
// } while (caractereActuel != EOF); // On continue tant que fgetc n'a pas retourné EOF (fin de fichier)
//
// /*---------- les pixels----------*/
// do
// {
//     caractereActuel = fgetc(fichier); // On lit le caractère
//     printf("%c\n", caractereActuel); // On l'affiche
//     compteur++;
// } while (caractereActuel != EOF); // On continue tant que fgetc n'a pas retourné EOF (fin de fichier)
// printf("%d\n", compteur);
// fclose(fichier);

int32_t **recuper_data(const char* file_pgm)
{
  FILE* fichier = fopen(file_pgm, "r+");
  int32_t** data = malloc(8*sizeof(int32_t*));
  for (size_t i = 0; i < 8; i++) {
    data[i] = malloc(8*sizeof(int32_t));
  }
  fseek(fichier, 11, SEEK_SET);
  for (size_t y = 0; y < 8; y++) {
    for (size_t x = 0; x < 8; x++) {
      data[y][x] = fgetc(fichier);
    }
  }
  fclose(fichier);
  return data;
}


int32_t *recuper_entete(const char* file_pgm)
{
  FILE* fichier = fopen(file_pgm, "r+");
  int32_t *vect = malloc(11*sizeof(int32_t));
  int compteur = 0;
  while (compteur < 11) {
    *(vect + compteur) = fgetc(fichier);
    compteur++;
  }
  fclose(fichier);
  return vect;
}





int main(int argc, char const *argv[])
{


  int32_t* entete = NULL; // entête du fichier pgm
  int32_t** data = NULL;   // le contenu, les pixels
  entete = recuper_entete(argv[1]);
  data = recuper_data(argv[1]);
  // test de la recuperation de data
  // for (size_t y = 0; y < 8; y++) {
  //   for (size_t x = 0; x < 8; x++) {
  //     printf("%d\n", data[x][y]);
  //     printf("%ld\n", 8*y+x);
  //   }
  // }

  return 0;
}
