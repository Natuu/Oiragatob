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
Buffer oiragatob (unsigned char *recu, Map *mapVisible){

  // Variable à modifier en fonction des actions
  int lenRecu;
  int len;
  int cellMort;
  int lenPackCell;

  printf("%d\n", recu[0]);

  switch (recu[0]) {
    case 17: // Position
      lenRecu = 13;
      printf("Pos recus\n");
    break;

    case 64: // Bords
      lenRecu = 33;
      printf("Bords recus\n");

      mapVisible->left = valeurPaquet(1, 8, recu);
      mapVisible->right = valeurPaquet(9, 8, recu);
      mapVisible->top = valeurPaquet(17, 8, recu);
      mapVisible->bottom = valeurPaquet(25, 8, recu);

    break;

    case 16: // MAJ cellules
      printf("MAJ recus\n");

      cellMort = valeurPaquet(1, 2, recu);  // ou l'inverse ?

      Cellule cellVivante;

      // Tri du buffer
      int parcourCell = 3 + cellMort * 8;

      while (recu[parcourCell] + recu[parcourCell + 1] + recu[parcourCell + 2] + recu[parcourCell + 3] != 0){
        cellVivante.id = valeurPaquet(parcourCell, 4, recu);
        cellVivante.x = valeurPaquet((parcourCell + 4), 4, recu);
        cellVivante.y = valeurPaquet((parcourCell + 8), 4, recu);
        cellVivante.size = valeurPaquet((parcourCell + 12), 2, recu);
        cellVivante.flag = valeurPaquet((parcourCell + 14), 1, recu);
        parcourCell += 18;
        while(recu[parcourCell] != 0){
          parcourCell ++;
        }
        parcourCell ++;

      }





    break;

    default:
      printf("Paquet inconnu %d", rbuf[0]);

  }

  Buffer envoi;

  len = 0;

  // envoi.len = uclen(recu);                          //Traitement de l'attribut taille
  envoi.buf = malloc(len * sizeof(unsigned char));  //Initialisation du pointeur


  envoi.len = len;
  return envoi;

}
