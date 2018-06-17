#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <signal.h>
#include <syslog.h>
#include <sys/time.h>
#include <unistd.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_ttf.h>

#include "../headers/oiragatob.h"
#include "../headers/sdlFonctions.h"
#include "../headers/client.h"


// Donne la position dans la fenetre d'affichage
void posInWindow(int *x, int *y, Infos *infos){

	int *windowHauteur = malloc(sizeof(int));
	int *windowLargeur = malloc(sizeof(int));

	SDL_GetWindowSize(window, windowLargeur, windowHauteur);
	*x = ((float)(*x - infos -> visibleG)/(float)(infos -> visibleD - infos -> visibleG))  * *windowLargeur;
	*y = ((float)(*y - infos -> visibleH)/(float)(infos -> visibleB - infos -> visibleH))  * *windowHauteur;

	free(windowHauteur);
	free(windowLargeur);
}

// Donne la taille dans la fenetre d'affichage
int tailleInWindow(int taille, Infos *infos) {
	int *windowHauteur = malloc(sizeof(int));
	int *windowLargeur = malloc(sizeof(int));
	SDL_GetWindowSize(window, windowLargeur, windowHauteur);
	taille = ((float)taille/(float)(infos -> visibleD - infos -> visibleG))  * *windowLargeur;

	free(windowHauteur);
	free(windowLargeur);

	return taille;
}

