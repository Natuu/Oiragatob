#ifndef CLIENT_H
#define CLIENT_H

#include "libwebsockets.h"
#define MAXLEN 50000

extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern SDL_Window *settings;
extern SDL_Renderer *settingsRenderer;
extern SDL_Event event;

int callbackOgar(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len);

int forceExit;

typedef struct s_packet {
        unsigned char buf[MAXLEN+LWS_PRE];
        unsigned int len;
        struct s_packet *next;
} t_packet;

int callbackOgar(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len);

#endif
