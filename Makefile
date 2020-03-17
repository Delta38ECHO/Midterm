CFLAGS=-Wfatal-errors -Wall -std=c++14
CFLAGS+=-I/home/fac/nick/public/ncurses/include
LFLAGS=-L/home/fac/nick/public/ncurses/lib -lncurses

all: flappyrunner

debug: CFLAGS += -DDEBUG -g
debug: flappyrunner

flappyrunner: clean main.cpp flappyrunner.h
	g++ $(CFLAGS) main.cpp -o flappyrunner $(LFLAGS)

.PHONY: clean

clean:
	rm -f flappyrunner