// Crée les curseurs de settings
void creerCurseur(int x, int y, float curseurX, char *toWrite) {

	SDL_Rect rect = {x, y, 260, 23};

	SDL_SetRenderDrawColor(settingsRenderer, 150, 150, 150, 255);
	SDL_RenderFillRect(settingsRenderer, &rect);
	filledCircleColor(settingsRenderer, x, y + 11, 11, 0xFF66E600);
	filledCircleColor(settingsRenderer, x + 259, y + 11, 11, 0xFF969696);

	SDL_Rect prog = {x, y, 260 * curseurX, 23};
	SDL_SetRenderDrawColor(settingsRenderer, 0, 0xE6, 0x66, 255);
	SDL_RenderFillRect(settingsRenderer, &prog);

	filledCircleColor(settingsRenderer, x + 260 * curseurX, y + 11, 14, 0xFFEEEEEE);

	SDL_Color color;
	color.r = 245;
	color.g = 245;
	color.b = 245;
	color.a = 255;
	SDL_Surface* textSurface = TTF_RenderUTF8_Blended(pfont, toWrite, color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(settingsRenderer, textSurface);

	SDL_Rect rekt;
	rekt.x = x - 10;
	rekt.y = y - 25;
	rekt.w = strlen(toWrite) * 6;
	rekt.h = 20;

	SDL_RenderCopy(settingsRenderer, texture, NULL, &rekt);

	SDL_FreeSurface(textSurface);
	SDL_DestroyTexture(texture);
}

// Réccupère la valeur du curseur en cas de clic dse la souris
void getCurseurValeur(Infos *infos, int x, int y) {

	x -= 20;

	// On actualise la valeur en fonction des parametres

	// distanceCoeff
	if (y > 37 && y < 57) {
		if (x > 0 && x < 260) {
			infos -> distanceCoeff = (x / 260.0) * 2000;
		}
	}
	// ratioSplitMulti
	else if (y > 37 + 57 * 1 && y < 57 + 57 * 1) {
		if (x > 0 && x < 260) {
			infos -> ratioSplitMulti = (x / 260.0) * 2;
		}
	}
	// aureolage
	else if (y > 37 + 57 * 2 && y < 57 + 57 * 2) {
		if (x > 0 && x < 260) {
			infos -> aureolage = (x / 260.0) * 50;
		}
	}
	// repulsionBords
	else if (y > 37 + 57 * 3 && y < 57 + 57 * 3) {
		if (x > 0 && x < 260) {
			infos -> repulsionBords = (x / 260.0) * 100;
		}
	}
	// intensiteAureole
	else if (y > 37 + 57 * 4 && y < 57 + 57 * 4) {
		if (x > 0 && x < 260) {
			infos -> intensiteAureole = (x / 260.0) * 0.5;
		}
	}
	// nombreSplit
	else if (y > 37 + 57 * 5 && y < 57 + 57 * 5) {
		if (x > 0 && x < 260) {
			infos -> nombreSplit = (x / 260.0) * 16;
		}
	}
	// gentils
	else if (y > 37 + 57 * 6 && y < 57 + 57 * 6) {
		if (x > 0 && x < 260) {
			infos -> gentils = (x / 260.0) * 50;
		}
	}
	// mechants
	else if (y > 37 + 57 * 7 && y < 57 + 57 * 7) {
		if (x > 0 && x < 260) {
			infos -> mechants = (x / 260.0) * 10000;
		}
	}

}

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
    int i;
    int j;
    int zoneX = 0;
    int zoneY = 0;
    int nbCases = 0;
    float distance = 0;
    float attrait = 0;
    int k;
    int l;
	int *x = malloc(sizeof(int));
	int *y = malloc(sizeof(int));
	*x = cellVivante.x;
	*y = cellVivante.y;
	posInWindow(x, y, infos);
	int color = 0;

    // Indice de la case centrale du remplissage
    zoneX = (cellVivante.x) / (infos->plusPetiteTaille * infos -> resolution);
    zoneY = (cellVivante.y) / (infos->plusPetiteTaille * infos -> resolution);

    // Gestion de l'attrait de la cellule

    // Si food
    if ((cellVivante.flag & 1) == 0 && (cellVivante.flag & 8) == 0 && food) {
        attrait = 1;

		// A l'envers litlle indian
		color = 0xFF0694F8;
    }
    // Si virus mais ignorer les vius
    else if ((cellVivante.flag & 1) == 1 && (cellVivante.flag & 8) == 0 && !virus) {
        if (infos -> taille > 1.4 * cellVivante.taille) {
            attrait = -1;
        }
		// A l'envers litlle indian
		color = 0xFF40E600;
    }
    // Si virus
    else if ((cellVivante.flag & 1) == 1 && (cellVivante.flag & 8) == 0 && virus) {
        if (infos -> taille > 1.4 * cellVivante.taille) {
            attrait = 1;
        }
		// A l'envers litlle indian
		color = 0xFF40E600;
    }
    // Si méchant
    else if (ennemis)
    {
        if (infos -> plusPetiteTaille > 1.4 * cellVivante.taille) {
            attrait = infos -> gentils;
			// A l'envers litlle indian
			color = 0xFF18CAF7;
        }
        else {
            // Nombre de cases occupées par la cellule
            nbCases = cellVivante.taille / ((infos->plusPetiteTaille * infos -> resolution) * 2) - 1;
            attrait = -infos ->  mechants;
			// A l'envers litlle indian
			color = 0xFF2B39C0;
        }
    }

    for (i = 0; i < 30; i++) {
        // Si Bot, attrait nul
        if (infos -> cellules[i].id == cellVivante.id)
        {
            attrait = 0;
            infos -> cellules[i].x = cellVivante.x;
            infos -> cellules[i].y = cellVivante.y;
            infos -> cellules[i].taille = cellVivante.taille;
			// A l'envers litlle indian
			color = 0xFFFEB519;
        }
    }

	// On affiche la cellule
	filledCircleColor(renderer, *x, *y, tailleInWindow(cellVivante.taille, infos), color);

	free(x);
	free(y);

    // Pour chaque zones qu'occupe la cellule
    for (i = -nbCases; i <= nbCases; i++) {
        for (j = -nbCases; j <= nbCases; j++) {

            // Auréolage (on rempli la grille avec beaucoup de densité à la position de la cellule et un peu autour)
            for (k = -tailleAureole; k < tailleAureole; k++) {
                for (l =  -tailleAureole; l <  tailleAureole; l++) {

                    // On évite le segfault...
                    if (zoneX + i + k < nombreZonesX && zoneY + j + l < nombreZonesY && zoneX + i + k >= 0 && zoneY + j + l >= 0) {

                        distance = sqrt(k*k + l*l);

                        // Cellule centrale 100 fois plus importante que celles juste autour
                        if (distance == 0)   distance = infos -> intensiteAureole;

                        densite[zoneY + j + l][zoneX + i  + k] += cellVivante.taille * attrait / distance;
                    }
                }
            }
        }
    }
}

