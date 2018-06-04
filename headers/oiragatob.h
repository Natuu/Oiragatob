typedef struct buffer{

  unsigned char *buf;
  unsigned int len;

} Buffer;

typedef struct cellule{

  int id;
  float x;
  float y;
  float size;
  int flag;
  int zoneX;
  int zoneY;
  struct cellule *prochaine;

} Cellule;

typedef struct infos{
  int carteG;
  int carteD;
  int carteH;
  int carteB;
  int posX;
  int posY;
  int taille;
  int visibleG;
  int visibleD;
  int visibleH;
  int visibleB;
  int sourisX;
  int sourisY;

} Infos;

Buffer oiragatob (unsigned char *recu, Infos *infos);

int valeurPaquet (int indiceDepart, int longueurPaquet, unsigned char *paquet);

void paquetValeur (int nombreOctets, int valeur, unsigned char *paquet);

void assemblerPaquets(unsigned char *paquet1, int longueurPaquet1, unsigned char *paquet2, int longueurPaquet2, unsigned char *paquet);
