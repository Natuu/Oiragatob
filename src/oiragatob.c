#include "../headers/oiragatob.h"


//Penser au free pour le malloc
Buffer oiragatob (unsigned char *recu){
  int i;

  // Variable à modifier en fonction des actions
  int len;

  Buffer envoi;
  // envoi.len = uclen(recu);                          //Traitement de l'attribut taille
  envoi.buf = malloc(len * sizeof(unsigned char));  //Initialisation du pointeur
  envoi.len = len;
  return envoi;

}
