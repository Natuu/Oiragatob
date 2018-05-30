#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <signal.h>
#include <syslog.h>
#include <sys/time.h>
#include <unistd.h>

#include "../headers/oiragatob.h"

//Penser au free pour le malloc
Buffer oiragatob (unsigned char *recu){

  // Variable à modifier en fonction des actions
  int lenRecu;
  int len;

  printf("%d\n", recu[0]);

  switch (recu[0]) {
    case 17: // Position
      lenRecu = 13;
      printf("Pos recus\n");
    break;

    case 64: // Bords
      lenRecu = 33;
      printf("Bords recus\n");
    break;

    case 16: // MAJ cellules
      printf("MAJ recus\n");

      // Jusqu'aux cellules non mortes
      lenRecu = 1 + 2 + 256 * recu[4] * 8 + recu[3] * 8;

      // Nombre de cellules vivantes
      int lenCellules = 0;

      // On parcourre les cellules vivantes
      while (recu[lenRecu] != 0) {
         // On ajoute une cellule vivante
        lenCellules ++;

        // Octets avant le nom (protocole < 5)
        lenRecu += 18;

        // Octets dans le nom
        while (recu[lenRecu - 1] != 0) {
          lenRecu ++;
        }

        lenRecu ++;
      }

      //Liste des cellules
      Cellule cellules[lenCellules];

    break;
  }

  Buffer envoi;

  len = 0;

  // envoi.len = uclen(recu);                          //Traitement de l'attribut taille
  envoi.buf = malloc(len * sizeof(unsigned char));  //Initialisation du pointeur


  envoi.len = len;
  return envoi;

}
