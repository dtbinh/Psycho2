CC = g++
CFLAGS = -I.
LIBS = -lm -lpthread -ldl

all : main.o board.o game.o minimax.o
	$(CC) -o psycho main.o board.o game.o minimax.o $(LIBS)

main.o : main.cpp game.h
	$(CC) -c -o $@ $< $(CFLAGS)

board.o : board.cpp board.h game.h const.h
	$(CC) -c -o $@ $< $(CFLAGS)

game.o : game.cpp game.h minimax.h board.h const.h
	$(CC) -c -o $@ $< $(CFLAGS)

minimax.o : minimax.cpp minimax.h game.h const.h
	$(CC) -c -o $@ $< $(CFLAGS)

clean :
	rm -f psycho *.o
