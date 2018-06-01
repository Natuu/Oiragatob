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
  struct cellule *prochaine;

} Cellule;

typedef struct map{
  int left;
  int right;
  int top;
  int bottom;

} Map;

Buffer oiragatob (unsigned char *rbuf, Map *mapVisible);
