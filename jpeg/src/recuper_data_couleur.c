#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

//
typedef struct Entete Entete;

// structure entete
struct Entete {
    int32_t nbre_octet;  // le nombre d'octets dans un entête
    int32_t type;        // ppm ou pgm (si type = 5 donc pgm sinon si type = 6 donc ppm)
    int32_t longueur;    // longuer de l'image
    int32_t largeur;     // largeur de l'image
};


// récupération des pixel des fichiers pgm dans une matrice de taille "longueurXlargeur"
int32_t **recuper_data_gris(const char* file_pgm,uint32_t largeur,uint32_t longueur)
{
  FILE* fichier = fopen(file_pgm, "r+");
  int32_t** data = malloc(longueur*sizeof(int32_t*));
  for (size_t i = 0; i < longueur; i++) {
    data[i] = malloc(largeur*sizeof(int32_t));
  }
  fseek(fichier, 11, SEEK_SET);
  for (size_t y = 0; y < longueur; y++) {
    for (size_t x = 0; x < largeur; x++) {
      data[y][x] = fgetc(fichier);
    }
  }
  fclose(fichier);
  return data;
}

// récupération d'un tableau de trois matrices des couleurs de taille "longueurXlargeur"
int32_t ***recuper_data_couleur(const char* file_ppm,uint32_t largeur,uint32_t longueur)
{
  FILE* fichier = fopen(file_ppm, "r+");
  int32_t*** data = malloc(3*sizeof(int32_t**)); // 3 pointeurs vers les trois matrices des différentes couleurs
  for (size_t i = 0; i < 3; i++) {
    data[i] = malloc(longueur*sizeof(int32_t*));
    for (size_t j = 0; j < longueur; j++) {
      data[i][j] = malloc(largeur*sizeof(int32_t));
    }
  }


  fseek(fichier, 11, SEEK_SET);// on enleve l'entete 11 octets
  // on parcours les données
  for (size_t y = 0; y < longueur; y++) {
    for (size_t x = 0; x < largeur; x++) {
      for (size_t i = 0; i < 3; i++) {
        data[i][y][x] = fgetc(fichier);
      }
    }
  }
  fclose(fichier);
  return data;
}

// nombre de chiffre dans un nombre pour savoir le nombre d'octets dans l'entête
int nombre_chiffre(int32_t nombre) {
    int s = 0, i = 1;
    if (nombre != 0) {
        s = (int)nombre / 10;
        while (s != 0) {
            s = (int)(s / 10);
            i = i + 1;
        }
    }
    return i;
}

// récupération de l'entête des deux fichiers pgm ou ppm
Entete* recuper_entete(const char* file_pgm) {
    FILE* fichier = fopen(file_pgm, "r+");
    Entete* entete = malloc(sizeof(*entete));
    int32_t longueur = 0;
    int32_t largeur = 0;

    fseek(fichier, 1, SEEK_SET);
    entete->type = fgetc(fichier);
    fseek(fichier, 3, SEEK_SET);

    longueur = fgetc(fichier) - 48;
    while (fgetc(fichier) != 32) {
        fseek(fichier, -1, SEEK_CUR);
        longueur = 10 * longueur + fgetc(fichier) - 48;
    }
    entete->longueur = longueur;
    largeur = fgetc(fichier) - 48;
    while (fgetc(fichier) != 10) {
        fseek(fichier, -1, SEEK_CUR);
        largeur = 10 * largeur + fgetc(fichier) - 48;
    }
    entete->largeur = largeur;
    fclose(fichier);
    entete->nbre_octet = 9 + nombre_chiffre(longueur) + nombre_chiffre(largeur);
    return entete;
}

int main(int argc, char const* argv[]) {
    (void) argc;
    Entete* entete = NULL;  // entête du fichier pgm
    int32_t** data_gris = NULL;   // la matrice des pixels gris : double pointeurs
    int32_t*** data_couleurs = NULL; // tableau de trois matrices des trois couleurs : triple pointeurs +
    entete = recuper_entete(argv[1]);

    // test de l'entete

    // printf("%d\n", entete->type);
    // printf("%d\n", entete->longueur);
    // printf("%d\n", entete->largeur);
    // printf("%d\n", entete->nbre_octet);

    if (entete->type = 53 /* on est dans le gris le fichier est pgm*/) {
      data_gris = recuper_data_gris(argv[1], entete->largeur, entete->longueur);
    } else if (entete->type = 54 /* on est dans le cas des couleurs, le fichier est ppm*/) {
      data_couleurs = recuper_data_couleur(argv[1], entete->largeur, entete->longueur);
    }

    return 0;
}
