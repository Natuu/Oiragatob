all: bot

bot: bot.o oiragatob.o
	gcc -o bot bot.o oiragatob.o -g -Wall -lwebsockets -lm `sdl2-config --cflags --libs`

bot.o: bot.c headers/oiragatob.h headers/client.h
	gcc -o bot.o -c bot.c -g -Wall

oiragatob.o: src/oiragatob.c headers/oiragatob.h headers/client.h
	gcc -o oiragatob.o -c src/oiragatob.c -g -Wall

clean:
	rm -rf *.o

mrproper: clean
	rm -rf bot
