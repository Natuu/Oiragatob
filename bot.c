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

#include "headers/client.h"
#include "headers/oiragatob.h"
#include "headers/sdlFonctions.h"

// Fine tuning
#define DISTANCECOEFF          1550
#define DENSITECOEFF           1
#define RESOLUTION             0.4
#define REPULSIONBORDS         14
#define DISTANCEVISE           12
#define AUREOLAGE              15
#define TAILLECOEFF            1000
#define TAILLESPLIT            75
#define NOMBRESPLIT            3
#define INTENSITEAUREOLE       0.36
#define INTENSITEAUREOLEBORDS  0.5
#define MECHANTS               72
#define VIRUS                  21
#define GENTILS                10
#define RATIOSPLITMULTI        2

// compile with gcc -Wall -g -o bot ./bot.c ./src/oiragatob.c -lwebsockets -lm
// call with: ./bot 127.0.0.1:1443      -s for solo

Infos infos;
int i;

t_packet* packetList = NULL;
struct lws_protocols protocols[] = { { "ogar_protocol", callbackOgar, 0,	20 }, { NULL, NULL, 0, 0 } };
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Window *densiteWindow = NULL;
SDL_Renderer *densiteRenderer = NULL;
SDL_Window *settings = NULL;
SDL_Renderer *settingsRenderer = NULL;
SDL_Event event;
TTF_Font *pfont = NULL;
int solo = 0;

// =========================================================================================================================
//	Start of function definition
// =========================================================================================================================

// Caught on CTRL C
void sighandler(int sig)
{
	forceExit = 1;
}

/**
\brief Allocate a packet structure and initialise it.
\param none
\return pointer to new allocated packet
****************************************************************************************************************************/
t_packet *allocatePacket()
{
	t_packet *tmp;

	if ((tmp=malloc(sizeof(t_packet))) == NULL ) return NULL;
	memset(tmp,0,sizeof(t_packet));
	return tmp;
}

/**
\brief Add a packet to the list of packet to be sent
\param wsi websocket descriptor
\param buf buffer to be sent
\param len length of packet
\return pointer to new allocated packet
****************************************************************************************************************************/
int sendCommand(struct lws *wsi,unsigned char *buf,unsigned int len)
{
	t_packet *tmp,*list=packetList;

	if (len > MAXLEN ) return -1;
	if ((tmp=allocatePacket()) == NULL ) return -1;
	memcpy(&(tmp->buf)[LWS_PRE],buf,len);
	tmp->len=len;
	if (packetList == NULL )
	packetList=tmp;
	else {
		while (list && list->next) {
			list=list->next;
		}
		list->next=tmp;
	}
	lws_callback_on_writable(wsi);
	return 1;
}


/****************************************************************************************************************************/
int writePacket(struct lws *wsi)
{
	t_packet *tmp=packetList;
	int ret;

	if (packetList == NULL ) return 0;

	packetList=tmp->next;
	ret=lws_write(wsi,&(tmp->buf)[LWS_PRE],tmp->len,LWS_WRITE_BINARY);
	free(tmp);
	lws_callback_on_writable(wsi);
	return(ret);
}

/****************************************************************************************************************************/

