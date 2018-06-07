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

// Fine tuning
#define DISTANCECOEFF  100000
#define DENSITECOEFF   1
#define RESOLUTION     0.4
#define SOLO           0
#define REPULSIONBORDS 50

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

// Convertit une valeur binaire en flottant
double binToDouble(int64_t x) {
    union {
        int64_t  x;
        double  f;
    } temp;
    temp.x = x;
    return temp.f;
}

// Permet de convertir un groupe d'octets en flottant
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

// Code une valeur entière en un paquet de nombreOctets
void paquetValeur (int nombreOctets, int valeur, unsigned char *paquet) {
    int i;

    for (i = 0; i < nombreOctets; i++) {
        paquet[i] = valeur % 256;
        valeur /= 256;
    }
}

// Assemble 2 paquets
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

// Remplis la grille de densité
void hydrater(Cellule cellVivante, Infos *infos, int **densite, int nombreZonesX, int nombreZonesY, int tailleAureole, int food, int ennemis, int virus) {
  int j;
  int zoneX = 0;
  int zoneY = 0;
  float distance = 0;
  float attrait = 0;
  int auBot = 0;

  int k;
  int l;

  // On selectionne la plus petite de nos cellules
  for (j = 0; j < 16; j++) {

      // On update la position et la taille
      if(cellVivante.id == infos->idCellules[j]) {
        auBot = 1;
        // printf("ID: %d Cell : %d  PosX: %d   PosY: %d\n", j, cellVivante.id, cellVivante.x, cellVivante.y);

        if (cellVivante.taille < infos -> plusPetiteTaille) {
          infos -> plusPetiteTaille = cellVivante.taille;
        }
        if (cellVivante.taille > infos -> plusGrosseTaille) {
          infos -> taille = cellVivante.taille;
          infos -> plusGrosseTaille = cellVivante.taille;
          infos -> posX = cellVivante.x;
          infos -> posY = cellVivante.y;
        }
      }
  }

  // Indice de la case centrale du remplissage
  zoneX = (cellVivante.x) / (infos->taille * RESOLUTION);
  zoneY = (cellVivante.y) / (infos->taille * RESOLUTION);

  // Gestion de l'attrait de la cellule

  // Si food
  if ((cellVivante.flag & 1) == 0 && (cellVivante.flag & 8) == 0 && food) {
    attrait = 1;
  }
  // Si virus
  if ((cellVivante.flag & 1) == 1 && (cellVivante.flag & 8) == 0 && virus) {
      if (infos -> plusPetiteTaille > 1.4 * cellVivante.taille) {
          attrait = 1;
      }
  }
  // Si méchant
  if ((cellVivante.flag & 1) == 0 && (cellVivante.flag & 8) == 1 && auBot == 0 && ennemis)
  {
    if (infos -> plusPetiteTaille > 1.4 * cellVivante.taille) {
        attrait = 1;
    }
    else if (infos -> plusGrosseTaille < 1.4 * cellVivante.taille){
      attrait = -10000;
    }
    else {
      attrait = 0;
    }
  }

  // Si Bot
  if (auBot == 1){
    attrait = 0;
  }

  // Auréolage (on rempli la grille avec beaucoup de densité à la position de la cellule et un peu autour)
  for (k = -tailleAureole; k < tailleAureole; k++) {
    for (l =  -tailleAureole; l <  tailleAureole; l++) {

      // On évite le segfault...
      if (zoneX + k < nombreZonesX && zoneY + l < nombreZonesY && zoneX + k >= 0 && zoneY + l >= 0) {

        distance = sqrt(k*k + l*l);

        // Cellule centrale 100 fois plus importante que celles juste autour
        if (distance == 0)   distance = 0.01;

        densite[zoneY + l][zoneX + k] += cellVivante.taille * attrait / distance;
      }
    }
  }
}

// Auréole les bords
void aureoleBords(int **densite, int nombreZonesX, int nombreZonesY, int repulsion, int tailleAureole) {

  int i;
  int j;
  float distance = 0;
  int k;
  int l;

  for (i = 0; i < nombreZonesY; i++) {
    for (j = 0; j < nombreZonesX; j++) {

      if (i == 0 || j == 0 || i == nombreZonesY - 1 || j == nombreZonesX - 1) {
        // Auréolage (on rempli la grille avec beaucoup de densité à la position de la cellule et un peu autour)
        for (k = -tailleAureole; k < tailleAureole; k++) {
          for (l =  -tailleAureole; l <  tailleAureole; l++) {

            // On évite le segfault...
            if (j + k < nombreZonesX && i + l < nombreZonesY && j + k >= 0 && i + l >= 0) {
              distance = sqrt(k*k + l*l);
              // Cellule centrale 100 fois plus importante que celles juste autour
              if (distance == 0)   distance = 0.5;
              densite[i + l][j + k] += - repulsion / distance;
            }
          }
        }
      }
    }
  }
}

