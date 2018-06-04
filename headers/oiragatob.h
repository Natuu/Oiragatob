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
  int visionG;
  int visionD;
  int visionH;
  int visionB;
  int sourisX;
  int sourisY;

} Infos;

Buffer oiragatob (unsigned char *rbuf, Infos *infos);

int valeurPaquet (int indiceDepart, int longueurPaquet, unsigned char *paquet);
