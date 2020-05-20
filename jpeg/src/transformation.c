// Creation d'un tableau de tableau contenant 3 matrices Y, Cb, Cr
int32_t ***recuper_data_couleur(int32_t*** data, int32_t longueur, int32_t largeur)
{
  int32_t*** data_nouv = malloc(longueur*sizeof(int32_t**)); 
  for (size_t i = 0; i < longueur; i++) {
    data_nouv[i] = malloc(largeur*sizeof(int32_t*));
    for (size_t j = 0; j < largeur; j++) {
      data_nouv[i][j] = malloc(3*sizeof(int32_t));
    }
  }

  // On définit trois pointeurs pour les calculs, un sur chaque composante RGB
  
  int32_t R;
  int32_t G;
  int32_t B;

  for (size_t lon = 0; lon < longueur; lon++) {
    for (size_t lar = 0; lar < largeur; lar++) {
        R = data[lon][lar][0];
        G = data[lon][lar][1];
        B = data[lon][lar][2];

        data_nouv[lon][lar][0] = (int) 0.299 * (float) R + 0.587 * (float) G + 0.114 * (float) B;
        data_nouv[lon][lar][1] = (int) -0.1687 * (float) R - 0.3313 * (float) G + 0.5 * (float) B + 128;
        data_nouv[lon][lar][2] = (int) 0.5 * (float) R - 0.4187* (float) G - 0.0813 * (float) B + 128;

    }
  }

  return data_nouv;

  

