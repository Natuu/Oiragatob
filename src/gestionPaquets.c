#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <signal.h>
#include <syslog.h>
#include <sys/time.h>
#include <unistd.h>

#include "../headers/gestionPaquets.h"

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