int callbackOgar(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
{
	static unsigned int offset=0;
	static unsigned char rbuf[MAXLEN];


	switch (reason) {
		case LWS_CALLBACK_CLIENT_ESTABLISHED:
		fprintf(stderr, "ogar: LWS_CALLBACK_CLIENT_ESTABLISHED\n");

		// Initialisation
		infos.carteG = 0;
		infos.carteD = 0;
		infos.carteH = 0;
		infos.carteB = 0;
		infos.taille = 10;
		infos.plusPetiteTaille = 9;
		infos.plusGrosseTaille = 999999;
		infos.posX = 0;
		infos.posY = 0;
		infos.visibleG = 0;
		infos.visibleD = 0;
		infos.visibleH = 0;
		infos.visibleB = 0;
		infos.sourisX = 0;
		infos.sourisY = 0;
		infos.split = 0;
		infos.atteintViser = 0;

		for (i = 0; i < 30; i++) {
			infos.cellules[i].id = 0;
		}

		for (i = 0; i < 9; i++) {
			infos.viserX[i] = 0;
			infos.viserX[i] = 0;
		}

		infos.distanceCoeff = DISTANCECOEFF;
		infos.densiteCoeff = DENSITECOEFF;
		infos.resolution = RESOLUTION;
		infos.solo = solo;
		infos.repulsionBords = REPULSIONBORDS;
		infos.distanceVise= DISTANCEVISE;
		infos.aureolage = AUREOLAGE;
		infos.tailleCoeff = TAILLECOEFF;
		infos.tailleSplit = TAILLESPLIT;
		infos.nombreSplit= NOMBRESPLIT;
		infos.intensiteAureole = INTENSITEAUREOLE;
		infos.intensiteAureoleBords = INTENSITEAUREOLEBORDS;
		infos.mechants = MECHANTS;
		infos.virus = VIRUS;
		infos.gentils = GENTILS;
		infos.ratioSplitMulti = RATIOSPLITMULTI;

		// Ouvrir la connexion
		unsigned char connexion[] = {0xff, 0x00, 0x00, 0x00, 0x00};
		sendCommand(wsi, connexion, 5);

		// Ouvrir la connexion
		unsigned char connexion2[] = {0xfe, 0x0d, 0x00, 0x00, 0x00};
		sendCommand(wsi, connexion2, 5);

		// Choisir un nom
		unsigned char nom[] = {0x00, 'L', 'e', 'c', 'h', 'B', 'o', 't', 0x00};
		sendCommand(wsi, nom, 9);

		break;

		case LWS_CALLBACK_CLIENT_WRITEABLE:
		if (writePacket(wsi) < 0 ) forceExit=1;
		break;

		case LWS_CALLBACK_CLIENT_RECEIVE:
		// we have receive some data, check if it can be written in static allocated buffer (length)

		if (offset + len < MAXLEN ) {
			memcpy(rbuf+offset,in,len);
			offset+=len;
			// we have receive some data, check with websocket API if this is a final fragment
			if (lws_is_final_fragment(wsi)) {
				// call recv function here !!!!

				// FONCTION PRINCIPALE

				Buffer command;
				command.buf = malloc(sizeof(unsigned char));
				oiragatob(rbuf, &command, &infos, &forceExit);
				sendCommand(wsi, command.buf, command.len);
				free(command.buf);

				offset=0;
			}
		} else {	// length is too long... get others but ignore them...
			offset=MAXLEN;
			if ( lws_is_final_fragment(wsi) ) {
				offset=0;
			}
		}
		break;
		case LWS_CALLBACK_CLOSED:
		lwsl_notice("ogar: LWS_CALLBACK_CLOSED\n");
		forceExit = 1;
		break;
		case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
		lwsl_err("ogar: LWS_CALLBACK_CLIENT_CONNECTION_ERROR\n");
		forceExit = 1;
		break;

		case LWS_CALLBACK_COMPLETED_CLIENT_HTTP:
		lwsl_err("ogar: LWS_CALLBACK_COMPLETED_CLIENT_HTTP\n");
		forceExit = 1;
		break;

		default:
		break;
	}

	return 0;
}

/****************************************************************************************************************************/
int main(int argc, char **argv)
{
	int j;

	FILE* fichier = NULL;

  fichier = fopen("param.md", "a");

	srandom(time(NULL));

	int secondesDepart = time(NULL);
	int secondesEcoules = 0;
	int distanceCoeff = rand() % 2000;
	int ratioSplitMulti = rand() % 10;
	int aureolage = rand() % 50;
	int repulsionBords = rand() % 100;
	float intensiteAureole = (float)(rand() % 50)/100;
	int gentils = rand() % 50;
	int mechants = rand() % 100;
	int virus = rand() % 50;
	// On initialise la fenetre de vision
    if(0 != init(&window, &renderer, 915, 530)) goto Quit;
	SDL_SetRenderDrawColor(renderer, 245, 245, 245, 255);
	SDL_SetWindowTitle(window, "Vision du Lechbot");
	SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

	// On initialise la fenetre de densité
	if(0 != init(&densiteWindow, &densiteRenderer, 500, 530)) goto Quit;
	SDL_SetRenderDrawColor(densiteRenderer, 245, 245, 245, 255);
	SDL_SetWindowTitle(densiteWindow, "Densite");

	// On initialise la fenetre de settings
	if(0 != init(&settings, &settingsRenderer, 300, 530)) goto Quit;
	SDL_SetRenderDrawColor(settingsRenderer, 200, 200, 200, 255);
	SDL_SetWindowTitle(settings, "Parametres du Lechbot");
	SDL_RenderClear(settingsRenderer);
    SDL_RenderPresent(settingsRenderer);

	// On déplace les fenetres
	int *x = malloc(sizeof(int));
	int *y = malloc(sizeof(int));

	SDL_GetWindowPosition(window, x, y);
	SDL_SetWindowPosition(window, *x + 50, *y);
	SDL_SetWindowPosition(densiteWindow, *x - 505 + 50, *y);
	SDL_SetWindowPosition(settings, *x + 920 + 50, *y);

	free(x);
	free(y);

	// On réccupère les parametres de la ligne de commande
	int n = 0;

	struct lws_context_creation_info info;
	struct lws_client_connect_info i;

	struct lws_context *context;
	const char *protocol,*temp;

	memset(&info, 0, sizeof info);
	memset(&i, 0, sizeof(i));

	if (argc < 2)
	goto usage;

	i.origin = "agar.io";

	while (n >= 0) {
		n = getopt(argc, argv, "hsp:P");
		if (n < 0)
		continue;
		switch (n) {

			// On met solo à 1 si -s
			case 's':
			solo = 1;

			break;
			case 'p':
			i.port = atoi(optarg);
			break;
			case 'P':
			info.http_proxy_address = optarg;
			break;
			case 'h':
			goto usage;
		}
	}

	srandom(time(NULL));

	if (optind >= argc)
	goto usage;

	signal(SIGINT, sighandler);

	if (lws_parse_uri(argv[optind], &protocol, &i.address, &i.port, &temp))
	goto usage;

	if (!strcmp(protocol, "http") || !strcmp(protocol, "ws"))
	i.ssl_connection = 0;
	if (!strcmp(protocol, "https") || !strcmp(protocol, "wss"))
	i.ssl_connection = 1;

	i.host = i.address;
	i.ietf_version_or_minus_one = -1;
	i.client_exts = NULL;
	i.path="/";

	info.port = CONTEXT_PORT_NO_LISTEN;
	info.protocols = protocols;
	info.gid = -1;
	info.uid = -1;

	context = lws_create_context(&info);
	if (context == NULL) {
		fprintf(stderr, "Creating libwebsocket context failed\n");
		return 1;
	}

	i.context = context;

	if (lws_client_connect_via_info(&i)); // just to prevent warning !!

	forceExit=0;
	// the main magic here !!
	while (!forceExit) {
		secondesEcoules = time(NULL) - secondesDepart;
		//printf("%d\n",secondesEcoules);
		if (secondesEcoules < 120){
			lws_service(context, 1000);
			infos.distanceCoeff = distanceCoeff;
			//printf("%d\n",infos.taille);
			infos.ratioSplitMulti  = ratioSplitMulti;
			infos.aureolage = aureolage;
			infos.repulsionBords = repulsionBords;
			infos.intensiteAureole = intensiteAureole;
			infos.gentils = gentils;
			infos.mechants = mechants;
			infos.virus = virus;

		}
		else {

			int tailleTotale = 0;
			for (j = 0; j < 30; j++) {
				tailleTotale += infos.cellules[j].taille;
			}

			if (tailleTotale > 100){
				fprintf(fichier,"# Score *%d* \n\n", infos.taille);
				fprintf(fichier,"|**Paramètre**|**Valeur**|\n");
				fprintf(fichier,"|-------------|----------|\n");
				fprintf(fichier,"| Distance    | %lf      |\n", infos.distanceCoeff);
				fprintf(fichier,"| ratioSplitMulti | %lf  |\n", infos.ratioSplitMulti);
				fprintf(fichier,"| aureolage   | %lf      |\n", infos.aureolage);
				fprintf(fichier,"| repulsionBords | %lf   |\n", infos.repulsionBords);
				fprintf(fichier,"| intensiteAureole | %lf |\n", infos.intensiteAureole);
				fprintf(fichier,"| nombreSplit | %lf      |\n", infos.nombreSplit);
				fprintf(fichier,"| gentils     | %lf      |\n", infos.gentils);
				fprintf(fichier,"| mechants    | %lf      |\n", infos.mechants);
				fprintf(fichier,"| virus       | %lf      |\n", infos.virus);
				fprintf(fichier, "\n\n-------------\n\n");
			}
			fclose(fichier);
			forceExit = 1;
		}
	}

	// On destroy les fenetres et renderer
Quit:
	if(NULL != renderer) SDL_DestroyRenderer(renderer);
	if(NULL != window) SDL_DestroyWindow(window);
	if(NULL != densiteRenderer) SDL_DestroyRenderer(densiteRenderer);
	if(NULL != densiteWindow) SDL_DestroyWindow(densiteWindow);
	if(NULL != settingsRenderer) SDL_DestroyRenderer(settingsRenderer);
	if(NULL != settings) SDL_DestroyWindow(settings);
	SDL_Quit();

	// if there is some errors, we just quit
	lwsl_err("Exiting\n");
	lws_context_destroy(context);

	return 0;

	usage:
	fprintf(stderr, "Usage: ogar-client -h -s -p <port> -P <proxy> <server address> \n");
	return 1;
}
