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

} Infos;

Buffer oiragatob (unsigned char *rbuf, Map *mapVisible);

int valeurPaquet (int indiceDepart, int longueurPaquet, unsigned char *paquet);