// Auréole les bords
void aureoleBords(Infos *infos, int **densite, int nombreZonesX, int nombreZonesY, int repulsion, int tailleAureole) {

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
                            if (distance == 0)   distance = infos -> intensiteAureoleBords;
                            densite[i + l][j + k] -= repulsion / distance;
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
    float tailleZoneX = infos->plusPetiteTaille * infos -> resolution;
    float tailleZoneY = infos->plusPetiteTaille * infos -> resolution;
    float posX;
    float posY;
    float distance = 1;
    float ratio = -9999999999999;
    float bestRatio = -999999999999;
    infos -> split = 0;
    infos -> plusGrosseTaille = 0;
    infos -> plusPetiteTaille = 999999;
    int nombreSplit = 0;
    int bestDensite = 0;

    for (k = 0; k < 30; k++) {

        // Si on pilote la cellule
        if (infos -> cellules[k].id != 0) {

            nombreSplit ++;

            posX = infos -> cellules[k].x;
            posY = infos -> cellules[k].y;

            // On set la plus grosse cellule pilotée
            if (infos -> cellules[k].taille >=  infos -> plusGrosseTaille) infos -> plusGrosseTaille = infos -> cellules[k].taille;

            // On set la plus petite cellule pilotée
            if (infos -> cellules[k].taille <=  infos -> plusPetiteTaille) infos -> plusPetiteTaille = infos -> cellules[k].taille;

            // On cherche le meilleur ratio
            for (i = 0; i < nombreZonesY; i++) {
                for (j = 0; j < nombreZonesX; j++) {

                    distanceX = (tailleZoneX * j + 0.5 * tailleZoneX) - posX;
                    distanceY = (tailleZoneY * i + 0.5 * tailleZoneY) - posY;
                    distance = sqrt((distanceX * distanceX) + (distanceY * distanceY));

                    if (distance != 0) {
    					ratio = densite[i][j] * infos -> densiteCoeff / ((infos -> distanceCoeff * distance) + (infos -> cellules[k].taille * infos -> tailleCoeff));
    				}
    				else {
    					ratio = 0;
    				}

                    // On selectionne la meilleure cellule et on pointe vers la cellule (pointeur infos -> distanceVise fois plus loin)
                    if (ratio > bestRatio) {
                        bestRatio = ratio;
                        bestDensite = densite[i][j];
                        infos -> taille = infos -> cellules[k].taille;
                        infos -> posX = infos -> cellules[k].x;
                        infos -> posY = infos -> cellules[k].y;

                        infos -> sourisX = (int)(posX + infos -> distanceVise * (tailleZoneX * j + 0.5 * tailleZoneX - posX));
                        infos -> sourisY = (int)(posY + infos -> distanceVise * (tailleZoneY * i + 0.5 * tailleZoneY - posY));

                        if (infos -> sourisX <= 0) infos -> sourisX = 1;
                        if (infos -> sourisY <= 0) infos -> sourisY = 1;
                        if (infos -> sourisX >= infos -> carteD) infos -> sourisX = infos -> carteD;
                        if (infos -> sourisY >= infos -> carteB) infos -> sourisY = infos -> carteB;
                    }
                }
            }
        }
    }

    if (infos -> solo && infos -> plusGrosseTaille > infos -> tailleSplit && nombreSplit < infos -> nombreSplit) {
        infos -> split = 1;
    }
    else if (!infos -> solo && bestDensite * infos -> intensiteAureole > infos -> taille * infos -> ratioSplitMulti * infos -> gentils && infos -> taille > infos -> tailleSplit && nombreSplit < infos -> nombreSplit) {
        infos -> split = 1;
    }

    // Si environnement vide, on va ailleurs
    if (bestRatio == 0) {
        if ((infos -> posX >= infos -> viserX[infos -> atteintViser] - 500 && infos -> posX <= infos -> viserX[infos -> atteintViser] + 500) && (infos -> posY >= infos -> viserY[infos -> atteintViser] - 500 && infos -> posY <= infos -> viserY[infos -> atteintViser] + 500))
        {
            infos -> atteintViser ++;
            infos -> atteintViser %= 9;
        }
        infos -> sourisX = infos -> viserX[infos -> atteintViser];
        infos -> sourisY = infos -> viserY[infos -> atteintViser];
    }

    //printf("From : %d, %d\nTo   : %d, %d\n",infos -> posX, infos -> posY, infos -> sourisX, infos -> sourisY);
}

