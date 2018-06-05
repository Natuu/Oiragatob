#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <signal.h>
#include <syslog.h>
#include <sys/time.h>
#include <unistd.h>
#include <math.h>

#include "../headers/oiragatob.h"

#define DISTANCECOEFF 1
#define DENSITECOEFF  1
#define RESOLUTION    0.5

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
            paquetActuel *= 0x100;
        }
        compteurBoucles ++;
        valeur += paquetActuel;
    }

    return valeur;
}

double binToDouble(int64_t x) {
    union {
        int64_t  x;
        double  f;
    } temp;
    temp.x = x;
    return temp.f;
}

double valeurPaquetF (int indiceDepart, int longueurPaquet, unsigned char *paquet) {
    int64_t valeur=0;
    int64_t paquetActuel;
    int i;
    int j;
    int compteurBoucles = 0;

    for (i = indiceDepart; i < longueurPaquet + indiceDepart; i++) {
        paquetActuel = paquet[i];
        for (j = 0; j < compteurBoucles; j++) {
            paquetActuel *= 0x100;
        }
        compteurBoucles ++;
        valeur += paquetActuel;
    }

    return binToDouble(valeur);
}

void paquetValeur (int nombreOctets, int valeur, unsigned char *paquet) {
    int i;

    for (i = 0; i < nombreOctets; i++) {
        paquet[i] = valeur % 256;
        valeur /= 256;
    }
}

void assemblerPaquets(unsigned char *paquet1, int longueurPaquet1, unsigned char *paquet2, int longueurPaquet2, unsigned char *paquet) {
    int i;

    for (i = 0; i < longueurPaquet1; i++) {
        paquet[i] = paquet1[i];
    }
    // Cause du seg Fault !!!!!!!!
    for (i = longueurPaquet1; i < longueurPaquet1 + longueurPaquet2; i++) {
        paquet[i] = paquet2[i - longueurPaquet1];
    }
}

void pointerVersPosition (Infos *infos, int nombreZonesX, int nombreZonesY, int **densite) {

    int i;
    int j;
    float distanceX = 0;
    float distanceY = 0;
    float tailleZoneX = infos->taille * RESOLUTION;
    float tailleZoneY = infos->taille * RESOLUTION;
    float posX = infos -> posX;
    float posY = infos -> posY;
    float distance = 1;
    float ratio = 0;
    float bestRatio = 0;

    for (i = 0; i < nombreZonesY; i++) {
        for (j = 0; j < nombreZonesX; j++) {

            distanceX = (tailleZoneX * j + 0.5 * tailleZoneX) - posX;
            distanceY = (tailleZoneY * i + 0.5 * tailleZoneY) - posY;
            distance = sqrt((distanceX * distanceX) + (distanceY * distanceY));

            if (distance == 0) distance = 1;

            ratio = (DENSITECOEFF * (float)densite[i][j]) / (DISTANCECOEFF * distance);

            if (ratio > bestRatio) {
                bestRatio = ratio;
                infos -> sourisX = (int)(tailleZoneX * j + 0.5 * tailleZoneX);
                infos -> sourisY = (int)(tailleZoneY * i + 0.5 * tailleZoneY);

                printf("from %d, %d   to %d, %d\n", infos -> posX, infos -> posY, infos -> sourisX, infos -> sourisY);
                printf("%d\n", densite[i][j]);
            }

        }
    }
}

