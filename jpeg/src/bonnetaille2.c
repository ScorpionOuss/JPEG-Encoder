
int32_t ***bonne_taille_couleur_ssechantillonnage(int32_t ***data, int32_t* entete, int8_t echantillonage)
//Transforme l'image de sorte que ses dimensions soient un multiple de 8
{
    int8_t na = 8;
    int8_t no = 8;
    
    // On met les bons parametres de dimensions des MCU, 8 ou 16
    
    if (echantillonage == 1){
        na = 16;
    }
    else if (echantillonage == 2){
        na = 16;
        no = 16;
    }

    // On calcule les bonnes dimensions
    int bonne_abscisse = *entete;
    if (*entete % na != 0)
    {
    bonne_abscisse += na - *entete % na;
    }
    
    int bonne_ordonnee = *(entete+1);
    if (*(entete+1) % no != 0)
    {
    bonne_ordonnee += (no - *(entete+1) % no);
    }
        
    // On créé un nouveau tableau data aux bonnes dimensions
    int32_t*** data_new = malloc(bonne_ordonnee*sizeof(int32_t*));
    for (size_t i = 0; i < bonne_ordonnee; i++) {
      
      data_new[i] = malloc(bonne_abscisse*sizeof(int32_t*));
      for (size_t j = 0; j < bonne_abscisse; j++)
      {
        data_new[i][j] = malloc(3*sizeof(int32_t));
      }
    }
    
    // On le remplit avec les anciennes valeurs
    for (size_t y = 0; y < *(entete+1); y++) {
      for (size_t x = 0; x < *(entete); x++) {
          for (size_t z = 0; z<3; z++)
          {
          data_new[y][x][z] = data[y][x][z];
          }
      }
    }
        
    // On met les nouvelles valeurs
    // En bas d'abord
     for (size_t y = *(entete+1); y < bonne_ordonnee; y++) {
        for (size_t x = 0; x < *entete; x++) {
          for (size_t z = 0; z<3; z++)
          {
            data_new[y][x][z] = data[*(entete+1)-1][x][z];
          }
        }
    }
    // Puis à droite
     for (size_t y = 0; y < bonne_ordonnee; y++) {
        for (size_t x = *entete; x < bonne_abscisse; x++) {
          for (size_t z = 0; z<3; z++)
          {
            data_new[y][x][z] = data[*(entete+1)-1][*entete -1][z];
          }
        }
    }

    
    return data_new;

}


