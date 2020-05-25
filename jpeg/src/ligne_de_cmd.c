#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
struct ligne_cmd {
    int8_t validite; // 1 si valide, 0 sinon
    char* nom;        // nom du fichier
    int8_t sample;    // version sous echantillonnage 0, 1, 2
    int8_t dtc; //version de dtc
};

struct ligne_cmd *lecture_ligne_cmd(int argc, char const *argv[])
{
    struct ligne_cmd* options = malloc(sizeof(struct ligne_cmd*));
    int i;

    // Valeur des attribus par défaut
    options->sample = 0;
    options->dtc = 0;
    options->nom = NULL;
    options->validite = 1;

    for (i = 2; i < argc; i++)
    {
        if (argv[i][0] == 45 && argv[i][1] == 45)
        {
            int8_t erreur = 1;
            if (argv[i][2] == 104 && argv[i][3] == 101 && argv[i][4] == 108 && argv[i][5] == 112)
            {
                erreur = 0;
                printf("\n \n \nBonjour et bienvenue dans l'aide.\n \n Trois options peuvent être utilisée sur la ligne de commande :\n \n");
                printf("\"--help\" que vous venez d'utiliser vous fournit une assitance sur les options disponibles en ligne de commande\n");
                printf("\"--outfile=nom_du_fichier_a_generer\" qui permet de choisir le nom de votre fichier .jpg de sortie\n");
                printf("\"--sample=--sample=h1xv1,h2xv2,h3xv3\" qui permet de choisir les coefficients de sous-échantillonnage\n");
                printf("\"--dct=naive ou --dct=cos\" pour avoir la version dct optimisée avec précalcul des cos ou non\n");
                printf("Toute entrée non standardisée selon ce qui précède renverra l'erreur \"Erreur de lecture ligne de commande\"\n \n \n");
            }
            if (argv[i][2] == 100 && argv[i][3] == 99 && argv[i][4] == 116 && argv[i][5] == 61)
            {
                erreur = 0;
                if (argv[i][6] == 110 && argv[i][7] == 97 && argv[i][8] == 105 && argv[i][9] == 118 && argv[i][10] == 101) 
                {
                options->dtc =0;
                printf("Valeur de dtc changée : %i\n",options->dtc);
                }
                if (argv[i][6] == 99 && argv[i][7] == 111 && argv[i][8] == 115) 
                {
                options->dtc =1;
                printf("Valeur de dtc changée : %i\n",options->dtc);
                }
            }
            
            if (argv[i][2] == 111 && argv[i][3] == 117 && argv[i][4] == 116 && argv[i][5] == 102 && argv[i][6] == 105 && argv[i][7] == 108 && argv[i][8] == 101 && argv[i][9] == 61)
            {
                erreur = 0;
                printf("On est dans outfile \n");
                int32_t curseur = 10;
                int32_t longueur = 0;
                int32_t limite = 0;
                while (argv[i][curseur] != 46)
                {
                    longueur += 1;
                    limite += 1;
                    curseur +=1;
                    if (limite >1000)
                    {
                        printf("Nom trop long ou absence de .jpg");
                        options->validite=0;
                        break;
                    }
                }
                printf("La longueur vaut %i", longueur);
                char* nouveau_nom = malloc((longueur)*sizeof(char));
                for (curseur = 0; curseur < longueur; curseur++)
                {
                    nouveau_nom[curseur] = argv[i][10+curseur];
                }
                options->nom = nouveau_nom;
                printf("\n %s \n", options->nom);
            }
            if (argv[i][2] == 115 && argv[i][3] == 97 && argv[i][4] == 109 && argv[i][5] == 112 && argv[i][6] == 108 && argv[i][7] == 101 && argv[i][8] == 61)
            {
                erreur = 0;
                if (argv[i][10] == 120 && argv[i][12] == 44 && argv[i][14] == 120 && argv[i][16] == 44 && argv[i][18] == 120)
                {

                    if (argv[i][9] == argv[i][13] && argv[i][13] == argv[i][17] && argv[i][11] == argv[i][15] && argv[i][11] == argv[i][19])
                    {
                        options->sample = 0;
                        printf("Valeur de sample changée : %i",options->sample);
                    }
                    else if ((int) argv[i][9] - 48 == ((int) argv[i][13] - 48)*2 && argv[i][13] == argv[i][17] && argv[i][11] == argv[i][15] && argv[i][11] == argv[i][19])
                    {
                        options->sample = 1;
                        printf("Valeur de sample changée : %i",options->sample);
                    }

                    else if (argv[i][9] == argv[i][11] && argv[i][9] == 50 && argv[i][13] == argv[i][15] && argv[i][15] == argv[i][17] && argv[i][17] == argv[i][19] && argv[i][13]== 49)
                    {
                        options->sample = 2;
                        printf("Valeur de sample changée : %i",options->sample);
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
                }

            }
            if (erreur == 1)
            {
                options->validite=0;
                printf("Erreur de lecture après --, consultez --help\n");
            }
        }
        else
        {
            options->validite=0;
            printf("Erreur de lecture, consultez --help\n");
        }
    }
    return options;
}


int main(int argc, char const *argv[])
{
    lecture_ligne_cmd(argc, argv);
    return EXIT_SUCCESS;
}

// Dans le main rajouter struct ligne_cmd* options = lecture_ligne_cmd(argc, argv);
// On a le nom du fichier a creer via options->nom
// Le type de sample via options->sample
// La valitidé de la lecture via options->validite
