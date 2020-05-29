#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>


/* Voici le fichier qui s'occupe de lire la ligne de commande */


// On définit une structure qui contient toute les informations nécessaires
struct ligne_cmd {
    int8_t validite;  // 1 si valide, 0 sinon
    char* nom;        // nom du fichier
    int8_t sample;    // version sous echantillonnage 0, 1, 2
    int8_t dtc;       //version de dtc
};


struct ligne_cmd *lecture_ligne_cmd(int argc, char const *argv[])
// Cette unique fonction lit et interprète ce qui est rentré par l'utilisateur
{
    // On alloue la mémoire nécessaire
    struct ligne_cmd* options = malloc(sizeof(struct ligne_cmd));
    printf("ICI");
    // Valeur des attribus par défaut
    options->sample = 0;
    options->dtc = 0;
    options->nom = malloc(13);
    options->nom = "resultat.jpg";
    options->validite = 1;
    
    // On parcourt tous les "mots" entrés sur la ligne de commande
    int ind;
    for (ind = 2; ind < argc; ind++)
    {
        // Si --
        if (argv[ind][0] == 45 && argv[ind][1] == 45)
        {
            int8_t erreur = 1;
            // Si --help
            if (argv[ind][2] == 104 && argv[ind][3] == 101 && argv[ind][4] == 108 && argv[ind][5] == 112)
            {
                erreur = 0;
                printf("\n \n \nBonjour et bienvenue dans l'aide.\n \n Trois options peuvent être utilisée sur la ligne de commande :\n \n");
                printf("\"--help\" que vous venez d'utiliser vous fournit une assitance sur les options disponibles en ligne de commande\n");
                printf("\"--outfile=nom_du_fichier_a_generer\" qui permet de choisir le nom de votre fichier .jpg de sortie\n");
                printf("\"--sample=--sample=h1xv1,h2xv2,h3xv3\" qui permet de choisir les coefficients de sous-échantillonnage\n");
                printf("\"--dct=naive ou --dct=cos\" pour avoir la version dct optimisée avec précalcul des cos ou non\n");
                printf("Toute entrée non standardisée selon ce qui précède renverra l'erreur \"Erreur de lecture ligne de commande\"\n \n \n");

                // On arrête le programme
                options->validite = 0;
            }
            // Si --dct=
            if (argv[ind][2] == 100 && argv[ind][3] == 99 && argv[ind][4] == 116 && argv[ind][5] == 61)
            {
                erreur = 0;
                // --dct=naive
                if (argv[ind][6] == 110 && argv[ind][7] == 97 && argv[ind][8] == 105 && argv[ind][9] == 118 && argv[ind][10] == 101) 
                {
                    options->dtc =0;
                }
                // --dct=cos
                if (argv[ind][6] == 99 && argv[ind][7] == 111 && argv[ind][8] == 115) 
                {
                    options->dtc =1;
                }
            }
            // Si --outfile=
            if (argv[ind][2] == 111 && argv[ind][3] == 117 && argv[ind][4] == 116 && argv[ind][5] == 102 && argv[ind][6] == 105 && argv[ind][7] == 108 && argv[ind][8] == 101 && argv[ind][9] == 61)
            {
                erreur = 0;
                int32_t curseur = 10;
                int32_t longueur_new = 0;
                int limite_depassee = 0;
                // On parcourt le nom jusqu'à trouver NULL
                while (argv[ind][curseur] != 0)
                {
                    longueur_new += 1;
                    curseur +=1;
                    // Si le nom est trop long, on s'arrête
                    if (longueur_new >1000)
                    {
                        printf("Nom trop long ou absence de .jpg\n");
                        options->validite=0;
                        limite_depassee = 1;
                        break;
                    }
                }
                // On change le nom en parametre
                if (limite_depassee == 0)
                {
                    free(options->nom);
                    options->nom = malloc((longueur_new)*sizeof(char));
                    for (curseur = 0; curseur < longueur_new; curseur++)
                    {
                        (options->nom)[curseur] = argv[ind][10+curseur];
                    }
                    //printf("\n %s \n", options->nom);
                    options->validite = 0;
                }
            }
            // Si --sample=
            if (argv[ind][2] == 115 && argv[ind][3] == 97 && argv[ind][4] == 109 && argv[ind][5] == 112 && argv[ind][6] == 108 && argv[ind][7] == 101 && argv[ind][8] == 61)
            {
                erreur = 0;
                // On vérifie les , et les x
                if (argv[ind][10] == 120 && argv[ind][12] == 44 && argv[ind][14] == 120 && argv[ind][16] == 44 && argv[ind][18] == 120)
                {
                    // Cas pas de sous-echantillonage
                    if (argv[ind][9] == argv[ind][13] && argv[ind][13] == argv[ind][17] && argv[ind][11] == argv[ind][15] && argv[ind][11] == argv[ind][19])
                    {
                        options->sample = 0;
                    }

                    // Cas sous-echantillonage horizontal
                    else if ((int) argv[ind][9] - 48 == ((int) argv[ind][13] - 48)*2 && argv[ind][13] == argv[ind][17] && argv[ind][11] == argv[ind][15] && argv[ind][11] == argv[ind][19])
                    {
                        options->sample = 1;
                    }

                    // Cas sous-chantillonage vertical et horizontal
                    else if (argv[ind][9] == argv[ind][11] && argv[ind][9] == 50 && argv[ind][13] == argv[ind][15] && argv[ind][15] == argv[ind][17] && argv[ind][17] == argv[ind][19] && argv[ind][13]== 49)
                    {
                        options->sample = 2;
                    }
                    else
                    {
                        options->validite = 0;
                        printf("Erreur de saisi des valeurs de sample");
                    }

                }
                else
                {
                    printf("Erreur d'écriture de sample, consultez --help\n");
                    options->validite = 0;
                }

            }
            // On a lu -- mais rien apres de correct
            if (erreur == 1)
            {
                options->validite=0;
                printf("Erreur de lecture après --, consultez --help\n");
            }
        }
        // On a pas lu --
        else
        {
            options->validite=0;
            printf("Erreur de lecture, consultez --help\n");
        }
    }

    return options;
}

