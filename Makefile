all: bot

bot: bin/bot.o bin/oiragatob.o bin/sdlFonctions.o
	gcc -o bot bin/bot.o bin/oiragatob.o bin/sdlFonctions.o -g -Wall -lwebsockets -lm `sdl2-config --cflags --libs` -lSDL2_gfx

bin/bot.o: bot.c headers/oiragatob.h headers/client.h headers/sdlFonctions.h
	gcc -o bin/bot.o -c bot.c -g -Wall

bin/oiragatob.o: src/oiragatob.c headers/oiragatob.h headers/sdlFonctions.h
	gcc -o bin/oiragatob.o -c src/oiragatob.c -g -Wall

bin/sdlFonctions.o : src/sdlFonctions.c headers/sdlFonctions.h
	gcc -o bin/sdlFonctions.o -c src/sdlFonctions.c -g -Wall

clean:
	rm -rf bin/*.o

mrproper: clean
	rm -rf bot