// Définit la position de la souris
void pointerVersPosition (Infos *infos, int nombreZonesX, int nombreZonesY, int **densite) {

    int i;
    int j;
    int k;
    float distanceX = 0;
    float distanceY = 0;
    float tailleZoneX = infos->taille * RESOLUTION;
    float tailleZoneY = infos->taille * RESOLUTION;
    float posX = infos -> posX;
    float posY = infos -> posY;
    float distance = 1;
    float ratio = 0;
    float bestRatio = 0;
    infos -> split = 0;

    for (i = 0; i < nombreZonesY; i++) {
        for (j = 0; j < nombreZonesX; j++) {

            distanceX = (tailleZoneX * j + 0.5 * tailleZoneX) - posX;
            distanceY = (tailleZoneY * i + 0.5 * tailleZoneY) - posY;
            distance = sqrt((distanceX * distanceX) + (distanceY * distanceY));

            if (distance == 0) distance = 1;

            ratio = (DENSITECOEFF * (float)densite[i][j]) / (DISTANCECOEFF * distance);

            // On selectionne la meilleure cellule
            if (ratio > bestRatio) {
                bestRatio = ratio;
                infos -> sourisX = (int)(tailleZoneX * j + 0.5 * tailleZoneX);
                infos -> sourisY = (int)(tailleZoneY * i + 0.5 * tailleZoneY);

                if ((densite[i][j] > 100 * infos -> taille / 3 || SOLO) && infos -> taille > 70) {
                  for (k = 0; k < 16; k++) {
                    if (infos -> idCellules[k] == 0) infos -> split = 1;
                  }
                }
            }
        }
    }
}

// Modifie le buffer
void creerPaquetDeplacement(Buffer *envoi, Infos *infos){

  if (infos -> split == 1){
    envoi -> len = 1;
    envoi -> buf = malloc(envoi -> len * sizeof(unsigned char));
    paquetValeur(1, 17, envoi -> buf);

  } else {
    // Envoi du paquet
    envoi -> len = 13;

    // Envoi du paquet
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
    envoi -> buf = malloc(envoi -> len * sizeof(unsigned char));

    unsigned char *paquetIntermediaire1;
    paquetIntermediaire1 = malloc(sizeof(unsigned char) * 5);
    assemblerPaquets(idPaquet, 1, xPaquet, 4, paquetIntermediaire1);
    unsigned char *paquetIntermediaire2;
    paquetIntermediaire2 = malloc(sizeof(unsigned char) * 9);
    assemblerPaquets(paquetIntermediaire1, 5, yPaquet, 4, paquetIntermediaire2);
    // Derniers paquets
    assemblerPaquets(paquetIntermediaire2, 9, uselessPaquet, 4, envoi -> buf);

    free(idPaquet);
    free(xPaquet);
    free(yPaquet);
    free(uselessPaquet);
    free(paquetIntermediaire1);
    free(paquetIntermediaire2);
  }
}


// Fonction principale
void oiragatob (unsigned char *recu, Buffer *envoi, Infos *infos){

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
    int aAjouter = valeurPaquet(1, 4, recu);

    while (infos->idCellules[i] != 0 && i < 16 && infos->idCellules[i] != aAjouter) i++;

    if (infos->idCellules[i] == 0) {
      infos->idCellules[i] = aAjouter;
    }

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

      int cellMort;
      int i;
      int j;

      // Mappage
      int nombreZonesX;
      int nombreZonesY;

      infos -> plusGrosseTaille = 0;

      nombreZonesX = (infos->carteD - infos->carteG) / (infos->taille * RESOLUTION);
      nombreZonesY = (infos->carteB - infos->carteH) / (infos->taille * RESOLUTION);

      // On pense aux zones coupées
      nombreZonesX ++;
      nombreZonesY ++;

      // Allocation de la mémoire de la densité
      int **densite;
      densite = malloc(sizeof(int*)*nombreZonesY);
      for (i = 0; i < nombreZonesY; i++) {
        densite[i] = malloc(sizeof(int)*nombreZonesX);
      }

      // Mise à 0 de la densité
      for (i = 0; i < nombreZonesY; i++) {
        for (j = 0; j < nombreZonesX; j++) {
          densite[i][j] = 0;
        }
      }

      // Nombre de cellules mortes
      cellMort = valeurPaquet(1, 2, recu);

      //Retirer nos cellules mortes
      for (i = 3; i < cellMort * 8 + 3; i += 8) {
          for (j = 0; j < 16; j++) {
              if(valeurPaquet(i+4, 4, recu) == infos->idCellules[j]) {
                  infos->idCellules[j] = 0;
              }
          }
      }

      Cellule cellVivante;

      // Tri du buffer
      int parcourCell = 3 + cellMort * 8;

      // Traitement des cellules une à une
      while (recu[parcourCell] != 0){

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

        // Remplissage de la grille de densité
        // On hydrate une fois sans virus ni ennemis et une fois avec
        hydrater(cellVivante, infos, densite, nombreZonesX, nombreZonesY, 10, 1, !SOLO, 30);
      }

      aureoleBords(densite, nombreZonesX, nombreZonesY, REPULSIONBORDS, 5);

      //
      pointerVersPosition (infos, nombreZonesX, nombreZonesY, densite);

      for (i = 0; i < nombreZonesY; i++) {
        free(densite[i]);
      }
      free(densite);

      creerPaquetDeplacement(envoi, infos);

  }

  // Scores
  else if (recu[0] == 49) {
    // Score
  }


  else printf("Paquet inconnu d'OPCODE %d\n", recu[0]);

}
