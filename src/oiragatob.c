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

float binToFloat(int x) {
    union {
        int  x;
        float  f;
    } temp;
    temp.x = x;
    return temp.f;
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
  int len = 0;
  int cellMort;
  int lenPackCell;
  Buffer envoi;

  // Position
  if (recu[0] == 17) {

      infos->posX = valeurPaquet(1, 4, recu);
      infos->posY = valeurPaquet(5, 4, recu);
      infos->taille = valeurPaquet(9, 4, recu);

      printf("PosX    : %d\nPosY    : %d\nTaille  : %d\n", infos->posX, infos->posY, infos->taille);
  }

  // Vider cellules
  else if (recu[0] == 18) {

      int i;

      for (i = 0; i < 16; i++) {
        infos->idCellules[i] = 0;
      }

      printf("Plus de cellules controlées\n");
  }

  // Ajout cellule
  else if (recu[0] == 32) {

    int i;

    while (infos->idCellules[i] != 0) i++;

    infos->idCellules[i] = valeurPaquet(1, 4, recu);

    printf("Ajout cellule d'ID : %d\n", infos->idCellules[i]);
  }

  // Bords
  else if (recu[0] == 64) {
      lenRecu = 33;

      // Espace visible
      infos->visibleG = (int)binToFloat(valeurPaquet(1, 8, recu));
      infos->visibleD = (int)binToFloat(valeurPaquet(9, 8, recu));
      infos->visibleH = (int)binToFloat(valeurPaquet(17, 8, recu));
      infos->visibleB = (int)binToFloat(valeurPaquet(25, 8, recu));

      // Carte
      if(infos->carteD < infos->visibleD) infos->carteD = infos->visibleD;
      if(infos->carteB < infos->visibleB) infos->carteB = infos->visibleB;

      printf("Bord gauche  : %d\nBord droit   : %d\nBord haut    : %d\nBord bas     : %d\n", infos->visibleG, infos->visibleD, infos->visibleH, infos->visibleB);
  }


  // MAJ cellules
  else if (recu[0] == 16) {

      cellMort = valeurPaquet(1, 2, recu);

      int nombreCasesX = (infos->visibleD - infos->visibleG) / (infos->taille * 0.7);
      int nombreCasesY = (infos->visibleB - infos->visibleH) / (infos->taille * 0.7);

      int cases[nombreCasesX][nombreCasesY];

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


      printf("Pointer vers : ??, ??\n");
  }


  else printf("Paquet inconnu d'OPCODE %d\n", recu[0]);




  // envoi.len = uclen(recu);                          //Traitement de l'attribut taille
  envoi.buf = malloc(len * sizeof(unsigned char));  //Initialisation du pointeur


  envoi.len = len;
  return envoi;

}
