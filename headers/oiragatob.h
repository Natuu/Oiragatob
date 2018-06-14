#ifndef OIGARATOB_H
#define OIGARATOB_H

typedef struct buffer{

    unsigned char *buf;
    unsigned int len;

} Buffer;

typedef struct cellule{

    unsigned int id;
    int x;
    int y;
    unsigned int taille;
    unsigned int flag;

} Cellule;

typedef struct infos{
    int carteG;
    int carteD;
    int carteH;
    int carteB;
    Cellule cellules[30];
    int posX;
    int posY;
    int taille;
    int plusPetiteTaille;
    int plusGrosseTaille;
    int visibleG;
    int visibleD;
    int visibleH;
    int visibleB;
    int sourisX;
    int sourisY;
    int split;
    int viserX[9];
    int viserY[9];
    int atteintViser;

} Infos;

void posInWindow(int *x, int *y, Infos *infos);

int tailleInWindow(int taille, Infos *infos);

void oiragatob (unsigned char *recu, Buffer *envoi, Infos *infos);

int valeurPaquet (int indiceDepart, int longueurPaquet, unsigned char *paquet);

double valeurPaquetF (int indiceDepart, int longueurPaquet, unsigned char *paquet);

void paquetValeur (int nombreOctets, int valeur, unsigned char *paquet);

void assemblerPaquets(unsigned char *paquet1, int longueurPaquet1, unsigned char *paquet2, int longueurPaquet2, unsigned char *paquet);

double binToDouble(int64_t x);

void pointerVersPosition (Infos *infos, int nombreZonesX, int nombreZonesY, int **densite);

void creerPaquetDeplacement(Buffer *envoi, Infos *infos);

void hydrater(Cellule cellVivante, Infos *infos, int **densite, int nombreZonesX, int nombreZonesY, int tailleAureole, int food, int ennemis, int virus);

void aureoleBords(int **densite, int nombreZonesX, int nombreZonesY, int repulsion, int tailleAureole);

#endif
