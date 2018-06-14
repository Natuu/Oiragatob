all: bot

bot: bot.o oiragatob.o
	gcc -o bot bot.o oiragatob.o sdlFonctions.o -g -Wall -lwebsockets -lm `sdl2-config --cflags --libs`

bot.o: bot.c headers/oiragatob.h headers/client.h headers/sdlFonctions.h
	gcc -o bot.o -c bot.c -g -Wall

oiragatob.o: src/oiragatob.c headers/oiragatob.h headers/client.h headers/sdlFonctions.h
	gcc -o oiragatob.o -c src/oiragatob.c -g -Wall

sdlFonctions.o : src/oiragatob.c headers/sdlFonctions.h
	gcc -o sdlFonctions.o -c src/sdlFonctions.c -g -Wall

clean:
	rm -rf *.o

mrproper: clean
	rm -rf bot
