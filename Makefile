all: game.out

game.out: game.o client.o server.o utils.o
	gcc -lm -pthread game.o client.o server.o utils.o -o game

game.o: game.c common.h battle-map.h
	gcc -c game.c

client.o: client.c common.h utils.h client.h battle-map.h
	gcc -c client.c

server.o: server.c common.h utils.h server.h battle-map.h
	gcc -c server.c

utils.o: utils.c utils.h
	gcc -c utils.c

clean:
	rm game.o client.o server.o utils.o game