// Modifie le buffer
void creerPaquetDeplacement (Buffer *envoi, Infos *infos){
    int i;
    int nombreCellules = 0;

    for (i = 0; i < 30; i++) {
        if (0 != infos->cellules[i].id) nombreCellules ++;
    }

    if (infos -> split == 1 && nombreCellules < 16){
        envoi -> len = 1;
        free(envoi -> buf);
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
        free(envoi -> buf);
        envoi -> buf = malloc(envoi -> len * sizeof(unsigned char));

        unsigned char *paquetIntermediaire1;
        paquetIntermediaire1 = malloc(sizeof(unsigned char) * 5);
        assemblerPaquets(idPaquet, 1, xPaquet, 4, paquetIntermediaire1);
        unsigned char *paquetIntermediaire2;
        paquetIntermediaire2 = malloc(sizeof(unsigned char) * 9);
        assemblerPaquets(paquetIntermediaire1, 5, yPaquet, 4, paquetIntermediaire2);
        // Derniers paquets
        assemblerPaquets(paquetIntermediaire2, 9, uselessPaquet, 4, envoi -> buf);

        //printf("xPaquet %d yPaquet %d\n\n",valeurPaquet (1, 4, envoi -> buf), valeurPaquet (5, 4, envoi -> buf));


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

        for (i = 0; i < 30; i++) {
            infos->cellules[i].id = 0;
        }

        // printf("Plus de cellules controlées\n");
    }

    // Ajout cellule
    else if (recu[0] == 32) {

        int i = 0;
        int aAjouter = valeurPaquet(1, 4, recu);

        while (infos->cellules[i].id != 0 && i < 30 && infos->cellules[i].id != aAjouter) i++;

        if (infos->cellules[i].id == 0) {
            infos->cellules[i].id = aAjouter;
        }
        // else {
        //   printf("Pas d'ajout ??!!?");
        // }

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

        // On set les points interessants en cas de vide sur la map
        infos -> viserX[0] = 200;
        infos -> viserY[0] = 200;

        infos -> viserX[1] = 200;
        infos -> viserY[1] = infos->carteB - 200;

        infos -> viserX[2] = infos->carteD - 200;
        infos -> viserY[2] = infos->carteB - 200;

        infos -> viserX[3] = infos->carteD - 200;
        infos -> viserY[3] = 200;

        infos -> viserX[4] = 200;
        infos -> viserY[4] = infos->carteB / 2;

        infos -> viserX[5] = infos->carteD / 2;
        infos -> viserY[5] = infos->carteB - 200;

        infos -> viserX[6] = infos->carteD - 200;
        infos -> viserY[6] = infos->carteB / 2;

        infos -> viserX[7] = infos->carteD / 2;
        infos -> viserY[7] = 200;

        infos -> viserX[8] = infos->carteD / 2;
        infos -> viserY[8] = infos->carteB / 2;

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

        nombreZonesX = (infos->carteD - infos->carteG) / (infos->plusPetiteTaille * infos -> resolution);
        nombreZonesY = (infos->carteB - infos->carteH) / (infos->plusPetiteTaille * infos -> resolution);

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
            for (j = 0; j < 30; j++) {
                if(valeurPaquet(i+4, 4, recu) == infos->cellules[j].id) {
                    infos->cellules[j].id = 0;
                }
            }
        }

        Cellule cellVivante;

        // Tri du buffer
        int parcourCell = 3 + cellMort * 8;

        // Traitement des cellules une à une
        while (recu[parcourCell] + recu[parcourCell + 1] + recu[parcourCell + 2] + recu[parcourCell + 3] != 0){

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
            hydrater(cellVivante, infos, densite, nombreZonesX, nombreZonesY, infos -> aureolage, 1, !infos -> solo, infos -> solo);
        }

		// On affiche la vision
		SDL_RenderPresent(renderer);
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(renderer, 245, 245, 245, 255);
		SDL_RenderClear(renderer);


		SDL_SetRenderDrawBlendMode(settingsRenderer, SDL_BLENDMODE_BLEND);

		// On vérifie si les curseurs ont changé
		if (SDL_PollEvent(&event)) {
			if(event.type == SDL_MOUSEBUTTONUP) {
		        getCurseurValeur(infos, event.button.x, event.button.y);
		    }
		}

		// On set nos curseurs
		float curseurDistance = infos -> distanceCoeff / 2000;
		float curseurRatioSplitMulti = infos -> ratioSplitMulti / 2;
		float curseurAureolage = infos -> aureolage / 50;
		float curseurRepulsionBords = infos -> repulsionBords / 100;
		float curseurIntensiteAureole = infos -> intensiteAureole / 0.5;
		float curseurNombreSplit = infos -> nombreSplit / 16;
		float curseurGentils = infos -> gentils / 50;
		float curseurMechants = infos -> mechants / 10000;

		char toWrite[50];

		// On affiche les settings
		sprintf(toWrite, "Distance : %d", (int)infos -> distanceCoeff);
		creerCurseur(20, 37, curseurDistance, toWrite);

		sprintf(toWrite, "Ratio Split : %d", (int)(infos -> ratioSplitMulti * 100));
		creerCurseur(20, 37 + 57 * 1, curseurRatioSplitMulti, toWrite);

		sprintf(toWrite, "Auréolage : %d", (int)infos -> aureolage);
		creerCurseur(20, 37 + 57 * 2, curseurAureolage, toWrite);

		sprintf(toWrite, "Repulsion Bords : %d", (int)infos -> repulsionBords);
		creerCurseur(20, 37 + 57 * 3, curseurRepulsionBords, toWrite);

		sprintf(toWrite, "Intensité Auréole : %d", (int)(infos -> intensiteAureole * 100));
		creerCurseur(20, 37 + 57 * 4, curseurIntensiteAureole, toWrite);

		sprintf(toWrite, "Nombre split : %d", (int)infos -> nombreSplit);
		creerCurseur(20, 37 + 57 * 5, curseurNombreSplit, toWrite);

		sprintf(toWrite, "Gentils : %d", (int)infos -> gentils);
		creerCurseur(20, 37 + 57 * 6, curseurGentils, toWrite);

		sprintf(toWrite, "Méchants : %d", (int)infos -> mechants);
		creerCurseur(20, 37 + 57 * 7, curseurMechants, toWrite);

		SDL_RenderPresent(settingsRenderer);
		SDL_SetRenderDrawColor(settingsRenderer, 60, 60, 60, 255);
		SDL_RenderClear(settingsRenderer);

        aureoleBords(infos, densite, nombreZonesX, nombreZonesY, infos -> repulsionBords, infos -> aureolage);

        // On se dirige ou on se splitte
        pointerVersPosition (infos, nombreZonesX, nombreZonesY, densite);

        for (i = 0; i < nombreZonesY; i++) {
            free(densite[i]);
        }
        free(densite);

        creerPaquetDeplacement(envoi, infos);
        // printf("paquet %s\n", envoi);

    }

    // Scores
    else if (recu[0] == 49) {
        // Score
    }


    else printf("Paquet inconnu d'OPCODE %d\n", recu[0]);

}