//Penser au free pour le malloc
Buffer oiragatob (unsigned char *recu, Infos *infos){

  // Variable à modifier en fonction des actions
  int len = 0;
  int cellMort;
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

      // printf("Plus de cellules controlées\n");
  }

  // Ajout cellule
  else if (recu[0] == 32) {

    int i = 0;

    while (infos->idCellules[i] != 0 && i < 16) i++;

    infos->idCellules[i] = valeurPaquet(1, 4, recu);

    // printf("Ajout cellule d'ID : %d\n", infos->idCellules[i]);
  }

  // Bords
  else if (recu[0] == 64) {
      // Espace visible
      infos->visibleG = (int)valeurPaquetF(1, 8, recu);
      infos->visibleH = (int)valeurPaquetF(9, 8, recu);
      infos->visibleD = (int)valeurPaquetF(17, 8, recu);
      infos->visibleB = (int)valeurPaquetF(25, 8, recu);

      // Carte
      if(infos->carteD < infos->visibleD) infos->carteD = infos->visibleD;
      if(infos->carteB < infos->visibleB) infos->carteB = infos->visibleB;

      // printf("Bord gauche  : %d\nBord droit   : %d\nBord haut    : %d\nBord bas     : %d\n", infos->visibleG, infos->visibleD, infos->visibleH, infos->visibleB);
  }


  // MAJ cellules
  else if (recu[0] == 16) {

      int i;
      int j;

      int nombreZonesX;
      int nombreZonesY;

      nombreZonesX = (infos->carteD - infos->carteG) / (infos->taille * RESOLUTION);
      nombreZonesY = (infos->carteB - infos->carteH) / (infos->taille * RESOLUTION);

      nombreZonesX ++;
      nombreZonesY ++;

      int **densite;
      densite = malloc(sizeof(int*)*nombreZonesY);
      for (i = 0; i < nombreZonesY; i++) {
        densite[i] = malloc(sizeof(int)*nombreZonesX);
      }

      for (i = 0; i < nombreZonesY; i++) {
        for (j = 0; j < nombreZonesX; j++) {
          densite[i][j] = 0;
        }
      }

      int zoneX = 0;
      int zoneY = 0;

      cellMort = valeurPaquet(1, 2, recu);

      // for (i = 3; i < cellMort * 8 + 3; i += 8) {
      //     for (j = 0; j < 16; j++) {
      //         if(valeurPaquet(i, 4, recu) == infos->idCellules[j]) {
      //             infos->idCellules[j] = 0;
      //         }
      //     }
      // }

      Cellule cellVivante;

      // Tri du buffer
      int parcourCell = 3 + cellMort * 8;

      while (recu[parcourCell] != 0){
        // printf("recu[%d] = %x\n", parcourCell, recu[parcourCell]);

        cellVivante.id = valeurPaquet(parcourCell, 4, recu);
        cellVivante.x = valeurPaquet((parcourCell + 4), 4, recu);
        cellVivante.y = valeurPaquet((parcourCell + 8), 4, recu);
        cellVivante.taille = valeurPaquet((parcourCell + 12), 2, recu);
        cellVivante.flag = valeurPaquet((parcourCell + 14), 1, recu);
        parcourCell += 18;
        if (cellVivante.flag & 8) {
            while(recu[parcourCell] != 0){
              parcourCell ++;
            }
            parcourCell ++;
        }


        // On selectionne la plus grosse de nos cellules
        for (j = 0; j < 16; j++) {

            // Si nous appartient
            if(cellVivante.id == infos->idCellules[j]) {
                infos -> taille = cellVivante.taille;
                infos -> posX = cellVivante.x;
                infos -> posY = cellVivante.y;
            }
        }

        // Si food
        if ((cellVivante.flag & 1) == 0 && (cellVivante.flag & 8) == 0) {
            zoneX = (cellVivante.x) / (infos->taille * RESOLUTION);
            zoneY = (cellVivante.y) / (infos->taille * RESOLUTION);

            if (!(zoneX >= nombreZonesX) && !(zoneY >= nombreZonesY) && !(zoneX < 0) && !(zoneY < 0))
            {
              densite[zoneY][zoneX] += cellVivante.taille;
            }
        }
        // Si virus
        else if ((cellVivante.flag & 1) == 1 && (cellVivante.flag & 8) == 0) {
            zoneX = (cellVivante.x) / (infos->taille * RESOLUTION);
            zoneY = (cellVivante.y) / (infos->taille * RESOLUTION);

            if (infos -> taille > 1.4 * cellVivante.taille) {
                densite[zoneY][zoneX] += cellVivante.taille;
            }
        }
      }

      pointerVersPosition (infos, nombreZonesX, nombreZonesY, densite);


      // Envoi du paquet
      len = 13;

      unsigned char *idPaquet;
      idPaquet = malloc(sizeof(unsigned char) * 1);
      unsigned char *xPaquet;
      xPaquet = malloc(sizeof(unsigned char) * 4);
      unsigned char *yPaquet;
      yPaquet = malloc(sizeof(unsigned char) * 4);
      unsigned char *uselessPaquet;
      uselessPaquet = malloc(sizeof(unsigned char) * 4);
      paquetValeur(1, 16, idPaquet);
      paquetValeur(4, infos->sourisX, xPaquet);
      paquetValeur(4, infos->sourisY, yPaquet);
      paquetValeur(4, 0, uselessPaquet);

      //Initialisation du pointeur
      envoi.buf = malloc(len * sizeof(unsigned char));

      unsigned char *paquetIntermediaire1;
      paquetIntermediaire1 = malloc(sizeof(unsigned char) * 5);
      assemblerPaquets(idPaquet, 1, xPaquet, 4, paquetIntermediaire1);
      unsigned char *paquetIntermediaire2;
      paquetIntermediaire2 = malloc(sizeof(unsigned char) * 9);
      assemblerPaquets(paquetIntermediaire1, 5, yPaquet, 4, paquetIntermediaire2);
      // Derniers paquets
      assemblerPaquets(paquetIntermediaire2, 9, uselessPaquet, 4, envoi.buf);

      free(densite);
  }

  // Scores
  else if (recu[0] == 49) {
    // Score
  }


  else printf("Paquet inconnu d'OPCODE %d\n", recu[0]);


  envoi.len = len;
  return envoi;

}
