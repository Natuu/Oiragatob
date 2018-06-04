#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <signal.h>
#include <syslog.h>
#include <sys/time.h>
#include <unistd.h>

#include "../headers/oiragatob.h"

// Permet de convertir un groupe d'octets en int
int valeurPaquet (int indiceDepart, int longueurPaquet, unsigned char *paquet) {
    int valeur=0;
    int paquetActuel;
    int i;
    int j;
    int compteurBoucles = 0;

    for (i = indiceDepart; i < longueurPaquet + indiceDepart; i++) {
        paquetActuel = paquet[i];
        for (j = 0; j < compteurBoucles; j++) {
            paquetActuel *= 256;
        }
        compteurBoucles ++;
        valeur += paquetActuel;
    }

    return valeur;
}

void paquetValeur (int nombreOctets, int valeur, unsigned char *paquet) {
    int i;

    paquet[0] = valeur % 256;

    for (i = 1; i < nombreOctets; i++) {
        paquet[i] = valeur % 256;
        valeur /= 256;
    }
}


void assemblerPaquets(unsigned char *paquet1, int longueurPaquet1, unsigned char *paquet2, int longueurPaquet2, unsigned char *paquet) {
    int i;

    for (i = 0; i < longueurPaquet1; i++) {
        paquet[i] = paquet1[i];
    }

    for (i = longueurPaquet1; i < longueurPaquet1 + longueurPaquet2; i++) {
        paquet[i] = paquet2[i];
    }
}


//Penser au free pour le malloc
Buffer oiragatob (unsigned char *recu, Infos *infos){

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

      infos->posX = valeurPaquet(1, 4, recu);
      infos->posY = valeurPaquet(5, 4, recu);
      infos->taille = valeurPaquet(9, 4, recu);

    break;

    case 64: // Bords
      lenRecu = 33;
      printf("Bords recus\n");

      // Espace visible
      infos->visibleG = valeurPaquet(1, 8, recu);
      infos->visibleD = valeurPaquet(9, 8, recu);
      infos->visibleH = valeurPaquet(17, 8, recu);
      infos->visibleB = valeurPaquet(25, 8, recu);

      // Carte
      if(infos->carteD < infos->visibleD) infos->carteD = infos->visibleD;
      if(infos->carteB < infos->visibleB) infos->carteB = infos->visibleB;

    break;

    case 16: // MAJ cellules
      printf("MAJ recus\n");

      cellMort = valeurPaquet(1, 2, recu);  // ou l'inverse ?

      Cellule *listeCellVivantes;
      Cellule *cellVivante;

      cellVivante = malloc(1 * sizeof(Cellule));
      listeCellVivantes = cellVivante;
      // Tri du buffer
      int parcourCell = 3 + cellMort * 8;

      while (recu[parcourCell] + recu[parcourCell + 1] + recu[parcourCell + 2] + recu[parcourCell + 3] != 0){

        cellVivante -> id = valeurPaquet(parcourCell, 4, recu);
        cellVivante -> x = valeurPaquet((parcourCell + 4), 4, recu);
        cellVivante -> y = valeurPaquet((parcourCell + 8), 4, recu);
        cellVivante -> size = valeurPaquet((parcourCell + 12), 2, recu);
        cellVivante -> flag = valeurPaquet((parcourCell + 14), 1, recu);
        parcourCell += 18;
        while(recu[parcourCell] != 0){
          parcourCell ++;
        }

        parcourCell ++;

        cellVivante -> prochaine = malloc(1 * sizeof(Cellule));
        cellVivante = cellVivante -> prochaine;

      }




    break;

    default:
      printf("Paquet inconnu %d", recu[0]);

  }



  Buffer envoi;

  len = 0;

  // envoi.len = uclen(recu);                          //Traitement de l'attribut taille
  envoi.buf = malloc(len * sizeof(unsigned char));  //Initialisation du pointeur


  envoi.len = len;
  return envoi;

}
